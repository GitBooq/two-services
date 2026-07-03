// message_sender.h
#pragma once

#include "core/event_message.h"

namespace client {

class IMessageSender {
public:
  virtual ~IMessageSender() = default;

  virtual bool Send(const EventMessage &message) = 0;
};

} // namespace application