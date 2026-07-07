// infrastructure/db/postgres_events_repo.h

#include "i_event_repository.h"

#include <pqxx/pqxx>

class PostgresEventsRepo : public IEventRepository {
public:
  explicit PostgresEventsRepo(std::shared_ptr<pqxx::connection> conn);

  shared::Result<> SaveEvents(std::span<const dto::Event> events) override;

  shared::Result<std::vector<dto::Event>>
  GetEvents(const std::optional<dto::EventFilter> &filter) override;

  shared::Result<dto::Stats> GetStats() override;

private:
  void PrepareStatements();

  std::shared_ptr<pqxx::connection> conn_;
};