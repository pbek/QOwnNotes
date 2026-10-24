#pragma once

#include <QObject>

class TestUtilsGit : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void testPrepareLogProcess_data();
    void testPrepareLogProcess();
    void testInvalidLogCommand_data();
    void testInvalidLogCommand();
    void testShowLog_data();
    void testShowLog();
};
