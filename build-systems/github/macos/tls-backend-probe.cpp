#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QTextStream>

#ifdef Q_OS_MACOS
#include <mach-o/dyld.h>

bool imageLoadedFrom(const QString &fileName, const QString &directory) {
    const QString expectedPath = QDir(directory).canonicalPath() + QLatin1Char('/') + fileName;
    for (uint32_t index = 0; index < _dyld_image_count(); ++index) {
        const char *imageName = _dyld_get_image_name(index);
        if (imageName != nullptr &&
            QFileInfo(QString::fromUtf8(imageName)).canonicalFilePath() == expectedPath) {
            return true;
        }
    }

    return false;
}
#endif

int main(int argc, char *argv[]) {
    QCoreApplication application(argc, argv);
    QTextStream error(stderr);

#ifdef Q_OS_MACOS
    if (application.arguments().size() != 3) {
        error << "Usage: qownnotes-tls-backend-probe <plugin directory> <framework directory>\n";
        return 1;
    }

    const QString pluginDirectory = application.arguments().at(1);
    const QString frameworkDirectory = application.arguments().at(2);
    QCoreApplication::setLibraryPaths({pluginDirectory});
#endif

    if (!QSslSocket::supportsSsl() || QSslSocket::activeBackend() != QStringLiteral("openssl")) {
        error << "Qt did not select the OpenSSL backend by default\n";
        return 1;
    }

    if (!QSslSocket::isFeatureSupported(QSsl::SupportedFeature::CertificateVerification)) {
        error << "The OpenSSL backend does not support certificate verification\n";
        return 1;
    }

    if (QSslConfiguration::systemCaCertificates().isEmpty()) {
        error << "The OpenSSL backend cannot access the system CA certificates\n";
        return 1;
    }

    if (!QSslSocket::isProtocolSupported(QSsl::TlsV1_2) ||
        !QSslSocket::isProtocolSupported(QSsl::TlsV1_3)) {
        error << "The OpenSSL backend must support TLS 1.2 and TLS 1.3\n";
        return 1;
    }

#ifdef Q_OS_MACOS
    if (!imageLoadedFrom(QStringLiteral("libqopensslbackend.dylib"),
                         pluginDirectory + QStringLiteral("/tls")) ||
        !imageLoadedFrom(QStringLiteral("libssl.3.dylib"), frameworkDirectory) ||
        !imageLoadedFrom(QStringLiteral("libcrypto.3.dylib"), frameworkDirectory)) {
        error << "Qt did not load the packaged TLS plugin and OpenSSL libraries\n";
        return 1;
    }
#endif

    QTextStream(stdout) << "Qt TLS backend: " << QSslSocket::activeBackend() << " ("
                        << QSslSocket::sslLibraryVersionString() << ")\n";
    return 0;
}
