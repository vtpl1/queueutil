// *****************************************************
//    Copyright 2024 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef queue_manager_h
#define queue_manager_h

#include "buffer_queue.h"

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

public:
  QueueManager() { std::cout << "QueueManager ctor\n"; };
  ~QueueManager() { std::cout << "QueueManager dtor\n"; };

  static QueueManager& getInstance() {
    static QueueManager instance;
    return instance;
  }

  QueueManager(QueueManager const&)   = delete;
  void operator=(QueueManager const&) = delete;

  std::shared_ptr<buffer_queue<T, size>> get_queue(std::string key) {
    try {
      std::lock_guard<std::mutex> lock(mutex_);
      if (queue_map_.find(key) == queue_map_.end()) {
        queue_map_.emplace(key, std::make_shared<buffer_queue<T, size>>());
      }

      return queue_map_.at(key);
    } catch (const std::runtime_error& e) {
      // std::cerr << e.what() << '\n';
    }
    return nullptr;
  }

  bool remove_queue(std::string key) {
    try {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_map_.erase(key);

      return true;
    } catch (const std::runtime_error& e) {
      // std::cerr << e.what() << '\n';
    }
    return false;
  }

  uint64_t get_src_uuid() { return 0; }

  uint64_t get_sink_uuid() { return uuid_generator_++; }
};

#endif // queue_manager_h
