#ifndef SOURCEMETA_CORE_TERMINAL_ANSI_ESCAPES_H_
#define SOURCEMETA_CORE_TERMINAL_ANSI_ESCAPES_H_

#include <string_view> // std::string_view

namespace sourcemeta::core::internal {

/// @name ECMA-48 Select Graphic Rendition (SGR) Control Sequences
/// Standards reference: ECMA-48 (5th Edition, 1991), Section 8.3.117 "SGR -
/// SELECT GRAPHIC RENDITION" (also standardized as ISO/IEC 6429).
/// Control sequences take the form CSI Ps... 'm' where CSI is ESC '[' (\033[).
/// @see
/// https://ecma-international.org/publications-and-standards/standards/ecma-48/
/// @{

/// Reset or normal display (ECMA-48 SGR parameter 0).
constexpr std::string_view escape_reset{"\033[0m"};

/// Bold or increased intensity (ECMA-48 SGR parameter 1).
constexpr std::string_view escape_bold{"\033[1m"};

/// Red foreground color (ECMA-48 SGR parameter 31).
constexpr std::string_view escape_red{"\033[31m"};

/// Green foreground color (ECMA-48 SGR parameter 32).
constexpr std::string_view escape_green{"\033[32m"};

/// Yellow foreground color (ECMA-48 SGR parameter 33).
constexpr std::string_view escape_yellow{"\033[33m"};

/// Blue foreground color (ECMA-48 SGR parameter 34).
constexpr std::string_view escape_blue{"\033[34m"};

/// Cyan foreground color (ECMA-48 SGR parameter 36).
constexpr std::string_view escape_cyan{"\033[36m"};

/// Bold red foreground color (ECMA-48 SGR parameters 1 and 31).
constexpr std::string_view escape_bold_red{"\033[1;31m"};

/// Bold green foreground color (ECMA-48 SGR parameters 1 and 32).
constexpr std::string_view escape_bold_green{"\033[1;32m"};

/// Bold yellow foreground color (ECMA-48 SGR parameters 1 and 33).
constexpr std::string_view escape_bold_yellow{"\033[1;33m"};

/// Bold blue foreground color (ECMA-48 SGR parameters 1 and 34).
constexpr std::string_view escape_bold_blue{"\033[1;34m"};

/// Bold cyan foreground color (ECMA-48 SGR parameters 1 and 36).
constexpr std::string_view escape_bold_cyan{"\033[1;36m"};

/// @}

} // namespace sourcemeta::core::internal

#endif
