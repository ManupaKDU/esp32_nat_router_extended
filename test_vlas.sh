#!/bin/bash
for f in $(find src -name "*.c"); do
  grep -Hn -E "char\s+[a-zA-Z0-9_]+\s*\[\s*[a-zA-Z0-9_>\-]+\s*\]|char\s+[a-zA-Z0-9_]+\s*\[\s*[a-zA-Z0-9_>\-]+\s*\+\s*[0-9]+\s*\]" "$f"
done
