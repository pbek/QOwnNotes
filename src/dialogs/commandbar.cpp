#include "commandbar.h"

#include <QAbstractTextDocumentLayout>
#include <QAction>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QTreeView>
#include <QVBoxLayout>

#include "libraries/fuzzy/kfuzzymatcher.h"
#include "models/commandmodel.h"
#include "services/settingsservice.h"
#include "utils/misc.h"

class CommandBarFilterModel : public QSortFilterProxyModel {
   public:
    CommandBarFilterModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}

    Q_SLOT void setFilterString(const QString &string) {
        beginResetModel();
        m_pattern = string;
        endResetModel();
    }

   protected:
    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override {
        const int l = sourceLeft.data(CommandModel::Score).toInt();
        const int r = sourceRight.data(CommandModel::Score).toInt();
        return l < r;
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
        if (m_pattern.isEmpty()) return true;

        const auto idx = sourceModel()->index(sourceRow, 0, sourceParent);
        const QString str = idx.data().toString();
        const QString actionName = str.split(QLatin1Char(':')).at(1);
        const auto res = KFuzzyMatcher::match(m_pattern, actionName);
        sourceModel()->setData(idx, res.score, CommandModel::Score);
        return res.matched;
    }

   private:
    QString m_pattern;
};

class CommandBarStyleDelegate : public QStyledItemDelegate {
   public:
    CommandBarStyleDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        QStyleOptionViewItem options = option;
        initStyleOption(&options, index);
        SettingsService settings;
        const bool hideMenuIcons = Utils::Misc::areMenuIconsHidden();
        if (hideMenuIcons) options.icon = QIcon();    // remove icon

        QTextDocument doc;

        const auto original = index.data().toString();

        const auto strs = index.data().toString().split(QLatin1Char(':'));
        QString str = strs.at(1);
        const QString nameColor = option.palette.color(QPalette::Link).name();
        str = KFuzzyMatcher::toFuzzyMatchedDisplayString(
            m_filterString, str, QStringLiteral("<b style=\"color:%1;\">").arg(nameColor),
            QStringLiteral("</b>"));
        //        fts::to_fuzzy_matched_display_string(m_filterString, str, QStringLiteral("<b
        //        style=\"color:%1;\">").arg(nameColor), QStringLiteral("</b>"));

        const QString component = QStringLiteral("<span style=\"color: gray;\">") + strs.at(0) +
                                  QStringLiteral(": </span>");

        str = component + str;
        bool overrideInterfaceFontSize =
            settings.value(QStringLiteral("overrideInterfaceFontSize"), false).toBool();
        if (overrideInterfaceFontSize) {
            str = QStringLiteral("<span style=\"font-size: %1px;\">")
                      .arg(settings.value(QStringLiteral("interfaceFontSize"), 11).toString()) +
                  str + QStringLiteral("</span>");
        }

        doc.setHtml(str);
        doc.setDocumentMargin(2);

        painter->save();

        // paint background
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
        } else {
            painter->fillRect(option.rect, option.palette.base());
        }

        options.text = QString();    // clear old text
        options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter,
                                             options.widget);

        // fix stuff for rtl
        // QTextDocument doesn't work with RTL text out of the box so we give it a hand here by
        // increasing the text width to our rect size. Icon displacement is also calculated here
        // because 'translate()' later will not work.
        const bool rtl = original.isRightToLeft();
        if (rtl) {
            auto r = options.widget->style()->subElementRect(QStyle::SE_ItemViewItemText, &options,
                                                             options.widget);
            auto hasIcon = index.data(Qt::DecorationRole).value<QIcon>().isNull();
            if (hasIcon && !hideMenuIcons)
                doc.setTextWidth(r.width() - 25);
            else
                doc.setTextWidth(r.width());
        }

        // draw text
        painter->translate(option.rect.x(), option.rect.y());
        // leave space for icon

        if (!rtl && !hideMenuIcons) painter->translate(25, 0);

        QAbstractTextDocumentLayout::PaintContext ctx;
        ctx.palette.setColor(QPalette::Text, options.palette.text().color());
        doc.documentLayout()->draw(painter, ctx);

        painter->restore();
    }

   public Q_SLOTS:
    void setFilterString(const QString &text) { m_filterString = text; }

   private:
    QString m_filterString;
};

