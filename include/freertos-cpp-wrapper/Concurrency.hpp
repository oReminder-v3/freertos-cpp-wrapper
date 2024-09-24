//
// Created by robcholz on 5/4/24.
//
#pragma once

#ifndef FREERTOS_CPP_WRAPPER_INCLUDE_CONCURRENCY_HPP
#define FREERTOS_CPP_WRAPPER_INCLUDE_CONCURRENCY_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "internal/Utils.hpp"

namespace freertos {
template<typename T>
class ScopeLock {
public:
  explicit ScopeLock(T &mutex) : mutex(&mutex) { mutex.lock(); }

  ~ScopeLock() { mutex->unlock(); }

  ScopeLock(const ScopeLock &) = delete;
  ScopeLock(const ScopeLock &&) = delete;
  ScopeLock operator=(const ScopeLock &) = delete;
  ScopeLock operator=(const ScopeLock &&) = delete;

private:
  T *mutex;
};

template<TickType_t DEFAULT_TICK = 10>
class Mutex {
public:
  Mutex() { object = xSemaphoreCreateMutex(); }

  ~Mutex() { vSemaphoreDelete(object); }

  FREERTOS_ALWAYS_INLINE void lock() {
	xSemaphoreTake(object, (TickType_t) DEFAULT_TICK);
  }

  FREERTOS_ALWAYS_INLINE bool tryLock() {
	return xSemaphoreTake(object, (TickType_t) DEFAULT_TICK) == pdTRUE;
  }

  FREERTOS_ALWAYS_INLINE void unlock() { xSemaphoreGive(object); }

  Mutex(const Mutex &) = delete;
  Mutex(const Mutex &&) = delete;
  Mutex operator=(const Mutex &) = delete;
  Mutex operator=(const Mutex &&) = delete;

private:
  SemaphoreHandle_t object;
};
}// namespace freertos

#endif// FREERTOS_CPP_WRAPPER_INCLUDE_CONCURRENCY_HPP
