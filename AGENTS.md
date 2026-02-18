# QOwnNotes - Agents Instructions

## Project Overview

QOwnNotes is an open source notepad with Markdown support and todo list manager for GNU/Linux, macOS, and Windows. It works together with Nextcloud Notes and ownCloud Notes.

- **Language**: C++
- **Framework**: Qt (supports both Qt5 and Qt6)
- **Build Systems**:
  - **qmake** (`.pro` files) - Primary build system
  - **CMake** (`CMakeLists.txt`) - Alternative build system
- **Task Runner**: just (justfile)

## Building the Application

### Quick Build (Recommended for Development)

Use the `just` task runner to build the application:

```bash
just src-build
```

This command will:

1. Check qmake version
2. Create the `build-QOwnNotes` directory
3. Run qmake with debug configuration and system Botan
4. Build using all available CPU cores with `make -j$(nproc)`

### Alternative Build Commands

- **Run tests**: `just src-test` (or `just test`)
- **Clean build directory**: `just src-clean`
- **Build and run**: `just src-build-run`
- **Run built application**: `just src-run`

### Building with Nix

For Nix users, multiple build options are available:

- **Qt6 build**: `just nix-build` or `just nix-build-qt6`
- **Qt5 build**: `just nix-build-qt5`
- **Force rebuild**: `just nix-build-force`

### Manual Build with CMake

Alternatively, use CMake for building:

```bash
mkdir build && cd build

# Configure with Qt6
cmake ../src -DQON_QT6_BUILD=ON -DDEV_MODE=ON -DCMAKE_BUILD_TYPE=Debug

# Or with Qt5 (default)
cmake ../src -DDEV_MODE=ON -DCMAKE_BUILD_TYPE=Debug

# Build
make -j4
```

**Available CMake Options:**

- `QON_QT6_BUILD=ON` - Build with Qt6 (default: OFF, uses Qt5)
- `DEV_MODE=ON` - Enable developer mode with extra warnings
- `BUILD_WITH_LIBGIT2=ON` - Use system libgit2 library
- `BUILD_WITH_ASPELL=ON` - Enable aspell spell checking support
- `USE_QLITEHTML=ON` - Use QLiteHtml for preview
- `CMAKE_BUILD_TYPE=Debug|Release` - Build type

Example with multiple options:

```bash
cmake ../src \
  -DQON_QT6_BUILD=ON \
  -DDEV_MODE=ON \
  -DBUILD_WITH_LIBGIT2=ON \
  -DBUILD_WITH_ASPELL=ON \
  -DCMAKE_BUILD_TYPE=Debug
```

## Project Structure

- `src/` - Main source code directory
  - `QOwnNotes.pro` - Main qmake project file
  - `CMakeLists.txt` - CMake build configuration
  - `libraries/` - Third-party libraries (submodules)
- `tests/` - Test files
- `build-QOwnNotes/` - Default build directory (gitignored)
- `justfile` - Task runner configuration
- `scripts/` - Helper scripts
- `docs/` - Documentation

## Key Libraries & Dependencies

### Required Qt Modules

- Qt Core, GUI, Widgets, SQL, SVG, Network, XML
- Qt PrintSupport, QML, WebSockets, Concurrent
- Qt Quick (for Qt6, enables more scripting options)

### Optional Dependencies

- **Botan**: Cryptographic library (can use system or internal version)
  - Default build uses system Botan: `USE_SYSTEM_BOTAN=1`
- **Aspell**: Spell checking support
- **QLiteHtml**: HTML rendering (optional, enable with `USE_QLITEHTML`)

## Development Configuration

### Build Modes (qmake)

- **Debug Mode**: `CONFIG+=debug` - Includes debug symbols
- **Dev Mode**: `CONFIG+=DEV_MODE` - Enables higher warning levels and precompiled headers
- **Integration Tests**: `DEFINES+=INTEGRATION_TESTS` - For test builds

