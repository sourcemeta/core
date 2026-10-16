#ifndef SOURCEMETA_CORE_MCP_TEST_HELPERS_H_
#define SOURCEMETA_CORE_MCP_TEST_HELPERS_H_

#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/test.h>

// Every MCP rejection carries a distinct message, so a test names the
// rejection it expects instead of accepting any failure. Catching the module's
// own error type also asserts that a contract violation is reported as such
// rather than as a plain invalid argument. The abort thrown by a nested
// expectation is re-raised so that it is not mistaken for a rejection
#define EXPECT_MCP_REJECTS(expression, expected_message)                       \
  try {                                                                        \
    static_cast<void>(expression);                                             \
    FAIL();                                                                    \
  } catch (const ::sourcemeta::core::MCPError &sourcemeta_test_error) {        \
    EXPECT_STREQ(sourcemeta_test_error.what(), expected_message);              \
  } catch (const ::sourcemeta::core::TestAbortError &) {                       \
    throw;                                                                     \
  } catch (...) {                                                              \
    FAIL();                                                                    \
  }

#endif
