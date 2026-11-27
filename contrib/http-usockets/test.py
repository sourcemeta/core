"""Loopback checks for the restricted Linux HTTP-over-uSockets experiment."""

import os
import selectors
import signal
import socket
import subprocess
import sys
import tempfile
import time

OK = b"HTTP/1.1 204 No Content\r\nConnection: close\r\n\r\n"
BAD = b"HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n"
TOO_LARGE = (
    b"HTTP/1.1 431 Request Header Fields Too Large\r\n"
    b"Content-Length: 0\r\nConnection: close\r\n\r\n"
)
REQUEST = b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
DEADLINE = 3


def check(actual, expected, label):
    if actual != expected:
        raise AssertionError(f"{label}: expected {expected!r}, got {actual!r}")


def read_response(client):
    deadline = time.monotonic() + DEADLINE
    response = bytearray()
    while True:
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            raise TimeoutError("response did not finish before the deadline")
        client.settimeout(remaining)
        chunk = client.recv(512)
        if not chunk:
            return bytes(response)
        response.extend(chunk)
        if len(response) > 512:
            raise AssertionError("unexpected response size or multiple responses")


def run(port):
    # Leave room inside CTest's 30 seconds for startup and process cleanup,
    # rather than allowing every slow connection its own full test budget.
    deadline = time.monotonic() + 12

    def connect():
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            raise TimeoutError("socket checks exceeded their overall deadline")
        return socket.create_connection(
            ("127.0.0.1", port), timeout=min(DEADLINE, remaining)
        )

    with connect() as partial, connect() as independent:
        partial.sendall(b"GET / HTTP/1.1\r\nHo")
        independent.sendall(REQUEST)
        check(read_response(independent), OK, "independent connection")
        partial.sendall(b"st: localhost\r\n\r")
        with selectors.DefaultSelector() as ready:
            ready.register(partial, selectors.EVENT_READ)
            if ready.select(0.1):
                raise AssertionError("server replied before the final LF")
        partial.sendall(b"\n")
        check(read_response(partial), OK, "fragmented terminator")
    passed = 2

    # Two writes can coalesce in TCP. The held request above additionally
    # checks that an incomplete connection remains buffered across loop turns.
    for split in range(1, len(REQUEST)):
        with connect() as client:
            client.sendall(REQUEST[:split])
            client.sendall(REQUEST[split:])
            check(read_response(client), OK, f"request split at {split}")
        passed += 1

    padded = b"GET / HTTP/1.1\r\nHost: localhost\r\nX-Pad: "
    exact = padded + b"a" * (8192 - len(padded) - 4) + b"\r\n\r\n"
    cases = (
        ("one request only", REQUEST + REQUEST, OK),
        ("ignored binary suffix", REQUEST + bytes(range(256)) * 64, OK),
        ("case-insensitive headers", REQUEST[:-2] + b"cOnTeNt-LeNgTh: 0\r\n\r\n", OK),
        ("body disallowed", REQUEST[:-2] + b"Content-Length: 4\r\n\r\ndata", BAD),
        (
            "chunked disallowed",
            REQUEST[:-2] + b"TRANSFER-ENCODING: chunked\r\n\r\n",
            BAD,
        ),
        (
            "ambiguous framing",
            REQUEST[:-2] + b"Content-Length: 0\r\nTransfer-Encoding: chunked\r\n\r\n",
            BAD,
        ),
        (
            "duplicate lengths",
            REQUEST[:-2] + b"Content-Length: 0\r\ncontent-length: 0\r\n\r\n",
            BAD,
        ),
        ("invalid length", REQUEST[:-2] + b"Content-Length: bad\r\n\r\n", BAD),
        ("duplicate Host", REQUEST[:-2] + b"host: example.com\r\n\r\n", BAD),
        ("empty Host", b"GET / HTTP/1.1\r\nHost: \t\r\n\r\n", BAD),
        ("missing Host", b"GET / HTTP/1.1\r\n\r\n", BAD),
        ("method policy", b"POST / HTTP/1.1\r\nHost: localhost\r\n\r\n", BAD),
        ("target policy", b"GET /other HTTP/1.1\r\nHost: localhost\r\n\r\n", BAD),
        ("malformed name", b"GET / HTTP/1.1\r\nHost : localhost\r\n\r\n", BAD),
        ("bare LF", b"GET / HTTP/1.1\nHost: localhost\n\n", BAD),
        ("header limit", exact, OK),
        ("limit excludes suffix", exact + REQUEST, OK),
        ("header over limit", exact[:-4] + b"a\r\n\r\n", TOO_LARGE),
    )
    for label, request, expected in cases:
        with connect() as client:
            client.sendall(request)
            check(read_response(client), expected, label)
        passed += 1

    for prefix in (b"", b"GET / HT", REQUEST[:-3], REQUEST[:-1]):
        with connect() as client:
            client.sendall(prefix)
            client.shutdown(socket.SHUT_WR)
            check(
                read_response(client), b"", f"premature EOF after {len(prefix)} bytes"
            )
        passed += 1

    # A complete request followed by a peer half-close must still get its reply.
    with connect() as client:
        client.sendall(REQUEST)
        client.shutdown(socket.SHUT_WR)
        check(read_response(client), OK, "complete request before EOF")
    return passed + 1


