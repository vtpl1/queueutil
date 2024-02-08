// *****************************************************
//    Copyright 2024 Videonetics Technology Pvt Ltd
// *****************************************************

#include "queue_manager.h"
#include <string>
#include <vector>
#include <utility>

auto QueueManagerRawBuffer::getInstance() -> QueueManagerRawBuffer& {
  static QueueManagerRawBuffer instance;
  return instance;
}

auto QueueManagerRawBuffer::get_keys(std::string key) -> std::vector<std::string> {
  return getInstance().queue_manager_.get_keys(std::move(key));
}

auto QueueManagerRawBuffer::remove_queue(std::string key) -> bool {
  return getInstance().queue_manager_.remove_queue(std::move(key));
}
// static std::shared_ptr<buffer_queue<RawBuffer, 1024>> get_queue(std::string key);
// static bool                                           remove_queue(std::string key);