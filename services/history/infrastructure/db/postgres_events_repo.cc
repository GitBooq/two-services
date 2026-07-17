// infrastructure/db/postgres_events_repo.cc

#include "postgres_events_repo.h"
#include "event.h"
#include "result.h"
#include <optional>
#include <string>

PostgresEventsRepo::PostgresEventsRepo(std::shared_ptr<ConnectionPool> pool)
    : pool_(std::move(pool)) {}

void PrepareStatements(pqxx::connection &conn) {
  conn.prepare("insert_event",
               "INSERT INTO events (source_service, timestamp_utc, status) "
               "VALUES ($1, $2, $3) RETURNING id");

  conn.prepare("insert_payload", "INSERT INTO payloads (event_id, raw_line, "
                                 "parsed_ip, filter_decision, reject_reason) "
                                 "VALUES ($1, $2, $3, $4, $5)");

  // returns 1 billion events MAX if no limit
  conn.prepare("get_events",
               "SELECT e.id, e.source_service, e.timestamp_utc, e.status, "
               "p.raw_line, p.parsed_ip, p.filter_decision, p.reject_reason "
               "FROM events e "
               "LEFT JOIN payloads p ON e.id = p.event_id "
               "WHERE e.source_service = COALESCE($1, e.source_service) "
               "AND e.timestamp_utc >= COALESCE($2, e.timestamp_utc) " // from
               "AND e.timestamp_utc <= COALESCE($3, e.timestamp_utc) " // to
               "AND e.status = COALESCE($4, e.status) "
               "ORDER BY e.timestamp_utc "
               "LIMIT COALESCE($5, 1e9) OFFSET GREATEST(COALESCE($6, 0), 0)");

  conn.prepare("get_stats",
               "SELECT "
               "  COUNT(*) AS total, "
               "  COUNT(*) FILTER (WHERE status = 'success') AS success, "
               "  COUNT(*) FILTER (WHERE status = 'error') AS error "
               "FROM events");
}

shared::Result<>
PostgresEventsRepo::SaveEvents(std::span<const dto::Event> events) {
  // nothing to insert is ok
  if (events.empty()) {
    return shared::Result<>::Ok();
  }

  auto conn = pool_->Acquire();

  try {
    pqxx::work txn(*conn);
    for (const auto &event : events) {

      auto res =
          txn.exec(pqxx::prepped{"insert_event"},
                   pqxx::params(event.source_service, event.timestamp_utc,
                                dto::Event::StatusToStr(event.status)));

      enum Query { ID = 0 };
      auto row_ref = res[0];
      int event_id = row_ref[Query::ID].as<int>();
      // or int event_id = res[0][0].as<int>();

      const auto &payload = event.payload;
      pqxx::params params;
      params.append(std::to_string(event_id));
      params.append(payload.raw_line);
      payload.parsed_ip.has_value() ? params.append(*payload.parsed_ip)
                                    : params.append();
      params.append(payload.filter_decision);
      payload.reject_reason.has_value()
          ? params.append(payload.reject_reason.value())
          : params.append();

      txn.exec(pqxx::prepped{"insert_payload"}, params);
    }

    txn.commit();
    return shared::Result<>::Ok();
  } catch (const std::exception &e) {
    return shared::Result<>::NotOk(std::string("[Postgres] ") + e.what());
  }
}

shared::Result<std::vector<dto::Event>>
PostgresEventsRepo::GetEvents(const std::optional<dto::EventFilter> &filter) {
  try {
    auto conn = pool_->Acquire();
    pqxx::read_transaction txn(*conn);
    pqxx::result res;

    if (filter.has_value()) {
      pqxx::params params;
      filter->source_service.has_value()
          ? params.append(*filter->source_service)
          : params.append();
      filter->from.has_value() ? params.append(*filter->from) : params.append();
      filter->to.has_value() ? params.append(*filter->to) : params.append();
      filter->status.has_value()
          ? params.append(dto::Event::StatusToStr(filter->status.value()))
          : params.append();
      filter->limit.has_value() ? params.append(std::to_string(*filter->limit))
                                : params.append();
      filter->offset.has_value() ? params.append(std::to_string(*filter->offset))
                                 : params.append();

      res = txn.exec(pqxx::prepped{"get_events"}, params);
    } else {
      res = txn.exec(pqxx::prepped{"get_events"});
    }

    enum Query {
      ID = 0,
      SRC_SRVC,
      TIMESTAMP,
      STATUS,
      RAW_LINE,
      PARSED_IP,
      FILTER_DES,
      REJ_REAS
    };
    std::vector<dto::Event> events;
    for (const auto &row : res) {
      dto::Event event;
      event.source_service = row[Query::SRC_SRVC].as<std::string>();
      event.timestamp_utc = row[Query::TIMESTAMP].as<std::string>();
      event.status =
          dto::Event::StrToStatus(row[Query::STATUS].as<std::string>());

      dto::Payload payload;
      payload.raw_line = row[Query::RAW_LINE].as<std::string>();

      if (!row[Query::PARSED_IP].is_null()) {
        payload.parsed_ip = row[Query::PARSED_IP].as<std::string>();
      }

      payload.filter_decision = row[Query::FILTER_DES].as<std::string>();

      if (!row[Query::REJ_REAS].is_null()) {
        payload.reject_reason = row[Query::REJ_REAS].as<std::string>();
      }

      event.payload = std::move(payload);
      events.push_back(std::move(event));
    }

    txn.commit();
    return shared::Result<std::vector<dto::Event>>::Ok(events);
  } catch (const std::exception &e) {
    return shared::Result<std::vector<dto::Event>>::NotOk(
        std::string("[Postgres] ") + e.what());
  }
}

shared::Result<dto::Stats> PostgresEventsRepo::GetStats() {
  try {
    auto conn = pool_->Acquire();
    pqxx::read_transaction txn(*conn);

    auto res = txn.exec(pqxx::prepped{"get_stats"});

    if (res.empty()) {
      txn.commit();
      return shared::Result<dto::Stats>::NotOk("No stats returned");
    }

    enum Query { TOTAL = 0, SUCCESS, ERROR };
    const auto &row = res[0];

    dto::Stats stats{};
    stats.events_total = row[Query::TOTAL].as<std::size_t>();
    stats.events_success = row[Query::SUCCESS].as<std::size_t>();
    stats.events_error = row[Query::ERROR].as<std::size_t>();

    txn.commit();
    return shared::Result<dto::Stats>::Ok(stats);
  } catch (const std::exception &e) {
    return shared::Result<dto::Stats>::NotOk(std::string("[Postgres] ") +
                                             e.what());
  }
}
