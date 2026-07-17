COMPOSE = docker compose -f docker-compose.yaml

.PHONY: up down clean logs build server client pgadmin restart-server

up:
	$(COMPOSE) up --build

down:
	$(COMPOSE) down

clean:
	$(COMPOSE) down -v

logs:
	$(COMPOSE) logs -f

build:
	$(COMPOSE) build

server:
	$(COMPOSE) up history-server

restart-server:
	$(COMPOSE) restart history-server

client:
	$(COMPOSE) run --rm ipv4filter-client ./ipv4filter-client

pgadmin:
	$(COMPOSE) up --build pgadmin