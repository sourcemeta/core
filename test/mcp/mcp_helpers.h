#ifndef SOURCEMETA_CORE_MCP_TEST_HELPERS_H_
#define SOURCEMETA_CORE_MCP_TEST_HELPERS_H_

#include <sourcemeta/core/test.h>

#include <stdexcept> // std::invalid_argument

// Every MCP rejection carries a distinct message, so a test names the
// rejection it expects instead of accepting any failure. The abort thrown by a
// nested expectation is re-raised so that it is not mistaken for a rejection
#define EXPECT_MCP_REJECTS(expression, expected_message)                       \
  try {                                                                        \
    expression;                                                                \
    FAIL();                                                                    \
  } catch (const std::invalid_argument &sourcemeta_test_error) {               \
    EXPECT_STREQ(sourcemeta_test_error.what(), expected_message);              \
  } catch (const ::sourcemeta::core::TestAbortError &) {                       \
    throw;                                                                     \
  } catch (...) {                                                              \
    FAIL();                                                                    \
  }

#endif
