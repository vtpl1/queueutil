// *****************************************************
//    Copyright 2023 Videonetics Technology Pvt Ltd
// *****************************************************

#include "raw_buffer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>

constexpr int MEMORY_ALIGNMENT = 32;

RawBuffer::RawBuffer(size_t initial_size) : RawBuffer(nullptr, initial_size) {}

RawBuffer::RawBuffer(const uint8_t* data_in, size_t valid_data_size) { assign(data_in, valid_data_size); }

RawBuffer::~RawBuffer() { release(); }

RawBuffer::RawBuffer(const RawBuffer& other) { assign(other.data(), other.size()); }

RawBuffer& RawBuffer::operator=(const RawBuffer& other) {
  if (this != &other) {
    assign(other.data(), other.size());
  }
  return *this;
}

RawBuffer::RawBuffer(RawBuffer&& other) noexcept
    : _buffer(std::move(other._buffer)), _buffer_capacity(other._buffer_capacity), _buffer_size(other._buffer_size) {
  other._buffer_capacity = 0;
  other._buffer_size     = 0;
}

RawBuffer& RawBuffer::operator=(RawBuffer&& other) noexcept {
  if (this != &other) {
    release(); // Free existing memory before stealing
    _buffer          = std::move(other._buffer);
    _buffer_capacity = other._buffer_capacity;
    _buffer_size     = other._buffer_size;

    other._buffer_capacity = 0;
    other._buffer_size     = 0;
  }
  return *this;
}

void RawBuffer::resize(size_t new_size) {
  if (new_size > _buffer_capacity) {
    release();
    size_t aligned_size = (new_size + MEMORY_ALIGNMENT - 1) & ~(MEMORY_ALIGNMENT - 1);
    _buffer             = std::make_unique<uint8_t[]>(aligned_size);
    _buffer_capacity    = aligned_size;
    RawBufferMemoryAuditor::instance().AddToTotalMemory(_buffer_capacity);
  }
  _buffer_size = new_size;
}

void RawBuffer::resizeAndPreserve(size_t new_size) {
  if (new_size <= _buffer_capacity) {
    _buffer_size = new_size;
    return;
  }

  auto                       new_capacity = (new_size + MEMORY_ALIGNMENT - 1) & ~(MEMORY_ALIGNMENT - 1);
  std::unique_ptr<uint8_t[]> new_buffer   = std::make_unique<uint8_t[]>(new_capacity);

  if (_buffer && _buffer_size > 0) {
    std::memcpy(new_buffer.get(), _buffer.get(), _buffer_size);
  }

  release(); // remove old memory from auditor
  _buffer          = std::move(new_buffer);
  _buffer_capacity = new_capacity;
  RawBufferMemoryAuditor::instance().AddToTotalMemory(_buffer_capacity);

  _buffer_size = new_size;
}

void RawBuffer::release() {
  if (_buffer) {
    RawBufferMemoryAuditor::instance().RemoveFromTotalMemory(_buffer_capacity);
    // _buffer.reset();
    _buffer = nullptr;
  }
  _buffer_capacity = 0;
  _buffer_size     = 0;
}

void RawBuffer::assign(const uint8_t* data_in, size_t valid_data_size) {
  resize(valid_data_size);
  if (data_in) {
    std::memcpy(_buffer.get(), data_in, valid_data_size);
  }
}

uint8_t* RawBuffer::data() const { return (_buffer_size > 0) ? _buffer.get() : nullptr; }

uint8_t* RawBuffer::end() const { return (_buffer_size > 0) ? _buffer.get() + _buffer_size : nullptr; }

uint8_t* RawBuffer::data(size_t offset) const {
  if (offset >= _buffer_size) {
    throw std::runtime_error("Offset out of bounds");
  }
  return _buffer.get() + offset;
}

void RawBuffer::append(const uint8_t* data_in, size_t data_size) {
  if (data_size == 0)
    return;
  size_t old_size = _buffer_size;
  resizeAndPreserve(_buffer_size + data_size);
  std::memcpy(_buffer.get() + old_size, data_in, data_size);
}

void RawBuffer::take(const uint8_t* data_in, size_t valid_data_size) { assign(data_in, valid_data_size); }

void RawBuffer::clear() { _buffer_size = 0; }

size_t RawBuffer::size() const { return _buffer_size; }
size_t RawBuffer::capacity() const { return _buffer_capacity; }

RawBufferMemoryAuditor& RawBufferMemoryAuditor::instance() {
  static RawBufferMemoryAuditor s_instance;
  return s_instance;
}

unsigned long long RawBufferMemoryAuditor::GetTotalMemory() const { return total_memory.load(); }

void RawBufferMemoryAuditor::AddToTotalMemory(unsigned long long total_size) {
  total_memory.fetch_add(total_size);
  std::cout << "+++ RawBufferMemoryAuditor Final size " << GetTotalMemory() << std::endl;
}

void RawBufferMemoryAuditor::RemoveFromTotalMemory(unsigned long long total_size) {
  // if (total_memory.load() > 0) {
  total_memory.fetch_sub(total_size);
  std::cout << "--- RawBufferMemoryAuditor Final size " << GetTotalMemory() << std::endl;
  // }
}