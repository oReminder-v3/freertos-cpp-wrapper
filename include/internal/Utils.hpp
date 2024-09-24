//
// Created by robcholz on 9/24/24.
//
#pragma once

#ifndef FREERTOS_CPP_WRAPPER_INCLUDE_INTERNAL_UTILS_HPP
#define FREERTOS_CPP_WRAPPER_INCLUDE_INTERNAL_UTILS_HPP

#define FREERTOS_ALWAYS_INLINE __attribute__((always_inline))

namespace freertos {
namespace internal {
template<typename Func>
using FunctionPointer = Func *;
}
}// namespace freertos

#endif// FREERTOS_CPP_WRAPPER_INCLUDE_INTERNAL_UTILS_HPP
