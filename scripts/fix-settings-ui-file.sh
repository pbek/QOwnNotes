#!/usr/bin/env bash
#
# This script is a bash script that replaces the string "Qt::Orientation::Vertical"
# with "Qt::Vertical" in "src/dialogs/settingsdialog.ui" to fix the UI in Qt 5.15
#

set -e

# Perform the replacement
sed -i 's/Qt::Orientation::Vertical/Qt::Vertical/g' src/dialogs/settingsdialog.ui

echo "Replacement complete."