class ShortcutStyleDelegate : public QStyledItemDelegate {
   public:
    ShortcutStyleDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        QStyleOptionViewItem options = option;
        initStyleOption(&options, index);

        QTextDocument doc;

        const auto strs = index.data().toString();
        doc.setDocumentMargin(2);
        doc.setHtml(strs);

        painter->save();

        // paint background
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
        } else {
            painter->fillRect(option.rect, option.palette.base());
        }

        options.text = QString();    // clear old text
        options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter,
                                             options.widget);

        if (!strs.isEmpty()) {
            // collect button-style pixmaps
            QVector<QPair<QRect, QString>> btnRects;
            auto list = strs.split(QLatin1Char('+'));
            for (auto text : list) {
                auto r = option.fontMetrics.boundingRect(text);
                r.setWidth(r.width() + 8);
                r.setHeight(r.height() + 4);
                btnRects.append({r, text});
            }

            auto plusRect = option.fontMetrics.boundingRect(QLatin1Char('+'));

            // draw them
            int dx = option.rect.x();
            int y = option.rect.y();
            int py = option.rect.y() + plusRect.height() / 2;
            int total = btnRects.size();
            int i = 0;
            painter->setRenderHint(QPainter::Antialiasing);    // :)
            for (const auto &pxm : btnRects) {
                // draw rounded rect shadown
                painter->setPen(Qt::NoPen);
                QRect r(dx, y, pxm.first.width(), pxm.first.height());
                auto shadow = r.translated(0, 1);

                painter->setBrush(option.palette.shadow());
                painter->drawRoundedRect(shadow, 3, 3);

                // draw rounded rect itself
                painter->setBrush(option.palette.button());
                painter->drawRoundedRect(r, 3, 3);

                // draw text inside rounded rect
                painter->setPen(option.palette.buttonText().color());
                painter->drawText(r, Qt::AlignCenter, pxm.second);

                // draw '+'
                if (i + 1 < total) {
                    dx += pxm.first.width() + 8;
                    painter->drawText(QPoint(dx, py + (pxm.first.height() / 2)),
                                      QStringLiteral("+"));
                    dx += plusRect.width() + 8;
                }
                i++;
            }
        }

        painter->restore();
    }
};

CommandBar::CommandBar(QWidget *parent) : QMenu(parent) {
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(4, 4, 4, 4);
    setLayout(layout);

    m_lineEdit = new QLineEdit(this);
    setFocusProxy(m_lineEdit);

    layout->addWidget(m_lineEdit);

    m_treeView = new QTreeView();
    layout->addWidget(m_treeView, 1);
    m_treeView->setTextElideMode(Qt::ElideLeft);
    m_treeView->setUniformRowHeights(true);

    m_model = new CommandModel(this);

    CommandBarStyleDelegate *delegate = new CommandBarStyleDelegate(this);
    ShortcutStyleDelegate *del = new ShortcutStyleDelegate(this);
    m_treeView->setItemDelegateForColumn(0, delegate);
    m_treeView->setItemDelegateForColumn(1, del);

    m_proxyModel = new CommandBarFilterModel(this);
    m_proxyModel->setFilterRole(Qt::DisplayRole);
    m_proxyModel->setSortRole(CommandModel::Score);
    m_proxyModel->setFilterKeyColumn(0);

    connect(m_lineEdit, &QLineEdit::returnPressed, this, &CommandBar::slotReturnPressed);
    connect(m_lineEdit, &QLineEdit::textChanged, m_proxyModel,
            &CommandBarFilterModel::setFilterString);
    connect(m_lineEdit, &QLineEdit::textChanged, delegate,
            &CommandBarStyleDelegate::setFilterString);
    connect(m_lineEdit, &QLineEdit::textChanged, this, [this]() {
        m_treeView->viewport()->update();
        reselectFirst();
    });
    connect(m_treeView, &QTreeView::clicked, this, &CommandBar::slotReturnPressed);

    m_proxyModel->setSourceModel(m_model);
    m_treeView->setSortingEnabled(true);
    m_treeView->setModel(m_proxyModel);

    m_treeView->installEventFilter(this);
    m_lineEdit->installEventFilter(this);

    m_treeView->setHeaderHidden(true);
    m_treeView->setRootIsDecorated(false);
    m_treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_treeView->setSelectionMode(QTreeView::SingleSelection);

    setHidden(true);
}

