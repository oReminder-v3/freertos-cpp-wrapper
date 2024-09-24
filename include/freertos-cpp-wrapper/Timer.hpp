//
// Created by robcholz on 5/24/24.
//
#pragma once

#ifndef FREERTOS_CPP_WRAPPER_INCLUDE_TIMER_HPP
#define FREERTOS_CPP_WRAPPER_INCLUDE_TIMER_HPP

#include <functional>

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include "freertos-cpp-wrapper/internal/Utils.hpp"

namespace utils {
namespace internal {
class TimerBase {
public:
  explicit TimerBase(TimerHandle_t timerHandle, std::function<void()> runnable)
	  : timerHandle(timerHandle), runnable(std::move(runnable)) {}

  virtual ~TimerBase() { xTimerDelete(timerHandle, 0); }

  FREERTOS_ALWAYS_INLINE
  void start() { xTimerStart(timerHandle, 0); }

  FREERTOS_ALWAYS_INLINE
  void stop() { xTimerStop(timerHandle, 0); }

  FREERTOS_ALWAYS_INLINE
  void reset() { xTimerReset(timerHandle, portMAX_DELAY); }

  FREERTOS_ALWAYS_INLINE
  void setPeriodAndStart(TickType_t millisecond) {
	xTimerChangePeriod(timerHandle, pdMS_TO_TICKS(millisecond), portMAX_DELAY);
  }

protected:
  static void RTOSTimerWrapper(TimerHandle_t xTimer) {
	static_cast<TimerBase *>(pvTimerGetTimerID(xTimer))->runnable();
  }

private:
  TimerHandle_t timerHandle;
  std::function<void()> runnable;
};
}// namespace internal

class DynamicTimer : public internal::TimerBase {
public:
  DynamicTimer() = delete;

  explicit DynamicTimer(
	  const char *name, TickType_t periodTick, bool runOnce,
	  freertos::internal::FunctionPointer<void(TimerHandle_t)> runnable)
	  : internal::TimerBase(
			xTimerCreate(name, periodTick, !runOnce, (void *) nullptr, runnable),
			nullptr) {
	stop();
  }

  explicit DynamicTimer(TickType_t periodMillisecond, bool runOnce,
						std::function<void()> runnable)
	  : internal::TimerBase(xTimerCreate("", pdMS_TO_TICKS(periodMillisecond),
										 !runOnce, this, RTOSTimerWrapper),
							std::move(runnable)) {
	stop();
  }
};

class StaticTimer : public internal::TimerBase {
public:
  StaticTimer() = delete;

  explicit StaticTimer(
	  const char *name, TickType_t periodTick, bool runOnce,
	  freertos::internal::FunctionPointer<void(TimerHandle_t)> runnable)
	  : internal::TimerBase(xTimerCreateStatic(name, periodTick, !runOnce,
											   (void *) nullptr, runnable,
											   &staticTimerBuffer),
							nullptr) {
	stop();
  }

  explicit StaticTimer(TickType_t periodMillisecond, bool runOnce,
					   std::function<void()> runnable)
	  : internal::TimerBase(
			xTimerCreateStatic("", pdMS_TO_TICKS(periodMillisecond), !runOnce,
							   this, RTOSTimerWrapper, &staticTimerBuffer),
			std::move(runnable)) {
	stop();
  }

private:
  StaticTimer_t staticTimerBuffer{};
};
}// namespace utils

#endif// FREERTOS_CPP_WRAPPER_INCLUDE_TIMER_HPP
