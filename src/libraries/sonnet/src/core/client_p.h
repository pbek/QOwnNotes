/*
 *
 * Copyright (C)  2003  Zack Rusin <zack@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#ifndef SONNET_CLIENT_P_H
#define SONNET_CLIENT_P_H

#include <QObject>
#include <QString>
#include <QStringList>
/*
 * The fact that this class inherits from QObject makes me
 * hugely unhappy. The reason for as of this writing is that
 * I don't really feel like writing my own KLibFactory
 * that would load anything else then QObject derivatives.
 */
namespace Sonnet {
class SpellerPlugin;

/**
 * \internal
 * Client
 */
class Client : public QObject {
    Q_OBJECT
   public:
    explicit Client(QObject *parent = nullptr);

    /**
     * @returns how reliable the answer is (higher is better).
     */
    virtual int reliability() const = 0;

    /**
     * Returns a dictionary for the given language.
     *
     * @param language specifies the language of the dictionary. If an
     *        empty string is passed the default language will be
     *        used. Has to be one of the values returned by
     *        languages()
     *
     * @returns a dictionary for the language or 0 if there was an error.
     */
    virtual SpellerPlugin *createSpeller(const QString &language) = 0;

    /**
     * @returns a list of supported languages.
     */
    virtual QStringList languages() const = 0;

    /**
     * @returns the name of the implementating class.
     */
    virtual QString name() const = 0;
};
}    // namespace Sonnet

Q_DECLARE_INTERFACE(Sonnet::Client, "org.kde.sonnet.Client")

#endif    // SONNET_CLIENT_P_H
