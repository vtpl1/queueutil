// *****************************************************
//    Copyright 2023 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef raw_buffer_h
#define raw_buffer_h
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
  void                       assign(uint8_t* data_in, size_t valid_data_size);

public:
  RawBuffer(bool resize_always = false);
  RawBuffer(uint8_t* data_in, size_t valid_data_size, bool resize_always = false);
  ~RawBuffer() = default;
  RawBuffer(const RawBuffer& o);
  RawBuffer& operator=(const RawBuffer& o);
  uint8_t*   data() const;
  uint8_t*   end() const;
  uint8_t*   data(size_t offset) const;
  void       resize(size_t new_size);
  size_t     size() const;
  size_t     capacity() const;
};

#endif // raw_buffer_h
