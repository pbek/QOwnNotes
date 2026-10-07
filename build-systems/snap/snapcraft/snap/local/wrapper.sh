#!/bin/sh

arch="${SNAP_LAUNCHER_ARCH_TRIPLET:-x86_64-linux-gnu}"
qt6_plugin_dir="$SNAP/usr/lib/$arch/qt6/plugins"
qt6_qml_dir="$SNAP/usr/lib/$arch/qt6/qml"

export QT_PLUGIN_PATH="$qt6_plugin_dir${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}"
export QT_QPA_PLATFORM_PLUGIN_PATH="$qt6_plugin_dir/platforms"
export QML2_IMPORT_PATH="$qt6_qml_dir:$SNAP/lib/$arch${QML2_IMPORT_PATH:+:$QML2_IMPORT_PATH}"
export QML_IMPORT_PATH="$qt6_qml_dir${QML_IMPORT_PATH:+:$QML_IMPORT_PATH}"

# Use native Wayland if the Wayland socket is reachable (Wayland session and
# connected "wayland" interface), otherwise use X11. An explicitly set
# QT_QPA_PLATFORM is respected, QOWNNOTES_SNAP_FORCE_X11=1 forces X11.
if [ -z "$QT_QPA_PLATFORM" ]; then
  wayland_socket="${WAYLAND_DISPLAY:-wayland-0}"
  case "$wayland_socket" in
  /*) ;;
  *) wayland_socket="$XDG_RUNTIME_DIR/$wayland_socket" ;;
  esac

  if [ -z "$QOWNNOTES_SNAP_FORCE_X11" ] &&
    { [ -n "$WAYLAND_DISPLAY" ] || [ "$XDG_SESSION_TYPE" = "wayland" ]; } &&
    [ -S "$wayland_socket" ]; then
    # Fall back to X11 if the Wayland connection fails
    export QT_QPA_PLATFORM="wayland;xcb"
  else
    export QT_QPA_PLATFORM=xcb
  fi
fi

exec "$SNAP/usr/bin/QOwnNotes" --snap "$@"
