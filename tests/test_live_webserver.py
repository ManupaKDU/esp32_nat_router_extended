#!/usr/bin/env python3
"""
Automated Live Webserver & UI Regression Test Suite for ESP32 NAT Router Extended.
Tests endpoint accessibility, complete HTML document generation (no truncation),
API response integrity, and HTTP concurrency.
"""

import sys
import time
import urllib.request
from concurrent.futures import ThreadPoolExecutor, as_completed

DEFAULT_IP = "192.168.1.61"

def test_endpoint(ip, path, timeout=5, expect_status=200, check_closing_html=True):
    url = f"http://{ip}{path}"
    t0 = time.time()
    try:
        req = urllib.request.Request(url, headers={"User-Agent": "ESP32Test/1.0"})
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            data = resp.read().decode("utf-8", errors="replace")
            dt = time.time() - t0
            status = resp.status
            if status != expect_status:
                return False, f"Unexpected status {status} (expected {expect_status})", dt
            if check_closing_html and not data.rstrip().endswith("</html>"):
                return False, f"HTML incomplete or truncated (length {len(data)} bytes, no </html>)", dt
            return True, f"OK ({len(data)} bytes in {dt:.2f}s)", dt
    except Exception as e:
        dt = time.time() - t0
        return False, f"Exception: {e}", dt

def test_concurrent(ip, paths, max_workers=5, timeout=8):
    print(f"[*] Testing concurrency with {len(paths)} requests across {max_workers} worker threads...")
    results = []
    with ThreadPoolExecutor(max_workers=max_workers) as pool:
        future_map = {
            pool.submit(test_endpoint, ip, p, timeout=timeout, check_closing_html=p.endswith(('.html', 'advanced', 'result', 'ota', 'portmap'))): p
            for p in paths
        }
        for fut in as_completed(future_map):
            path = future_map[fut]
            ok, msg, dt = fut.result()
            results.append((path, ok, msg, dt))
            status_str = "PASS" if ok else "FAIL"
            print(f"    [{status_str}] {path:15s} -> {msg}")
    all_ok = all(r[1] for r in results)
    return all_ok, results

def main():
    ip = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_IP
    print(f"=== Starting ESP32 Webserver Automated Regression Tests against http://{ip} ===")
    
    endpoints = [
        ("/", True),
        ("/result", True),
        ("/advanced", True),
        ("/ota", True),
        ("/portmap", True),
        ("/api", False),
    ]

    passed = 0
    failed = 0

    print("\n--- Phase 1: Individual Endpoint Validation ---")
    for path, check_html in endpoints:
        ok, msg, dt = test_endpoint(ip, path, check_closing_html=check_html)
        if ok:
            passed += 1
            print(f"  [PASS] {path:12s} -> {msg}")
        else:
            failed += 1
            print(f"  [FAIL] {path:12s} -> {msg}")

    print("\n--- Phase 2: High Concurrency Load Test ---")
    concurrent_paths = [
        "/advanced",
        "/api",
        "/result",
        "/ota",
        "/portmap",
        "/advanced",
    ]
    conc_ok, conc_res = test_concurrent(ip, concurrent_paths, max_workers=6)
    if conc_ok:
        passed += 1
        print("  [PASS] Concurrency test passed without dropped connections.")
    else:
        failed += 1
        print("  [FAIL] Concurrency test encountered failures.")

    print(f"\n=== Test Summary: {passed} PASSED, {failed} FAILED ===")
    sys.exit(0 if failed == 0 else 1)

if __name__ == "__main__":
    main()
