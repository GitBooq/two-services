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