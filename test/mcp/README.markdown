# MCP contract and conformance checks

The MCP tests cover the Core helper API across 2025-03-26, 2025-06-18,
2025-11-25 and 2026-07-28. `mcp_conformance_test.cc` adds regression cases
for contracts that cannot be established by inspecting successful output
alone: invalid IDs, capability gates, header mirrors, metadata, continuations,
subscription opt-in and borrowed output. A directional method matrix is derived
from the five unions in the pinned official schemas and checked across revisions.

## Official schema checks

`conformance.py` validates JSON emitted by the actual C++ builders against the
official schemas at the following immutable upstream revision:

<https://github.com/modelcontextprotocol/modelcontextprotocol/tree/75db1e987cbbba6d170315dc99d0dfc440754aef/schema>

The runner selects each schema's declared JSON Schema dialect and verifies its
SHA-256 checksum against `schema-sha256.json`. Definition selection preserves
`$schema` and the original resource identity; runner regressions cover dialect
selection and absolute self-references. It is a development tool, not a
new dependency of the library or the ordinary CTest suite.

```sh
python3 -m pip install jsonschema==4.26.0
python3 test/mcp/conformance.py build-debug/test/mcp/sourcemeta_core_mcp_unit
```

For offline use, pass `--schema-directory /path/to/upstream/schema`. The
directory must contain `<revision>/schema.json` for all four revisions; the
same checksum checks apply. The corpus is created in a temporary directory
by running the `mcp_conformance.` tests with `SOURCEMETA_MCP_CORPUS` set.

The current corpus contains 142 payloads. Structural schema validation does
not assert JSON Schema `format` annotations or establish all normative prose
requirements. The C++ negative tests complement it; this is not a certification
of a complete MCP server implementation.

## Contracts and migration

| Area | Contract |
| --- | --- |
| Version selection | Exact dated revisions; initialization and discovery take the deployment's actual supported revision span. Compiled-in support is not a deployment declaration. |
| Methods | Client requests, server requests, notifications and nested MRTR input requests have separate revision-aware predicates. Method existence alone does not establish a negotiated capability. |
| Stateless metadata | Envelope and parameter validation are separate APIs. Validated `MCPRequestMeta` borrows the source JSON; it does not copy capabilities. `clientInfo` is optional, with required `name` and `version` when present. |
| Headers | Raw header collections use case-insensitive names, reject duplicate recognized singleton fields and check required modern mirrors. Legacy endpoints ignore later routing fields. |
| Header encoding | Core's Base64 and Unicode utilities implement one-pass sentinel decoding and validate decoded UTF-8. Literal sentinel-looking values are encoded. |
| Parameter mirrors | `x-mcp-header` is collected only on statically reachable properties. Primitive argument types and safe integer boundaries are checked before comparison. |
| Errors | Explicit version and request/transport context govern ID omission, legacy null-ID transport fallback, retired codes and HTTP status mapping. |
| Capabilities | Known shapes and feature revisions are checked. An explicitly parsed capability object retains a source snapshot to preserve settings, unknown fields and legacy task data. The stateless view does not create that snapshot. |
| Complete results | IDs, content, descriptors, schemas, metadata and discriminators are checked at runtime, including Release builds. New cacheable helpers require explicit cache policy. |
| MRTR | Only `tools/call`, `resources/read` and `prompts/get` admit `input_required`. Nested request kinds, client capabilities and continuation response shapes are checked. |
| Subscriptions | Acknowledgements intersect requested and supported filters. Explicit false, missing and empty URI lists remain distinguishable. Modern update notifications require an acknowledged matching subscription. |
| Borrowed serialization | `mcp_write_result` validates and serializes a precomputed cacheable result without copying its entry arrays. It writes compact JSON; stdio adapters append the newline. |

Callers of the previous in-progress API need to provide explicit versions,
supported revision spans, cache policy and subscription context as appropriate.
Use the returned metadata view instead of the removed convenience accessors;
use `MCPImplementation` for both client and server information. Invalid inputs
previously accepted in Release now throw `std::invalid_argument` or return a
documented validation failure. Valid legacy wire behavior is preserved where
the protocol allows it; this is not a promise to preserve malformed output.

The public header follows the Core module convention and includes the HTTP
module aggregate. MCP also uses Crypto, Unicode and Text internally. Their
targets must precede MCP in the build/install dependency graph, including
static exports. The consumer below verifies an installed package with only
the MCP component requested.

## API migration examples

`mcp_make_resource` and `mcp_make_resource_template` now take an explicit revision.
`MCPDescriptorPresentation` supplies optional title and icons; fields unavailable
in the selected revision are omitted. `MCPResourceAnnotations` adds audience,
priority and last-modified information. Both presentation and annotation views
borrow their inputs only for the duration of the call; built JSON owns its data.
The installed consumer compiles and exercises these APIs.

