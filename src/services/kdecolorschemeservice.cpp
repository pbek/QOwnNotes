#include "kdecolorschemeservice.h"

#include <KColorScheme>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QApplication>
#include <QCollator>
#include <QDir>
#include <QFileInfo>
#include <QPalette>
#include <QStandardPaths>
#include <QVariant>
#include <algorithm>
#include <map>

const QString KdeColorSchemeService::settingsKey = QStringLiteral("kdeColorScheme");

namespace {
// Tells the Plasma platform theme to keep the application palette when the system color
// scheme changes, and to use the scheme for the window decoration
const char schemePathProperty[] = "KDE_COLOR_SCHEME_PATH";

bool s_schemeApplied = false;
QString s_activeSchemeId;
// Value of the property before a scheme was applied (e.g. set by a platform theme)
QVariant s_initialSchemePath;
}    // namespace

QList<KdeColorSchemeService::Scheme> KdeColorSchemeService::availableSchemes() {
    // Like KColorSchemeModel: a scheme in the user's data directory overrides a system
    // scheme with the same file name, because locateAll() returns it first
    const QStringList dirPaths =
        QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                  QStringLiteral("color-schemes"), QStandardPaths::LocateDirectory);
    std::map<QString, QString> schemeFiles;

    for (const QString &dirPath : dirPaths) {
        const QDir dir(dirPath);
        const QStringList fileNames =
            dir.entryList({QStringLiteral("*.colors")}, QDir::Files | QDir::Readable);

        for (const QString &fileName : fileNames) {
            schemeFiles.insert({fileName, dir.filePath(fileName)});
        }
    }

    QList<Scheme> schemes;
    schemes.reserve(static_cast<int>(schemeFiles.size()));

    for (const auto &schemeFile : schemeFiles) {
        const QString &path = schemeFile.second;
        const KSharedConfigPtr config = KSharedConfig::openConfig(path, KConfig::SimpleConfig);
        const KConfigGroup group(config, QStringLiteral("General"));
        const QString id = QFileInfo(schemeFile.first).completeBaseName();
        schemes.append({id, group.readEntry("Name", id), path});
    }

    QCollator collator;
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    std::sort(schemes.begin(), schemes.end(), [&collator](const Scheme &a, const Scheme &b) {
        return collator.compare(a.name, b.name) < 0;
    });

    return schemes;
}

bool KdeColorSchemeService::apply(const QString &schemeId, bool darkModeTheme) {
    QString schemePath;

    if (!schemeId.isEmpty() && !darkModeTheme) {
        const QList<Scheme> schemes = availableSchemes();
        const auto it = std::find_if(schemes.cbegin(), schemes.cend(),
                                     [&schemeId](const Scheme &s) { return s.id == schemeId; });

        if (it != schemes.cend()) {
            schemePath = it->path;
        }
    }

    if (schemePath.isEmpty()) {
        if (s_schemeApplied) {
            // Restore the palette of the platform theme, which also follows system changes
            // again once the property is reset
            qApp->setProperty(schemePathProperty, s_initialSchemePath);
            QApplication::setPalette(QPalette());
            s_schemeApplied = false;
            s_activeSchemeId.clear();
        }

        return false;
    }

    if (!s_schemeApplied) {
        s_initialSchemePath = qApp->property(schemePathProperty);
    }

    // The property needs to be set before the palette changes, because the Plasma platform
    // theme checks it when it handles the palette change
    qApp->setProperty(schemePathProperty, schemePath);
    QApplication::setPalette(
        KColorScheme::createApplicationPalette(KSharedConfig::openConfig(schemePath)));
    s_schemeApplied = true;
    s_activeSchemeId = schemeId;

    return true;
}

QString KdeColorSchemeService::activeSchemeId() { return s_activeSchemeId; }
