#include "botanwrapper.h"

#include <QDebug>
#include <algorithm>
#include <random>

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
constexpr auto LegacyPbkdf = "PBKDF2(HMAC(SHA-1))";
constexpr auto NotePbkdfV2 = "PBKDF2(HMAC(SHA-1))";
constexpr auto NoteCipherV2 = "AES-256/CBC/PKCS7";
constexpr auto NoteMacV2 = "HMAC(SHA-1)";

Botan::secure_vector<uint8_t> deriveMasterKey(const QString &password, const QVector<uint8_t> &salt,
                                              size_t length, size_t iterations,
                                              const char *algorithm) {
#if BOTAN_VERSION_MAJOR >= 3
    auto pbkdfFamily = Botan::PasswordHashFamily::create(algorithm);
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
    auto pbkdf = Botan::PBKDF::create(algorithm);
    if (!pbkdf) {
        qWarning() << "[Botan Error] Failed to create PBKDF2";
        return {};
    }

    return pbkdf->pbkdf_iterations(length, password.toStdString(), salt.data(), salt.size(),
                                   iterations);
#endif
}

Botan::secure_vector<uint8_t> secureVectorFromBase64(const QString &text) {
    const QByteArray bytes = QByteArray::fromBase64(text.toLatin1());
    Botan::secure_vector<uint8_t> result;
    result.reserve(static_cast<size_t>(bytes.size()));

    for (const char byte : bytes) {
        result.push_back(static_cast<uint8_t>(byte));
    }

    return result;
}

QVector<uint8_t> vectorFromBase64(const QString &text) {
    const QByteArray bytes = QByteArray::fromBase64(text.toLatin1());
    QVector<uint8_t> result;
    result.reserve(bytes.size());

    for (const char byte : bytes) {
        result.append(static_cast<uint8_t>(byte));
    }

    return result;
}

Botan::secure_vector<uint8_t> left(const Botan::secure_vector<uint8_t> &bytes, size_t size) {
    return Botan::secure_vector<uint8_t>(bytes.begin(), bytes.begin() + size);
}

Botan::secure_vector<uint8_t> mid(const Botan::secure_vector<uint8_t> &bytes, size_t offset,
                                  size_t size) {
    return Botan::secure_vector<uint8_t>(bytes.begin() + offset, bytes.begin() + offset + size);
}

QString createMacBase64(const Botan::secure_vector<uint8_t> &macKey,
                        const Botan::secure_vector<uint8_t> &nonce,
                        const Botan::secure_vector<uint8_t> &cipherText) {
    auto mac = Botan::MessageAuthenticationCode::create(NoteMacV2);
    if (!mac) {
        qWarning() << "[Botan Error] Failed to create" << NoteMacV2;
        return {};
    }

    mac->set_key(macKey);
    mac->update(nonce);
    mac->update(cipherText);
    return QString::fromStdString(Botan::base64_encode(mac->final()));
}

