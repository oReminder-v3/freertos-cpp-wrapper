//
// Created by robcholz on 5/26/24.
//
#pragma once

#ifndef FREERTOS_CPP_WRAPPER_INCLUDE_ATOMIC_HPP
#define FREERTOS_CPP_WRAPPER_INCLUDE_ATOMIC_HPP

#include <cstdint>

#include <freertos/atomic.h>

#include "internal/Utils.hpp"

namespace freertos {

template<typename T>
class Atomic {
public:
  Atomic() = default;
  Atomic(const T &value) { assign(value); }
  Atomic(const Atomic &value) { assign(value.destination); }
  ~Atomic() = default;

  Atomic &operator=(const T &value) {
	assign(value);
	return *this;
  }

  explicit operator bool() const { return !get(); }

  FREERTOS_ALWAYS_INLINE
  Atomic operator+(const Atomic &rhs) {
	Atomic atomic(*this);
	atomic.add(rhs.get());
	return atomic;
  }

  FREERTOS_ALWAYS_INLINE
  Atomic operator+(const T &rhs) {
	Atomic atomic(*this);
	atomic.add(rhs);
	return atomic;
  }

  FREERTOS_ALWAYS_INLINE
  Atomic operator-(const Atomic &rhs) {
	Atomic atomic(*this);
	atomic.subtract(rhs.get());
	return atomic;
  }

  FREERTOS_ALWAYS_INLINE
  Atomic operator-(const T &rhs) {
	Atomic atomic(*this);
	atomic.subtract(rhs);
	return atomic;
  }

  FREERTOS_ALWAYS_INLINE
  Atomic &operator+=(const Atomic &rhs) {
	this->add(rhs.get());
	return *this;
  }

  FREERTOS_ALWAYS_INLINE
  Atomic &operator+=(const T &rhs) {
	this->add(rhs);
	return *this;
  }

  FREERTOS_ALWAYS_INLINE
  Atomic &operator-=(const Atomic &rhs) {
	this->subtract(rhs.get());
	return *this;
  }

  FREERTOS_ALWAYS_INLINE
  Atomic &operator-=(const T &rhs) {
	this->subtract(rhs);
	return *this;
  }

  FREERTOS_ALWAYS_INLINE
  bool operator==(const Atomic &rhs) const {
	return this->compare(rhs.get());
	return *this;
  }

  FREERTOS_ALWAYS_INLINE
  bool operator==(const T &rhs) const {
	return this->compare(rhs);
	return *this;
  }

  FREERTOS_ALWAYS_INLINE
  bool compare(const T &value) const {
	bool returnValue;
	ATOMIC_ENTER_CRITICAL();
	{ returnValue = (destination == value); }
	ATOMIC_EXIT_CRITICAL();
	return returnValue;
  }

  FREERTOS_ALWAYS_INLINE
  bool compare(const Atomic &value) const { return compare(value.get()); }

  FREERTOS_ALWAYS_INLINE void assign(const T &value) {
	ATOMIC_ENTER_CRITICAL();
	{ destination = value; }
	ATOMIC_EXIT_CRITICAL();
  }

  FREERTOS_ALWAYS_INLINE void assign(const Atomic &value) {
	assign(value.get());
  }

  FREERTOS_ALWAYS_INLINE void add(const T &value) {
	ATOMIC_ENTER_CRITICAL();
	{ destination += value; }
	ATOMIC_EXIT_CRITICAL();
  }

  FREERTOS_ALWAYS_INLINE void add(const Atomic &value) { add(value.get()); }

  FREERTOS_ALWAYS_INLINE
  void subtract(const T &value) {
	ATOMIC_ENTER_CRITICAL();
	{ destination -= value; }
	ATOMIC_EXIT_CRITICAL();
  }

  FREERTOS_ALWAYS_INLINE
  void subtract(const Atomic &value) { subtract(value.get()); }

  FREERTOS_ALWAYS_INLINE
  T get() const {
	T temp;
	ATOMIC_ENTER_CRITICAL();
	{ temp = destination; }
	ATOMIC_EXIT_CRITICAL();
	return temp;
  }

private:
  T volatile destination;
};
}// namespace freertos

#endif// FREERTOS_CPP_WRAPPER_INCLUDE_ATOMIC_HPP
