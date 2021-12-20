/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of QLiteHtml.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qlitehtmlwidget.h>

#include <QAction>
#include <QApplication>
#if QT_CONFIG(clipboard)
#include <QClipboard>
#endif
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLoggingCategory>
#include <QMenuBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPushButton>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

Q_LOGGING_CATEGORY(log, "qlitehtml.browser")

class BrowserWindow : public QWidget
{
    Q_OBJECT

public:
    BrowserWindow();

private:
    QNetworkAccessManager m_nam;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    BrowserWindow w;
    w.show();
    return app.exec();
}

BrowserWindow::BrowserWindow()
{
    auto vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(0, 0, 0, 0);
    setLayout(vlayout);

#if QT_CONFIG(clipboard)
    auto menuBar = new QMenuBar;
    vlayout->addWidget(menuBar);
    auto editMenu = menuBar->addMenu(tr("Edit"));
    auto copyAction = editMenu->addAction(tr("Copy"));
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setEnabled(false);

    auto toolBar = new QToolBar;
    vlayout->addWidget(toolBar);
    toolBar->addAction(copyAction);
#endif

    auto centerWidget = new QWidget;
    auto centerLayout = new QVBoxLayout;
    centerWidget->setLayout(centerLayout);
    vlayout->addWidget(centerWidget, 10);
    auto urlLayout = new QHBoxLayout;
    urlLayout->addWidget(new QLabel(tr("URL:")));
    auto urlInput = new QLineEdit;
    urlLayout->addWidget(urlInput);
    auto browseButton = new QPushButton(tr("Browse..."));
    urlLayout->addWidget(browseButton);

    centerLayout->addLayout(urlLayout);
    auto htmlWidget = new QLiteHtmlWidget;
    centerLayout->addWidget(htmlWidget);

    auto statusBar = new QStatusBar;
    vlayout->addWidget(statusBar);

    resize(1000, 650);

    htmlWidget->setResourceHandler([this](const QUrl &url) {
        // create blocking request
        // TODO implement asynchronous requests in container_qpainter
        QEventLoop loop;
        QByteArray data;
        qCDebug(log) << "Resource requested:" << url;
        QNetworkReply *reply = m_nam.get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::finished, this, [&data, &loop, reply] {
            qCDebug(log) << "Resource finished:" << reply->url() << reply->error();
            if (reply->error() == QNetworkReply::NoError)
                data = reply->readAll();
            reply->deleteLater();
            loop.exit();
        });
        loop.exec(QEventLoop::ExcludeUserInputEvents);
        return data;
    });
    connect(htmlWidget, &QLiteHtmlWidget::linkHighlighted, statusBar, [statusBar](const QUrl &url) {
        statusBar->showMessage(url.toString());
    });
#if QT_CONFIG(clipboard)
    connect(htmlWidget, &QLiteHtmlWidget::copyAvailable, copyAction, &QAction::setEnabled);
    connect(copyAction, &QAction::triggered, htmlWidget, [htmlWidget] {
        QGuiApplication::clipboard()->setText(htmlWidget->selectedText());
    });
#endif

    const auto loadUrl = [this, htmlWidget, urlInput, browseButton] {
        urlInput->setEnabled(false);
        browseButton->setEnabled(false);
        const QUrl url = QUrl(urlInput->text().trimmed());
        qCDebug(log) << "Url requested:" << url;
        QNetworkReply *reply = m_nam.get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::finished, this, [htmlWidget, urlInput, browseButton, reply] {
            qCDebug(log) << "Url finished:" << reply->url() << reply->error();
            if (reply->error() == QNetworkReply::NoError) {
                const QByteArray data = reply->readAll();
                htmlWidget->setUrl(reply->url());
                htmlWidget->setHtml(QString::fromUtf8(data));
            }
            urlInput->setEnabled(true);
            browseButton->setEnabled(true);
            reply->deleteLater();
        });
    };
    connect(urlInput, &QLineEdit::returnPressed, this, loadUrl);
    connect(browseButton, &QPushButton::clicked, this, [this, urlInput, loadUrl] {
        const QUrl url = QFileDialog::getOpenFileUrl(this, tr("Open File"));
        if (url.isValid()) {
            urlInput->setText(url.toString());
            loadUrl();
        }
    });

    QAction *action;
    action = new QAction(tr("Enter location"));
    action->setShortcut({"Ctrl+L"});
    connect(action, &QAction::triggered, [urlInput] { urlInput->setFocus(); });
    addAction(action);
}

#include "main.moc"
