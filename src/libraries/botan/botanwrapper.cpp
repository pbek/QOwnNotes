#include "botanwrapper.h"

BotanWrapper::BotanWrapper(QObject *parent) :
    QObject(parent)
{
    //Set the default salt size
    mSalt.resize(48);

    //Set the default salt
    for(int i = 0; i < mSalt.size(); i++)
    {
        mSalt[i] = i;
    }

    //Set the default password
    mPassword = Hash("!@&^jdshUG24!T^!@*&!Y@()&^909+!-@!@#07");
}

QString BotanWrapper::Hash(QString Data)
{
    try
    {
        Pipe pipe(new Hash_Filter("SHA-1"));
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch(...)
    {
        return "";
    }
}

QString BotanWrapper::HexHash(QString Data)
{
    try
    {
        Pipe pipe(new Hash_Filter("SHA-1"), new Hex_Encoder);
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch(...)
    {
        return "";
    }
}

QString BotanWrapper::Encode(QString Data)
{
    try
    {
        Pipe pipe(new Base64_Encoder);
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch(...)
    {
        return "";
    }
}

QString BotanWrapper::Decode(QString Data)
{
    try
    {
        Pipe pipe(new Base64_Decoder);
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch(...)
    {
        return "";
    }
}

QString BotanWrapper::Encrypt(QString Data)
{
    try
    {
        //Setup the key derive functions
        PKCS5_PBKDF2 pbkdf2(new HMAC(new SHA_160));
        const u32bit PBKDF2_ITERATIONS = 8192;

        //Create the KEY and IV
        KDF* kdf = get_kdf("KDF2(SHA-1)");

        //Create the master key
        SecureVector<byte> mMaster = pbkdf2.derive_key(48, mPassword.toStdString(), &mSalt[0], mSalt.size(),PBKDF2_ITERATIONS).bits_of();
        SymmetricKey mKey = kdf->derive_key(32, mMaster, "salt1");
        InitializationVector mIV = kdf->derive_key(16, mMaster, "salt2");

        Pipe pipe(get_cipher("AES-256/CBC/PKCS7", mKey, mIV,ENCRYPTION),new Base64_Encoder);
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch(...)
    {
        return "";
    }
}

QString BotanWrapper::Decrypt(QString Data)
{
    try
    {
        //Setup the key derive functions
        PKCS5_PBKDF2 pbkdf2(new HMAC(new SHA_160));
        const u32bit PBKDF2_ITERATIONS = 8192;

        //Create the KEY and IV
        KDF* kdf = get_kdf("KDF2(SHA-1)");

        //Create the master key
        SecureVector<byte> mMaster = pbkdf2.derive_key(48, mPassword.toStdString(), &mSalt[0], mSalt.size(),PBKDF2_ITERATIONS).bits_of();
        SymmetricKey mKey = kdf->derive_key(32, mMaster, "salt1");
        InitializationVector mIV = kdf->derive_key(16, mMaster, "salt2");

        Pipe pipe(new Base64_Decoder,get_cipher("AES-256/CBC/PKCS7", mKey, mIV,DECRYPTION));
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch(...)
    {
        return "";
    }
}

bool BotanWrapper::EncryptFile(QString Source, QString Destination)
{
    try
    {
        //Setup the key derive functions
        PKCS5_PBKDF2 pbkdf2(new HMAC(new SHA_160));
        const u32bit PBKDF2_ITERATIONS = 8192;

        //Create the KEY and IV
        KDF* kdf = get_kdf("KDF2(SHA-1)");

        //Create the master key
        SecureVector<byte> mMaster = pbkdf2.derive_key(48, mPassword.toStdString(), &mSalt[0], mSalt.size(),PBKDF2_ITERATIONS).bits_of();
        SymmetricKey mKey = kdf->derive_key(32, mMaster, "salt1");
        InitializationVector mIV = kdf->derive_key(16, mMaster, "salt2");

        string inFilename = Source.toStdString();
        string outFilename = Destination.toStdString();
        std::ifstream in(inFilename.c_str(),std::ios::binary);
        std::ofstream out(outFilename.c_str(),std::ios::binary);

        Pipe pipe(get_cipher("AES-256/CBC/PKCS7", mKey, mIV,ENCRYPTION),new DataSink_Stream(out));
        pipe.start_msg();
        in >> pipe;
        pipe.end_msg();

        out.flush();
        out.close();
        in.close();

        return true;
    }
    catch(...)
    {
        return false;
    }
}

bool BotanWrapper::DecryptFile(QString Source, QString Destination)
{
    try
    {
        //Setup the key derive functions
        PKCS5_PBKDF2 pbkdf2(new HMAC(new SHA_160));
        const u32bit PBKDF2_ITERATIONS = 8192;

        //Create the KEY and IV
        KDF* kdf = get_kdf("KDF2(SHA-1)");

        //Create the master key
        SecureVector<byte> mMaster = pbkdf2.derive_key(48, mPassword.toStdString(), &mSalt[0], mSalt.size(),PBKDF2_ITERATIONS).bits_of();
        SymmetricKey mKey = kdf->derive_key(32, mMaster, "salt1");
        InitializationVector mIV = kdf->derive_key(16, mMaster, "salt2");

        string inFilename = Source.toStdString();
        string outFilename = Destination.toStdString();
        std::ifstream in(inFilename.c_str(),std::ios::binary);
        std::ofstream out(outFilename.c_str(),std::ios::binary);

        Pipe pipe(get_cipher("AES-256/CBC/PKCS7", mKey, mIV,DECRYPTION),new DataSink_Stream(out));
        pipe.start_msg();
        in >> pipe;
        pipe.end_msg();

        out.flush();
        out.close();
        in.close();

        return true;
    }
    catch(...)
    {
        return false;
    }
}

void BotanWrapper::setPassword(QString Password)
{
    //Set the password
    mPassword = Password;
}

void BotanWrapper::setSalt(QString Salt)
{
    QByteArray cBytes = Salt.toLatin1();
    for(int i = 0;i < mSalt.size(); i++)
    {
        mSalt[i] = cBytes[i];
    }
}



