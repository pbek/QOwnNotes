#pragma once

#include <QString>

namespace Utils {
// Copy a directory atomically without merging, overwriting, or following symlinks.
bool copyDirectory(const QString &sourcePath, const QString &destinationPath, QString *error);
}    // namespace Utils
