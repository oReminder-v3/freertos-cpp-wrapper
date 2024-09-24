//
// Created by robcholz on 5/10/24.
//
#pragma once

#ifndef OREMINDERGEN3_THREAD_HPP
#define OREMINDERGEN3_THREAD_HPP

#include <functional>

#include <freertos/task.h>

#include "freertos-cpp-wrapper/internal/Utils.hpp"

namespace freertos {
class Thread {
public:
  Thread() = delete;
  Thread(const Thread &) = delete;

  explicit Thread(freertos::internal::FunctionPointer<void(void *)> runnable,
				  const char *name, const uint32_t stackDepth, void *parameters,
				  const UBaseType_t priority)
	  : task(nullptr) {
	xTaskCreate(runnable, name, stackDepth, &parameters, priority, &task);
	stop();
  }

  explicit Thread(std::function<void()> runnable, const char *name,
				  const uint32_t stackDepth, const UBaseType_t priority)
	  : task(nullptr), runnable(std::move(runnable)) {
	xTaskCreate(RTOSTaskWrapper, name, stackDepth, this, priority, &task);
	stop();
  }

  ~Thread() { vTaskDelete(task); }

  FREERTOS_ALWAYS_INLINE void run() { vTaskResume(task); }

  FREERTOS_ALWAYS_INLINE void stop() { vTaskSuspend(task); }

private:
  TaskHandle_t task;
  std::function<void()> runnable;

  static void RTOSTaskWrapper(void *parameter) {
	static_cast<Thread *>(parameter)->runnable();
  }
};
}// namespace freertos

#endif// OREMINDERGEN3_THREAD_HPP
