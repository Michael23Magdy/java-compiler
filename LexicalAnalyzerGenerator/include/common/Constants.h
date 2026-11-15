#pragma once

#include "Types.h"
#include <string>

namespace compiler {

inline constexpr Symbol EPSILON = '\0';
inline constexpr Symbol LAMBDA = '\0';

inline constexpr StateID INVALID_STATE = -1;
inline constexpr StateID DEAD_STATE = -1;

inline constexpr Symbol EXPLICIT_CONCAT = '\x08';

inline const std::string LAMBDA_STR = "\\L";

}