### Build Modes (CMake)

- **CMAKE_BUILD_TYPE=Debug** - Debug build with symbols
- **CMAKE_BUILD_TYPE=Release** - Optimized release build
- **DEV_MODE=ON** - Developer mode with extra warnings and -Werror on Linux
- **INTEGRATION_TESTS** - For test builds

### Common Configuration

The recommended development build uses:

**qmake:**

```bash
qmake "CONFIG+=debug USE_SYSTEM_BOTAN=1" ../src/QOwnNotes.pro CONFIG+=DEV_MODE
```

**CMake:**

```bash
cmake ../src -DCMAKE_BUILD_TYPE=Debug -DDEV_MODE=ON
```

## Testing

Run the test suite:

```bash
just src-test
```

This will:

1. Build tests in `build-tests-QOwnNotes/` directory
2. Run tests in minimal platform mode
3. Clean up the test build directory

For VM tests (integration tests):

```bash
just vm-test
```

## Code Quality & Linting

### Format Code

Format the entire project with clang-format:

```bash
just clang-format
# Or aliases:
just fix-linting
just linter-fix
```

### Check Links

Validate links in Markdown files:

```bash
just link-check
```

## Translation Workflow

- **Download translations**: `just translations-download`
- **Build translations**: `just translations-build`
- **Upload to Crowdin**: `just translations-upload`
- **Update translation files**: `just translations-update-files`

Translations are managed via Crowdin: https://crowdin.com/project/qownnotes

## Important Notes for AI Assistants

1. **Always use `just src-build` to build the project** - Don't try to run qmake/make directly unless specifically requested
2. **Always use `just nix-build` to build the project when asked to use nix**
3. **The project uses Qt/C++** - Be mindful of Qt's signal/slot mechanism, QObject lifecycle, and Qt-specific patterns
4. **Submodules are used** - Libraries in `src/libraries/` are git submodules
5. **Multiple Qt versions supported** - Code must work with both Qt5 and Qt6
6. **Cross-platform** - Code must work on Linux, macOS, and Windows
7. **Two build systems available** - Primary is qmake (`.pro` files), but CMake (`CMakeLists.txt`) is also supported
8. **Build projects** - The primary build project files are `src/QOwnNotes.pro` and `src/CMakeLists.txt`
9. **Development mode** - Use `CONFIG+=DEV_MODE` for qmake or `-DDEV_MODE=ON` for CMake in active development
10. **Version bumping** - Update `src/version.h` and `src/build_number.h`, `BUILD` just needs to be increase by one
    - Version format is `MAJOR.MINOR.PATCH` (e.g. `22.12.0`), build number is a separate integer that increments with each release
    - The MAJOR part is always the last two digits of the year of the release (e.g. `22` for 2022),
    - The MINOR part is the month of the release (e.g. `12` for December),
    - The PATCH part is a counter that starts at `0` and increments with each release in the same month
      (e.g. `0` for the first release in December 2022, `1` for the second release in December 2022, etc.)
11. **This project has translations** - They can be found in the `src/translations/` directory as `.ts` files
12. **Comments** - Always write clear and concise comments for complex logic, especially when dealing with Qt-specific features, start comments with uppercase letters
13. **Changelog** - Update the changelog with each release, following the format in `CHANGELOG.md`

## Minimum Requirements

- Qt 5.5+ or Qt 6.0+
- GCC 4.8+
- Desktop operating system supporting Qt

## Useful Resources

- Website: https://www.qownnotes.org
- Documentation: https://www.qownnotes.org/getting-started/concept.html
- GitHub Issues: https://github.com/pbek/QOwnNotes/issues
- Script Repository: https://github.com/qownnotes/scripts
- Changelog: https://www.qownnotes.org/changelog.html

## Quick Reference Commands

```bash
# Build and run
just src-build-run

# Run tests
just test

# Format code
just clang-format

# Clean build
just src-clean

# List all available commands
just --list
```
