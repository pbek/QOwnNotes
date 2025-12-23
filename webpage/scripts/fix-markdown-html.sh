#!/usr/bin/env bash
# Wrapper script to fix HTML validation issues in markdown files
# Uses Python script for more sophisticated fixing

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Use Python script for better handling of complex cases
python3 "$SCRIPT_DIR/fix-markdown-html.py"

exit $?
