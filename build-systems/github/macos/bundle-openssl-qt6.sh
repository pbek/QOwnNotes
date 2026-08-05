#!/bin/sh

set -eu

APP_BUNDLE=${1:?Usage: bundle-openssl-qt6.sh <application bundle>}
APP_BUNDLE=$(CDPATH='' cd -- "$(dirname -- "$APP_BUNDLE")" && pwd)/$(basename -- "$APP_BUNDLE")
OPENSSL_VERSION=3.5.7
OPENSSL_SHA256=a8c0d28a529ca480f9f36cf5792e2cd21984552a3c8e4aa11a24aa31aeac98e8
OPENSSL_ARCHIVE="openssl-${OPENSSL_VERSION}.tar.gz"
OPENSSL_URL="https://github.com/openssl/openssl/releases/download/openssl-${OPENSSL_VERSION}/${OPENSSL_ARCHIVE}"
MACOSX_DEPLOYMENT_TARGET=${MACOSX_DEPLOYMENT_TARGET:-13.0}
SCRIPT_DIR=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
WORK_DIR=$(mktemp -d "${TMPDIR:-/tmp}/qownnotes-openssl.XXXXXX")
FRAMEWORKS_DIR="${APP_BUNDLE}/Contents/Frameworks"
OPENSSL_PLUGIN="${APP_BUNDLE}/Contents/PlugIns/tls/libqopensslbackend.dylib"

cleanup() {
  rm -rf "$WORK_DIR"
}
trap cleanup EXIT HUP INT TERM

if [ "$(uname)" != "Darwin" ]; then
  echo "OpenSSL can only be bundled into the macOS application on macOS"
  exit 1
fi

if [ ! -d "$FRAMEWORKS_DIR" ]; then
  echo "Application Frameworks directory not found: $FRAMEWORKS_DIR"
  exit 1
fi

if [ ! -f "$OPENSSL_PLUGIN" ]; then
  echo "Qt OpenSSL TLS plugin not found: $OPENSSL_PLUGIN"
  exit 1
fi

echo "Downloading OpenSSL ${OPENSSL_VERSION}"
curl --fail --location --retry 3 --output "$WORK_DIR/$OPENSSL_ARCHIVE" "$OPENSSL_URL"
printf '%s  %s\n' "$OPENSSL_SHA256" "$WORK_DIR/$OPENSSL_ARCHIVE" | shasum -a 256 --check

SDKROOT=$(xcrun --sdk macosx --show-sdk-path)
JOBS=$(sysctl -n hw.logicalcpu)

for ARCH in arm64 x86_64; do
  SOURCE_DIR="$WORK_DIR/source-$ARCH"
  mkdir "$SOURCE_DIR"
  tar -xzf "$WORK_DIR/$OPENSSL_ARCHIVE" -C "$SOURCE_DIR" --strip-components=1

  case "$ARCH" in
  arm64) OPENSSL_TARGET=darwin64-arm64-cc ;;
  x86_64) OPENSSL_TARGET=darwin64-x86_64-cc ;;
  esac

  echo "Building OpenSSL ${OPENSSL_VERSION} for $ARCH"
  (
    cd "$SOURCE_DIR"
    env \
      CC=clang \
      CFLAGS="-arch $ARCH -isysroot $SDKROOT -mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET" \
      LDFLAGS="-arch $ARCH -isysroot $SDKROOT -mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET" \
      ./Configure "$OPENSSL_TARGET" shared no-tests no-apps
    make -s -j "$JOBS" build_sw
  )
done

echo "Creating universal OpenSSL libraries"
lipo -create \
  "$WORK_DIR/source-arm64/libcrypto.3.dylib" \
  "$WORK_DIR/source-x86_64/libcrypto.3.dylib" \
  -output "$FRAMEWORKS_DIR/libcrypto.3.dylib"
lipo -create \
  "$WORK_DIR/source-arm64/libssl.3.dylib" \
  "$WORK_DIR/source-x86_64/libssl.3.dylib" \
  -output "$FRAMEWORKS_DIR/libssl.3.dylib"

install_name_tool -id @rpath/libcrypto.3.dylib "$FRAMEWORKS_DIR/libcrypto.3.dylib"
install_name_tool -id @rpath/libssl.3.dylib "$FRAMEWORKS_DIR/libssl.3.dylib"

