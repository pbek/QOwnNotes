#!/usr/bin/env bash

# Exit on any error
set -e

# Configuration
SRC_IMAGES_DIR="../src/images"
WEBPAGE_FAVICON="../webpage/src/.vuepress/public/favicon.png"
INPUT_SVG="icon.svg"
INPUT_DARK_SVG="icon-dark.svg"
SIZES_APP=(16 24 32 48 64 96 128 256 512)
APP_NAME="QOwnNotes"

# Check if input file exists
if [ ! -f "$INPUT_SVG" ]; then
  echo "Error: Input file '$INPUT_SVG' not found!"
  exit 1
fi

#Check if dark input file exists
if [ ! -f "$INPUT_DARK_SVG" ]; then
  echo "Error: Input file '$INPUT_DARK_SVG' not found!"
  exit 1
fi

# Function to convert images
convert_icon_png() {
  local size=$1
  local output_file=$2
  local input_file=$3

  # Use fallback to $INPUT_SVG if no local input file is provided
  if [ -z "$input_file" ]; then
    input_file=$INPUT_SVG
  fi

  echo "PNG converting to ${size}x${size} -> ${output_file}"
  #    magick "$input_file" -resize "${size}x${size}" "$output_file"
  rsvg-convert -w "$size" -h "$size" "$input_file" -o "$output_file"
}

convert_icon_ico() {
  local output_file=$1
  local input_file=$2

  # Use fallback to $INPUT_SVG if no local input file is provided
  if [ -z "$input_file" ]; then
    input_file=$INPUT_SVG
  fi

  echo "ICO converting to -> ${output_file}"
  magick -density 300 -define icon:auto-resize=256,128,96,64,48,32,16 -background none "$input_file" "$output_file"
}

convert_icon_icns() {
  local output_file=$1
  local input_file=$2

  # Use fallback to $INPUT_SVG if no local input file is provided
  if [ -z "$input_file" ]; then
    input_file=$INPUT_SVG
  fi

  echo "ICNS converting to -> ${output_file}"

  mkdir iconset
  for size in 16 32 64 128 256 512 1024; do
    rsvg-convert -w $size -h $size $INPUT_SVG -o iconset/icon_${size}x${size}.png
  done

  png2icns "$output_file" iconset/*.png
  rm -rf iconset
}

# Generate all sizes
convert_icon_png 128 "icon.png"
convert_icon_icns "icon.icns"
convert_icon_ico "icon.ico"
convert_icon_png 800 "icon-800.png"
convert_icon_png 512 "../snap/gui/QOwnNotes.png"
convert_icon_png 512 "../build-systems/github/snap.qt6/gui/QOwnNotes.png"
convert_icon_png 512 "../build-systems/snap/snapcraft/snap/gui/QOwnNotes.png"
convert_icon_icns "../src/QOwnNotes.icns"
convert_icon_ico "../src/QOwnNotes.ico"

# Generate app icons
for size in "${SIZES_APP[@]}"; do
  convert_icon_png ${size} "${SRC_IMAGES_DIR}/icons/${size}x${size}/apps/${APP_NAME}.png"
done

# Copy to src/images
echo "Copying to src/images"
cp icon.png ${SRC_IMAGES_DIR}/icon.png
cp icon.png ${WEBPAGE_FAVICON}
cp ${INPUT_DARK_SVG} ${SRC_IMAGES_DIR}/icon-dark.svg
cp ${INPUT_SVG} ${SRC_IMAGES_DIR}/icons/scalable/apps/QOwnNotes.svg
convert_icon_png 128 ${SRC_IMAGES_DIR}/icon-dark.png ${INPUT_DARK_SVG}

echo "Icon conversion complete!"
