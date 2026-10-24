#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

// A real executable lets the regression tests check argument boundaries without a shell.
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    const QStringList arguments = app.arguments();
    if (arguments.size() < 2) {
        return 1;
    }

    QSaveFile output(arguments.at(1));
    if (!output.open(QIODevice::WriteOnly)) {
        return 1;
    }
    const QJsonObject result{
        {QStringLiteral("arguments"), QJsonArray::fromStringList(arguments.mid(2))},
        {QStringLiteral("workingDirectory"), QDir::currentPath()}};
    const QByteArray data = QJsonDocument(result).toJson();
    return output.write(data) == data.size() && output.commit() ? 0 : 1;
}