for ARCH in arm64 x86_64; do
  CRYPTO_DEPENDENCY=$(otool -arch "$ARCH" -L "$FRAMEWORKS_DIR/libssl.3.dylib" |
    sed -nE 's/^[[:space:]]*([^[:space:]]*libcrypto\.3\.dylib).*/\1/p')
  if [ -z "$CRYPTO_DEPENDENCY" ]; then
    echo "Could not determine the $ARCH libcrypto dependency of libssl"
    exit 1
  fi
  if [ "$CRYPTO_DEPENDENCY" != "@loader_path/libcrypto.3.dylib" ]; then
    install_name_tool -change "$CRYPTO_DEPENDENCY" @loader_path/libcrypto.3.dylib \
      "$FRAMEWORKS_DIR/libssl.3.dylib"
  fi
done

cp "$WORK_DIR/source-arm64/LICENSE.txt" \
  "$APP_BUNDLE/Contents/Resources/OpenSSL-LICENSE.txt"

for LIBRARY in libcrypto.3.dylib libssl.3.dylib; do
  ARCHITECTURES=$(lipo -archs "$FRAMEWORKS_DIR/$LIBRARY")
  case "$ARCHITECTURES" in
  *arm64*) ;;
  *)
    echo "$LIBRARY has no arm64 slice"
    exit 1
    ;;
  esac
  case "$ARCHITECTURES" in
  *x86_64*) ;;
  *)
    echo "$LIBRARY has no x86_64 slice"
    exit 1
    ;;
  esac
done

PLUGIN_ARCHITECTURES=$(lipo -archs "$OPENSSL_PLUGIN")
for ARCH in arm64 x86_64; do
  case "$PLUGIN_ARCHITECTURES" in
  *$ARCH*) ;;
  *)
    echo "Qt OpenSSL TLS plugin has no $ARCH slice"
    exit 1
    ;;
  esac
done

for ARCH in arm64 x86_64; do
  CRYPTO_DEPENDENCY=$(otool -arch "$ARCH" -L "$FRAMEWORKS_DIR/libssl.3.dylib" |
    sed -nE 's/^[[:space:]]*([^[:space:]]*libcrypto\.3\.dylib).*/\1/p')
  if [ "$CRYPTO_DEPENDENCY" != "@loader_path/libcrypto.3.dylib" ]; then
    echo "The $ARCH libssl dependency is not bundle-relative: $CRYPTO_DEPENDENCY"
    exit 1
  fi
  echo "$ARCH libssl dependency: $CRYPTO_DEPENDENCY"
done

echo "Verifying the bundled Qt OpenSSL backend"
PROBE_DIR="$WORK_DIR/tls-probe"
mkdir "$PROBE_DIR"
(
  cd "$PROBE_DIR"
  qmake "$SCRIPT_DIR/tls-backend-probe.pro" "QMAKE_APPLE_DEVICE_ARCHS=arm64 x86_64"
  make -s -j "$JOBS"
  PROBE_APP="$PROBE_DIR/qownnotes-tls-backend-probe.app"
  mkdir -p "$PROBE_APP/Contents/Frameworks"
  cp "$FRAMEWORKS_DIR/libcrypto.3.dylib" "$PROBE_APP/Contents/Frameworks/"
  cp "$FRAMEWORKS_DIR/libssl.3.dylib" "$PROBE_APP/Contents/Frameworks/"
  for ARCH in arm64 x86_64; do
    PROBE_OUTPUT=$(env \
      -u DYLD_FALLBACK_FRAMEWORK_PATH \
      -u DYLD_FALLBACK_LIBRARY_PATH \
      -u DYLD_FRAMEWORK_PATH \
      -u DYLD_INSERT_LIBRARIES \
      -u DYLD_LIBRARY_PATH \
      -u DYLD_ROOT_PATH \
      -u DYLD_VERSIONED_LIBRARY_PATH \
      arch -"$ARCH" "$PROBE_APP/Contents/MacOS/qownnotes-tls-backend-probe" \
      "$APP_BUNDLE/Contents/PlugIns" "$PROBE_APP/Contents/Frameworks")
    printf '%s: %s\n' "$ARCH" "$PROBE_OUTPUT"
    case "$PROBE_OUTPUT" in
    *"OpenSSL $OPENSSL_VERSION"*) ;;
    *)
      echo "$ARCH probe did not load OpenSSL $OPENSSL_VERSION"
      exit 1
      ;;
    esac
  done
)

# macdeployqt signed the existing nested code. Sign the new libraries and the
# bundle again after its contents have changed.
codesign --force --sign - "$FRAMEWORKS_DIR/libcrypto.3.dylib"
codesign --force --sign - "$FRAMEWORKS_DIR/libssl.3.dylib"
codesign --force --sign - "$APP_BUNDLE"
codesign --verify --deep --strict --verbose=2 "$APP_BUNDLE"

echo "Bundled and verified OpenSSL ${OPENSSL_VERSION}"
