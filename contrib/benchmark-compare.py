#!/usr/bin/env python3

# Turns two benchmark result files into a Markdown report, and optionally
# posts it as a pull request comment

import json
import os
import sys
import urllib.error
import urllib.request

# Hosted runners do not hold still enough for anything tighter to mean much
THRESHOLD = 5

REMOVED_LIMIT = 10

UNITS = ((1_000_000_000, "s"), (1_000_000, "ms"), (1_000, "us"), (1, "ns"))

PAGE_SIZE = 100


def duration(nanoseconds):
    for factor, suffix in UNITS:
        if nanoseconds >= factor:
            return f"{nanoseconds / factor:.2f} {suffix}"
    return f"{nanoseconds:.2f} ns"


def read_results(path):
    with open(path, encoding="utf-8") as handle:
        return json.load(handle).get("benchmarks", [])


def status_of(delta):
    if delta is None:
        return "new"
    if delta > THRESHOLD:
        return "slower"
    if delta < -THRESHOLD:
        return "faster"
    return "same"


def compare(previous, current):
    rows = []
    for entry in current:
        before = previous.get(entry["name"])
        delta = None
        if before is not None and before > 0:
            delta = (entry["real_time"] - before) / before * 100
        rows.append({"name": entry["name"], "current": entry["real_time"],
                     "previous": before, "delta": delta,
                     "status": status_of(delta)})
    return rows


def table(rows, baseline_name):
    lines = [f"| Benchmark | `{baseline_name}` | This branch | Change |",
             "| --- | --- | --- | --- |"]
    for row in rows:
        before = "n/a" if row["previous"] is None \
            else duration(row["previous"])
        change = "new" if row["delta"] is None else f"{row['delta']:+.1f}%"
        lines.append(f"| `{row['name']}` | {before} | "
                     f"{duration(row['current'])} | {change} |")
    return lines


def regression_first(row):
    return -(row["delta"] if row["delta"] is not None else 0)


def summarise(rows, removed, baseline_name, baseline_revision):
    counts = {name: [row for row in rows if row["status"] == name]
              for name in ("slower", "faster", "same", "new")}
    revision = f" at `{baseline_revision}`" if baseline_revision else ""
    summary = (f"**{len(counts['slower'])} slower**, "
               f"{len(counts['faster'])} faster, "
               f"{len(counts['same'])} within {THRESHOLD}%")
    if counts["new"]:
        summary += f", {len(counts['new'])} new"
    if removed:
        summary += f", {len(removed)} gone"
    return counts, [
        f"Measured against `{baseline_name}`{revision}.",
        "",
        summary + ".",
        ""]


def report(label, rows, removed, baseline_name, baseline_revision):
    lines = [f"<!-- sourcemeta-benchmark: {label} -->",
             f"### Benchmark `{label}`",
             ""]

    counts = {}
    if not rows:
        lines += ["No benchmarks ran.", ""]
    else:
        counts, preamble = summarise(rows, removed, baseline_name,
                                     baseline_revision)
        lines += preamble

    # The rows that moved are worth reading without unfolding anything
    notable = counts.get("slower", []) + counts.get("faster", []) \
        + counts.get("new", [])
    if notable:
        lines += table(sorted(notable, key=regression_first),
                       baseline_name) + [""]

    if rows:
        lines += ["<details>",
                  f"<summary>All {len(rows)} benchmarks</summary>",
                  ""]
        lines += table(rows, baseline_name)
        lines += ["", "</details>", ""]

    if removed:
        names = ", ".join(f"`{name}`" for name in removed[:REMOVED_LIMIT])
        rest = len(removed) - REMOVED_LIMIT
        more = f" and {rest} more" if rest > 0 else ""
        lines += [f"Gone from this branch: {names}{more}."]

    return "\n".join(lines).rstrip("\n")


def call(url, token, method="GET", payload=None):
    data = None if payload is None else json.dumps(payload).encode("utf-8")
    message = urllib.request.Request(url, data=data, method=method)
    message.add_header("Authorization", f"Bearer {token}")
    message.add_header("Accept", "application/vnd.github+json")
    message.add_header("X-GitHub-Api-Version", "2022-11-28")
    if data is not None:
        message.add_header("Content-Type", "application/json")
    with urllib.request.urlopen(message) as response:
        return json.load(response)


def existing_comment(api, repository, pull_request, token, marker):
    page = 1
    while True:
        batch = call(f"{api}/repos/{repository}/issues/{pull_request}"
                     f"/comments?per_page={PAGE_SIZE}&page={page}", token)
        for comment in batch:
            if comment["body"].startswith(marker):
                return comment["id"]
        if len(batch) < PAGE_SIZE:
            return None
        page += 1


# One comment per label, rewritten in place, so that pushing to a branch
# updates what is already there instead of burying the discussion. The marker
# is on the first line of every report this writes
def publish(pull_request, label, body):
    repository = os.environ.get("GITHUB_REPOSITORY", "")
    token = os.environ.get("GITHUB_TOKEN", "")
    if not repository or not token:
        print("error: Posting a comment needs a repository and a token",
              file=sys.stderr)
        print(f"  at pull request {pull_request}", file=sys.stderr)
        return 1

    api = os.environ.get("GITHUB_API_URL", "https://api.github.com")
    marker = f"<!-- sourcemeta-benchmark: {label} -->"
    try:
        comment = existing_comment(api, repository, pull_request, token,
                                   marker)
        if comment is None:
            call(f"{api}/repos/{repository}/issues/{pull_request}/comments",
                 token, "POST", {"body": body})
        else:
            call(f"{api}/repos/{repository}/issues/comments/{comment}",
                 token, "PATCH", {"body": body})
    except urllib.error.HTTPError as failure:
        print("error: The benchmark comment could not be posted",
              file=sys.stderr)
        print(f"  at pull request {pull_request}", file=sys.stderr)
        print(f"  at status {failure.code}", file=sys.stderr)
        print(f"  at response {failure.read().decode('utf-8', 'replace')}",
              file=sys.stderr)
        return 1

    return 0


def main(arguments):
    if not 3 <= len(arguments) <= 4:
        print(f"Usage: {sys.argv[0]} <label> <baseline-json> <current-json> "
              "[<pull-request>]", file=sys.stderr)
        return 1

    label, baseline_path, current_path = arguments[:3]
    pull_request = arguments[3] if len(arguments) == 4 else ""

    for path in (baseline_path, current_path):
        if not os.path.isfile(path):
            print("error: No such benchmark result file", file=sys.stderr)
            print(f"  at file path {path}", file=sys.stderr)
            return 1

    previous = {entry["name"]: entry["real_time"]
                for entry in read_results(baseline_path)}
    current = read_results(current_path)
    rows = compare(previous, current)
    removed = sorted(set(previous) - {row["name"] for row in rows})

    body = report(label, rows, removed,
                  os.environ.get("GITHUB_BASE_REF") or "the base branch",
                  os.environ.get("BENCHMARK_BASELINE_REVISION", ""))
    print(body)

    # Where a workflow collects what it wants shown on the run itself, which
    # is the only place a pull request from a fork gets to see this
    summary = os.environ.get("GITHUB_STEP_SUMMARY", "")
    if summary:
        with open(summary, "a", encoding="utf-8") as handle:
            handle.write(body + "\n")

    if not pull_request:
        return 0

    return publish(pull_request, label, body)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
