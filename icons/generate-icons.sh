#!/usr/bin/env bash

# Exit on any error
set -e

# Configuration
SRC_IMAGES_DIR="../src/images"
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
convert_icon() {
    local size=$1
    local output_file=$2
    local input_file=$3

    # Use fallback to $INPUT_SVG if no local input file is provided
    if [ -z "$input_file" ]; then
        input_file=$INPUT_SVG
    fi

    echo "Converting to ${size}x${size} -> ${output_file}"
#    magick "$input_file" -resize "${size}x${size}" "$output_file"
    rsvg-convert -w "$size" -h "$size" "$input_file" -o "$output_file"
}

# Generate all sizes
convert_icon 128 "icon.png"
convert_icon 16 "icon.icns"
convert_icon 48 "icon.ico"
convert_icon 800 "icon-800.png"
convert_icon 512 "../snap/gui/QOwnNotes.png"
convert_icon 512 "../build-systems/github/snap.qt6/gui/QOwnNotes.png"
convert_icon 512 "../build-systems/snap/snapcraft/snap/gui/QOwnNotes.png"

# Generate app icons
for size in "${SIZES_APP[@]}"; do
    convert_icon ${size} "${SRC_IMAGES_DIR}/icons/${size}x${size}/apps/${APP_NAME}.png"
done

# Copy to src/images
echo "Copying to src/images"
cp icon.png ${SRC_IMAGES_DIR}/icon.png
cp ${INPUT_DARK_SVG} ${SRC_IMAGES_DIR}/icon-dark.svg
convert_icon 128 ${SRC_IMAGES_DIR}/icon-dark.png ${INPUT_DARK_SVG}

echo "Icon conversion complete!"
