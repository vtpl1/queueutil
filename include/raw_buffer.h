// *****************************************************
//    Copyright 2023 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef raw_buffer_h
#define raw_buffer_h
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <queueutil_export.h>
#include <vector>

class QUEUEUTIL_EXPORT RawBuffer {
private:
  std::unique_ptr<uint8_t[]> _buffer;
  size_t                     _buffer_capacity{0};
  size_t                     _buffer_size{0};
  bool                       _resize_always{false};
  void                       assign(const uint8_t* data_in, size_t valid_data_size);

public:
  RawBuffer(bool resize_always = false);
  RawBuffer(const uint8_t* data_in, size_t valid_data_size, bool resize_always = false);
  ~RawBuffer();
  RawBuffer(const RawBuffer& o);
  RawBuffer& operator=(const RawBuffer& o);
  uint8_t*   data() const;
  uint8_t*   end() const;
  uint8_t*   data(size_t offset) const;
  void       resize(size_t new_size);
  size_t     size() const;
  size_t     capacity() const;
  void       append(const uint8_t* data_in, size_t data_size);
  void       take(const uint8_t* data_in, size_t valid_data_size);
};

class QUEUEUTIL_EXPORT RawBufferMemoryAuditor {
private:
  RawBufferMemoryAuditor();
  ~RawBufferMemoryAuditor() = default;
  std::atomic_ullong total_memory{0};

public:
  static RawBufferMemoryAuditor& instance();
  RawBufferMemoryAuditor(const RawBufferMemoryAuditor&)            = delete;
  RawBufferMemoryAuditor& operator=(const RawBufferMemoryAuditor&) = delete;
  unsigned long long GetTotalMemory();
  void AddToTotalMemory(unsigned long long total_size);
  void RemoveFromTotalMemory(unsigned long long total_size);
};

void QUEUEUTIL_EXPORT use_raw_buffer();

#endif // raw_buffer_h
