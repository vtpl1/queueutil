#pragma once
#ifndef buffer_queue_h
#define buffer_queue_h

#include <condition_variable>
#include <deque>
#include <mutex>
#include <queue>
#include <queueutil_export.h>
#include <stdexcept>
#include <stdint.h>
#include <string>

template <typename T, typename Container = std::deque<T>> class iterable_queue : public std::queue<T, Container> {
public:
  typedef typename Container::iterator       iterator;
  typedef typename Container::const_iterator const_iterator;

  iterator       begin() { return this->c.begin(); }
  iterator       end() { return this->c.end(); }
  const_iterator begin() const { return this->c.begin(); }
  const_iterator end() const { return this->c.end(); }
};

template <typename T, uint32_t SIZE> class QUEUEUTIL_EXPORT buffer_queue {
  static_assert(SIZE && !(SIZE & (SIZE - 1)), "SIZE must be a power of 2");

private:
  // std::vector<T> _block;
  std::vector<T*>         _block;
  std::mutex              _write_q_mutex;
  std::condition_variable _write_q_available;
  std::mutex              _read_q_mutex;
  std::condition_variable _read_q_available;
  iterable_queue<T*>      _writable_q;
  iterable_queue<T*>      _readable_q;
  std::mutex              should_block_mutex_;
  bool                    should_block_{false};
  std::mutex              associated_bool_mutex_;
  bool                    associated_bool_{false};
  std::mutex              associated_int_mutex_;
  int64_t                 associated_int_{0};
  std::mutex              associated_int2_mutex_;
  int64_t                 associated_int2_{0};
  std::mutex              associated_data_mutex_;
  std::string             associated_data_{""};

public:
  buffer_queue() {
    // _block.resize(SIZE);
    for (int i = 0; i < SIZE; i++) {
      //  _writable_q.push(&_block[i]);
      _block.push_back(new T());
      _writable_q.push(_block[i]);
    }
  }

  buffer_queue(buffer_queue const& other) = delete;

  ~buffer_queue() {
    reset();
    for (int i = 0; i < SIZE; i++) {
      delete _block[i];
    }
  }

  void reset() {
    // _write_reset();
    // _read_reset();
    // T* t = nullptr;
    // while ((t = get_readble_buffer()) != nullptr) {
    //   commit_read(t);
    // }
    std::scoped_lock lock{_write_q_mutex, _read_q_mutex};

    {
      std::queue<T*> empty_queue;
      _writable_q.swap(empty_queue);
    }
    {
      std::queue<T*> empty_queue;
      _readable_q.swap(empty_queue);
    }
    for (int i = 0; i < _block.size(); i++) {
      _writable_q.push(_block[i]);
    }
  }

  T* get_writable_buffer(int wait_milli_sec = 10) {
    T*                           t = nullptr;
    std::unique_lock<std::mutex> lock_write(_write_q_mutex);
    if (_write_q_available.wait_for(lock_write, std::chrono::milliseconds(wait_milli_sec),
                                    [this] { return (!_writable_q.empty()); })) {
      t = _writable_q.front();
      _writable_q.pop();
    }
    return t;
  }

  void commit_write(T* buf_ptr) {
    std::lock_guard<std::mutex> lock_read(_read_q_mutex);
    if (nullptr == buf_ptr) {
      return;
    }
    bool is_my_child = false;
    for (int i = 0; i < SIZE; i++) {
      if (_block[i] == buf_ptr) {
        is_my_child = true;
      }
    }
    if (!is_my_child) {
      std::runtime_error("The buffer does not belong to this queue");
    }
    bool is_already_commited = false;
    for (auto it = _readable_q.begin(); it != _readable_q.end(); ++it) {
      if (*it == buf_ptr) {
        is_already_commited = true;
      }
    }

    if (is_already_commited == false) {
      _readable_q.push(buf_ptr);
    }
    _read_q_available.notify_one();
  }

  T* get_readble_buffer(int wait_milli_sec = 10) {
    T*                           t = nullptr;
    std::unique_lock<std::mutex> lock_read(_read_q_mutex);
    if (_read_q_available.wait_for(lock_read, std::chrono::milliseconds(wait_milli_sec),
                                   [this] { return (!_readable_q.empty()); })) {
      t = _readable_q.front();
      _readable_q.pop();
    }
    return t;
  }

  void commit_read(T* buf_ptr) {
    std::lock_guard<std::mutex> lock_write(_write_q_mutex);
    if (nullptr == buf_ptr) {
      return;
    }
    bool is_my_child = false;
    for (int i = 0; i < SIZE; i++) {
      if (_block[i] == buf_ptr) {
        is_my_child = true;
      }
    }
    if (!is_my_child) {
      std::runtime_error("The buffer does not belong to this queue");
    }
    bool is_already_commited = false;
    for (auto it = _writable_q.begin(); it != _writable_q.end(); ++it) {
      if (*it == buf_ptr) {
        is_already_commited = true;
      }
    }
    if (is_already_commited == false) {
      _writable_q.push(buf_ptr);
    }
    _write_q_available.notify_one();
  }

  int write_buffer_size() {
    std::lock_guard<std::mutex> lock_write(_write_q_mutex);
    return _writable_q.size();
  }

  int read_buffer_size() {
    std::lock_guard<std::mutex> lock_read(_read_q_mutex);
    return _readable_q.size();
  }

  int get_size() { return read_buffer_size(); }

  int is_sufficient_space_available() { return read_buffer_size() < SIZE / 2; }

  bool get_associated_bool() {
    std::lock_guard<std::mutex> lock(associated_bool_mutex_);
    return associated_bool_;
  }
  void set_associated_bool() {
    std::lock_guard<std::mutex> lock(associated_bool_mutex_);
    associated_bool_ = true;
  }
  void reset_associated_bool() {
    std::lock_guard<std::mutex> lock(associated_bool_mutex_);
    associated_bool_ = false;
  }

  bool get_should_block() {
    std::lock_guard<std::mutex> lock(should_block_mutex_);
    return should_block_;
  }
  void set_should_block() {
    std::lock_guard<std::mutex> lock(should_block_mutex_);
    should_block_ = true;
  }
  void reset_should_block() {
    std::lock_guard<std::mutex> lock(should_block_mutex_);
    should_block_ = false;
  }

  std::string get_associated_data() {
    std::lock_guard<std::mutex> lock(associated_data_mutex_);
    return associated_data_;
  }
  void set_associated_data(const std::string& data) {
    std::lock_guard<std::mutex> lock(associated_data_mutex_);
    associated_data_ = data;
  }
  void reset_associated_data() {
    std::lock_guard<std::mutex> lock(associated_data_mutex_);
    associated_data_ = "";
  }

  int64_t get_associated_int() {
    std::lock_guard<std::mutex> lock(associated_int_mutex_);
    return associated_int_;
  }
  void set_associated_int(int64_t data) {
    std::lock_guard<std::mutex> lock(associated_int_mutex_);
    associated_int_ = data;
  }
  void increment_associated_int() {
    std::lock_guard<std::mutex> lock(associated_int_mutex_);
    associated_int_++;
  }
  void reset_associated_int() {
    std::lock_guard<std::mutex> lock(associated_int_mutex_);
    associated_int_ = 0;
  }

  int64_t get_associated_int1() { return get_associated_int(); }
  void    set_associated_int1(int64_t data) { set_associated_int(data); }
  void    reset_associated_int1() { reset_associated_int(); }
  void    increment_associated_int1() { increment_associated_int(); }

  int64_t get_associated_int2() {
    std::lock_guard<std::mutex> lock(associated_int2_mutex_);
    return associated_int2_;
  }
  void set_associated_int2(int64_t data) {
    std::lock_guard<std::mutex> lock(associated_int2_mutex_);
    associated_int2_ = data;
  }
  void increment_associated_int2() {
    std::lock_guard<std::mutex> lock(associated_int2_mutex_);
    associated_int2_++;
  }
  void reset_associated_int2() {
    std::lock_guard<std::mutex> lock(associated_int2_mutex_);
    associated_int2_ = 0;
  }
};

void QUEUEUTIL_EXPORT use_buffer_queue();

#endif
