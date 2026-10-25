# MCP performance baselines

The `MCP_` cases use the existing Core benchmark harness. They measure local
protocol helpers, not model generation, networking, a transport/session runtime,
or application JSON Schema evaluation. No production code is optimized here.
Fixtures are deterministic, authored microbenchmark inputs, not captured traffic.

## Run and record

Configure a Release build with MCP and benchmarks enabled:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DSOURCEMETA_CORE_MCP=ON -DSOURCEMETA_CORE_BENCHMARK=ON \
  -DSOURCEMETA_CORE_USE_SYSTEM_ALLOCATOR=ON
cmake --build build --config Release --target sourcemeta_core_benchmarks
# Single-configuration generators; multi-configuration builds may add Release/
./build/benchmark/sourcemeta_core_benchmarks --filter MCP_ --output build/mcp.json
```

Keep the JSON output alongside the commit SHA, complete configure command,
`cmake -LA -N build` output, compiler/version (`c++ --version` or the platform
compiler equivalent), CPU/OS, build configuration, SIMD flags, and system load.
The command above explicitly selects the system allocator. Repeat in a separate
build with `SOURCEMETA_CORE_USE_SYSTEM_ALLOCATOR=OFF` to study the configured
mimalloc allocator; do not mix allocator/build/compiler results. The harness
JSON records timings and core count, not all of this environment information.

Run the filtered executable multiple times on an otherwise idle machine. The
harness reports per-operation real/CPU time, a cold measurement, and the fastest
of three steady-state measurement windows; it does **not** report request p95 or
allocation counts. Use an external allocation profiler separately if needed,
and do not compare profiler timings with uninstrumented runs. There are no new
noisy timing thresholds in CI. MCP-disabled builds do not register these cases.

## Workloads and measurement boundaries

All fixture construction and preflight correctness checks occur before the
state loop. Checks throw on failure even with `NDEBUG`. Results are consumed
through the harness optimization barrier. Per-iteration output destruction is
included. Counts below are part of the benchmark names.

| Cases | Small / large | Timed operation |
|---|---|---|
| `MCP_Parse_Metadata_*` | 0 / 128 extra metadata keys | Parse cached UTF-8 JSON and destroy the DOM; no MCP validation |
| `MCP_Validate_Metadata_*` | 0 / 128 extra keys | Validate an already parsed request, including metadata-key checks |
| `MCP_Reject_Metadata_*` | 0 / 128 extra keys | Reject object-valued `progressToken`, after other metadata checks |
| `MCP_Make_Headers_*` | 2 / 64 descriptors | Descriptor checks, header creation and plain/Unicode sentinel encoding |
| `MCP_Validate_Headers_*` | 2 / 64 descriptors | Validate already parsed arguments against 4 / 128 borrowed headers |
| `MCP_Reject_Headers_*` | 2 / 64 descriptors | Same, with a mismatch on the last mirrored parameter; includes error construction |
| `MCP_Write_Result_*` | 1 / 256 resources | Const-input result validation, copying, decoration, JSON-RPC envelope and ostream serialization |
| `MCP_Copy_Move_Write_Result_*` | 1 / 256 resources | Explicit fresh-payload copy plus move-taking writer; not a copy-free move comparison |
| `MCP_Reject_Result_*` | 1 / 256 resources | Invalid URI type on the last entry; includes exception construction/catching, not successful serialization |
| `MCP_Intersect_Overlap_*` | 8 / 512 URIs per side | Intersection with half overlap, preserving requested ordering |
| `MCP_Intersect_Disjoint_*` | 8 / 512 per side | Empty intersection; a valid no-match result, not malformed input |

Each serialization iteration creates and destroys an `ostringstream`; its buffer
allocation is included. The successful const path is round-trip checked before
timing. Both writer variants receive the same source payload. The move-taking
path explicitly includes the fresh input copy required for repeated iterations.
These are whole public-API operations, not isolated JSON stringify kernels.

The valid metadata wire sizes are **185 / 10,187 bytes** (compact UTF-8 JSON).
Each extra key is `bench-{index}` with a 64-byte ASCII value. Rejection adds
`progressToken:{}`. Resource payloads before envelope/cache decoration are
**201 / 48,435 bytes**; each resource has a name, a `file:///bench/{index}` URI
and a 128-byte description. Headers mirror alternating `plain` and ` café `
values; half require UTF-8/base64 sentinel encoding. An equal number of unrelated
headers precedes mirrored headers to exercise lookup work. These sizes exclude
HTTP framing and are not protocol payload limits.

Subscription URIs are `file:///bench/{index}`. Requested IDs are `[0,N)`;
supported IDs are `[N/2,3N/2)` for overlap and `[N,2N)` for disjoint workloads.
Owners are fully populated before any borrowed views are constructed. Reuse
these exact fixture shapes for before/after comparisons; do not attribute gains
from changing payloads, validation policy or included setup to an optimization.