bool verifyMac(const Botan::secure_vector<uint8_t> &macKey,
               const Botan::secure_vector<uint8_t> &nonce,
               const Botan::secure_vector<uint8_t> &cipherText, const QString &macBase64) {
    auto mac = Botan::MessageAuthenticationCode::create(NoteMacV2);
    if (!mac) {
        qWarning() << "[Botan Error] Failed to create" << NoteMacV2;
        return false;
    }

    const Botan::secure_vector<uint8_t> expectedMac = secureVectorFromBase64(macBase64);
    mac->set_key(macKey);
    mac->update(nonce);
    mac->update(cipherText);
    return mac->verify_mac(expectedMac);
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
        auto master_key = deriveMasterKey(mPassword, mSalt, 48, PBKDF2_ITERATIONS, LegacyPbkdf);
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
        auto master_key = deriveMasterKey(mPassword, mSalt, 48, PBKDF2_ITERATIONS, LegacyPbkdf);
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

QString BotanWrapper::EncryptV2(const QString &Data, const QString &saltBase64,
                                const QString &nonceBase64, size_t iterations, QString *macBase64) {
    try {
        const QVector<uint8_t> salt = vectorFromBase64(saltBase64);
        const auto nonce = secureVectorFromBase64(nonceBase64);

        if (salt.isEmpty() || nonce.empty()) {
            qWarning() << "[Botan Error] Missing salt or nonce for note encryption";
            return {};
        }

        auto keyMaterial = deriveMasterKey(mPassword, salt, 64, iterations, NotePbkdfV2);
        if (keyMaterial.empty()) {
            return {};
        }

        const Botan::secure_vector<uint8_t> encryptionKey = left(keyMaterial, 32);
        const Botan::secure_vector<uint8_t> macKey = mid(keyMaterial, 32, 32);

        auto cipher = Botan::Cipher_Mode::create(NoteCipherV2, encryptionDirection());
        if (!cipher) {
            qWarning() << "[Botan Error] Failed to create" << NoteCipherV2 << "cipher";
            return {};
        }

        cipher->set_key(encryptionKey);
        cipher->start(nonce);

        const std::string input = Data.toStdString();
        Botan::secure_vector<uint8_t> buffer(input.begin(), input.end());
        cipher->finish(buffer);

        if (macBase64 != nullptr) {
            *macBase64 = createMacBase64(macKey, nonce, buffer);
            if (macBase64->isEmpty()) {
                return {};
            }
        }

        return QString::fromStdString(Botan::base64_encode(buffer));
    } catch (const Botan::Exception &e) {
        qWarning() << "[Botan Error] " << e.what();
        return {};
    }
}

QString BotanWrapper::DecryptV2(const QString &Data, const QString &saltBase64,
                                const QString &nonceBase64, const QString &macBase64,
                                size_t iterations) {
    try {
        const QVector<uint8_t> salt = vectorFromBase64(saltBase64);
        const auto nonce = secureVectorFromBase64(nonceBase64);

        if (salt.isEmpty() || nonce.empty()) {
            qWarning() << "[Botan Error] Missing salt or nonce for note decryption";
            return {};
        }

        auto keyMaterial = deriveMasterKey(mPassword, salt, 64, iterations, NotePbkdfV2);
        if (keyMaterial.empty()) {
            return {};
        }

        const Botan::secure_vector<uint8_t> encryptionKey = left(keyMaterial, 32);
        const Botan::secure_vector<uint8_t> macKey = mid(keyMaterial, 32, 32);

        Botan::secure_vector<uint8_t> buffer = Botan::base64_decode(Data.toStdString());
        if (!verifyMac(macKey, nonce, buffer, macBase64)) {
            qWarning() << "[Botan Error] Note authentication failed";
            return {};
        }

        auto cipher = Botan::Cipher_Mode::create(NoteCipherV2, decryptionDirection());
        if (!cipher) {
            qWarning() << "[Botan Error] Failed to create" << NoteCipherV2 << "cipher";
            return {};
        }

        cipher->set_key(encryptionKey);
        cipher->start(nonce);
        cipher->finish(buffer);

        const std::string output(buffer.begin(), buffer.end());
        return QString::fromStdString(output);
    } catch (const Botan::Exception &e) {
        qWarning() << "[Botan Error] " << e.what();
        return {};
    }
}

QString BotanWrapper::randomBytesBase64(int size) {
    if (size <= 0) {
        return {};
    }

    std::vector<uint8_t> bytes(static_cast<size_t>(size));
    std::random_device randomDevice;

    for (uint8_t &byte : bytes) {
        byte = static_cast<uint8_t>(randomDevice());
    }

    const QByteArray byteArray(reinterpret_cast<const char *>(bytes.data()),
                               static_cast<int>(bytes.size()));
    return QString::fromLatin1(byteArray.toBase64());
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
