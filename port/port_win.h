// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//

#ifndef STORAGE_LEVELDB_PORT_PORT_WINDOWS_H_
#define STORAGE_LEVELDB_PORT_PORT_WINDOWS_H_

#include <string>
#include <windows.h>

#ifdef SNAPPY
#include <snappy.h>
#endif

#include "port/atomic_pointer.h"

namespace leveldb {
namespace port {

static const bool kLittleEndian = true;

// ------------------ Threading -------------------

class Mutex {
 public:
  Mutex();
  ~Mutex();

  void Lock();
  void Unlock();
  void AssertHeld() {}
 private:
  CRITICAL_SECTION cs;
};

class CondVar {
 public:
  explicit CondVar(Mutex* mu);
  ~CondVar();

  void Wait();
  void Signal();
  void SignalAll();
  
private:
  Mutex* mu;
  
  Mutex waiting_mutex;
  long waiting;
  
  HANDLE sem1;
  HANDLE sem2;
};

typedef INIT_ONCE  OnceType;
#define LEVELDB_ONCE_INIT INIT_ONCE_STATIC_INIT 
extern void InitOnce(port::OnceType*, void (*initializer)());

inline bool Snappy_Compress(const char* input, size_t length,
                            ::std::string* output) {
#ifdef SNAPPY
  output->resize(snappy::MaxCompressedLength(length));
  size_t outlen;
  snappy::RawCompress(input, length, &(*output)[0], &outlen);
  output->resize(outlen);
  return true;
#endif

  return false;
}

inline bool Snappy_GetUncompressedLength(const char* input, size_t length,
                                         size_t* result) {
#ifdef SNAPPY
  return snappy::GetUncompressedLength(input, length, result);
#else
  return false;
#endif
}

inline bool Snappy_Uncompress(const char* input, size_t length,
                              char* output) {
#ifdef SNAPPY
  return snappy::RawUncompress(input, length, output);
#else
  return false;
#endif
}

inline bool GetHeapProfile(void (*func)(void*, const char*, int), void* arg) {
  return false;
}

uint32_t AcceleratedCRC32C(uint32_t crc, const char* buf, size_t size);

}  // namespace port
}  // namespace leveldb

#endif  // STORAGE_LEVELDB_PORT_PORT_WINDOWS_H_
