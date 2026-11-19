#pragma once

#include <QObject>

class TestTimePickerButton : public QObject {
    Q_OBJECT

   private slots:
    void testDialValues();
    void testDialKeyboard();
    void testPopupAppliesTime();
    void testPopupEscapeRestoresTime();
    void testFollowsEnabledState();
    void testDarkModeStyleSheet();
};
