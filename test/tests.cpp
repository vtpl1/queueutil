// *****************************************************
//    Copyright 2024 Videonetics Technology Pvt Ltd
// *****************************************************

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <thread>
#include <vector>
#include <memory>

#include "queue_manager.h"

void func() { /*auto queue = QueueManager<int, 2>::getInstance().get_queue("key");*/ }

TEST_CASE("queue_manager", "[queue]") {
  const uint64_t SIZE = 100000;


  std::vector<std::unique_ptr<std::thread>> threads;

  for (int i = 0; i < SIZE; i++) {
    threads.emplace_back(std::make_unique<std::thread>(func));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  for (auto& thread : threads) {
    if (thread->joinable()) {
      thread->join();
    }
  }
}
