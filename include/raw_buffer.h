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

constexpr int KB_300 = (300 * 1024);

class QUEUEUTIL_EXPORT RawBuffer {
private:
  std::unique_ptr<uint8_t[]> buffer_;
  size_t                     buffer_capacity_{0};
  size_t                     buffer_size_{0};
  size_t                     initial_capacity_{0};
  int64_t                    plus_minus_{0};
  void                       assign(const uint8_t* data_in, size_t valid_data_size);
  void                       release();

public:
  RawBuffer(size_t initial_size = 0);
  RawBuffer(const uint8_t* data_in, size_t valid_data_size);
  ~RawBuffer();

  RawBuffer(const RawBuffer& other);
  RawBuffer& operator=(const RawBuffer& other);

  RawBuffer(RawBuffer&& other) noexcept;
  RawBuffer& operator=(RawBuffer&& other) noexcept;

  uint8_t* data() const;
  uint8_t* end() const;
  uint8_t* data(size_t offset) const;

  void   resize(size_t new_size);
  void   resizeAndPreserve(size_t new_size);
  void   clear();
  size_t size() const;
  size_t capacity() const;

  void append(const uint8_t* data_in, size_t data_size);
  void take(const uint8_t* data_in, size_t valid_data_size);
};

class QUEUEUTIL_EXPORT RawBufferMemoryAuditor {
private:
  std::atomic_ullong total_memory{0};
  RawBufferMemoryAuditor() = default;

public:
  static RawBufferMemoryAuditor& instance();

  RawBufferMemoryAuditor(const RawBufferMemoryAuditor&)            = delete;
  RawBufferMemoryAuditor& operator=(const RawBufferMemoryAuditor&) = delete;

  unsigned long long GetTotalMemory() const;
  void               AddToTotalMemory(unsigned long long total_size);
  void               RemoveFromTotalMemory(unsigned long long total_size);
};

void QUEUEUTIL_EXPORT use_raw_buffer();

#endif // raw_buffer_h
