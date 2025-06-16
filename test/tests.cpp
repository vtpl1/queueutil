// *****************************************************
//    Copyright 2024 Videonetics Technology Pvt Ltd
// *****************************************************

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <memory>
#include <vector>

#include "queue_manager.h"

TEST_CASE("queue_manager", "[queue]") {

  std::string pub_key = "pub[siteId:0,channelId:0,appId:-1,liveOrRec:1,streamType:0,startTs:-1]uuid:0";
  std::string sub_key = "sub[siteId:0,channelId:0,appId:-1,liveOrRec:1,streamType:0,startTs:-1]uuid:0";

  {
    auto queue  = QueueManager<RawBuffer, 2>::get_queue(pub_key);
    auto queue1 = QueueManager<RawBuffer, 2>::get_queue(sub_key);

    auto keys = QueueManager<RawBuffer, 2>::get_keys();

    for (auto&& key : keys) {
      std::cout << key << "\n";
      QueueManager<RawBuffer, 2>::remove_queue(key);
    }
  }

  REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == 0);
}

TEST_CASE("test_basic_allocation", "[queue]") {
  RawBuffer buf(1024);
  REQUIRE(buf.size() == 1024);
  REQUIRE(buf.capacity() >= 1024);
}

TEST_CASE("test_assignment_and_copy", "[queue]") {
  uint8_t   data[] = {1, 2, 3, 4};
  RawBuffer buf1(data, 4);
  RawBuffer buf2 = buf1;
  REQUIRE(std::memcmp(buf1.data(), buf2.data(), 4) == 0);
}

TEST_CASE("test_move_semantics", "[queue]") {
  RawBuffer buf1(512);
  uint8_t*  original_data = buf1.data();
  RawBuffer buf2          = std::move(buf1);
  REQUIRE(buf2.data() == original_data);
  REQUIRE(buf1.data() == nullptr);
}

TEST_CASE("test_append_and_resize", "[queue]") {
  RawBuffer buf;
  uint8_t   part1[] = {1, 2, 3};
  uint8_t   part2[] = {4, 5};
  buf.take(part1, 3);
  buf.append(part2, 2);
  REQUIRE(buf.size() == 5);
  REQUIRE(std::memcmp(buf.data(), "\x01\x02\x03\x04\x05", 5) == 0);
}

TEST_CASE("test_memory_auditor", "[queue]") {
  auto initial_mem = RawBufferMemoryAuditor::instance().GetTotalMemory();
  {
    RawBuffer b(1024);
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() >= initial_mem + 1024);
  }
  REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem);
}

TEST_CASE("test_reset_raw_buffer", "[queue]") {
  auto initial_mem = RawBufferMemoryAuditor::instance().GetTotalMemory();
  {
    std::unique_ptr<RawBuffer> b(new RawBuffer(1024));
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1024);
    {
      std::unique_ptr<RawBuffer> b1(new RawBuffer(1025));
      *b = *b1;
    }
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1056);
    {
      std::unique_ptr<RawBuffer> b1(new RawBuffer(1025));
      *b = *b1;
    }
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1056);
    {
      auto b1 = std::make_unique<RawBuffer>(1025);
      *b      = *b1;
    }
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1056);
    {
      std::unique_ptr<RawBuffer> b1(new RawBuffer(1024));
      *b = *b1;
    }
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1056);
    {
      auto b1 = std::make_unique<RawBuffer>(1023);
      *b      = *b1;
    }
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1056);
    {
      auto b1 = std::make_unique<RawBuffer>(1023);
      *b      = *b1;
    }
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1056);
    {
      auto b1 = std::make_unique<RawBuffer>(1023);
      *b      = *b1;
    }
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1056);
    {
      auto b1 = std::make_unique<RawBuffer>(1023);
      *b      = *b1;
    }
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1056);
    {
      auto b1 = std::make_unique<RawBuffer>(1023);
      *b      = *b1;
    }
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1056);
    {
      auto b1 = std::make_unique<RawBuffer>(1023);
      *b      = *b1;
    }
    REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem + 1024);
  }
  REQUIRE(RawBufferMemoryAuditor::instance().GetTotalMemory() == initial_mem);
}
