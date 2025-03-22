#!/usr/bin/env bash
# pre-commit hook script

echo "Running pre-commit hook with treefmt"

# Run treefmt to format the code and check if there are any changes after formatting
if ! just fmt --fail-on-change; then
  echo -e "\nCode may have been formatted. Please review the changes and commit again."
  exit 1
fi
