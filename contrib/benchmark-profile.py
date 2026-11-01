#!/usr/bin/env python3

"""Record one existing Core workload with heaptrack on Linux.

Configure Core with RelWithDebInfo, CMAKE_EXPORT_COMPILE_COMMANDS=ON,
SOURCEMETA_CORE_BENCHMARK=ON, SOURCEMETA_CORE_USE_SYSTEM_ALLOCATOR=ON and
CMAKE_CXX_FLAGS=-fno-omit-frame-pointer, then build sourcemeta_core_benchmarks.

Example: python3 contrib/benchmark-profile.py build --filter JSON_Parse_KrakenD
         --iterations 10 --output profiles/krakend

Requires heaptrack and heaptrack_print on PATH. Capture includes process startup,
fixture preparation and teardown, not just the benchmark loop. Histogram sizes
are requested bytes, not allocator usable sizes or RSS. Heaptrack's temporary
count means immediately deallocated, not every short-lived allocation. Memory
left at exit includes runtime caches and is not by itself evidence of a leak.
RSS includes profiler overhead; Massif snapshots can miss short-lived peaks.
Use these profiles to select workloads, not to compare execution times. Run
ordinary benchmarks separately without heaptrack for timing measurements.
Automatic provenance is limited to the verified workload names listed by --help.
Other benchmarks remain available through the benchmark executable's --profile
mode, but need a verified source/input entry before this launcher can record them.
"""

import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess


# Keep fixture provenance explicit rather than guessing inputs from a name.
WORKLOADS = {
    "JSON_Parse_KrakenD": ("json.cc", ("2019_09_krakend.json",)),
    "JSON_Parse_Schema_ISO_Language": ("json.cc", ("2020_12_iso_language_2023_set_3.json",)),
    "JSON_Stringify_ISO_Language": ("json.cc", ("2020_12_iso_language_2023_set_3.json",)),
    "JSON_Prettify_KrakenD": ("json.cc", ("2019_09_krakend.json",)),
    "JSON_Parse_Integer": ("json.cc", ("integer_array.json",)),
}


def run(command, **kwargs):
    result = subprocess.run(command, text=True, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, **kwargs)
    if result.returncode:
        raise RuntimeError(f"Command failed ({result.returncode}): {result.stdout}")
    return result.stdout


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("build", type=Path)
    parser.add_argument("--filter", required=True, choices=sorted(WORKLOADS))
    parser.add_argument("--iterations", type=int, default=10)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if platform.system() != "Linux" or args.iterations < 1:
        parser.error("requires Linux and a positive iteration count")
    if os.environ.get("LD_PRELOAD"):
        parser.error("unset LD_PRELOAD so another allocator cannot shadow heaptrack")
    tools = [shutil.which(name) for name in ("heaptrack", "heaptrack_print")]
    if not all(tools):
        parser.error("heaptrack and heaptrack_print must be on PATH")

    build = args.build.resolve()
    cache = {}
    for line in (build / "CMakeCache.txt").read_text().splitlines():
        match = re.match(r"([^:#/][^:]*):[^=]+=(.*)", line)
        if match:
            cache[match[1]] = match[2]
    if cache.get("SOURCEMETA_CORE_USE_SYSTEM_ALLOCATOR") != "ON":
        parser.error("use a system-allocator build; mimalloc bypasses malloc tracing")
    if cache.get("CMAKE_BUILD_TYPE") != "RelWithDebInfo":
        parser.error("use RelWithDebInfo so call stacks have debug information")
    source = Path(cache["CMAKE_HOME_DIRECTORY"])
    binary = build / "benchmark/sourcemeta_core_benchmarks"
    commands = json.loads((build / "compile_commands.json").read_text())
    source_name, fixture_names = WORKLOADS[args.filter]
    workload_source = source / "benchmark" / source_name
    compilation = next((entry for entry in commands
                        if Path(entry["file"]).resolve() == workload_source.resolve()), None)
    if compilation is None:
        parser.error("no compile command found for the selected workload source")
    compiler = cache["CMAKE_CXX_COMPILER"]
    revision = run(["git", "-C", str(source), "rev-parse", "HEAD"]).strip()
    diff = run(["git", "-C", str(source), "diff", "HEAD", "--"])
    compiler_version = run([compiler, "--version"])
    versions = [run([tool, "--version"]).strip() for tool in tools]

    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    (output / "source.patch").write_text(diff)
    command = [tools[0], "--record-only", "-o", str(output / "trace"),
               str(binary), "--filter", args.filter,
               "--profile", str(args.iterations)]
    capture = run(command)
    (output / "capture.txt").write_text(capture)
    completed = re.search(r"^Profiled (.+): (\d+) iterations \(timings suppressed\)$",
                          capture, re.MULTILINE)
    if (not completed or completed[1] != args.filter
            or int(completed[2]) != args.iterations):
        raise RuntimeError("profile workload did not report completion")
    traces = [path for path in output.glob("trace.*") if path.is_file()]
    if len(traces) != 1:
        raise RuntimeError("expected exactly one raw heaptrack trace")
    analyze = [tools[1], "--file", str(traces[0]), "--merge-backtraces=0",
               "--disable-builtin-suppressions", "--disable-embedded-suppressions",
               "--print-leaks=1",
               "--print-histogram", str(output / "histogram.txt"),
               "--print-massif", str(output / "memory.massif"),
               "--print-flamegraph", str(output / "allocations.stacks"),
               "--flamegraph-cost-type", "allocations"]
    (output / "report.txt").write_text(run(analyze))
    inputs = source / "benchmark/files"
    manifest = {
        "revision": revision,
        "tracked_diff_sha256": hashlib.sha256(diff.encode()).hexdigest(),
        "binary_sha256": digest(binary),
        "runner_sha256": digest(Path(__file__)),
        "platform": platform.platform(), "machine": platform.machine(),
        "cpu_count": os.cpu_count(),
        "build_flags": {key: value for key, value in cache.items()
                        if key.startswith("CMAKE_CXX_FLAGS") or key in (
                            "CMAKE_BUILD_TYPE", "BUILD_SHARED_LIBS",
                            "SOURCEMETA_CORE_USE_SYSTEM_ALLOCATOR",
                            "SOURCEMETA_CORE_ADDRESS_SANITIZER",
                            "SOURCEMETA_CORE_UNDEFINED_SANITIZER")},
        "compiler_version": compiler_version, "profiler_versions": versions,
        "benchmark": completed[1], "iterations": args.iterations,
        "capture_scope": "whole process including fixture setup and teardown",
        "timing_comparison": False,
        "system_allocator_build": True, "compilation": compilation,
        "workload_source": str(workload_source),
        "capture_command": command, "analysis_command": analyze,
        "inputs": [{"path": str(inputs / name),
                    "bytes": (inputs / name).stat().st_size,
                    "sha256": digest(inputs / name)} for name in fixture_names],
        "artifacts": {path.name: digest(path) for path in output.iterdir()
                      if path.is_file()}}
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"Recorded {completed[1]} in {output}; profiler timings are not benchmark results")


if __name__ == "__main__":
    main()
