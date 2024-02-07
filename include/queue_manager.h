// *****************************************************
//    Copyright 2024 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef queue_manager_h
#define queue_manager_h

#include "buffer_queue.h"

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
    try {
      std::lock_guard<std::mutex> lock(mutex_);
      if (queue_map_.find(key) == queue_map_.end()) {
        queue_map_.emplace(key, std::make_shared<buffer_queue<T, size>>());
      }

      return queue_map_.at(key);
    } catch (const std::runtime_error& e) {
      std::cout << e.what() << '\n';
    }
    return nullptr;
  }

  bool remove_queue_(std::string key) {
    try {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_map_.erase(key);

      return true;
    } catch (const std::runtime_error& e) {
      std::cout << e.what() << '\n';
    }
    return false;
  }

  uint64_t get_src_uuid_() { return 0; }

  uint64_t get_sink_uuid_() { return uuid_generator_++; }

  std::vector<std::string> get_keys_(std::string type) {
    std::vector<std::string> keys;
    try {
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
    } catch (const std::runtime_error& e) {
      std::cout << e.what() << '\n';
    }

    return keys;
  }

public:
  QueueManager() { std::cout << "QueueManager ctor\n"; };
  ~QueueManager() { std::cout << "QueueManager dtor\n"; };

  QueueManager(QueueManager const&)   = delete;
  void operator=(QueueManager const&) = delete;

  static std::shared_ptr<buffer_queue<T, size>> get_queue(std::string key) { return getInstance().get_queue(key); }

  static bool remove_queue(std::string key) { return getInstance().remove_queue_(key); }

  static uint64_t get_src_uuid() { return getInstance().get_src_uuid_(); }

  static uint64_t get_sink_uuid() { return getInstance().get_sink_uuid_(); }

  static std::vector<std::string> get_keys(std::string type = std::string("")) { return getInstance().get_keys_(type); }
};

#endif // queue_manager_h
