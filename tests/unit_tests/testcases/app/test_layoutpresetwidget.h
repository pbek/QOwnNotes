#pragma once

#include <QObject>

class TestLayoutPresetWidget : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void testTransparentBackground();
    void testReuseScene();
};