void CommandBar::updateBar(const QVector<QPair<QString, QAction *>> &actions) {
    m_model->refresh(actions);
    reselectFirst();

    updateViewGeometry();
    show();
    setFocus();
}

bool CommandBar::eventFilter(QObject *obj, QEvent *event) {
    // catch key presses + shortcut overrides to allow to have ESC as application wide shortcut,
    // too, see bug 409856
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::ShortcutOverride) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (obj == m_lineEdit) {
            const bool forward2list =
                (keyEvent->key() == Qt::Key_Up) || (keyEvent->key() == Qt::Key_Down) ||
                (keyEvent->key() == Qt::Key_PageUp) || (keyEvent->key() == Qt::Key_PageDown);
            if (forward2list) {
                QCoreApplication::sendEvent(m_treeView, event);
                return true;
            }

            if (keyEvent->key() == Qt::Key_Escape) {
                m_lineEdit->clear();
                keyEvent->accept();
                hide();
                return true;
            }
        } else {
            const bool forward2input =
                (keyEvent->key() != Qt::Key_Up) && (keyEvent->key() != Qt::Key_Down) &&
                (keyEvent->key() != Qt::Key_PageUp) && (keyEvent->key() != Qt::Key_PageDown) &&
                (keyEvent->key() != Qt::Key_Tab) && (keyEvent->key() != Qt::Key_Backtab);
            if (forward2input) {
                QCoreApplication::sendEvent(m_lineEdit, event);
                return true;
            }
        }
    }

    // hide on focus out, if neither input field nor list have focus!
    else if (event->type() == QEvent::FocusOut &&
             !(m_lineEdit->hasFocus() || m_treeView->hasFocus())) {
        m_lineEdit->clear();
        hide();
        return true;
    }

    return QWidget::eventFilter(obj, event);
}

void CommandBar::slotReturnPressed() {
    auto act = m_proxyModel->data(m_treeView->currentIndex(), Qt::UserRole).value<QAction *>();
    if (act) {
        // if the action is a menu, we take all its actions
        // and reload our dialog with these instead.
        if (auto menu = act->menu()) {
            auto menuActions = menu->actions();
            QVector<QPair<QString, QAction *>> list;
            list.reserve(menuActions.size());

            // if there are no actions, trigger load actions
            // this happens with some menus that are loaded on demand
            if (menuActions.size() == 0) {
                Q_EMIT menu->aboutToShow();
                menuActions = menu->actions();
            }

            for (auto menuAction : Utils::asConst(menuActions)) {
                if (menuAction) {
                    list.append({Utils::Misc::removeAcceleratorMarker(act->text()), menuAction});
                }
            }
            m_model->refresh(list);
            m_lineEdit->clear();
            return;
        } else {
            act->trigger();
        }
    }
    m_lineEdit->clear();
    hide();
}

void CommandBar::reselectFirst() {
    QModelIndex index = m_proxyModel->index(0, 0);
    m_treeView->setCurrentIndex(index);
}

void CommandBar::updateViewGeometry() {
    m_treeView->resizeColumnToContents(0);
    m_treeView->resizeColumnToContents(1);

    const QSize centralSize = parentWidget()->size();

    // width: 1/2 of editor, height: 1/2 of editor
    const QSize viewMaxSize(centralSize.width() / 2, centralSize.height() / 2);

    // Position should be central over window
    const int xPos = std::max<int>(0, (centralSize.width() - viewMaxSize.width()) / 2);
    const int yPos = std::max<int>(0, (centralSize.height() - viewMaxSize.height()) * 1 / 4);

    const QPoint p(xPos, yPos);
    move(p + parentWidget()->pos());

    this->setFixedSize(viewMaxSize);
}
