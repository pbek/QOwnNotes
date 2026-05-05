#include "botanwrapper.h"

#include <QDebug>
#include <algorithm>

#ifdef USE_SYSTEM_BOTAN
#include <botan/base64.h>
#include <botan/build.h>
#include <botan/cipher_mode.h>
#include <botan/exceptn.h>
#include <botan/hash.h>
#include <botan/kdf.h>
#include <botan/mac.h>
#include <botan/pbkdf.h>
#include <botan/pwdhash.h>
#include <botan/secmem.h>
#include <botan/system_rng.h>
#else
#include "botan.h"
#endif

namespace {
Botan::secure_vector<uint8_t> deriveMasterKey(const QString &password, const QVector<uint8_t> &salt,
                                              size_t length, size_t iterations) {
#if BOTAN_VERSION_MAJOR >= 3
    auto pbkdfFamily = Botan::PasswordHashFamily::create("PBKDF2(HMAC(SHA-1))");
    if (!pbkdfFamily) {
        qWarning() << "[Botan Error] Failed to create PBKDF2 family";
        return {};
    }

    auto pbkdf = pbkdfFamily->from_iterations(iterations);
    if (!pbkdf) {
        qWarning() << "[Botan Error] Failed to create PBKDF2 parameters";
        return {};
    }

    Botan::secure_vector<uint8_t> masterKey(length);
    const std::string botanPassword = password.toStdString();
    pbkdf->derive_key(masterKey.data(), masterKey.size(), botanPassword.data(),
                      botanPassword.size(), salt.data(), salt.size());

    return masterKey;
#else
    auto pbkdf = Botan::PBKDF::create("PBKDF2(HMAC(SHA-1))");
    if (!pbkdf) {
        qWarning() << "[Botan Error] Failed to create PBKDF2";
        return {};
    }

    return pbkdf->pbkdf_iterations(length, password.toStdString(), salt.data(), salt.size(),
                                   iterations);
#endif
}

Botan::Cipher_Dir encryptionDirection() {
#if BOTAN_VERSION_MAJOR >= 3
    return Botan::Cipher_Dir::Encryption;
#else
    return Botan::Cipher_Dir::ENCRYPTION;
#endif
}

Botan::Cipher_Dir decryptionDirection() {
#if BOTAN_VERSION_MAJOR >= 3
    return Botan::Cipher_Dir::Decryption;
#else
    return Botan::Cipher_Dir::DECRYPTION;
#endif
}
}    // namespace

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
        auto hash = Botan::HashFunction::create("SHA-1");
        if (!hash) {
            qWarning() << "[Botan Error] Failed to create SHA-1 hash function";
            return {};
        }

        std::string input = Data.toStdString();
        hash->update(input);
        Botan::secure_vector<uint8_t> result = hash->final();

        std::string output(result.begin(), result.end());
        return QString::fromStdString(output);
    } catch (const Botan::Exception &e) {
        qWarning() << "[Botan Error] " << e.what();
        return {};
    }
}

QString BotanWrapper::Encrypt(const QString &Data) {
    try {
        const uint32_t PBKDF2_ITERATIONS = 8192;

        // Create the master key using PBKDF2
        auto master_key = deriveMasterKey(mPassword, mSalt, 48, PBKDF2_ITERATIONS);
        if (master_key.empty()) {
            return {};
        }

        // Create KDF for key and IV derivation
        auto kdf = Botan::KDF::create("KDF2(SHA-1)");
        if (!kdf) {
            qWarning() << "[Botan Error] Failed to create KDF2";
            return {};
        }

        // Derive key and IV
        auto key = kdf->derive_key(32, master_key, "salt1");
        auto iv = kdf->derive_key(16, master_key, "salt2");

        // Create cipher
        auto cipher = Botan::Cipher_Mode::create("AES-256/CBC/PKCS7", encryptionDirection());
        if (!cipher) {
            qWarning() << "[Botan Error] Failed to create AES-256/CBC/PKCS7 cipher";
            return {};
        }

        cipher->set_key(key);
        cipher->start(iv);

        // Encrypt the data
        std::string input = Data.toStdString();
        Botan::secure_vector<uint8_t> buffer(input.begin(), input.end());
        cipher->finish(buffer);

        // Base64 encode the result
        std::string encoded = Botan::base64_encode(buffer);
        return QString::fromStdString(encoded);

    } catch (const Botan::Exception &e) {
        qWarning() << "[Botan Error] " << e.what();
        return {};
    }
}

QString BotanWrapper::Decrypt(const QString &Data) {
    try {
        const uint32_t PBKDF2_ITERATIONS = 8192;

        // Create the master key using PBKDF2
        auto master_key = deriveMasterKey(mPassword, mSalt, 48, PBKDF2_ITERATIONS);
        if (master_key.empty()) {
            return {};
        }

        // Create KDF for key and IV derivation
        auto kdf = Botan::KDF::create("KDF2(SHA-1)");
        if (!kdf) {
            qWarning() << "[Botan Error] Failed to create KDF2";
            return {};
        }

        // Derive key and IV
        auto key = kdf->derive_key(32, master_key, "salt1");
        auto iv = kdf->derive_key(16, master_key, "salt2");

        // Base64 decode the input
        Botan::secure_vector<uint8_t> buffer = Botan::base64_decode(Data.toStdString());

        // Create cipher
        auto cipher = Botan::Cipher_Mode::create("AES-256/CBC/PKCS7", decryptionDirection());
        if (!cipher) {
            qWarning() << "[Botan Error] Failed to create AES-256/CBC/PKCS7 cipher";
            return {};
        }

        cipher->set_key(key);
        cipher->start(iv);
        cipher->finish(buffer);

        // Convert result to string
        std::string output(buffer.begin(), buffer.end());
        return QString::fromStdString(output);

    } catch (const Botan::Exception &e) {
        qWarning() << "[Botan Error] " << e.what();
        return {};
    }
}

void BotanWrapper::setPassword(const QString &Password) {
    // Set the password
    mPassword = Password;
}

void BotanWrapper::setSalt(const QString &Salt) {
    QByteArray cBytes = Salt.toLatin1();

    if (cBytes.size() > 48) cBytes.resize(48);

    mSalt.fill(0);
    std::copy(cBytes.cbegin(), cBytes.cend(), mSalt.begin());
}
