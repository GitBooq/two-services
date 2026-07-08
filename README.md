# Event History Service + IPv4 Filtering Service


IPv4 Filtering Service processes logs, forms events and sends them to Event History Service.  
Event History Service save events in db and send them back on request.

**DBMS**: PostgreSQL  
**Transport**: gRPC + Protobuf  
## DTOs:

### Event:
| Field | Type | Description |
|-------|------|-------------|
| `source_service` | string | Source service name: `ipv4_filter` |
| `timestamp_utc` | string | ISO-8601 UTC timestamp |
| `status` | string | Event status: `success` or `error` |
| `payload` | object | Domain-specific event data |

### Payload:
| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `raw_line` | string | Yes | Original input line |
| `filter_decision` | string | Yes | `accepted` or `rejected` |
| `parsed_ip` | string | No | Extracted IPv4 address if valid |
| `reject_reason` | string | No | Rejection reason (for rejected) |


### Database Schema
```SQL
CREATE TABLE IF NOT EXISTS events (
  id SERIAL PRIMARY KEY,
  source_service VARCHAR (20) NOT NULL,
  timestamp_utc TIMESTAMPTZ DEFAULT NOW(),
  status VARCHAR (10) NOT NULL,

  CONSTRAINT check_source_service CHECK (source_service IN ('calculator', 'ipv4_filter')),
  CONSTRAINT check_status CHECK (status IN ('success', 'error'))
);
-- 1-to-1
CREATE TABLE IF NOT EXISTS payloads (
    event_id INT PRIMARY KEY,
    raw_line TEXT NOT NULL,
    parsed_ip INET,
    filter_decision VARCHAR(10) NOT NULL,
    reject_reason TEXT,
    
    CONSTRAINT fk_events_payloads FOREIGN KEY (event_id) REFERENCES events (id) ON DELETE CASCADE,
    CONSTRAINT check_filter_decision CHECK (filter_decision IN ('accepted', 'rejected'))
);
```

## Use Cases:
### SaveEvent  
Client sends events in batch. Server saves them in db.

### GetEvents 
Client send request w/ `filter`. Server make request to db applying filter and send events back.

#### Filters
   - from (timestamp)
   - to (timestamp)
   - limit  (sql like)
   - offset (sql like)
   - status (`success` or `error`)
   - source_service

### GetStats
Server send stats.
- total events
- success events
- error events

## Requirements
- Docker (w/ compose)
- Make

## Quick Start
``` bash
make build -- build all
make server -- start server and postgres
```
*in another window:*
``` bash
make client -- start client with CLI
```
### Client CLI
``` bash
Available commands:
  Save <file_path>               - Save events from file to server
  Get [options]                  - Get events with optional filters
  Stats                          - Get statistics
  Help                           - Show this help message
  Exit                           - Exit program

GetEvents options:
  --limit <number>               - Limit number of events
  --offset <number>              - Offset for pagination
  --status <status>              - Filter by status (success/error)
  --source_service <service>     - Filter by source service

Examples:
  Save /data/ip_logs.txt
  Get --limit 10 --offset 1
  Get --status <success>
  Get --source_service ipv4_filter
```
