// *****************************************************
//    Copyright 2023 Videonetics Technology Pvt Ltd
// *****************************************************

#include "queueutil.h"
#include "buffer_queue.h"
#include "queue_manager.h"
#include "raw_buffer.h"
#include "version.h"
#include <string>

auto get_version() -> std::string { return std::string{queueutil_VERSION}; }
void use_buffer_queue() {}
void use_raw_buffer() {}
void use_queue_manager() {}