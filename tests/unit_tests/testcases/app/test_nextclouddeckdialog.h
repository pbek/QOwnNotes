#ifndef TESTNEXTCLOUDDECKDIALOG_H
#define TESTNEXTCLOUDDECKDIALOG_H

#include <QObject>

class TestNextcloudDeckDialog : public QObject {
    Q_OBJECT

   private Q_SLOTS:
    void testModes();
    void testInsertLinks_data();
    void testInsertLinks();
    void testPickerDoesNotCreateOrDeleteCards();
    void testNewCardButtons();
    void testLabelsWidget();
    void testLabelPopup();
    void testSearchCardsByTag();
};

#endif    // TESTNEXTCLOUDDECKDIALOG_H
