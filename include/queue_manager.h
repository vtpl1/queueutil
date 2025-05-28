// *****************************************************
//    Copyright 2024 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef queue_manager_h
#define queue_manager_h

#include "buffer_queue.h"

#include "raw_buffer.h"
#include <absl/strings/match.h>
#include <atomic>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>

template <typename T, uint64_t size> class QUEUEUTIL_EXPORT QueueManager {
private:
  std::map<std::string, std::shared_ptr<buffer_queue<T, size>>> queue_map_;

  std::mutex           mutex_;
  std::atomic_uint64_t uuid_generator_{0};

  static QueueManager& getInstance() {
    static QueueManager instance;
    return instance;
  }

  std::shared_ptr<buffer_queue<T, size>> get_queue_(std::string key) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_map_.find(key) == queue_map_.end()) {
      queue_map_.emplace(key, std::make_shared<buffer_queue<T, size>>());
    }
    return queue_map_.at(key);
  }

  std::shared_ptr<buffer_queue<T, size>> get_queue_if_exists_(std::string key) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_map_.find(key) != queue_map_.end()) {
      return queue_map_.at(key);
    }
    return nullptr;
  }

  bool remove_queue_(std::string key) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_map_.erase(key);
    return true;
  }

  uint64_t get_src_uuid_() { return 0; }

  uint64_t get_sink_uuid_() { return uuid_generator_++; }

  std::vector<std::string> get_keys_(std::string type) {
    std::vector<std::string> keys;

    std::lock_guard<std::mutex> lock(mutex_);
    for (auto&& itr : queue_map_) {
      std::string key = itr.first;
      if (type == std::string("")) {
        keys.emplace_back(key);
      } else {
        if (absl::StartsWithIgnoreCase(key, type)) {
          keys.emplace_back(key);
        }
      }
    }
    return keys;
  }

public:
  QueueManager() { std::cout << "QueueManager ctor\n"; };
  ~QueueManager() {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_map_.clear();
    std::cout << "QueueManager dtor\n";
  };

  QueueManager(QueueManager const&)   = delete;
  void operator=(QueueManager const&) = delete;

  static std::shared_ptr<buffer_queue<T, size>> get_queue(std::string key) { return getInstance().get_queue_(key); }

  static std::shared_ptr<buffer_queue<T, size>> get_queue_if_exists(std::string key) {
    return getInstance().get_queue_if_exists_(key);
  }

  static bool remove_queue(std::string key) { return getInstance().remove_queue_(key); }

  static uint64_t get_src_uuid() { return getInstance().get_src_uuid_(); }

  static uint64_t get_sink_uuid() { return getInstance().get_sink_uuid_(); }

  static std::vector<std::string> get_keys(std::string type = std::string("")) { return getInstance().get_keys_(type); }
};

class QUEUEUTIL_EXPORT QueueManagerRawBuffer {
public:
  static std::vector<std::string>                       get_keys(std::string key = std::string(""));
  static std::shared_ptr<buffer_queue<RawBuffer, 1024>> get_queue(std::string key);
  static std::shared_ptr<buffer_queue<RawBuffer, 1024>> get_queue_if_exists(std::string key);
  static bool                                           remove_queue(std::string key);
};

void QUEUEUTIL_EXPORT use_queue_manager();

#endif // queue_manager_h
