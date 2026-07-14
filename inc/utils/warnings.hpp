#pragma once

#include <string_view>

namespace utils {
    // Optimizer warnings
    inline constexpr std::string_view OPM_WRN_USELESS_COMPOUND = "OptimizerWarning: The compound is useless";
    inline constexpr std::string_view OPM_WRN_USELESS_FOR_INIT = "OptimizerWarning: The for initialize-part is useless";
    inline constexpr std::string_view OPM_WRN_USELESS_FOR_END = "OptimizerWarning: The for end-part is useless";
    inline constexpr std::string_view OPM_WRN_USELESS_FOR = "OptimizerWarning: The for statement is useless";
    inline constexpr std::string_view OPM_WRN_USELESS_GLOBAL = "OptimizerWarning: The global keyword is useless";
    inline constexpr std::string_view OPM_WRN_USELESS_IF = "OptimizerWarning: The if statement is useless";
    inline constexpr std::string_view OPM_WRN_USELESS_TERNARY = "OptimizerWarning: The ternary-operation is useless";
    inline constexpr std::string_view OPM_WRN_USELESS_WHILE = "OptimizerWarning: The while statement is useless";
}