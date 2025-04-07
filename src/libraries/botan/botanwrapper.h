#ifndef BOTANWRAPPER_H
#define BOTANWRAPPER_H

#include <QString>
#include <QVector>

class BotanWrapper {
   public:
    BotanWrapper();

    /*!
     * Creates a hash
     * @param Data The string to hash
     */
    QString Hash(const QString &Data);

    /*!
     * Returns a Base64 encrypted QString
     * @param Data The string to encypt
     */
    QString Encrypt(const QString &Data);

    /*!
     * Returns a decrypted string from a Base64 encypted string
     * @param Data The string to encypt
     */
    QString Decrypt(const QString &Data);

    /*!
     * Sets the Password
     * @param Password The password
     */
    void setPassword(const QString &Password);

    /*!
     * Sets the Salt
     * @param Salt The salt value
     */
    void setSalt(const QString &Salt);

   private:
    QVector<uint8_t> mSalt;
    QString mPassword;
};

#endif    // BOTANWRAPPER_H
