Botan: Crypto and TLS for Modern C++
========================================

Botan (Japanese for peony flower) is a C++ cryptography library released under the
permissive `Simplified BSD <https://botan.randombit.net/license.txt>`_ license.

Botan's goal is to be the best option for cryptography in C++ by offering the
tools necessary to implement a range of practical systems, such as TLS protocol,
X.509 certificates, modern AEAD ciphers, PKCS#11 and TPM hardware support,
password hashing, and post quantum crypto schemes. A Python binding is included,
and several other `language bindings
<https://github.com/randombit/botan/wiki/Language-Bindings>`_ are available.
It is used in many `open source and commercial products <https://github.com/randombit/botan/wiki/Users>`_.

See the `documentation <https://botan.randombit.net/handbook>`_ for more
information about included features.

Development is coordinated on `GitHub <https://github.com/randombit/botan>`_
and contributions are welcome. If you need help, please open an issue on
`GitHub <https://github.com/randombit/botan/issues>`_ or email the
`botan-devel mailing list <https://lists.randombit.net/mailman/listinfo/botan-devel/>`_.
New releases are announced on the `botan-announce mailing list
<https://lists.randombit.net/mailman/listinfo/botan-announce/>`_.
If you think you have found a security issue, see the `security page
<https://botan.randombit.net/security.html>`_ for contact information.

The latest release is
`2.12.0 <https://botan.randombit.net/releases/Botan-2.12.0.tar.xz>`_
`(sig) <https://botan.randombit.net/releases/Botan-2.12.0.tar.xz.asc>`_,
released on 2019-10-07.
All releases are signed with a `PGP key <https://botan.randombit.net/pgpkey.txt>`_.
See the `release notes <https://botan.randombit.net/news.html>`_ for
what is new. Botan is also available through most
`distributions <https://github.com/randombit/botan/wiki/Distros>`_
such as Fedora, Debian, Arch and Homebrew.

.. image:: https://travis-ci.org/randombit/botan.svg?branch=master
    :target: https://travis-ci.org/randombit/botan
    :alt: Travis CI status

.. image:: https://ci.appveyor.com/api/projects/status/n9f94dljd03j2lce/branch/master?svg=true
    :target: https://ci.appveyor.com/project/randombit/botan/branch/master
    :alt: AppVeyor CI status

.. image:: https://codecov.io/github/randombit/botan/coverage.svg?branch=master
    :target: https://codecov.io/github/randombit/botan
    :alt: Code coverage report

.. image:: https://img.shields.io/lgtm/alerts/g/randombit/botan.svg
    :target: https://lgtm.com/projects/g/randombit/botan/alerts/
    :alt: LGTM alerts

.. image:: https://oss-fuzz-build-logs.storage.googleapis.com/badges/botan.svg
    :target: https://oss-fuzz.com/coverage-report/job/libfuzzer_asan_botan/latest
    :alt: OSS-Fuzz status

.. image:: https://scan.coverity.com/projects/624/badge.svg
    :target: https://scan.coverity.com/projects/624
    :alt: Coverity results

.. image:: https://repology.org/badge/tiny-repos/botan.svg
    :target: https://repology.org/project/botan/versions
    :alt: Packaging status

.. image:: https://bestpractices.coreinfrastructure.org/projects/531/badge
    :target: https://bestpractices.coreinfrastructure.org/projects/531
    :alt: CII Best Practices statement

Find Enclosed
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Transport Layer Security (TLS) Protocol
----------------------------------------

* TLS v1.0, v1.1, and v1.2. The broken SSLv3 protocol is no longer supported.
* DTLS v1.0 and v1.2 are adaptations of TLS to datagram operation.
* Supported extensions include session tickets, SNI, ALPN, OCSP stapling,
  encrypt-then-mac CBC, and extended master secret.
* Supports authentication using preshared keys (PSK) or passwords (SRP)
* Supports record encryption with ChaCha20Poly1305, AES/OCB, AES/GCM, AES/CCM,
  Camellia/GCM as well as legacy CBC ciphersuites.
* Key exchange using CECPQ1, ECDH, FFDHE, or RSA

Public Key Infrastructure
----------------------------------------

* X.509v3 certificates and CRL creation and handling
* PKIX certificate path validation, including name constraints.
* OCSP request creation and response handling
* PKCS #10 certificate request generation and processing
* Access to Windows, macOS and Unix system certificate stores
* SQL database backed certificate store

Public Key Cryptography
----------------------------------------

* RSA signatures and encryption
* DH and ECDH key agreement
* Signature schemes ECDSA, DSA, Ed25519, ECGDSA, ECKCDSA, SM2, GOST 34.10
* Post-quantum signature scheme XMSS
* Post-quantum key agreement schemes McEliece and NewHope
* ElGamal encryption
* Padding schemes OAEP, PSS, PKCS #1 v1.5, X9.31

Ciphers, hashes, MACs, and checksums
----------------------------------------

* Authenticated cipher modes EAX, OCB, GCM, SIV, CCM, (X)ChaCha20Poly1305
* Cipher modes CTR, CBC, XTS, CFB, OFB
* Block ciphers AES, ARIA, Blowfish, Camellia, CAST-128, DES/3DES, GOST 28147,
  IDEA, Lion, Noekeon, SEED, Serpent, SHACAL2, SM4, Threefish-512, Twofish, XTEA
* Stream ciphers (X)ChaCha20, (X)Salsa20, SHAKE-128, RC4
* Hash functions SHA-1, SHA-2, SHA-3, MD4, MD5, RIPEMD-160, BLAKE2b,
  Skein-512, SM3, Tiger, Whirlpool, GOST 34.11-94, GOST 34.11-2012
* Authentication codes HMAC, CMAC, Poly1305, SipHash, GMAC, X9.19 DES-MAC
* Non-cryptographic checksums Adler32, CRC24, CRC32

Other Useful Things
----------------------------------------

* Full C++ PKCS #11 API wrapper
* Interfaces for TPM v1.2 device access
* Simple compression API wrapping zlib, bzip2, and lzma libraries
* RNG wrappers for system RNG and hardware RNGs
* HMAC_DRBG and entropy collection system for userspace RNGs
* Password hashing schemes PBKDF2, Argon2, Scrypt, bcrypt
* SRP-6a password authenticated key exchange
* Key derivation functions including HKDF, KDF2, SP 800-108, SP 800-56A, SP 800-56C
* HOTP and TOTP algorithms
* Format preserving encryption scheme FE1
* Threshold secret sharing
* NIST key wrapping
* Boost.Asio compatible TLS client stream
