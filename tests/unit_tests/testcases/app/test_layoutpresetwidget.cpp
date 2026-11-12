#include "test_layoutpresetwidget.h"

#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QtTest>

#include "widgets/layoutpresetwidget.h"

void TestLayoutPresetWidget::testTransparentBackground() {
    QWidget dialog;
    dialog.resize(800, 600);
    LayoutPresetWidget widget(&dialog);
    widget.resize(dialog.size());
    auto *view = widget.findChild<QGraphicsView *>(QStringLiteral("layoutPresetGraphicsView"));
    QVERIFY(view != nullptr);
    QPixmap image(100, 100);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.fillRect(40, 40, 20, 20, Qt::red);
    painter.end();
    view->scene()->addPixmap(image);
    dialog.show();

    // Stylesheets need not update palette().window(), so test both themes after construction.
    for (const QString &color : {QStringLiteral("#303030"), QStringLiteral("#f0f0f0")}) {
        dialog.setStyleSheet(QStringLiteral("QWidget { background-color: %1; }").arg(color));
        QCoreApplication::processEvents();
        widget.resizeLayoutPresetImage();
        // Capture the parent too: grabbing a transparent viewport alone returns transparent pixels.
        const QImage rendered = dialog.grab().toImage();
        const QPoint origin = view->viewport()->mapTo(&dialog, QPoint(0, 0));
        QVERIFY(!rendered.isNull());
        QCOMPARE(rendered.pixelColor(origin), QColor(color));
        QCOMPARE(rendered.pixelColor(origin + QPoint(view->viewport()->width() / 2,
                                                     view->viewport()->height() / 2)),
                 QColor(Qt::red));
    }
}

void TestLayoutPresetWidget::testReuseScene() {
    LayoutPresetWidget widget;
    auto *view = widget.findChild<QGraphicsView *>(QStringLiteral("layoutPresetGraphicsView"));
    auto *combo = widget.findChild<QComboBox *>(QStringLiteral("layoutPresetComboBox"));
    QVERIFY(view != nullptr);
    QVERIFY(combo != nullptr);
    auto *scene = view->scene();
    QVERIFY(scene != nullptr);
    QCOMPARE(scene->parent(), view);
    combo->addItem(QStringLiteral("Minimal"), QStringLiteral("minimal"));
    combo->addItem(QStringLiteral("Full"), QStringLiteral("full"));
    for (int i = 0; i < 10; ++i) {
        combo->setCurrentIndex(i % combo->count());
        QCOMPARE(view->scene(), scene);
    }
    QCOMPARE(view->findChildren<QGraphicsScene *>().count(), 1);
}
