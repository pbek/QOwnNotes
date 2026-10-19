#include <services/kdesharemenu.h>

#include <QApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QMenu>
#include <QMimeDatabase>
#include <QTemporaryDir>
#include <QUrl>
#include <QtTest>

class TestKdeShareMenu : public QObject {
    Q_OBJECT

   private:
    QTemporaryDir _dir;

    QString writeFile(const QString &fileName, const QByteArray &content) {
        const QString path = _dir.filePath(fileName);
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(content);
        }
        return path;
    }

   private slots:
    void sharesSingleNote() {
        const QString path = writeFile(QStringLiteral("My note.md"), "# My note\n");
        const QJsonObject data = KdeShareMenu::inputData({path});
        QCOMPARE(data.value(QStringLiteral("urls")).toArray(),
                 QJsonArray{QUrl::fromLocalFile(path).toString()});
        QCOMPARE(data.value(QStringLiteral("mimeType")).toString(),
                 QMimeDatabase().mimeTypeForFile(path).name());
        QCOMPARE(data.value(QStringLiteral("title")).toString(), QStringLiteral("My note"));
    }

    void sharesMultipleFiles() {
        const QString markdown = writeFile(QStringLiteral("a.md"), "# A\n");
        const QString text = writeFile(QStringLiteral("b.txt"), "B\n");
        QJsonObject data = KdeShareMenu::inputData({markdown, text});
        QCOMPARE(data.value(QStringLiteral("urls")).toArray().size(), 2);
        // Targets need to accept all files
        QCOMPARE(data.value(QStringLiteral("mimeType")).toString(), QStringLiteral("text/plain"));
        QVERIFY(!data.contains(QStringLiteral("title")));

        const QString image = writeFile(QStringLiteral("c.png"), "\x89PNG\r\n\x1a\n");
        data = KdeShareMenu::inputData({markdown, image});
        QCOMPARE(data.value(QStringLiteral("mimeType")).toString(),
                 QStringLiteral("application/octet-stream"));
    }

    void hidesMenuWithoutTargets() {
        QWidget parent;
        KdeShareMenu shareMenu(QStringLiteral("Send note to"), &parent);
        QVERIFY(!shareMenu.menu()->menuAction()->isVisible());

        const QString path = writeFile(QStringLiteral("note.md"), "# Note\n");
        const bool hasTargets = shareMenu.setFiles({path});
        // Depends on the installed Purpose plugins
        qInfo() << "Share targets:" << shareMenu.menu()->actions().size();
        if (qEnvironmentVariableIsSet("QON_EXPECT_SHARE_TARGETS")) {
            QVERIFY(hasTargets);
        }
        QCOMPARE(hasTargets, !shareMenu.menu()->isEmpty());
        QCOMPARE(shareMenu.menu()->menuAction()->isVisible(), hasTargets);

        QVERIFY(!shareMenu.setFiles({}));
        QVERIFY(!shareMenu.menu()->menuAction()->isVisible());
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    TestKdeShareMenu tests;
    return QTest::qExec(&tests, argc, argv);
}

#include "test_kdesharemenu.moc"
