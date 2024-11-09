//
// Created by robcholz on 5/12/24.
//
#pragma once

#ifndef FREERTOS_CPP_WRAPPER_INCLUDE_MESSAGECHANNEL_HPP
#define FREERTOS_CPP_WRAPPER_INCLUDE_MESSAGECHANNEL_HPP

#include <cstdint>
#include <cstring>
#include <optional>

#include <freertos/message_buffer.h>

#include "internal/Utils.hpp"

namespace freertos {
namespace internal {
template<typename T>
class MessageChannelBase {
public:
  explicit MessageChannelBase(MessageBufferHandle_t messageBuffer)
	  : messageBuffer(messageBuffer) {}

  // Destructor: Cleanup the message buffer resource
  virtual ~MessageChannelBase() {
	if (messageBuffer != nullptr) {
	  vMessageBufferDelete(messageBuffer);
	}
  }

  // Disable copy constructor and copy assignment operator
  MessageChannelBase(const MessageChannelBase &) = delete;
  MessageChannelBase &operator=(const MessageChannelBase &) = delete;

  // Enable move constructor and move assignment operator
  MessageChannelBase(MessageChannelBase &&other) noexcept
	  : messageBuffer(other.messageBuffer) {
	other.messageBuffer = nullptr;
  }

  MessageChannelBase &operator=(MessageChannelBase &&other) noexcept {
	if (this != &other) {
	  if (messageBuffer != nullptr) {
		vMessageBufferDelete(messageBuffer);
	  }
	  messageBuffer = other.messageBuffer;
	  other.messageBuffer = nullptr;
	}
	return *this;
  }

  [[nodiscard]] FREERTOS_ALWAYS_INLINE bool hasElement() const {
	assert(messageBuffer != nullptr);
	return xMessageBufferIsEmpty(messageBuffer) != pdTRUE;
  }

  void sendMessage(T message) {
	assert(messageBuffer != nullptr);
	static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
	T t = std::move(message);
	xMessageBufferSend(messageBuffer, &t, sizeof(T), portMAX_DELAY);
  }

  std::optional<T> getMessage() {
	if (!hasElement()) {
	  return std::nullopt;
	}
	T t{};
	xMessageBufferReceive(messageBuffer, &t, sizeof(T), portMAX_DELAY);
	return t;
  }

private:
  MessageBufferHandle_t messageBuffer{};
};
}// namespace internal

template<typename T, size_t SIZE>
class StaticMessageChannel : public internal::MessageChannelBase<T> {
public:
  explicit StaticMessageChannel()
	  : internal::MessageChannelBase<T>(
			xMessageBufferCreateStatic(sizeof(messageBufferStorage),
									   messageBufferStorage,
									   &messageBufferStruct)) {}

private:
  uint8_t messageBufferStorage[SIZE * sizeof(T)]{};
  StaticMessageBuffer_t messageBufferStruct{};
};

template<typename T, size_t SIZE>
class DynamicMessageChannel : public internal::MessageChannelBase<T> {
public:
  explicit DynamicMessageChannel()
	  : internal::MessageChannelBase<T>(xMessageBufferCreate(sizeof(T) * SIZE)) {}
};
}// namespace freertos

#endif// FREERTOS_CPP_WRAPPER_INCLUDE_MESSAGECHANNEL_HPP
