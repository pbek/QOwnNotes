#!/usr/bin/env bash
#
# This script is a bash script that replaces the string "Qt::Orientation::Vertical"
# with "Qt::Vertical" in "src/dialogs/settingsdialog.ui" to fix the UI in Qt 5.15
#

set -euo pipefail

function update_qt_syntax() {
  local file="$1"

  # Array of search and replace patterns
  declare -A replacements=(
    ["Qt::Orientation::Vertical"]="Qt::Vertical"
    ["Qt::Horizontal"]="Qt::Orientation::Horizontal"
    ["QSizePolicy::Expanding"]="QSizePolicy::Policy::Expanding"
    ["QSizePolicy::Fixed"]="QSizePolicy::Policy::Fixed"
    ["QFrame::NoFrame"]="QFrame::Shape::NoFrame"
    ["QFrame::Raised"]="QFrame::Shadow::Raised"
    ["QFrame::Plain"]="QFrame::Shadow::Plain"
    ["QAbstractItemView::InternalMove"]="QAbstractItemView::DragDropMode::InternalMove"
    ["Qt::TargetMoveAction"]="Qt::DropAction::TargetMoveAction"
    ["Qt::AlignLeading"]="Qt::AlignmentFlag::AlignLeading"
    ["Qt::AlignLeft"]="Qt::AlignmentFlag::AlignLeft"
    ["Qt::AlignRight"]="Qt::AlignmentFlag::AlignRight"
    ["Qt::AlignVCenter"]="Qt::AlignmentFlag::AlignVCenter"
    ["Qt::AlignTrailing"]="Qt::AlignmentFlag::AlignTrailing"
    ["Qt::AlignTop"]="Qt::AlignmentFlag::AlignTop"
    ["Qt::LinksAccessibleByMouse"]="Qt::TextInteractionFlag::LinksAccessibleByMouse"
    ["Qt::TextSelectableByMouse"]="Qt::TextInteractionFlag::TextSelectableByMouse"
    ["Qt::TextSelectableByKeyboard"]="Qt::TextInteractionFlag::TextSelectableByKeyboard"
    ["QLineEdit::Password"]="QLineEdit::EchoMode::Password"
    ["QFormLayout::ExpandingFieldsGrow"]="QFormLayout::FieldGrowthPolicy::ExpandingFieldsGrow"
    ["Qt::ScrollBarAlwaysOff"]="Qt::ScrollBarPolicy::ScrollBarAlwaysOff"
    ["Qt::PlainText"]="Qt::TextFormat::PlainText"
    ["Qt::LeftToRight"]="Qt::LayoutDirection::LeftToRight"
    ["Qt::AutoText"]="Qt::TextFormat::AutoText"
    ["Qt::RichText"]="Qt::TextFormat::RichText"
    ["QDialogButtonBox::Ok"]="QDialogButtonBox::StandardButton::Ok"
    ["QDialogButtonBox::Cancel"]="QDialogButtonBox::StandardButton::Cancel"
  )

  # Check if file exists
  if [ ! -f "$file" ]; then
    echo "Error: File '$file' not found!"
    return 1
  fi

  # Perform all replacements
  for search in "${!replacements[@]}"; do
    replace="${replacements[$search]}"
    sed -i "s|$search|$replace|g" "$file"
  done

  echo "All Qt syntax updates completed in $file"
}

# Usage example:
dialogFile="src/dialogs/settingsdialog.ui"
update_qt_syntax "$dialogFile"