def wait_ready(server):
    deadline = time.monotonic() + 5
    announcement = bytearray()
    os.set_blocking(server.stdout.fileno(), False)
    with selectors.DefaultSelector() as ready:
        ready.register(server.stdout, selectors.EVENT_READ)
        while b"\n" not in announcement:
            remaining = deadline - time.monotonic()
            if remaining <= 0 or not ready.select(remaining):
                raise TimeoutError(
                    "prototype did not announce its port before the deadline"
                )
            chunk = os.read(server.stdout.fileno(), 64 - len(announcement))
            if not chunk:
                raise RuntimeError(
                    f"prototype exited before readiness: {server.poll()}"
                )
            announcement.extend(chunk)
            if len(announcement) >= 64 and b"\n" not in announcement:
                raise RuntimeError("prototype port announcement is too long")
    port = bytes(announcement).strip()
    if not port.isdigit() or not 0 < int(port) <= 65535:
        raise RuntimeError(f"invalid prototype port announcement: {port!r}")
    return int(port)


def stop(server):
    if server.poll() is not None:
        raise RuntimeError(f"prototype exited unexpectedly: {server.returncode}")
    server.terminate()
    try:
        server.wait(timeout=DEADLINE)
    except subprocess.TimeoutExpired:
        server.kill()
        server.wait(timeout=DEADLINE)
        raise RuntimeError("prototype required SIGKILL to stop") from None
    if server.returncode != -signal.SIGTERM:
        raise RuntimeError(f"prototype terminated unexpectedly: {server.returncode}")


def main():
    with tempfile.TemporaryFile() as diagnostics:
        server = subprocess.Popen(
            [sys.argv[1]], stdout=subprocess.PIPE, stderr=diagnostics, bufsize=0
        )
        try:
            passed = run(wait_ready(server))
        finally:
            unwinding = sys.exc_info()[0] is not None
            cleanup_error = None
            try:
                stop(server)
            except (OSError, subprocess.TimeoutExpired, RuntimeError) as error:
                cleanup_error = error
            finally:
                server.stdout.close()
            if unwinding or cleanup_error:
                diagnostics.seek(0)
                sys.stderr.write(
                    diagnostics.read(16384).decode("utf-8", errors="replace")
                )
            if cleanup_error:
                if unwinding:
                    print(
                        f"Additional cleanup failure: {cleanup_error}", file=sys.stderr
                    )
                else:
                    raise cleanup_error
    print(f"uSockets HTTP checks passed ({passed} checks)")


if __name__ == "__main__":
    main()
