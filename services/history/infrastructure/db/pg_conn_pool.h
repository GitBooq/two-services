// pg_conn_pool.h

//****************************************************************
//  Postgres(libpqxx) connection management header only library
//****************************************************************

// Example:
// #include "pg_conn_pool.h"

// ConnectionPool pool("dbname=mydb user=root password=root host=localhost",
// 10);

// void handleClientRequest(ConnectionPool& pool) {
//     {
//         auto conn = pool.acquire(); // blocking call
//         pqxx::work txn(*conn);
//         auto result = txn.exec("SELECT * FROM users WHERE id = 1");
//         txn.commit();
//         // handle
//     } // conn destroyed
// }

#pragma once

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include <queue>
#include <stdexcept>
#include <utility>

class ConnectionPool final {
public:
  static constexpr std::size_t kDefaultPoolSz = 2;

  // pool_size -- shouldn't exceed max_connections in PostgreSQL
  explicit ConnectionPool(std::string conn_str,
                          size_t pool_size = kDefaultPoolSz)
      : conn_string_(std::move(conn_str)), pool_size_(pool_size) {
    std::queue<std::unique_ptr<pqxx::connection>> temp_connections;

    for (size_t i = 0; i < pool_size_; ++i) {
      try {
        temp_connections.push(std::make_unique<pqxx::connection>(conn_string_));
      } catch (const std::exception &e) {
        std::cerr << "Failed to create connection " << i << ": " << e.what()
                  << std::endl;
        throw;
      }
    }
    connections_ = std::move(temp_connections);
  }

  ConnectionPool(const ConnectionPool &) = delete;
  ConnectionPool &operator=(const ConnectionPool &) = delete;
  ConnectionPool(ConnectionPool &&) = delete;
  ConnectionPool &operator=(ConnectionPool &&) = delete;
  ~ConnectionPool() = default;

  // Connection RAII-wrapper
  class ConnectionHandle {
  public:
    pqxx::connection *operator->() const { return conn_.get(); }
    pqxx::connection &operator*() const { return *conn_; }

    ~ConnectionHandle() noexcept {
      if ((pool_ != nullptr) && conn_) {
        pool_->ReturnConnection(std::move(conn_));
      }
    }

    ConnectionHandle(const ConnectionHandle &) = delete;
    ConnectionHandle &operator=(const ConnectionHandle &) = delete;
    ConnectionHandle(ConnectionHandle &&other) noexcept
        : pool_(other.pool_), conn_(std::move(other.conn_)) {
      other.pool_ = nullptr;
    }
    ConnectionHandle &operator=(ConnectionHandle &&other) noexcept {
      if (this != &other) {
        if ((pool_ != nullptr) && conn_) {
          pool_->ReturnConnection(std::move(conn_));
        }
        pool_ = other.pool_;
        conn_ = std::move(other.conn_);
        other.pool_ = nullptr;
      }
      return *this;
    }

  private:
    friend class ConnectionPool;
    // only ConnectionPool can create handle
    ConnectionHandle(ConnectionPool *pool,
                     std::unique_ptr<pqxx::connection> conn)
        : pool_(pool), conn_(std::move(conn)) {}

    ConnectionPool *pool_;
    std::unique_ptr<pqxx::connection> conn_;
  };

  // blocking call
  ConnectionHandle Acquire() {
    std::unique_lock lock(mutex_);
    cv_.wait(lock, [this]() { return !connections_.empty(); });

    std::unique_ptr<pqxx::connection> conn = std::move(connections_.front());
    connections_.pop();
    return {this, std::move(conn)};
  }

  ConnectionHandle Acquire(std::chrono::milliseconds timeout_ms) {
    std::unique_lock lock(mutex_);
    if (!cv_.wait_for(lock, timeout_ms,
                      [this]() { return !connections_.empty(); })) {
      throw std::runtime_error("Timeout waiting for free connection");
    }

    std::unique_ptr<pqxx::connection> conn = std::move(connections_.front());
    connections_.pop();
    return {this, std::move(conn)};
  }

private:
  // Try to return connection back to queue
  // On exception reduce real pool size
  void ReturnConnection(std::unique_ptr<pqxx::connection> conn) noexcept {
    try {
      if (!conn || !conn->is_open()) {
        try {
          conn = std::make_unique<pqxx::connection>(conn_string_);
        } catch (const std::exception &e) {
          std::cerr << "Failed to reconnect: " << e.what() << std::endl;
          // suppress
        }
      }

      {
        std::scoped_lock lock(mutex_);
        if (conn) {
          connections_.push(std::move(conn));
        } else {
          std::cerr
              << "Connection lost and cannot be recreated. Pool size reduced."
              << std::endl;
        }
      }
    } catch (...) {
      // suppress all
    }
    cv_.notify_one();
  }

  std::string conn_string_;
  size_t pool_size_;
  std::queue<std::unique_ptr<pqxx::connection>> connections_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

template <typename Preparer>
std::shared_ptr<ConnectionPool> CreatePreparedPool(const std::string &conn_str,
                                                   size_t pool_size,
                                                   Preparer &&preparer) {
  auto pool = std::make_shared<ConnectionPool>(conn_str, pool_size);

  // Apply callback on connections
  for (size_t i = 0; i < pool_size; ++i) {
    auto conn = pool->Acquire();
    try {
      preparer(*conn);
    } catch (const std::exception &e) {
      std::cerr << "Failed to prepare: " << e.what() << std::endl;
    }
  }

  return pool;
}