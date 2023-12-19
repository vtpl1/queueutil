// *****************************************************
//    Copyright 2023 Videonetics Technology Pvt Ltd
// *****************************************************

#include "raw_buffer.h"

#include <cstring>
#include <stdexcept>

constexpr int MEMORY_ALIGNMENT = 32;

RawBuffer::RawBuffer(bool resize_always) : RawBuffer(nullptr, 0, resize_always) {}

RawBuffer::RawBuffer(uint8_t* data_in, size_t valid_data_size, bool resize_always) : _resize_always(resize_always) {
  assign(data_in, valid_data_size);
}

void RawBuffer::resize(size_t new_size) {
  if (new_size > _buffer_capacity) {
    _buffer_capacity = new_size;
    _buffer_capacity = (_buffer_capacity % MEMORY_ALIGNMENT == 0)
                           ? _buffer_capacity
                           : ((_buffer_capacity / MEMORY_ALIGNMENT) * MEMORY_ALIGNMENT) + MEMORY_ALIGNMENT;
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays,modernize-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays)
    _buffer = std::make_unique<uint8_t[]>(_buffer_capacity);
  }
  _buffer_size = new_size;
}

RawBuffer::RawBuffer(const RawBuffer& other) { assign(other.data(), other.size()); }

RawBuffer& RawBuffer::operator=(const RawBuffer& other) {
  if (this == &other) {
    return *this;
  }
  assign(other.data(), other.size());
  return *this;
}

uint8_t* RawBuffer::data() const {
  if (_buffer_size == 0) {
    return nullptr;
  }
  return _buffer.get();
}

uint8_t* RawBuffer::data(size_t offset) const {
  if (_buffer_size == 0) {
    return nullptr;
  }
  if (offset >= size()) {
    throw std::runtime_error("reading outside of memory boundary");
  }
  return (_buffer.get() + offset);
}

uint8_t* RawBuffer::end() const {
  if (_buffer_size == 0) {
    return nullptr;
  }
  return (_buffer.get() + _buffer_size);
}

size_t RawBuffer::size() const { return _buffer_size; }

size_t RawBuffer::capacity() const { return _buffer_capacity; }

void RawBuffer::assign(uint8_t* data_in, size_t valid_data_size) {
  resize(valid_data_size);
  std::memcpy(_buffer.get(), data_in, valid_data_size);
}