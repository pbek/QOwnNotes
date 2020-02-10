#!/bin/bash
#

# Variable that will hold the name of the clang-format command
FMT=""

# Some distros just call it clang-format. Others (e.g. Ubuntu) are insistent
# that the version number be part of the command. We prefer clang-format if
# that's present, otherwise we work backwards from highest version to lowest
# version.
for clangfmt in clang-format{,-{4,3}.{9,8,7,6,5,4,3,2,1,0}}; do
    if which "$clangfmt" &>/dev/null; then
        FMT="$clangfmt"
        break
    fi
done

# Check if we found a working clang-format
if [ -z "$FMT" ]; then
    echo "failed to find clang-format"
    exit 1
fi

$FMT -i src/*.cpp
$FMT -i src/*.h
$FMT -i src/entities/*.cpp
$FMT -i src/entities/*.h
$FMT -i src/utils/*.cpp
$FMT -i src/utils/*.h
$FMT -i src/helpers/*.cpp
$FMT -i src/helpers/*.h
$FMT -i src/services/*.cpp
$FMT -i src/services/*.h
$FMT -i src/widgets/*.cpp
$FMT -i src/widgets/*.h
$FMT -i src/dialogs/*.cpp
$FMT -i src/dialogs/*.h
$FMT -i src/api/*.cpp
$FMT -i src/api/*.h

$FMT -i tests/unit_tests/*.cpp
$FMT -i tests/unit_tests/testcases/app/*.cpp
$FMT -i tests/unit_tests/testcases/app/*.h
