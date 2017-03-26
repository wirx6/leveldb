// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//

#include "port/port_win.h"

#include <windows.h>

namespace leveldb {
namespace port {

Mutex::Mutex() {
  InitializeCriticalSection(&cs);    
}
    
Mutex::~Mutex() {
  DeleteCriticalSection(&cs);
}

void Mutex::Lock() {
  EnterCriticalSection(&cs);
}

void Mutex::Unlock() {
  LeaveCriticalSection(&cs);
}

CondVar::CondVar(Mutex* m) :
  mu(m) {
  sem1 = CreateSemaphore(NULL, 0, 10000, NULL);
  sem2 = CreateSemaphore(NULL, 0, 10000, NULL);
}

CondVar::~CondVar() {
  CloseHandle(sem1);
  CloseHandle(sem2);
}
  
void CondVar::Wait() {
  waiting_mutex.Lock();
  waiting++;
  waiting_mutex.Unlock();

  mu->Unlock();

  WaitForSingleObject(sem1, INFINITE);
  ReleaseSemaphore(sem2, 1, NULL);
  mu->Lock();
}

void CondVar::Signal() {
  waiting_mutex.Lock();
  if (waiting > 0) {
    waiting--;    
    ReleaseSemaphore(sem1, 1, NULL);
    WaitForSingleObject(sem2, INFINITE);
  }
  waiting_mutex.Unlock();
}

void CondVar::SignalAll() {
  waiting_mutex.Lock();
  for(long i = 0; i < waiting; i++) {
    ReleaseSemaphore(sem1, 1, NULL);
    while(waiting > 0) {
      waiting--;
      WaitForSingleObject(sem2, INFINITE);
    }
  }
  waiting_mutex.Unlock();
}

typedef void (*ini_fun_t)();

BOOL InitOnce_Impl(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *lpContext) {
    ini_fun_t initializer = reinterpret_cast<ini_fun_t>(Parameter);
    initializer();
}

void InitOnce(port::OnceType* once, void (*initializer)()) {
    InitOnceExecuteOnce(once, &InitOnce_Impl, reinterpret_cast<void*>(initializer), NULL);
}

uint32_t AcceleratedCRC32C(uint32_t crc, const char* buf, size_t size) {
    return 0;
}

}  // namespace port
}  // namespace leveldb
