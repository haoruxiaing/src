#ifndef NET_BUFFER_H
#define NET_BUFFER_H

#include <algorithm>
#include <vector>
#include <string>

#include <assert.h>
#include <string.h>

/// A buffer class modeled after muduo
///
/// @code
/// +------------------+------------------+
/// |  readable bytes  |  writable bytes  |
/// |                  |     (CONTENT)    |
/// +------------------+------------------+
/// |                  |                  |
/// 0 <= readerIndex <= writerIndex <= size
/// @endcode
class Buffer {
 public:
  static const size_t kInitialSize = 1024;

  Buffer()
    : buffer_(kInitialSize),
      reader_index_(0),
      writer_index_(0) {
    assert(ReadableBytes() == 0);
    assert(WritableBytes() == kInitialSize);
  }

  // default copy-ctor, dtor and assignment are fine

  void Swap(Buffer& rhs) {
    buffer_.swap(rhs.buffer_);
    std::swap(reader_index_, rhs.reader_index_);
    std::swap(writer_index_, rhs.writer_index_);
  }

  size_t ReadableBytes() const { return writer_index_ - reader_index_; }

  size_t WritableBytes() const { return buffer_.size() - writer_index_; }

  size_t PrependableBytes() const { return reader_index_; }
  
  const char* Peek() const { return Begin() + reader_index_; }

  // retrieve returns void, to prevent
  // string str(retrieve(ReadableBytes()), ReadableBytes());
  // the evaluation of two functions are unspecified
  void Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    if (len < ReadableBytes()) {
      reader_index_ += len;
    } else {
      RetrieveAll();
    }
  }

  void RetrieveUntil(const char* end) {
    assert(Peek() <= end);
    assert(end <= BeginWrite());
    Retrieve(end - Peek());
  }

  void RetrieveAll() {
    reader_index_ = 0;
    writer_index_ = 0;
  }

  std::string RetrieveAllAsString() {
    return RetrieveAsString(ReadableBytes());
  }

  std::string RetrieveAsString(size_t len) {
    assert(len <= ReadableBytes());
    std::string result(Peek(), len);
    Retrieve(len);
    return result;
  }
  
  void Append(const std::string& str) {
    Append(str.c_str(), str.size());
  }

  void Append(const char* data, size_t len) {
    EnsureWritableBytes(len);
    std::copy(data, data+len, BeginWrite());
    HasWritten(len);
  }

  void Append(const void* data, size_t len) {
    Append(static_cast<const char*>(data), len);
  }

  void EnsureWritableBytes(size_t len) {
    if (WritableBytes() < len) {
      MakeSpace(len);
    }
    assert(WritableBytes() >= len);
  }

  char* BeginWrite() { return Begin() + writer_index_; }

  const char* BeginWrite() const { return Begin() + writer_index_; }

  void HasWritten(size_t len) { writer_index_ += len; }

  ssize_t ReadFd(int fd);
  
 private:
  char* Begin() { return &*buffer_.begin(); }

  const char* Begin() const { return &*buffer_.begin(); }

  void MakeSpace(size_t len) {
    if (WritableBytes() + PrependableBytes() < len) {
      buffer_.resize(writer_index_ + len);
    } else {
      // move readable data to the front, make space inside buffer
      size_t readable = ReadableBytes();
      std::copy(Begin()+reader_index_,
                Begin()+writer_index_,
                Begin());
      reader_index_ = 0;
      writer_index_ = reader_index_ + readable;
      assert(readable == ReadableBytes());
    }
  }

  std::vector<char> buffer_;
  size_t reader_index_;
  size_t writer_index_;
};

#endif

