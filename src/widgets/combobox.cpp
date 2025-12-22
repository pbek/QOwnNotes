/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include "combobox.h"

#include <QApplication>
#include <QDebug>

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent) {
    // we (sadly) have to watch for all focus changes of the whole app,
    // there is no other way I know of that works reliably with QComboBox
    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)), this,
            SLOT(onFocusChanged(QWidget *, QWidget *)));
}

/**
 * Emits focusIn() and focusOut() signals if the focus of the combo box changed
 */
void ComboBox::onFocusChanged(QWidget *old, QWidget *now) {
    bool focusOld = old != nullptr && isAncestorOf(old);
    bool focusNow = now != nullptr && isAncestorOf(now);

    // check two parents too, because the QComboBoxListView is nested
    if (!focusOld && old != nullptr) {
        QObject *oldParent = old->parent();
        if (oldParent == this) {
            focusOld = true;
        }

        if (!focusOld && oldParent != nullptr) {
            QObject *oldParent2 = oldParent->parent();

            if (oldParent2 == this) {
                focusOld = true;
            }
        }
    }

    // check two parents too, because the QComboBoxListView is nested
    if (!focusNow && now != nullptr) {
        QObject *nowParent = now->parent();
        if (nowParent == this) {
            focusNow = true;
        }

        if (!focusNow && nowParent != nullptr) {
            QObject *nowParent2 = nowParent->parent();

            if (nowParent2 == this) {
                focusNow = true;
            }
        }
    }

    if (!focusOld && focusNow) {
        emit focusIn();
    } else if (focusOld && !focusNow) {
        emit focusOut();
    }
}
