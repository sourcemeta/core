#ifndef SOURCEMETA_CORE_OPTIONS_ERROR_H_
#define SOURCEMETA_CORE_OPTIONS_ERROR_H_

#ifndef SOURCEMETA_CORE_OPTIONS_EXPORT
#include <sourcemeta/core/options_export.h>
#endif

#include <stdexcept> // std::runtime_error
#include <string>    // std::string
#include <utility>   // std::move

namespace sourcemeta::core {

// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(disable : 4251 4275)
#endif

/// @ingroup options
/// This class represents a general options error
struct SOURCEMETA_CORE_OPTIONS_EXPORT OptionError : public std::runtime_error {
  explicit OptionError(const std::string &message)
      : std::runtime_error{message} {}
};

/// @ingroup options
/// This class represents a unknown option error
struct SOURCEMETA_CORE_OPTIONS_EXPORT OptionsUnknownOptionError
    : public OptionError {
  explicit OptionsUnknownOptionError(std::string name)
      : OptionError{"Unknown option"}, name_{std::move(name)} {}
  [[nodiscard]] auto name() const -> const auto & { return this->name_; }

private:
  std::string name_;
};

/// @ingroup options
/// This class represents a value being passed to a flag
struct SOURCEMETA_CORE_OPTIONS_EXPORT OptionUnexpectedValueFlagError
    : public OptionError {
  explicit OptionUnexpectedValueFlagError(std::string name)
      : OptionError{"Cannot pass a value to a flag"}, name_{std::move(name)} {}
  [[nodiscard]] auto name() const -> const auto & { return this->name_; }

private:
  std::string name_;
};

/// @ingroup options
/// This class represents a missing value from an option
struct SOURCEMETA_CORE_OPTIONS_EXPORT OptionMissingOptionValueError
    : public OptionError {
  explicit OptionMissingOptionValueError(std::string name)
      : OptionError{"This option takes a value"}, name_{std::move(name)} {}
  [[nodiscard]] auto name() const -> const auto & { return this->name_; }

private:
  std::string name_;
};

#if defined(_MSC_VER)
#pragma warning(default : 4251 4275)
#endif

} // namespace sourcemeta::core

#endif
