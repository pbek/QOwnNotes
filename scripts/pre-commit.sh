#!/usr/bin/env bash
# pre-commit hook script

echo "Running pre-commit hook with treefmt"

# Run treefmt to format the code
just fmt --fail-on-change

# Check if there are any changes after formatting
#if ! git diff-index --quiet HEAD --; then
#  echo "Code has been formatted. Please review the changes and commit again."
#  exit 1
#fi
