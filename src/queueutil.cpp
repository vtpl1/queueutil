// *****************************************************
//    Copyright 2023 Videonetics Technology Pvt Ltd
// *****************************************************

#include "buffer_queue.h"
#include "version.h"
#include <queueutil.h>

std::string get_version() { return std::string{queueutil_VERSION}; }