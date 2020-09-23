#include "botanwrapper.h"

#include <QDebug>

#ifdef USE_SYSTEM_BOTAN
#include <botan/pipe.h>
#include <botan/cipher_mode.h>
#include <botan/base64.h>
#include <botan/filters.h>
#include <botan/pbkdf2.h>
#include <botan/kdf.h>
#include <botan/hmac.h>
#include <botan/sha160.h>
#include <botan/types.h>
#include <botan/secmem.h>
#else
#include "botan.h"
#endif

BotanWrapper::BotanWrapper() {
    // Set the default salt size
    mSalt.resize(48);

    // Set the default salt
    for (uint8_t i = 0; i < mSalt.size(); i++) {
        mSalt[i] = i;
    }

    // Set the default password
    mPassword = Hash(QStringLiteral("!@&^jdshUG24!T^!@*&!Y@()&^909+!-@!@#07"));
}

QString BotanWrapper::Hash(const QString &Data) {
    try {
        Botan::Pipe pipe(new Botan::Hash_Filter("SHA-1"));
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    } catch (Botan::Exception &e) {
        qWarning() << "[Botan Error] " << e.what();
        return QString();
    }
}

QString BotanWrapper::Encrypt(const QString &Data) {
    try {
        // Setup the key derive functions
        Botan::PKCS5_PBKDF2 pbkdf2(new Botan::HMAC(new Botan::SHA_160));
        const uint32_t PBKDF2_ITERATIONS = 8192;

        // Create the KEY and IV
        Botan::KDF *kdf = Botan::get_kdf("KDF2(SHA-1)");

        mSalt.data();

        // Create the master key
        Botan::SecureVector<Botan::byte> mMaster = pbkdf2.derive_key(
               48,
               mPassword.toStdString(),
               &mSalt[0], mSalt.size(),
               PBKDF2_ITERATIONS).bits_of();
        Botan::SymmetricKey mKey = kdf->derive_key(32, mMaster, "salt1");
        Botan::InitializationVector mIV = kdf->derive_key(16, mMaster, "salt2");

        Botan::Pipe pipe(get_cipher("AES-256/CBC/PKCS7", mKey, mIV, Botan::ENCRYPTION),
                  new Botan::Base64_Encoder);
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    } catch (Botan::Exception &e) {
        qWarning() << "[Botan Error] " << e.what();
        return QString();
    }
}

QString BotanWrapper::Decrypt(const QString &Data) {
    try {
        //Setup the key derive functions
        Botan::PKCS5_PBKDF2 pbkdf2(new Botan::HMAC(new Botan::SHA_160));
        const uint32_t PBKDF2_ITERATIONS = 8192;

        //Create the KEY and IV
        Botan::KDF *kdf = Botan::get_kdf("KDF2(SHA-1)");

        //Create the master key
        Botan::SecureVector<Botan::byte> mMaster = pbkdf2.derive_key(
               48,
               mPassword.toStdString(),
               &mSalt[0], mSalt.size(),
               PBKDF2_ITERATIONS).bits_of();
        Botan::SymmetricKey mKey = kdf->derive_key(32, mMaster, "salt1");
        Botan::InitializationVector mIV = kdf->derive_key(16, mMaster, "salt2");

        Botan::Pipe pipe(new Botan::Base64_Decoder,
                  get_cipher("AES-256/CBC/PKCS7", mKey, mIV, Botan::DECRYPTION));
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch (Botan::Exception &e) {
        qWarning() << "[Botan Error] " << e.what();
        return QString();
    }
}

void BotanWrapper::setPassword(const QString &Password) {
    // Set the password
    mPassword = Password;
}

void BotanWrapper::setSalt(const QString &Salt) {
    QByteArray cBytes = Salt.toLatin1();

    if (cBytes.size() > 48)
        cBytes.resize(48);

    mSalt.fill(0);
    std::copy(cBytes.cbegin(), cBytes.cend(), mSalt.begin());
}
