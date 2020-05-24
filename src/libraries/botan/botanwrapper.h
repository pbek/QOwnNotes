#ifndef BOTANWRAPPER_H
#define BOTANWRAPPER_H

#include <QByteArray>
#include <QString>
//#include <fstream>
//#include <iostream>
//#include <string>
#ifdef USE_SYSTEM_BOTAN
#include <botan/types.h>
#include <botan/secmem.h>
#else
#include "botan.h"
#endif

class BotanWrapper
{
public:
    BotanWrapper();
    
    /*!
    * Creates a hash
    * @param Data The string to hash
    */
    QString Hash(const QString &Data);

    /*!
    * Creates a hash in hex format
    * @param Data The string to hash
    */
//    QString HexHash(QString Data);

    /*!
    * Returns a Base64 encoded QString
    * @param Data The string to encode
    */
//    QString Encode(QString Data);

    /*!
    * Returns a decoded string from a Base64 encoded string
    * @param Data The string to decode
    */
//    QString Decode(QString Data);

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
    * Encrypts a file and returns a bool indicating success
    * @param Source The source file
    * @param Destination The destination file
    */
//    bool EncryptFile(QString Source, QString Destination);

    /*!
    * Decrypts a file and returns a bool indicating success
    * @param Source The source file
    * @param Destination The destination file
    */
//    bool DecryptFile(QString Source, QString Destination);

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
    /*!
    * The botan libary initilizer
    */
    //Botan::LibraryInitializer mInit;

    /*!
    * The Salt
    */
    Botan::secure_vector<Botan::byte> mSalt;

    /*!
    * The password
    */
    QString mPassword;

};

#endif // BOTANWRAPPER_H
