-Сервис#1 содержит ipFilter(тема 1) как основу, логер(тема 4), модули-интерфейсы для передачи,
   получения ивентов и статистики(SaveEvent, GetEvents, GetStats) и сетевой интерфейс(тема 6).
   Весь вывод направляется через модуль-логер.
   На вход принимает логи, применяет заданные правила фильтрации, отправляет во второй сервис(сервер) запросы по сети.

-Сервис#2 сервер, содержит модули-интерфейсы для передачи ивентов и статистики(SaveEvent, SendEvents, SendStats),
  интерфейс для взаимодействия с субд(тема 7), сетевой интерфейс(тема 6) и логер для вывода.
  Слушает порт, получает запросы на сохранение данных/отправку сохраненных данных/статистики,
  сохраняет в БД/возвращает данные из БД/возвращает статы.

-СУБД.



## Event
Обязательные поля:
- source_service (ipv4_filter) string
- timestamp_utc (Время в ISO-8601 UTC) string
- status (success или error) string
- payload (Доменные данные события) object
---
payload для IPv4-фильтра:  
- raw_line   string   обязательно  
- parsed_ip   string   опционально  
- filter_decision   string   accepted или rejected  
- reject_reason   string   опционально  

## RDBMS
PostgreSQL

Schema:
``` sql
CREATE TABLE IF NOT EXISTS Events (
  id SERIAL PRIMARY KEY,
  timestamp_utc TIMESTAMPTZ DEFAULT NOW(),
  status VARCHAR (255) NOT NULL,
  payload_id 

  CONSTRAINT check_status CHECK (status IN ('success', 'error'))
);

CREATE TABLE IF NOT EXISTS Ipv4_filter_payloads (
    id BIGSERIAL PRIMARY KEY,
    event_id BIGINT REFERENCES events(id) ON DELETE CASCADE,
    raw_line TEXT NOT NULL,
    parsed_ip INET,
    filter_decision VARCHAR(10) NOT NULL CHECK (filter_decision IN ('accepted', 'rejected')),
    reject_reason TEXT,
    
    UNIQUE (event_id)
);
```

192.168.1.25 - GET /index.html
10.0.0.100 - POST /login
192.168.1.0 - NETWORK_ADDR
192.168.1.255 - BROADCAST_ADDR
10.0.0.1 - LOWER_BOUND
10.0.0.100 - UPPER_BOUND