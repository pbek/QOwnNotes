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

    QString EncryptV2(const QString &Data, const QString &saltBase64, const QString &nonceBase64,
                      size_t iterations, QString *macBase64);

    QString DecryptV2(const QString &Data, const QString &saltBase64, const QString &nonceBase64,
                      const QString &macBase64, size_t iterations);

    static QString randomBytesBase64(int size);

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
