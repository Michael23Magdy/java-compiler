#pragma once

#include "Types.h"
#include <string>

namespace compiler {

// Epsilon / Lambda transition symbol
inline constexpr Symbol EPSILON = '\0';
inline constexpr Symbol LAMBDA = '\0';

// Invalid / Dead state representation
inline constexpr StateID INVALID_STATE = -1;
inline constexpr StateID DEAD_STATE = -1;

// Internal representation for explicit concatenation operator during regex parsing
inline constexpr Symbol EXPLICIT_CONCAT = '\x08'; // Backspace character unused in source

// Reserved rule string representing lambda in input files
inline const std::string LAMBDA_STR = "\\L";

} // namespace compiler