```cpp
const auto resource = sourcemeta::core::mcp_make_resource(
    sourcemeta::core::MCPProtocolVersion::V_2026_07_28,
    "file:///schema.json", "schema", "application/schema+json");
```

Applications choosing a text representation of a resource link for 2025-03-26
must construct that text themselves. Core refuses unsupported resource-link
content instead of choosing application presentation:

```cpp
const auto block = sourcemeta::core::mcp_make_text_block("Schema: file:///schema.json");
```

Use `mcp_decorate_result(version, std::move(result))` and
`mcp_decorate_cacheable_result` instead of the removed public in-place variants.
Use `mcp_validate_request_meta(...).first` instead of
`mcp_has_required_request_meta`. Negotiation takes the deployment's explicit
supported revision span; the nullary latest-initialization helper is removed.

Client and server capability parsers own their snapshots. Typed fields override
known source fields during serialization; unknown settings survive on retained
capabilities. Turning a child flag off removes stale enabled source settings;
an originally explicit false flag can remain false. Clearing `extensions` or
`experimental` removes that map. The roots `listChanged` field, task capabilities
and newer elicitation/sampling fields are removed when inapplicable to the target
revision. Parent features can also be implied by an enabled child flag.

`mcp_request_name_from_body` returns a view borrowing the envelope. Keep its
source string alive and unchanged. `mcp_result_type` is only a raw extractor;
`mcp_resolve_result_type` checks revision support and an explicitly negotiated
extension-result allowlist. The latter does not negotiate extensions itself.

Private cache scope means reuse within the same authorization context. Tool
annotations are advisory hints, not safety guarantees. Core retains the text
copy of structured tool results required for backward compatibility.

## Application and transport responsibilities

Core cannot establish authorization, active request history, stream lifetime,
monotonically increasing progress or the
integrity of opaque request state. It does not validate user form answers
against arbitrary JSON Schema. A transport must enforce HTTP framing,
Origin/session policy, SSE ordering and subscription acknowledgement ordering.
Core enforces the requested logging severity and rejects subscription-scoped
logs. The caller must still route logs on the requesting response stream.
The stdio-only server cancellation behavior also requires adapter-level
enforcement. These responsibilities remain in callers such as Sourcemeta One;
this change does not update or deploy its server.

## Installed consumer and allocation probe

After building and installing Core, run:

```sh
cmake --install build-release --prefix build/dist
cmake -S test/mcp/consumer -B build-mcp-consumer \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$PWD/build/dist"
cmake --build build-mcp-consumer
build-mcp-consumer/mcp_consumer
build-mcp-consumer/mcp_writer_profile
```

The standalone probe uses the system allocator and counts C++ `new` requests
and requested bytes during output construction, not RSS or allocator internals.
It serializes 4,096 resources with 256-character descriptions to a counting
stream. It checks complete output equality before measurement, warms both
orders, alternates route order, then reports all seven samples and medians
for allocation calls, requested bytes and elapsed time. The executable fails
if the borrowed path does not reduce allocations; timing has no pass threshold. The
ordinary conformance tests also compare parsed copied and borrowed envelopes.

Observed on 4 October 2026, Linux x86_64, GCC 13.3, Release:

| Path | `new` calls | Requested bytes | Output bytes | Median microseconds |
| --- | ---: | ---: | ---: | ---: |
| Copy and decorate | 16,414 | 3,813,538 | 1,476,542 | 3,317 |
| Borrow and serialize | 3 | 784 | 1,476,542 | 2,704 |

These numbers describe this local workload only. The borrowed path additionally
checks result entries; the generic copy/decorator baseline is not an identical
validation pipeline. Timing depends on the machine and is not a server speedup
claim. The allocation counts demonstrate the avoided entry-array deep copy.

## Validation recorded for this change

- Full Debug and Release builds: 176/176 CTest jobs passed in each.
- Direct MCP binaries: 258/258 cases passed in both configurations.
- Official schema checks: 142 payloads passed in Debug, Release and shared Release.
- Shared Release MCP test and installed static Debug/Release consumers passed.
- Project ClangFormat 20.1.6 check and `git diff --check` passed.

Pinned clang-tidy 22.1.7 was also run with the repository configuration over
the modified production and unit-test translation units. CI includes an
assertion-enabled Debug MCP run, official-schema validation and the installed
consumer/allocation probe in the Linux GCC job.

This local environment has GCC, not Clang; the project's AddressSanitizer
configuration rejects that compiler combination. ASan and the macOS/Windows
matrix were not run locally and remain CI checks.
