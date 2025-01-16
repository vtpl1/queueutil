// *****************************************************
//    Copyright 2024 Videonetics Technology Pvt Ltd
// *****************************************************

#include "queue_manager.h"
#include "buffer_queue.h"
#include "raw_buffer.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

constexpr int MAX_QUEUE_SIZE = 1024;
using QueueManagerRaw        = QueueManager<RawBuffer, MAX_QUEUE_SIZE>;

auto QueueManagerRawBuffer::get_keys(std::string key) -> std::vector<std::string> {
  return QueueManagerRaw::get_keys(std::move(key));
}

auto QueueManagerRawBuffer::get_queue(std::string key) -> std::shared_ptr<buffer_queue<RawBuffer, MAX_QUEUE_SIZE>> {
  return QueueManagerRaw::get_queue(std::move(key));
}

auto QueueManagerRawBuffer::get_queue_if_exists(std::string key)
    -> std::shared_ptr<buffer_queue<RawBuffer, MAX_QUEUE_SIZE>> {
  return QueueManagerRaw::get_queue_if_exists(std::move(key));
}

auto QueueManagerRawBuffer::remove_queue(std::string key) -> bool {
  return QueueManagerRaw::remove_queue(std::move(key));
}
