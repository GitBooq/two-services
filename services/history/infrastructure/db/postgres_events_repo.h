// infrastructure/db/postgres_events_repo.h

#pragma once

#include "i_event_repository.h"
#include "pg_conn_pool.h"

#include <pqxx/pqxx>

class PostgresEventsRepo : public IEventRepository {
public:
  explicit PostgresEventsRepo(std::shared_ptr<ConnectionPool> pool);

  shared::Result<> SaveEvents(std::span<const dto::Event> events) override;

  shared::Result<std::vector<dto::Event>>
  GetEvents(const std::optional<dto::EventFilter> &filter) override;

  shared::Result<dto::Stats> GetStats() override;

private:
  std::shared_ptr<ConnectionPool> pool_;
};

void PrepareStatements(pqxx::connection &conn);