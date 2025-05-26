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

  auto queue  = QueueManager<RawBuffer, 2>::get_queue(pub_key);
  auto queue1 = QueueManager<RawBuffer, 2>::get_queue(sub_key);

  auto keys = QueueManager<RawBuffer, 2>::get_keys();

  for (auto&& key : keys) {
    std::cout << key << "\n";
  }

  auto keys1 = QueueManager<RawBuffer, 2>::get_keys("sub");

  for (auto&& key : keys1) {
    std::cout << key << "\n";
  }
}
