#ifndef COMMANDBAR_H
#define COMMANDBAR_H

#include <QMenu>

class QMenuBar;
class QTreeView;
class CommandModel;
class CommandBarFilterModel;
class QLineEdit;

class CommandBar : public QMenu {
    Q_OBJECT
   public:
    explicit CommandBar(QWidget* parent = nullptr);

    void updateBar(const QVector<QPair<QString, QAction*>>& actions);

    void updateViewGeometry();

   protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

   private Q_SLOTS:
    void slotReturnPressed();
    void reselectFirst();

   private:
    QTreeView* m_treeView;
    QLineEdit* m_lineEdit;
    CommandModel* m_model;
    CommandBarFilterModel* m_proxyModel;
};

#endif    // COMMANDBAR_H
