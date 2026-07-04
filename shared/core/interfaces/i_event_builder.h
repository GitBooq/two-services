// core/interfaces/i_event_builder.h
#pragma once

#include "core/entities/event.h"
#include "net_logger/types.h" // for LogEntry

class IEventBuilder {
public:
    virtual ~IEventBuilder() = default;
    
    // Build one Event from one LogEntry
    virtual dto::Event Build(const net::logger::LogEntry& log_entry) = 0;
};