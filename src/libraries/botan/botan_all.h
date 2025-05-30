/*
 * Botan 3.8.1 Amalgamation
 * (C) 1999-2023 The Botan Authors
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#ifndef BOTAN_AMALGAMATION_H_
#define BOTAN_AMALGAMATION_H_

#include <algorithm>
#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <initializer_list>
#include <iosfwd>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

/**
 * @file  build.h
 * @brief Build configuration for Botan 3.8.1
 */

/**
 * @defgroup buildinfo Build Information
 */

/**
 * @ingroup buildinfo
 * @defgroup buildinfo_version Build version information
 * @{
 */

/**
 * The major version of the release
 */
#define BOTAN_VERSION_MAJOR 3

/**
 * The minor version of the release
 */
#define BOTAN_VERSION_MINOR 8

/**
 * The patch version of the release
 */
#define BOTAN_VERSION_PATCH 1

/**
 * Expands to an integer of the form YYYYMMDD if this is an official
 * release, or 0 otherwise. For instance, 2.19.0, which was released
 * on January 19, 2022, has a `BOTAN_VERSION_DATESTAMP` of 20220119.
 *
 * This macro is deprecated; use version_datestamp from version.h
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_VERSION_DATESTAMP 20250507

/**
 * A string set to the release type
 *
 * This macro is deprecated
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_VERSION_RELEASE_TYPE "release"

/**
 * A macro expanding to a string that is set to a revision identifier
 * corresponding to the source, or "unknown" if this could not be
 * determined. It is set for all official releases.
 *
 * This macro is deprecated; use version_vc_revision from version.h
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_VERSION_VC_REVISION "git:c06967092edd9baa2b42f89135380912113ae209"

/**
 * A macro expanding to a string that is set at build time using the
 * `--distribution-info` option. It allows a packager of the library
 * to specify any distribution-specific patches. If no value is given
 * at build time, the value is the string "unspecified".
 *
 * This macro is deprecated; use version_distribution_info from version.h
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_DISTRIBUTION_INFO "unspecified"

/**
 * @}
 */

/**
 * @ingroup buildinfo
 * @defgroup buildinfo_configuration Build configurations
 * @{
 */

#ifndef BOTAN_DLL
#define BOTAN_DLL
#endif

/* Target identification and feature test macros */

#define BOTAN_TARGET_OS_HAS_FILESYSTEM
#define BOTAN_TARGET_OS_HAS_THREADS

/**
 * @}
 */

/**
 * @ingroup buildinfo
 * @defgroup buildinfo_modules Enabled modules and API versions
 * @{
 */

/*
 * Module availability definitions
 */
#define BOTAN_HAS_AES 20131128
#define BOTAN_HAS_BASE64_CODEC 20131128
#define BOTAN_HAS_BLOCK_CIPHER 20131128
#define BOTAN_HAS_CIPHER_MODES 20180124
#define BOTAN_HAS_CIPHER_MODE_PADDING 20131128
#define BOTAN_HAS_CMAC 20131128
#define BOTAN_HAS_CODEC_FILTERS 20131128
#define BOTAN_HAS_FILTERS 20160415
#define BOTAN_HAS_HASH 20180112
#define BOTAN_HAS_HASH_ID 20131128
#define BOTAN_HAS_HEX_CODEC 20131128
#define BOTAN_HAS_HMAC 20131128
#define BOTAN_HAS_HMAC_DRBG 20140319
#define BOTAN_HAS_KDF2 20131128
#define BOTAN_HAS_KDF_BASE 20131128
#define BOTAN_HAS_MAC 20150626
#define BOTAN_HAS_MDX_HASH_FUNCTION 20131128
#define BOTAN_HAS_MODES 20150626
#define BOTAN_HAS_MODE_CBC 20131128
#define BOTAN_HAS_OS_UTILS 20241202
#define BOTAN_HAS_PASSWORD_HASHING 20210419
#define BOTAN_HAS_PBKDF 20180902
#define BOTAN_HAS_PBKDF2 20180902
#define BOTAN_HAS_PK_PADDING 20131128
#define BOTAN_HAS_POLY_DBL 20170927
#define BOTAN_HAS_SHA1 20131128
#define BOTAN_HAS_STATEFUL_RNG 20160819
#define BOTAN_HAS_UTIL_FUNCTIONS 20180903

/**
 * @}
 */

/**
 * @addtogroup buildinfo_configuration
 * @{
 */

/** Local/misc configuration options (if any) follow */

/**
 * @}
 */

/**
 * Used to annotate API exports which are public and supported.
 * These APIs will not be broken/removed unless strictly required for
 * functionality or security, and only in new major versions.
 * @param maj The major version this public API was released in
 * @param min The minor version this public API was released in
 */
#define BOTAN_PUBLIC_API(maj, min) BOTAN_DLL

/**
 * Used to annotate API exports which are public, but are now deprecated
 * and which will be removed in a future major release.
 */
#define BOTAN_DEPRECATED_API(msg) BOTAN_DEPRECATED(msg) BOTAN_DLL

/**
 * Used to annotate API exports which are public and can be used by
 * applications if needed, but which are intentionally not documented,
 * and which may change incompatibly in a future major version.
 */
#define BOTAN_UNSTABLE_API BOTAN_DLL

/**
 * Used to annotate API exports which are exported but only for the
 * purposes of testing. They should not be used by applications and
 * may be removed or changed without notice.
 */
#define BOTAN_TEST_API BOTAN_DLL

/**
 * Used to annotate API exports which are exported but only for the
 * purposes of fuzzing. They should not be used by applications and
 * may be removed or changed without notice.
 *
 * They are only exported if the fuzzers are being built
 */
#if defined(BOTAN_FUZZERS_ARE_BEING_BUILT)
#define BOTAN_FUZZER_API BOTAN_DLL
#else
#define BOTAN_FUZZER_API
#endif

/*
 * Define BOTAN_DEPRECATED
 */
#if !defined(BOTAN_NO_DEPRECATED_WARNINGS) && !defined(BOTAN_AMALGAMATION_H_) && \
    !defined(BOTAN_IS_BEING_BUILT)

#define BOTAN_DEPRECATED(msg) [[deprecated(msg)]]

#if defined(__clang__)
#define BOTAN_DEPRECATED_HEADER(hdr) _Pragma("message \"this header is deprecated\"")
#define BOTAN_FUTURE_INTERNAL_HEADER(hdr) \
    _Pragma("message \"this header will be made internal in the future\"")
#elif defined(_MSC_VER)
#define BOTAN_DEPRECATED_HEADER(hdr) __pragma(message("this header is deprecated"))
#define BOTAN_FUTURE_INTERNAL_HEADER(hdr) \
    __pragma(message("this header will be made internal in the future"))
#elif defined(__GNUC__)
#define BOTAN_DEPRECATED_HEADER(hdr) _Pragma("GCC warning \"this header is deprecated\"")
#define BOTAN_FUTURE_INTERNAL_HEADER(hdr) \
    _Pragma("GCC warning \"this header will be made internal in the future\"")
#endif

#endif

#if !defined(BOTAN_DEPRECATED)
#define BOTAN_DEPRECATED(msg)
#endif

#if !defined(BOTAN_DEPRECATED_HEADER)
#define BOTAN_DEPRECATED_HEADER(hdr)
#endif

#if !defined(BOTAN_FUTURE_INTERNAL_HEADER)
#define BOTAN_FUTURE_INTERNAL_HEADER(hdr)
#endif

#if defined(__clang__)
#define BOTAN_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
#define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS \
    _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
#define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE
#define BOTAN_DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#elif defined(__GNUG__)
#define BOTAN_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
#define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS \
    _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE
#define BOTAN_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#define BOTAN_DIAGNOSTIC_PUSH __pragma(warning(push))
#define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS __pragma(warning(disable : 4996))
#define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE __pragma(warning(disable : 4250))
#define BOTAN_DIAGNOSTIC_POP __pragma(warning(pop))
#else
#define BOTAN_DIAGNOSTIC_PUSH
#define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
#define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE
#define BOTAN_DIAGNOSTIC_POP
#endif

/**
 * MSVC does define __cplusplus but pins it at 199711L, because "legacy".
 * Note: There is a compiler switch to enable standard behavior (/Zc:__cplusplus),
 *       but we can't control that in downstream applications.
 *
 * See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
 */
#if defined(_MSVC_LANG)
#define BOTAN_CPLUSPLUS _MSVC_LANG
#else
#define BOTAN_CPLUSPLUS __cplusplus
#endif

#if BOTAN_CPLUSPLUS < 202002L
#error "Botan 3.x requires at least C++20"
#endif

namespace Botan {

/**
* @mainpage Botan Crypto Library API Reference
*
* <dl>
* <dt>Abstract Base Classes<dd>
*        BlockCipher, HashFunction, KDF, MessageAuthenticationCode, RandomNumberGenerator,
*        StreamCipher, SymmetricAlgorithm, AEAD_Mode, Cipher_Mode, XOF
* <dt>Public Key Interface Classes<dd>
*        PK_Key_Agreement, PK_Signer, PK_Verifier, PK_Encryptor, PK_Decryptor, PK_KEM_Encryptor,
PK_KEM_Decryptor
* <dt>Authenticated Encryption Modes<dd>
*        @ref CCM_Mode "CCM", @ref ChaCha20Poly1305_Mode "ChaCha20Poly1305", @ref EAX_Mode "EAX",
*        @ref GCM_Mode "GCM", @ref OCB_Mode "OCB", @ref SIV_Mode "SIV"
* <dt>Block Ciphers<dd>
*        @ref aria.h "ARIA", @ref aes.h "AES", @ref Blowfish, @ref camellia.h "Camellia", @ref
Cascade_Cipher "Cascade",
*        @ref CAST_128 "CAST-128", @ref CAST_128 DES, @ref TripleDES "3DES",
*        @ref GOST_28147_89 "GOST 28147-89", IDEA, Kuznyechik, Lion, Noekeon, SEED, Serpent,
SHACAL2, SM4,
*        @ref Threefish_512 "Threefish", Twofish
* <dt>Stream Ciphers<dd>
*        ChaCha, @ref CTR_BE "CTR", OFB, RC4, Salsa20
* <dt>Hash Functions<dd>
*        BLAKE2b, @ref GOST_34_11 "GOST 34.11", @ref Keccak_1600 "Keccak", MD4, MD5, @ref RIPEMD_160
"RIPEMD-160",
*        @ref SHA_1 "SHA-1", @ref SHA_224 "SHA-224", @ref SHA_256 "SHA-256", @ref SHA_384 "SHA-384",
*        @ref SHA_512 "SHA-512", @ref Skein_512 "Skein-512", SM3, Streebog, Whirlpool
* <dt>Non-Cryptographic Checksums<dd>
*        Adler32, CRC24, CRC32
* <dt>Message Authentication Codes<dd>
*        @ref BLAKE2bMAC "BLAKE2b", CMAC, HMAC, KMAC, Poly1305, SipHash, ANSI_X919_MAC
* <dt>Random Number Generators<dd>
*        AutoSeeded_RNG, HMAC_DRBG, Processor_RNG, System_RNG
* <dt>Key Derivation<dd>
*        HKDF, @ref KDF1 "KDF1 (IEEE 1363)", @ref KDF1_18033 "KDF1 (ISO 18033-2)", @ref KDF2 "KDF2
(IEEE 1363)",
*        @ref sp800_108.h "SP800-108", @ref SP800_56C "SP800-56C", @ref PKCS5_PBKDF2 "PBKDF2
(PKCS#5)"
* <dt>Password Hashing<dd>
*        @ref argon2.h "Argon2", @ref scrypt.h "scrypt", @ref bcrypt.h "bcrypt", @ref passhash9.h
"passhash9"
* <dt>Public Key Cryptosystems<dd>
*        @ref dlies.h "DLIES", @ref ecies.h "ECIES", @ref elgamal.h "ElGamal",
*        @ref rsa.h "RSA", @ref mceliece.h "McEliece", @ref sm2.h "SM2"
* <dt>Key Encapsulation Mechanisms<dd>
*        @ref cmce.h "Classic McEliece", @ref frodokem.h "FrodoKEM", @ref kyber.h "ML-KEM/Kyber",
@ref rsa.h "RSA"
* <dt>Public Key Signature Schemes<dd>
*        @ref dsa.h "DSA", @ref dilithium.h "ML-DSA/Dilithium", @ref ecdsa.h "ECDSA", @ref ecgdsa.h
"ECGDSA",
*        @ref eckcdsa.h "ECKCDSA", @ref gost_3410.h "GOST 34.10-2001", @ref hss_lms.h "HSS/LMS",
@ref sm2.h "SM2",
         @ref sphincsplus.h "SLH-DSA/SPHINCS+", @ref xmss.h "XMSS"
* <dt>Key Agreement<dd>
*        @ref dh.h "DH", @ref ecdh.h "ECDH"
* <dt>Compression<dd>
*        @ref bzip2.h "bzip2", @ref lzma.h "lzma", @ref zlib.h "zlib"
* <dt>TLS<dd>
*        TLS::Client, TLS::Server, TLS::Policy, TLS::Protocol_Version, TLS::Callbacks,
TLS::Ciphersuite,
*        TLS::Session, TLS::Session_Summary, TLS::Session_Manager, Credentials_Manager
* <dt>X.509<dd>
*        X509_Certificate, X509_CRL, X509_CA, Certificate_Extension, PKCS10_Request,
X509_Cert_Options,
*        Certificate_Store, Certificate_Store_In_SQL, Certificate_Store_In_SQLite
* <dt>eXtendable Output Functions<dd>
*        @ref SHAKE_XOF "SHAKE"
* </dl>
*/

using std::int32_t;
using std::int64_t;
using std::size_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

#if !defined(BOTAN_IS_BEING_BUILT)
/*
 * These typedefs are no longer used within the library headers
 * or code. They are kept only for compatibility with software
 * written against older versions.
 */
using byte = std::uint8_t;
using u16bit = std::uint16_t;
using u32bit = std::uint32_t;
using u64bit = std::uint64_t;
using s32bit = std::int32_t;
#endif

static constexpr bool HasNative64BitRegisters = sizeof(void*) >= 8;

using word = std::conditional_t<HasNative64BitRegisters, std::uint64_t, uint32_t>;

#if defined(__SIZEOF_INT128__)
#define BOTAN_TARGET_HAS_NATIVE_UINT128

// GCC complains if this isn't marked with __extension__
__extension__ typedef unsigned __int128 uint128_t;
#endif

/*
 * Should this assert fail on your system please contact the developers
 * for assistance in porting.
 */
static_assert(sizeof(std::size_t) == 8 || sizeof(std::size_t) == 4,
              "This platform has an unexpected size for size_t");

/**
 * How much to allocate for a buffer of no particular size
 */
constexpr size_t DefaultBufferSize = 4096;

}    // namespace Botan

namespace Botan {

/*
 * Define BOTAN_MALLOC_FN
 */
#if defined(__clang__) || defined(__GNUG__)
#define BOTAN_MALLOC_FN __attribute__((malloc))
#elif defined(_MSC_VER)
#define BOTAN_MALLOC_FN __declspec(restrict)
#else
#define BOTAN_MALLOC_FN
#endif

/**
 * Allocate a memory buffer by some method. This should only be used for
 * primitive types (uint8_t, uint32_t, etc).
 *
 * @param elems the number of elements
 * @param elem_size the size of each element
 * @return pointer to allocated and zeroed memory, or throw std::bad_alloc on failure
 */
BOTAN_PUBLIC_API(2, 3) BOTAN_MALLOC_FN void* allocate_memory(size_t elems, size_t elem_size);

/**
 * Free a pointer returned by allocate_memory
 * @param p the pointer returned by allocate_memory
 * @param elems the number of elements, as passed to allocate_memory
 * @param elem_size the size of each element, as passed to allocate_memory
 */
BOTAN_PUBLIC_API(2, 3) void deallocate_memory(void* p, size_t elems, size_t elem_size);

/**
 * Ensure the allocator is initialized
 */
void BOTAN_UNSTABLE_API initialize_allocator();

class Allocator_Initializer final {
   public:
    Allocator_Initializer() { initialize_allocator(); }
};

}    // namespace Botan

BOTAN_FUTURE_INTERNAL_HEADER(assert.h)

namespace Botan {

/**
 * Called when an assertion fails
 * Throws an Exception object
 */
[[noreturn]] void BOTAN_PUBLIC_API(2, 0)
    assertion_failure(const char* expr_str, const char* assertion_made, const char* func,
                      const char* file, int line);

/**
 * Called when an invalid argument is used
 * Throws Invalid_Argument
 */
[[noreturn]] void BOTAN_UNSTABLE_API throw_invalid_argument(const char* message, const char* func,
                                                            const char* file);

#define BOTAN_ARG_CHECK(expr, msg)                                           \
    do {                                                                     \
        if (!(expr)) Botan::throw_invalid_argument(msg, __func__, __FILE__); \
    } while (0)

/**
 * Called when an invalid state is encountered
 * Throws Invalid_State
 */
[[noreturn]] void BOTAN_UNSTABLE_API throw_invalid_state(const char* message, const char* func,
                                                         const char* file);

#define BOTAN_STATE_CHECK(expr)                                             \
    do {                                                                    \
        if (!(expr)) Botan::throw_invalid_state(#expr, __func__, __FILE__); \
    } while (0)

/**
 * Make an assertion
 */
#define BOTAN_ASSERT(expr, assertion_made)                                                 \
    do {                                                                                   \
        if (!(expr))                                                                       \
            Botan::assertion_failure(#expr, assertion_made, __func__, __FILE__, __LINE__); \
    } while (0)

/**
 * Make an assertion
 */
#define BOTAN_ASSERT_NOMSG(expr)                                                        \
    do {                                                                                \
        if (!(expr)) Botan::assertion_failure(#expr, "", __func__, __FILE__, __LINE__); \
    } while (0)

/**
 * Assert that value1 == value2
 */
#define BOTAN_ASSERT_EQUAL(expr1, expr2, assertion_made)                                       \
    do {                                                                                       \
        if ((expr1) != (expr2))                                                                \
            Botan::assertion_failure(#expr1 " == " #expr2, assertion_made, __func__, __FILE__, \
                                     __LINE__);                                                \
    } while (0)

/**
 * Assert that expr1 (if true) implies expr2 is also true
 */
#define BOTAN_ASSERT_IMPLICATION(expr1, expr2, msg)                                      \
    do {                                                                                 \
        if ((expr1) && !(expr2))                                                         \
            Botan::assertion_failure(#expr1 " implies " #expr2, msg, __func__, __FILE__, \
                                     __LINE__);                                          \
    } while (0)

/**
 * Assert that a pointer is not null
 */
#define BOTAN_ASSERT_NONNULL(ptr)                                                            \
    do {                                                                                     \
        if ((ptr) == nullptr)                                                                \
            Botan::assertion_failure(#ptr " is not null", "", __func__, __FILE__, __LINE__); \
    } while (0)

#if defined(BOTAN_ENABLE_DEBUG_ASSERTS)

#define BOTAN_DEBUG_ASSERT(expr) BOTAN_ASSERT_NOMSG(expr)

#else

#define BOTAN_DEBUG_ASSERT(expr) \
    do {                         \
    } while (0)

#endif

/**
 * Mark variable as unused.
 *
 * Takes any number of arguments and marks all as unused, for instance
 * BOTAN_UNUSED(a); or BOTAN_UNUSED(x, y, z);
 */
template <typename T>
constexpr void ignore_param(T&&) {}

template <typename... T>
constexpr void ignore_params(T&&... args) {
    (ignore_param(args), ...);
}

#define BOTAN_UNUSED Botan::ignore_params

/*
 * Define Botan::assert_unreachable and BOTAN_ASSERT_UNREACHABLE
 *
 * This is intended to be used in the same situations as `std::unreachable()`;
 * a codepath that (should not) be reachable but where the compiler cannot
 * tell that it is unreachable.
 *
 * Unlike `std::unreachable()`, or equivalent compiler builtins like GCC's
 * `__builtin_unreachable`, this function is not UB. By default it will
 * throw an exception. If `BOTAN_TERMINATE_ON_ASSERTS` is defined, it will
 * instead print a message to stderr and abort.
 *
 * Due to this difference, and the fact that it is not inlined, calling
 * this is significantly more costly than using `std::unreachable`.
 */
[[noreturn]] void BOTAN_UNSTABLE_API assert_unreachable(const char* file, int line);

#define BOTAN_ASSERT_UNREACHABLE() Botan::assert_unreachable(__FILE__, __LINE__)

}    // namespace Botan

#if !defined(BOTAN_IS_BEING_BUILT) && !defined(BOTAN_DISABLE_DEPRECATED_FEATURES)
     // TODO(Botan4) remove this
#include <deque>
#endif

namespace Botan {

template <typename T>
#if !defined(_ITERATOR_DEBUG_LEVEL) || _ITERATOR_DEBUG_LEVEL == 0
/*
 * Assert exists to prevent someone from doing something that will
 * probably crash anyway (like secure_vector<non_POD_t> where ~non_POD_t
 * deletes a member pointer which was zeroed before it ran).
 * MSVC in debug mode uses non-integral proxy types in container types
 * like std::vector, thus we disable the check there.
 */
    requires std::is_integral<T>::value || std::is_enum<T>::value
#endif
class secure_allocator {

   public:
    typedef T value_type;
    typedef std::size_t size_type;

    secure_allocator() noexcept = default;
    secure_allocator(const secure_allocator&) noexcept = default;
    secure_allocator& operator=(const secure_allocator&) noexcept = default;
    ~secure_allocator() noexcept = default;

    template <typename U>
    secure_allocator(const secure_allocator<U>&) noexcept {}

    T* allocate(std::size_t n) { return static_cast<T*>(allocate_memory(n, sizeof(T))); }

    void deallocate(T* p, std::size_t n) { deallocate_memory(p, n, sizeof(T)); }
};

template <typename T, typename U>
inline bool operator==(const secure_allocator<T>&, const secure_allocator<U>&) {
    return true;
}

template <typename T, typename U>
inline bool operator!=(const secure_allocator<T>&, const secure_allocator<U>&) {
    return false;
}

template <typename T>
using secure_vector = std::vector<T, secure_allocator<T>>;

#if !defined(BOTAN_IS_BEING_BUILT) && !defined(BOTAN_DISABLE_DEPRECATED_FEATURES)
template <typename T>
using secure_deque = std::deque<T, secure_allocator<T>>;
#endif

// For better compatibility with 1.10 API
template <typename T>
using SecureVector = secure_vector<T>;

template <typename T>
secure_vector<T> lock(const std::vector<T>& in) {
    return secure_vector<T>(in.begin(), in.end());
}

template <typename T>
std::vector<T> unlock(const secure_vector<T>& in) {
    return std::vector<T>(in.begin(), in.end());
}

template <typename T, typename Alloc, typename Alloc2>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, const std::vector<T, Alloc2>& in) {
    out.insert(out.end(), in.begin(), in.end());
    return out;
}

template <typename T, typename Alloc>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, T in) {
    out.push_back(in);
    return out;
}

template <typename T, typename Alloc, typename L>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, const std::pair<const T*, L>& in) {
    out.insert(out.end(), in.first, in.first + in.second);
    return out;
}

template <typename T, typename Alloc, typename L>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, const std::pair<T*, L>& in) {
    out.insert(out.end(), in.first, in.first + in.second);
    return out;
}

/**
 * Zeroise the values; length remains unchanged
 * @param vec the vector to zeroise
 */
template <typename T, typename Alloc>
void zeroise(std::vector<T, Alloc>& vec) {
    std::fill(vec.begin(), vec.end(), static_cast<T>(0));
}

/**
 * Zeroise the values then free the memory
 * @param vec the vector to zeroise and free
 */
template <typename T, typename Alloc>
void zap(std::vector<T, Alloc>& vec) {
    zeroise(vec);
    vec.clear();
    vec.shrink_to_fit();
}

}    // namespace Botan

namespace Botan {

/**
* Perform base64 encoding
* @param output an array of at least base64_encode_max_output bytes
* @param input is some binary data
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param final_inputs true iff this is the last input, in which case
         padding chars will be applied if needed
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
    base64_encode(char output[], const uint8_t input[], size_t input_length, size_t& input_consumed,
                  bool final_inputs);

/**
 * Perform base64 encoding
 * @param input some input
 * @param input_length length of input in bytes
 * @return base64adecimal representation of input
 */
std::string BOTAN_PUBLIC_API(2, 0) base64_encode(const uint8_t input[], size_t input_length);

/**
 * Perform base64 encoding
 * @param input some input
 * @return base64adecimal representation of input
 */
inline std::string base64_encode(std::span<const uint8_t> input) {
    return base64_encode(input.data(), input.size());
}

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param final_inputs true iff this is the last input, in which case
         padding is allowed
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
    base64_decode(uint8_t output[], const char input[], size_t input_length, size_t& input_consumed,
                  bool final_inputs, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param input_length length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
    base64_decode(uint8_t output[], const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0)
    base64_decode(uint8_t output[], std::string_view input, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output a contiguous byte buffer of at least base64_decode_max_output bytes
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0)
    base64_decode(std::span<uint8_t> output, std::string_view input, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param input some base64 input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded base64 output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(2, 0)
    base64_decode(const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded base64 output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(3, 0)
    base64_decode(std::string_view input, bool ignore_ws = true);

/**
 * Calculate the size of output buffer for base64_encode
 * @param input_length the length of input in bytes
 * @return the size of output buffer in bytes
 */
size_t BOTAN_PUBLIC_API(2, 1) base64_encode_max_output(size_t input_length);

/**
 * Calculate the size of output buffer for base64_decode
 * @param input_length the length of input in bytes
 * @return the size of output buffer in bytes
 */
size_t BOTAN_PUBLIC_API(2, 1) base64_decode_max_output(size_t input_length);

}    // namespace Botan

namespace Botan {

class OctetString;

/**
 * Represents the length requirements on an algorithm key
 */
class BOTAN_PUBLIC_API(2, 0) Key_Length_Specification final {
   public:
    /**
     * Constructor for fixed length keys
     * @param keylen the supported key length
     */
    explicit Key_Length_Specification(size_t keylen)
        : m_min_keylen(keylen), m_max_keylen(keylen), m_keylen_mod(1) {}

    /**
     * Constructor for variable length keys
     * @param min_k the smallest supported key length
     * @param max_k the largest supported key length
     * @param k_mod the number of bytes the key must be a multiple of
     */
    Key_Length_Specification(size_t min_k, size_t max_k, size_t k_mod = 1)
        : m_min_keylen(min_k), m_max_keylen(max_k ? max_k : min_k), m_keylen_mod(k_mod) {}

    /**
     * @param length is a key length in bytes
     * @return true iff this length is a valid length for this algo
     */
    bool valid_keylength(size_t length) const {
        return ((length >= m_min_keylen) && (length <= m_max_keylen) &&
                (length % m_keylen_mod == 0));
    }

    /**
     * @return minimum key length in bytes
     */
    size_t minimum_keylength() const { return m_min_keylen; }

    /**
     * @return maximum key length in bytes
     */
    size_t maximum_keylength() const { return m_max_keylen; }

    /**
     * @return key length multiple in bytes
     */
    size_t keylength_multiple() const { return m_keylen_mod; }

    /*
     * Multiplies all length requirements with the given factor
     * @param n the multiplication factor
     * @return a key length specification multiplied by the factor
     */
    Key_Length_Specification multiple(size_t n) const {
        return Key_Length_Specification(n * m_min_keylen, n * m_max_keylen, n * m_keylen_mod);
    }

   private:
    size_t m_min_keylen, m_max_keylen, m_keylen_mod;
};

/**
 * This class represents a symmetric algorithm object.
 */
class BOTAN_PUBLIC_API(2, 0) SymmetricAlgorithm {
   public:
    virtual ~SymmetricAlgorithm() = default;

    /**
     * Reset the internal state. This includes not just the key, but
     * any partial message that may have been in process.
     */
    virtual void clear() = 0;

    /**
     * @return object describing limits on key size
     */
    virtual Key_Length_Specification key_spec() const = 0;

    /**
     * @return maximum allowed key length
     */
    size_t maximum_keylength() const { return key_spec().maximum_keylength(); }

    /**
     * @return minimum allowed key length
     */
    size_t minimum_keylength() const { return key_spec().minimum_keylength(); }

    /**
     * Check whether a given key length is valid for this algorithm.
     * @param length the key length to be checked.
     * @return true if the key length is valid.
     */
    bool valid_keylength(size_t length) const { return key_spec().valid_keylength(length); }

    /**
     * Set the symmetric key of this object.
     * @param key the SymmetricKey to be set.
     */
    void set_key(const OctetString& key);

    /**
     * Set the symmetric key of this object.
     * @param key the contiguous byte range to be set.
     */
    void set_key(std::span<const uint8_t> key);

    /**
     * Set the symmetric key of this object.
     * @param key the to be set as a byte array.
     * @param length in bytes of key param
     */
    void set_key(const uint8_t key[], size_t length) { set_key(std::span{key, length}); }

    /**
     * @return the algorithm name
     */
    virtual std::string name() const = 0;

    /**
     * @return true if a key has been set on this object
     */
    virtual bool has_keying_material() const = 0;

   protected:
    void assert_key_material_set() const { assert_key_material_set(has_keying_material()); }

    void assert_key_material_set(bool predicate) const {
        if (!predicate) {
            throw_key_not_set_error();
        }
    }

   private:
    void throw_key_not_set_error() const;

    /**
     * Run the key schedule
     * @param key the key
     */
    virtual void key_schedule(std::span<const uint8_t> key) = 0;
};

}    // namespace Botan

namespace Botan {

/**
 * This class represents a block cipher object.
 */
class BOTAN_PUBLIC_API(2, 0) BlockCipher : public SymmetricAlgorithm {
   public:
    /**
     * Create an instance based on a name
     * If provider is empty then best available is chosen.
     * @param algo_spec algorithm name
     * @param provider provider implementation to choose
     * @return a null pointer if the algo/provider combination cannot be found
     */
    static std::unique_ptr<BlockCipher> create(std::string_view algo_spec,
                                               std::string_view provider = "");

    /**
     * Create an instance based on a name, or throw if the
     * algo/provider combination cannot be found. If provider is
     * empty then best available is chosen.
     */
    static std::unique_ptr<BlockCipher> create_or_throw(std::string_view algo_spec,
                                                        std::string_view provider = "");

    /**
     * @return list of available providers for this algorithm, empty if not available
     * @param algo_spec algorithm name
     */
    static std::vector<std::string> providers(std::string_view algo_spec);

    /**
     * Multiplier on a block cipher's native parallelism
     *
     * Usually notable performance gains come from further loop blocking,
     * at least for 2 or 4x
     */
    static constexpr size_t ParallelismMult = 4;

    /**
     * @return block size of this algorithm
     */
    virtual size_t block_size() const = 0;

    /**
     * @return native parallelism of this cipher in blocks
     */
    virtual size_t parallelism() const { return 1; }

    /**
     * @return prefererred parallelism of this cipher in bytes
     */
    size_t parallel_bytes() const {
        return parallelism() * block_size() * BlockCipher::ParallelismMult;
    }

    /**
     * @return provider information about this implementation. Default is "base",
     * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
     */
    virtual std::string provider() const { return "base"; }

    /**
     * Encrypt a block.
     * @param in The plaintext block to be encrypted as a byte array.
     * Must be of length block_size().
     * @param out The byte array designated to hold the encrypted block.
     * Must be of length block_size().
     */
    void encrypt(const uint8_t in[], uint8_t out[]) const { encrypt_n(in, out, 1); }

    /**
     * Decrypt a block.
     * @param in The ciphertext block to be decypted as a byte array.
     * Must be of length block_size().
     * @param out The byte array designated to hold the decrypted block.
     * Must be of length block_size().
     */
    void decrypt(const uint8_t in[], uint8_t out[]) const { decrypt_n(in, out, 1); }

    /**
     * Encrypt a block.
     * @param block the plaintext block to be encrypted
     * Must be of length block_size(). Will hold the result when the function
     * has finished.
     */
    void encrypt(uint8_t block[]) const { encrypt_n(block, block, 1); }

    /**
     * Decrypt a block.
     * @param block the ciphertext block to be decrypted
     * Must be of length block_size(). Will hold the result when the function
     * has finished.
     */
    void decrypt(uint8_t block[]) const { decrypt_n(block, block, 1); }

    /**
     * Encrypt one or more blocks
     * @param block the input/output buffer (multiple of block_size())
     */
    void encrypt(std::span<uint8_t> block) const {
        return encrypt_n(block.data(), block.data(), block.size() / block_size());
    }

    /**
     * Decrypt one or more blocks
     * @param block the input/output buffer (multiple of block_size())
     */
    void decrypt(std::span<uint8_t> block) const {
        return decrypt_n(block.data(), block.data(), block.size() / block_size());
    }

    /**
     * Encrypt one or more blocks
     * @param in the input buffer (multiple of block_size())
     * @param out the output buffer (same size as in)
     */
    void encrypt(std::span<const uint8_t> in, std::span<uint8_t> out) const {
        return encrypt_n(in.data(), out.data(), in.size() / block_size());
    }

    /**
     * Decrypt one or more blocks
     * @param in the input buffer (multiple of block_size())
     * @param out the output buffer (same size as in)
     */
    void decrypt(std::span<const uint8_t> in, std::span<uint8_t> out) const {
        return decrypt_n(in.data(), out.data(), in.size() / block_size());
    }

    /**
     * Encrypt one or more blocks
     * @param in the input buffer (multiple of block_size())
     * @param out the output buffer (same size as in)
     * @param blocks the number of blocks to process
     */
    virtual void encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const = 0;

    /**
     * Decrypt one or more blocks
     * @param in the input buffer (multiple of block_size())
     * @param out the output buffer (same size as in)
     * @param blocks the number of blocks to process
     */
    virtual void decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const = 0;

    BOTAN_DEPRECATED("Deprecated no replacement")
    void encrypt_n_xex(uint8_t data[], const uint8_t mask[], size_t blocks) const {
        const size_t BS = block_size();
        for (size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
        }
        encrypt_n(data, data, blocks);
        for (size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
        }
    }

    BOTAN_DEPRECATED("Deprecated no replacement")
    void decrypt_n_xex(uint8_t data[], const uint8_t mask[], size_t blocks) const {
        const size_t BS = block_size();
        for (size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
        }
        decrypt_n(data, data, blocks);
        for (size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
        }
    }

    /**
     * @return new object representing the same algorithm as *this
     */
    virtual std::unique_ptr<BlockCipher> new_object() const = 0;

    BlockCipher* clone() const { return this->new_object().release(); }

    ~BlockCipher() override = default;
};

/**
 * Tweakable block ciphers allow setting a tweak which is a non-keyed
 * value which affects the encryption/decryption operation.
 */
class BOTAN_PUBLIC_API(2, 8) Tweakable_Block_Cipher : public BlockCipher {
   public:
    /**
     * Set the tweak value. This must be called after setting a key. The value
     * persists until either set_tweak, set_key, or clear is called.
     * Different algorithms support different tweak length(s). If called with
     * an unsupported length, Invalid_Argument will be thrown.
     */
    virtual void set_tweak(const uint8_t tweak[], size_t len) = 0;
};

/**
 * Represents a block cipher with a single fixed block size
 */
template <size_t BS, size_t KMIN, size_t KMAX = 0, size_t KMOD = 1,
          typename BaseClass = BlockCipher>
class Block_Cipher_Fixed_Params : public BaseClass {
   public:
    enum { BLOCK_SIZE = BS };

    size_t block_size() const final { return BS; }

    Key_Length_Specification key_spec() const final {
        return Key_Length_Specification(KMIN, KMAX, KMOD);
    }
};

}    // namespace Botan

namespace Botan {

/**
 * Different types of errors that might occur
 */
enum class ErrorType {
    /** Some unknown error */
    Unknown = 1,
    /** An error while calling a system interface */
    SystemError,
    /** An operation seems valid, but not supported by the current version */
    NotImplemented,
    /** Memory allocation failure */
    OutOfMemory,
    /** An internal error occurred */
    InternalError,
    /** An I/O error occurred */
    IoError,

    /** Invalid object state */
    InvalidObjectState = 100,
    /** A key was not set on an object when this is required */
    KeyNotSet,
    /** The application provided an argument which is invalid */
    InvalidArgument,
    /** A key with invalid length was provided */
    InvalidKeyLength,
    /** A nonce with invalid length was provided */
    InvalidNonceLength,
    /** An object type was requested but cannot be found */
    LookupError,
    /** Encoding a message or datum failed */
    EncodingFailure,
    /** Decoding a message or datum failed */
    DecodingFailure,
    /** A TLS error (error_code will be the alert type) */
    TLSError,
    /** An error during an HTTP operation */
    HttpError,
    /** A message with an invalid authentication tag was detected */
    InvalidTag,
    /** An error during Roughtime validation */
    RoughtimeError,

    /** An error when interacting with CommonCrypto API */
    CommonCryptoError = 201,
    /** An error when interacting with a PKCS11 device */
    Pkcs11Error,
    /** An error when interacting with a TPM device */
    TPMError,
    /** An error when interacting with a database */
    DatabaseError,

    /** An error when interacting with zlib */
    ZlibError = 300,
    /** An error when interacting with bzip2 */
    Bzip2Error,
    /** An error when interacting with lzma */
    LzmaError,

};

//! \brief Convert an ErrorType to string
std::string BOTAN_PUBLIC_API(2, 11) to_string(ErrorType type);

/**
 * Base class for all exceptions thrown by the library
 */
class BOTAN_PUBLIC_API(2, 0) Exception : public std::exception {
   public:
    /**
     * Return a descriptive string which is hopefully comprehensible to
     * a developer. It will likely not be useful for an end user.
     *
     * The string has no particular format, and the content of exception
     * messages may change from release to release. Thus the main use of this
     * function is for logging or debugging.
     */
    const char* what() const noexcept override { return m_msg.c_str(); }

    /**
     * Return the "type" of error which occurred.
     */
    virtual ErrorType error_type() const noexcept { return ErrorType::Unknown; }

    /**
     * Return an error code associated with this exception, or otherwise 0.
     *
     * The domain of this error varies depending on the source, for example on
     * POSIX systems it might be errno, while on a Windows system it might be
     * the result of GetLastError or WSAGetLastError.
     */
    virtual int error_code() const noexcept { return 0; }

    /**
     * Avoid throwing base Exception, use a subclass
     */
    explicit Exception(std::string_view msg);

    /**
     * Avoid throwing base Exception, use a subclass
     */
    Exception(const char* prefix, std::string_view msg);

    /**
     * Avoid throwing base Exception, use a subclass
     */
    Exception(std::string_view msg, const std::exception& e);

   private:
    std::string m_msg;
};

/**
 * An invalid argument was provided to an API call.
 */
class BOTAN_PUBLIC_API(2, 0) Invalid_Argument : public Exception {
   public:
    explicit Invalid_Argument(std::string_view msg);

    explicit Invalid_Argument(std::string_view msg, std::string_view where);

    Invalid_Argument(std::string_view msg, const std::exception& e);

    ErrorType error_type() const noexcept override { return ErrorType::InvalidArgument; }
};

/**
 * An invalid/unknown field name was passed to Public_Key::get_int_field
 */
class BOTAN_PUBLIC_API(3, 0) Unknown_PK_Field_Name final : public Invalid_Argument {
   public:
    Unknown_PK_Field_Name(std::string_view algo_name, std::string_view field_name);
};

/**
 * An invalid key length was used
 */
class BOTAN_PUBLIC_API(2, 0) Invalid_Key_Length final : public Invalid_Argument {
   public:
    Invalid_Key_Length(std::string_view name, size_t length);

    ErrorType error_type() const noexcept override { return ErrorType::InvalidKeyLength; }
};

/**
 * An invalid nonce length was used
 */
class BOTAN_PUBLIC_API(2, 0) Invalid_IV_Length final : public Invalid_Argument {
   public:
    Invalid_IV_Length(std::string_view mode, size_t bad_len);

    ErrorType error_type() const noexcept override { return ErrorType::InvalidNonceLength; }
};

/**
 * Invalid_Algorithm_Name Exception
 */
class BOTAN_PUBLIC_API(2, 0) Invalid_Algorithm_Name final : public Invalid_Argument {
   public:
    explicit Invalid_Algorithm_Name(std::string_view name);
};

/**
 * Encoding_Error Exception
 */
class BOTAN_PUBLIC_API(2, 0) Encoding_Error final : public Exception {
   public:
    explicit Encoding_Error(std::string_view name);

    ErrorType error_type() const noexcept override { return ErrorType::EncodingFailure; }
};

/**
 * A decoding error occurred.
 */
class BOTAN_PUBLIC_API(2, 0) Decoding_Error : public Exception {
   public:
    explicit Decoding_Error(std::string_view name);

    Decoding_Error(std::string_view category, std::string_view err);

    Decoding_Error(std::string_view msg, const std::exception& e);

    ErrorType error_type() const noexcept override { return ErrorType::DecodingFailure; }
};

/**
 * Invalid state was encountered. A request was made on an object while the
 * object was in a state where the operation cannot be performed.
 */
class BOTAN_PUBLIC_API(2, 0) Invalid_State : public Exception {
   public:
    explicit Invalid_State(std::string_view err) : Exception(err) {}

    ErrorType error_type() const noexcept override { return ErrorType::InvalidObjectState; }
};

/**
 * A PRNG was called on to produce output while still unseeded
 */
class BOTAN_PUBLIC_API(2, 0) PRNG_Unseeded final : public Invalid_State {
   public:
    explicit PRNG_Unseeded(std::string_view algo);
};

/**
 * The key was not set on an object. This occurs with symmetric objects where
 * an operation which requires the key is called prior to set_key being called.
 */
class BOTAN_PUBLIC_API(2, 4) Key_Not_Set : public Invalid_State {
   public:
    explicit Key_Not_Set(std::string_view algo);

    ErrorType error_type() const noexcept override { return ErrorType::KeyNotSet; }
};

/**
 * A request was made for some kind of object which could not be located
 */
class BOTAN_PUBLIC_API(2, 0) Lookup_Error : public Exception {
   public:
    explicit Lookup_Error(std::string_view err) : Exception(err) {}

    Lookup_Error(std::string_view type, std::string_view algo, std::string_view provider = "");

    ErrorType error_type() const noexcept override { return ErrorType::LookupError; }
};

/**
 * Algorithm_Not_Found Exception
 *
 * @warning This exception type will be removed in the future. Instead
 * just catch Lookup_Error.
 */
class BOTAN_PUBLIC_API(2, 0) Algorithm_Not_Found final : public Lookup_Error {
   public:
    explicit Algorithm_Not_Found(std::string_view name);
};

/**
 * Provider_Not_Found is thrown when a specific provider was requested
 * but that provider is not available.
 *
 * @warning This exception type will be removed in the future. Instead
 * just catch Lookup_Error.
 */
class BOTAN_PUBLIC_API(2, 0) Provider_Not_Found final : public Lookup_Error {
   public:
    Provider_Not_Found(std::string_view algo, std::string_view provider);
};

/**
 * An AEAD or MAC check detected a message modification
 *
 * In versions before 2.10, Invalid_Authentication_Tag was named
 * Integrity_Failure, it was renamed to make its usage more clear.
 */
class BOTAN_PUBLIC_API(2, 0) Invalid_Authentication_Tag final : public Exception {
   public:
    explicit Invalid_Authentication_Tag(std::string_view msg);

    ErrorType error_type() const noexcept override { return ErrorType::InvalidTag; }
};

/**
 * For compatibility with older versions
 */
typedef Invalid_Authentication_Tag Integrity_Failure;

/**
 * An error occurred while operating on an IO stream
 */
class BOTAN_PUBLIC_API(2, 0) Stream_IO_Error final : public Exception {
   public:
    explicit Stream_IO_Error(std::string_view err);

    ErrorType error_type() const noexcept override { return ErrorType::IoError; }
};

/**
 * System_Error
 *
 * This exception is thrown in the event of an error related to interacting
 * with the operating system.
 *
 * This exception type also (optionally) captures an integer error code eg
 * POSIX errno or Windows GetLastError.
 */
class BOTAN_PUBLIC_API(2, 9) System_Error : public Exception {
   public:
    System_Error(std::string_view msg) : Exception(msg), m_error_code(0) {}

    System_Error(std::string_view msg, int err_code);

    ErrorType error_type() const noexcept override { return ErrorType::SystemError; }

    int error_code() const noexcept override { return m_error_code; }

   private:
    int m_error_code;
};

/**
 * An internal error occurred. If observed, please file a bug.
 */
class BOTAN_PUBLIC_API(2, 0) Internal_Error : public Exception {
   public:
    explicit Internal_Error(std::string_view err);

    ErrorType error_type() const noexcept override { return ErrorType::InternalError; }
};

/**
 * Not Implemented Exception
 *
 * This is thrown in the situation where a requested operation is
 * logically valid but is not implemented by this version of the library.
 */
class BOTAN_PUBLIC_API(2, 0) Not_Implemented final : public Exception {
   public:
    explicit Not_Implemented(std::string_view err);

    ErrorType error_type() const noexcept override { return ErrorType::NotImplemented; }
};

template <typename E, typename... Args>
inline void do_throw_error(const char* file, int line, const char* func, Args... args) {
    throw E(file, line, func, args...);
}

}    // namespace Botan

namespace Botan {

template <typename T, typename Tag, typename... Capabilities>
class Strong;

template <typename... Ts>
struct is_strong_type : std::false_type {};

template <typename... Ts>
struct is_strong_type<Strong<Ts...>> : std::true_type {};

template <typename... Ts>
constexpr bool is_strong_type_v = is_strong_type<std::remove_const_t<Ts>...>::value;

template <typename T0 = void, typename... Ts>
struct all_same {
    static constexpr bool value = (std::is_same_v<T0, Ts> && ... && true);
};

template <typename... Ts>
static constexpr bool all_same_v = all_same<Ts...>::value;

namespace detail {

/**
 * Helper type to indicate that a certain type should be automatically
 * detected based on the context.
 */
struct AutoDetect {
    constexpr AutoDetect() = delete;
};

}    // namespace detail

namespace ranges {

/**
 * Models a std::ranges::contiguous_range that (optionally) restricts its
 * value_type to ValueT. In other words: a stretch of contiguous memory of
 * a certain type (optional ValueT).
 */
template <typename T, typename ValueT = std::ranges::range_value_t<T>>
concept contiguous_range =
    std::ranges::contiguous_range<T> && std::same_as<ValueT, std::ranges::range_value_t<T>>;

/**
 * Models a std::ranges::contiguous_range that satisfies
 * std::ranges::output_range with an arbitrary value_type. In other words: a
 * stretch of contiguous memory of a certain type (optional ValueT) that can be
 * written to.
 */
template <typename T, typename ValueT = std::ranges::range_value_t<T>>
concept contiguous_output_range =
    contiguous_range<T, ValueT> && std::ranges::output_range<T, ValueT>;

/**
 * Models a range that can be turned into a std::span<>. Typically, this is some
 * form of ranges::contiguous_range.
 */
template <typename T>
concept spanable_range = std::constructible_from<std::span<const std::ranges::range_value_t<T>>, T>;

/**
 * Models a range that can be turned into a std::span<> with a static extent.
 * Typically, this is a std::array or a std::span derived from an array.
 */
// clang-format off
template <typename T>
concept statically_spanable_range = spanable_range<T> &&
                                    decltype(std::span{std::declval<T&>()})::extent != std::dynamic_extent;

// clang-format on

/**
 * Find the length in bytes of a given contiguous range @p r.
 */
inline constexpr size_t size_bytes(spanable_range auto&& r) { return std::span{r}.size_bytes(); }

/**
 * Check that a given range @p r has a certain statically-known byte length. If
 * the range's extent is known at compile time, this is a static check,
 * otherwise a runtime argument check will be added.
 *
 * @throws Invalid_Argument  if range @p r has a dynamic extent and does not
 *                           feature the expected byte length.
 */
template <size_t expected, spanable_range R>
inline constexpr void assert_exact_byte_length(R&& r) {
    const std::span s{r};
    if constexpr (statically_spanable_range<R>) {
        static_assert(s.size_bytes() == expected,
                      "memory region does not have expected byte lengths");
    } else {
        if (s.size_bytes() != expected) {
            throw Invalid_Argument("Memory regions did not have expected byte lengths");
        }
    }
}

/**
 * Check that a list of ranges (in @p r0 and @p rs) all have the same byte
 * lengths. If the first range's extent is known at compile time, this will be a
 * static check for all other ranges whose extents are known at compile time,
 * otherwise a runtime argument check will be added.
 *
 * @throws Invalid_Argument  if any range has a dynamic extent and not all
 *                           ranges feature the same byte length.
 */
template <spanable_range R0, spanable_range... Rs>
inline constexpr void assert_equal_byte_lengths(R0&& r0, Rs&&... rs)
    requires(sizeof...(Rs) > 0)
{
    const std::span s0{r0};

    if constexpr (statically_spanable_range<R0>) {
        constexpr size_t expected_size = s0.size_bytes();
        (assert_exact_byte_length<expected_size>(rs), ...);
    } else {
        const size_t expected_size = s0.size_bytes();
        const bool correct_size =
            ((std::span<const std::ranges::range_value_t<Rs>>{rs}.size_bytes() == expected_size) &&
             ...);

        if (!correct_size) {
            throw Invalid_Argument("Memory regions did not have equal lengths");
        }
    }
}

}    // namespace ranges

namespace concepts {

// TODO: C++20 provides concepts like std::ranges::range or ::sized_range
//       but at the time of this writing clang had not caught up on all
//       platforms. E.g. clang 14 on Xcode does not support ranges properly.

template <typename IterT, typename ContainerT>
concept container_iterator = std::same_as<IterT, typename ContainerT::iterator> ||
                             std::same_as<IterT, typename ContainerT::const_iterator>;

template <typename PtrT, typename ContainerT>
concept container_pointer = std::same_as<PtrT, typename ContainerT::pointer> ||
                            std::same_as<PtrT, typename ContainerT::const_pointer>;

template <typename T>
concept container = requires(T a) {
    { a.begin() } -> container_iterator<T>;
    { a.end() } -> container_iterator<T>;
    { a.cbegin() } -> container_iterator<T>;
    { a.cend() } -> container_iterator<T>;
    { a.size() } -> std::same_as<typename T::size_type>;
    typename T::value_type;
};

template <typename T>
concept contiguous_container = container<T> && requires(T a) {
    { a.data() } -> container_pointer<T>;
};

template <typename T>
concept has_empty = requires(T a) {
    { a.empty() } -> std::same_as<bool>;
};

// clang-format off
template <typename T>
concept has_bounds_checked_accessors = container<T> && (
                                          requires(T a, const T ac, typename T::size_type s) {
                                             { a.at(s) } -> std::same_as<typename T::value_type&>;
                                             { ac.at(s) } -> std::same_as<const typename T::value_type&>;
                                          } ||
                                          requires(T a, const T ac, typename T::key_type k) {
                                             { a.at(k) } -> std::same_as<typename T::mapped_type&>;
                                             { ac.at(k) } -> std::same_as<const typename T::mapped_type&>;
                                          });
// clang-format on

template <typename T>
concept resizable_container = container<T> && requires(T& c, typename T::size_type s) {
    T(s);
    c.resize(s);
};

template <typename T>
concept reservable_container =
    container<T> && requires(T& c, typename T::size_type s) { c.reserve(s); };

template <typename T>
concept resizable_byte_buffer = contiguous_container<T> && resizable_container<T> &&
                                std::same_as<typename T::value_type, uint8_t>;

template <typename T>
concept streamable = requires(std::ostream& os, T a) { os << a; };

template <class T>
concept strong_type = is_strong_type_v<T>;

template <class T>
concept contiguous_strong_type = strong_type<T> && contiguous_container<T>;

template <class T>
concept integral_strong_type = strong_type<T> && std::integral<typename T::wrapped_type>;

template <class T>
concept unsigned_integral_strong_type =
    strong_type<T> && std::unsigned_integral<typename T::wrapped_type>;

template <typename T, typename Capability>
concept strong_type_with_capability = T::template has_capability<Capability>();

}    // namespace concepts

}    // namespace Botan

namespace Botan {

/**
 * This class represents any kind of computation which uses an internal
 * state, such as hash functions or MACs
 */
class BOTAN_PUBLIC_API(2, 0) Buffered_Computation {
   public:
    /**
     * @return length of the output of this function in bytes
     */
    virtual size_t output_length() const = 0;

    /**
     * Add new input to process.
     * @param in the input to process as a byte array
     * @param length of param in in bytes
     */
    void update(const uint8_t in[], size_t length) { add_data({in, length}); }

    /**
     * Add new input to process.
     * @param in the input to process as a contiguous data range
     */
    void update(std::span<const uint8_t> in) { add_data(in); }

    void update_be(uint16_t val);
    void update_be(uint32_t val);
    void update_be(uint64_t val);

    void update_le(uint16_t val);
    void update_le(uint32_t val);
    void update_le(uint64_t val);

    /**
     * Add new input to process.
     * @param str the input to process as a std::string_view. Will be interpreted
     * as a byte array based on the strings encoding.
     */
    void update(std::string_view str);

    /**
     * Process a single byte.
     * @param in the byte to process
     */
    void update(uint8_t in) { add_data({&in, 1}); }

    /**
     * Complete the computation and retrieve the
     * final result.
     * @param out The byte array to be filled with the result.
     * Must be of length output_length()
     */
    void final(uint8_t out[]) { final_result({out, output_length()}); }

    /**
     * Complete the computation and retrieve the
     * final result as a container of your choice.
     * @return a contiguous container holding the result
     */
    template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
    T final() {
        T output(output_length());
        final_result(output);
        return output;
    }

    std::vector<uint8_t> final_stdvec() { return final<std::vector<uint8_t>>(); }

    void final(std::span<uint8_t> out);

    template <concepts::resizable_byte_buffer T>
    void final(T& out) {
        out.resize(output_length());
        final_result(out);
    }

    /**
     * Update and finalize computation. Does the same as calling update()
     * and final() consecutively.
     * @param in the input to process as a byte array
     * @param length the length of the byte array
     * @result the result of the call to final()
     */
    template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
    T process(const uint8_t in[], size_t length) {
        update(in, length);
        return final<T>();
    }

    /**
     * Update and finalize computation. Does the same as calling update()
     * and final() consecutively.
     * @param in the input to process as a string
     * @result the result of the call to final()
     */
    template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
    T process(std::string_view in) {
        update(in);
        return final<T>();
    }

    /**
     * Update and finalize computation. Does the same as calling update()
     * and final() consecutively.
     * @param in the input to process as a contiguous container
     * @result the result of the call to final()
     */
    template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
    T process(std::span<const uint8_t> in) {
        update(in);
        return final<T>();
    }

    virtual ~Buffered_Computation() = default;

   private:
    /**
     * Add more data to the computation
     * @param input is an input buffer
     */
    virtual void add_data(std::span<const uint8_t> input) = 0;

    /**
     * Write the final output to out
     * @param out is an output buffer of output_length()
     */
    virtual void final_result(std::span<uint8_t> out) = 0;
};

}    // namespace Botan

namespace Botan {

/**
 * The two possible directions a Cipher_Mode can operate in
 */
enum class Cipher_Dir : int {
    Encryption,
    Decryption,

    ENCRYPTION BOTAN_DEPRECATED("Use Cipher_Dir::Encryption") = Encryption,
    DECRYPTION BOTAN_DEPRECATED("Use Cipher_Dir::Decryption") = Decryption,
};

/**
 * Interface for cipher modes
 */
class BOTAN_PUBLIC_API(2, 0) Cipher_Mode : public SymmetricAlgorithm {
   public:
    /**
     * @return list of available providers for this algorithm, empty if not available
     * @param algo_spec algorithm name
     */
    static std::vector<std::string> providers(std::string_view algo_spec);

    /**
     * Create an AEAD mode
     * @param algo the algorithm to create
     * @param direction specify if this should be an encryption or decryption AEAD
     * @param provider optional specification for provider to use
     * @return an AEAD mode or a null pointer if not available
     */
    static std::unique_ptr<Cipher_Mode> create(std::string_view algo, Cipher_Dir direction,
                                               std::string_view provider = "");

    /**
     * Create an AEAD mode, or throw
     * @param algo the algorithm to create
     * @param direction specify if this should be an encryption or decryption AEAD
     * @param provider optional specification for provider to use
     * @return an AEAD mode, or throw an exception
     */
    static std::unique_ptr<Cipher_Mode> create_or_throw(std::string_view algo, Cipher_Dir direction,
                                                        std::string_view provider = "");

   protected:
    /*
     * Prepare for processing a message under the specified nonce
     */
    virtual void start_msg(const uint8_t nonce[], size_t nonce_len) = 0;

    /*
     * Process message blocks
     * Input must be a multiple of update_granularity.
     */
    virtual size_t process_msg(uint8_t msg[], size_t msg_len) = 0;

    /*
     * Finishes a message
     */
    virtual void finish_msg(secure_vector<uint8_t>& final_block, size_t offset = 0) = 0;

   public:
    /**
     * Begin processing a message with a fresh nonce.
     *
     * @warning Typically one must not reuse the same nonce for more than one
     *          message under the same key. For most cipher modes this would
     *          mean total loss of security and/or authenticity guarantees.
     *
     * @note If reliably generating unique nonces is difficult in your
     *       environment, use SIV which retains security even if nonces
     *       are repeated.
     *
     * @param nonce the per message nonce
     */
    void start(std::span<const uint8_t> nonce) { start_msg(nonce.data(), nonce.size()); }

    /**
     * Begin processing a message with a fresh nonce.
     * @param nonce the per message nonce
     * @param nonce_len length of nonce
     */
    void start(const uint8_t nonce[], size_t nonce_len) { start_msg(nonce, nonce_len); }

    /**
     * Begin processing a message.
     *
     * The exact semantics of this depend on the mode. For many modes, the call
     * will fail since a nonce must be provided.
     *
     * For certain modes such as CBC this will instead cause the last
     * ciphertext block to be used as the nonce of the new message; doing this
     * isn't a good idea, but some (mostly older) protocols do this.
     */
    void start() { return start_msg(nullptr, 0); }

    /**
     * Process message blocks
     *
     * Input must be a multiple of update_granularity
     *
     * Processes msg in place and returns bytes written. Normally
     * this will be either msg_len (indicating the entire message was
     * processed) or for certain AEAD modes zero (indicating that the
     * mode requires the entire message be processed in one pass).
     *
     * @param msg the message to be processed
     * @return bytes written in-place
     */
    size_t process(std::span<uint8_t> msg) { return this->process_msg(msg.data(), msg.size()); }

    size_t process(uint8_t msg[], size_t msg_len) { return this->process_msg(msg, msg_len); }

    /**
     * Process some data. Input must be in size update_granularity() uint8_t
     * blocks. The @p buffer is an in/out parameter and may be resized. In
     * particular, some modes require that all input be consumed before any
     * output is produced; with these modes, @p buffer will be returned empty.
     *
     * The first @p offset bytes of @p buffer will be ignored (this allows in
     * place processing of a buffer that contains an initial plaintext header).
     *
     * @param buffer in/out parameter which will possibly be resized
     * @param offset an offset into blocks to begin processing
     */
    template <concepts::resizable_byte_buffer T>
    void update(T& buffer, size_t offset = 0) {
        const size_t written = process(std::span(buffer).subspan(offset));
        buffer.resize(offset + written);
    }

    /**
     * Complete procession of a message with a final input of @p buffer, which
     * is treated the same as with update(). If you have the entire message in
     * hand, calling finish() without ever calling update() is both efficient
     * and convenient.
     *
     * When using an AEAD_Mode, if the supplied authentication tag does not
     * validate, this will throw an instance of Invalid_Authentication_Tag.
     *
     * If this occurs, all plaintext previously output via calls to update must
     * be destroyed and not used in any way that an attacker could observe the
     * effects of. This could be anything from echoing the plaintext back
     * (perhaps in an error message), or by making an external RPC whose
     * destination or contents depend on the plaintext. The only thing you can
     * do is buffer it, and in the event of an invalid tag, erase the
     * previously decrypted content from memory.
     *
     * One simple way to assure this could never happen is to never call
     * update, and instead always marshal the entire message into a single
     * buffer and call finish on it when decrypting.
     *
     * @param final_block in/out parameter which must be at least
     *        minimum_final_size() bytes, and will be set to any final output
     * @param offset an offset into final_block to begin processing
     */
    void finish(secure_vector<uint8_t>& final_block, size_t offset = 0) {
        finish_msg(final_block, offset);
    }

    /**
     * Complete procession of a message.
     *
     * Note: Using this overload with anything but a Botan::secure_vector<>
     *       is copying the bytes in the in/out buffer.
     *
     * @param final_block in/out parameter which must be at least
     *        minimum_final_size() bytes, and will be set to any final output
     * @param offset an offset into final_block to begin processing
     */
    template <concepts::resizable_byte_buffer T>
    void finish(T& final_block, size_t offset = 0) {
        Botan::secure_vector<uint8_t> tmp(final_block.begin(), final_block.end());
        finish_msg(tmp, offset);
        final_block.resize(tmp.size());
        std::copy(tmp.begin(), tmp.end(), final_block.begin());
    }

    /**
     * Returns the size of the output if this transform is used to process a
     * message with input_length bytes. In most cases the answer is precise.
     * If it is not possible to precise (namely for CBC decryption) instead an
     * upper bound is returned.
     */
    virtual size_t output_length(size_t input_length) const = 0;

    /**
     * The :cpp:class:`Cipher_Mode` interface requires message processing in
     * multiples of the block size. This returns size of required blocks to
     * update. If the mode implementation does not require buffering it will
     * return 1.
     * @return size of required blocks to update
     */
    virtual size_t update_granularity() const = 0;

    /**
     * Return an ideal granularity. This will be a multiple of the result of
     * update_granularity but may be larger. If so it indicates that better
     * performance may be achieved by providing buffers that are at least that
     * size (due to SIMD execution, etc).
     */
    virtual size_t ideal_granularity() const = 0;

    /**
     * Certain modes require the entire message be available before
     * any processing can occur. For such modes, input will be consumed
     * but not returned, until `finish` is called, which returns the
     * entire message.
     *
     * This function returns true if this mode has this style of
     * operation.
     */
    virtual bool requires_entire_message() const { return false; }

    /**
     * @return required minimium size to finalize() - may be any
     *         length larger than this.
     */
    virtual size_t minimum_final_size() const = 0;

    /**
     * @return the default size for a nonce
     */
    virtual size_t default_nonce_length() const = 0;

    /**
     * @return true iff nonce_len is a valid length for the nonce
     */
    virtual bool valid_nonce_length(size_t nonce_len) const = 0;

    /**
     * Resets just the message specific state and allows encrypting again under the existing key
     */
    virtual void reset() = 0;

    /**
     * Return the length in bytes of the authentication tag this algorithm
     * generates. If the mode is not authenticated, this will return 0.
     *
     * @return true iff this mode provides authentication as well as
     *         confidentiality.
     */
    bool authenticated() const { return this->tag_size() > 0; }

    /**
     * @return the size of the authentication tag used (in bytes)
     */
    virtual size_t tag_size() const { return 0; }

    /**
     * @return provider information about this implementation. Default is "base",
     * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
     */
    virtual std::string provider() const { return "base"; }
};

/**
 * Get a cipher mode by name (eg "AES-128/CBC" or "Serpent/XTS")
 * @param algo_spec cipher name
 * @param direction Cipher_Dir::Encryption or Cipher_Dir::Decryption
 * @param provider provider implementation to choose
 */
BOTAN_DEPRECATED("Use Cipher_Mode::create")
inline Cipher_Mode* get_cipher_mode(std::string_view algo_spec, Cipher_Dir direction,
                                    std::string_view provider = "") {
    return Cipher_Mode::create(algo_spec, direction, provider).release();
}

}    // namespace Botan

BOTAN_FUTURE_INTERNAL_HEADER(compiler.h)

/*
 * Define BOTAN_COMPILER_HAS_BUILTIN
 */
#if defined(__has_builtin)
#define BOTAN_COMPILER_HAS_BUILTIN(x) __has_builtin(x)
#else
#define BOTAN_COMPILER_HAS_BUILTIN(x) 0
#endif

/*
 * Define BOTAN_COMPILER_HAS_ATTRIBUTE
 */
#if defined(__has_attribute)
#define BOTAN_COMPILER_HAS_ATTRIBUTE(x) __has_attribute(x)
#define BOTAN_COMPILER_ATTRIBUTE(x) __attribute__((x))
#else
#define BOTAN_COMPILER_HAS_ATTRIBUTE(x) 0
#define BOTAN_COMPILER_ATTRIBUTE(x) /**/
#endif

/*
 * Hack for Loongarch64 GCC bug
 *
 * For some reason __has_attribute(target) is true, but it does not support the
 * target attribute... this supposedly is fixed in GCC 15 but this is untested.
 */
#if defined(__GNUC__) && defined(__loongarch64) && (__GNUC__ <= 14)
#define BOTAN_COMPILER_DOES_NOT_HAVE_TARGET_ATTRIBUTE
#endif

/*
 * Define BOTAN_FUNC_ISA
 *
 * TODO(Botan4) Move this to isa_extn.h
 */
#if BOTAN_COMPILER_HAS_ATTRIBUTE(target) && !defined(BOTAN_COMPILER_DOES_NOT_HAVE_TARGET_ATTRIBUTE)
#define BOTAN_FUNC_ISA(isa) BOTAN_COMPILER_ATTRIBUTE(target(isa))
#else
#define BOTAN_FUNC_ISA(isa)
#endif

/*
 * Define BOTAN_FUNC_ISA_INLINE
 *
 * TODO(Botan4) Remove this
 */
#define BOTAN_FUNC_ISA_INLINE(isa) BOTAN_FUNC_ISA(isa) BOTAN_FORCE_INLINE

/*
 * Define BOTAN_EARLY_INIT
 */
#if BOTAN_COMPILER_HAS_ATTRIBUTE(init_priority)
#define BOTAN_EARLY_INIT(prio) BOTAN_COMPILER_ATTRIBUTE(init_priority(prio))
#else
#define BOTAN_EARLY_INIT(prio) /**/
#endif

/*
 * Define BOTAN_FORCE_INLINE
 */
#if !defined(BOTAN_FORCE_INLINE)

#if BOTAN_COMPILER_HAS_ATTRIBUTE(always_inline)
#define BOTAN_FORCE_INLINE inline BOTAN_COMPILER_ATTRIBUTE(always_inline)

#elif defined(_MSC_VER)
#define BOTAN_FORCE_INLINE __forceinline

#else
#define BOTAN_FORCE_INLINE inline
#endif

#endif

namespace Botan {

/**
 * This class represents general abstract filter objects.
 */
class BOTAN_PUBLIC_API(2, 0) Filter {
   public:
    /**
     * @return descriptive name for this filter
     */
    virtual std::string name() const = 0;

    /**
     * Write a portion of a message to this filter.
     * @param input the input as a byte array
     * @param length the length of the byte array input
     */
    virtual void write(const uint8_t input[], size_t length) = 0;

    /**
     * Start a new message. Must be closed by end_msg() before another
     * message can be started.
     */
    virtual void start_msg() { /* default empty */ }

    /**
     * Notify that the current message is finished; flush buffers and
     * do end-of-message processing (if any).
     */
    virtual void end_msg() { /* default empty */ }

    /**
     * Check whether this filter is an attachable filter.
     * @return true if this filter is attachable, false otherwise
     */
    virtual bool attachable() { return true; }

    virtual ~Filter() = default;

    Filter(const Filter&) = delete;
    Filter& operator=(const Filter&) = delete;

   protected:
    /**
     * @param in some input for the filter
     * @param length the length of in
     */
    virtual void send(const uint8_t in[], size_t length);

    /**
     * @param in some input for the filter
     */
    void send(uint8_t in) { send(&in, 1); }

    /**
     * @param in some input for the filter
     */
    void send(std::span<const uint8_t> in) { send(in.data(), in.size()); }

    /**
     * @param in some input for the filter
     * @param length the number of bytes of in to send
     *
     * This previously took a std::vector, for which the length field (allowing
     * using just a prefix of the vector) somewhat made sense. It makes less
     * sense now that we are using a span here; you can just use `first` to get
     * a prefix.
     */
    void send(std::span<const uint8_t> in, size_t length);

    Filter();

   private:
    /**
     * Start a new message in *this and all following filters. Only for
     * internal use, not intended for use in client applications.
     */
    void new_msg();

    /**
     * End a new message in *this and all following filters. Only for
     * internal use, not intended for use in client applications.
     */
    void finish_msg();

    friend class Pipe;
    friend class Fanout_Filter;
    friend class Threaded_Fork;

    size_t total_ports() const;

    size_t current_port() const { return m_port_num; }

    /**
     * Set the active port
     * @param new_port the new value
     */
    void set_port(size_t new_port);

    size_t owns() const { return m_filter_owns; }

    /**
     * Attach another filter to this one
     * @param f filter to attach
     */
    void attach(Filter* f);

    /**
     * @param filters the filters to set
     * @param count number of items in filters
     */
    void set_next(Filter* filters[], size_t count);
    Filter* get_next() const;

    secure_vector<uint8_t> m_write_queue;
    std::vector<Filter*> m_next;    // not owned
    size_t m_port_num, m_filter_owns;

    // true if filter belongs to a pipe --> prohibit filter sharing!
    bool m_owned;
};

/**
 * This is the abstract Fanout_Filter base class.
 **/
class BOTAN_PUBLIC_API(2, 0) Fanout_Filter : public Filter {
   protected:
    /**
     * Increment the number of filters past us that we own
     */
    void incr_owns() { ++m_filter_owns; }

    void set_port(size_t n) { Filter::set_port(n); }

    void set_next(Filter* f[], size_t n) { Filter::set_next(f, n); }

    void attach(Filter* f) { Filter::attach(f); }
};

/**
 * The type of checking to be performed by decoders:
 * NONE - no checks, IGNORE_WS - perform checks, but ignore
 * whitespaces, FULL_CHECK - perform checks, also complain
 * about white spaces.
 */
enum Decoder_Checking { NONE, IGNORE_WS, FULL_CHECK };

}    // namespace Botan

namespace Botan {

/**
 * This class represents abstract data sink objects.
 */
class BOTAN_PUBLIC_API(2, 0) DataSink : public Filter {
   public:
    bool attachable() override { return false; }

    DataSink() = default;
    ~DataSink() override = default;

    DataSink& operator=(const DataSink&) = delete;
    DataSink(const DataSink&) = delete;
};

/**
 * This class represents a data sink which writes its output to a stream.
 */
class BOTAN_PUBLIC_API(2, 0) DataSink_Stream final : public DataSink {
   public:
    /**
     * Construct a DataSink_Stream from a stream.
     * @param stream the stream to write to
     * @param name identifier
     */
    DataSink_Stream(std::ostream& stream, std::string_view name = "<std::ostream>");

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)

    /**
     * Construct a DataSink_Stream from a filesystem path name.
     * @param pathname the name of the file to open a stream to
     * @param use_binary indicates whether to treat the file
     * as a binary file or not
     */
    DataSink_Stream(std::string_view pathname, bool use_binary = false);
#endif

    std::string name() const override { return m_identifier; }

    void write(const uint8_t[], size_t) override;

    void end_msg() override;

    ~DataSink_Stream() override;

   private:
    const std::string m_identifier;

    // May be null, if m_sink was an external reference
    std::unique_ptr<std::ostream> m_sink_memory;
    std::ostream& m_sink;
};

}    // namespace Botan

namespace Botan {

/**
 * This class represents an abstract data source object.
 */
class BOTAN_PUBLIC_API(2, 0) DataSource {
   public:
    /**
     * Read from the source. Moves the internal offset so that every
     * call to read will return a new portion of the source.
     *
     * @param out the byte array to write the result to
     * @param length the length of the byte array out
     * @return length in bytes that was actually read and put
     * into out
     */
    [[nodiscard]] virtual size_t read(uint8_t out[], size_t length) = 0;

    virtual bool check_available(size_t n) = 0;

    /**
     * Read from the source but do not modify the internal
     * offset. Consecutive calls to peek() will return portions of
     * the source starting at the same position.
     *
     * @param out the byte array to write the output to
     * @param length the length of the byte array out
     * @param peek_offset the offset into the stream to read at
     * @return length in bytes that was actually read and put
     * into out
     */
    [[nodiscard]] virtual size_t peek(uint8_t out[], size_t length, size_t peek_offset) const = 0;

    /**
     * Test whether the source still has data that can be read.
     * @return true if there is no more data to read, false otherwise
     */
    virtual bool end_of_data() const = 0;

    /**
     * return the id of this data source
     * @return std::string representing the id of this data source
     */
    virtual std::string id() const { return ""; }

    /**
     * Read one byte.
     * @param out the byte to read to
     * @return length in bytes that was actually read and put
     * into out
     */
    size_t read_byte(uint8_t& out);

    /**
     * Peek at one byte.
     * @param out an output byte
     * @return length in bytes that was actually read and put
     * into out
     */
    size_t peek_byte(uint8_t& out) const;

    /**
     * Discard the next N bytes of the data
     * @param N the number of bytes to discard
     * @return number of bytes actually discarded
     */
    size_t discard_next(size_t N);

    /**
     * @return number of bytes read so far.
     */
    virtual size_t get_bytes_read() const = 0;

    DataSource() = default;
    virtual ~DataSource() = default;
    DataSource& operator=(const DataSource&) = delete;
    DataSource(const DataSource&) = delete;
};

/**
 * This class represents a Memory-Based DataSource
 */
class BOTAN_PUBLIC_API(2, 0) DataSource_Memory final : public DataSource {
   public:
    size_t read(uint8_t[], size_t) override;
    size_t peek(uint8_t[], size_t, size_t) const override;
    bool check_available(size_t n) override;
    bool end_of_data() const override;

    /**
     * Construct a memory source that reads from a string
     * @param in the string to read from
     */
    explicit DataSource_Memory(std::string_view in);

    /**
     * Construct a memory source that reads from a byte array
     * @param in the byte array to read from
     * @param length the length of the byte array
     */
    DataSource_Memory(const uint8_t in[], size_t length) : m_source(in, in + length), m_offset(0) {}

    /**
     * Construct a memory source that reads from a secure_vector
     * @param in the MemoryRegion to read from
     */
    explicit DataSource_Memory(secure_vector<uint8_t> in) : m_source(std::move(in)), m_offset(0) {}

    /**
     * Construct a memory source that reads from an arbitrary byte buffer
     * @param in the MemoryRegion to read from
     */
    explicit DataSource_Memory(std::span<const uint8_t> in)
        : m_source(in.begin(), in.end()), m_offset(0) {}

    /**
     * Construct a memory source that reads from a std::vector
     * @param in the MemoryRegion to read from
     */
    explicit DataSource_Memory(const std::vector<uint8_t>& in)
        : m_source(in.begin(), in.end()), m_offset(0) {}

    size_t get_bytes_read() const override { return m_offset; }

   private:
    secure_vector<uint8_t> m_source;
    size_t m_offset;
};

/**
 * This class represents a Stream-Based DataSource.
 */
class BOTAN_PUBLIC_API(2, 0) DataSource_Stream final : public DataSource {
   public:
    size_t read(uint8_t[], size_t) override;
    size_t peek(uint8_t[], size_t, size_t) const override;
    bool check_available(size_t n) override;
    bool end_of_data() const override;
    std::string id() const override;

    DataSource_Stream(std::istream&, std::string_view id = "<std::istream>");

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
    /**
     * Construct a Stream-Based DataSource from filesystem path
     * @param filename the path to the file
     * @param use_binary whether to treat the file as binary or not
     */
    DataSource_Stream(std::string_view filename, bool use_binary = false);
#endif

    DataSource_Stream(const DataSource_Stream&) = delete;

    DataSource_Stream& operator=(const DataSource_Stream&) = delete;

    ~DataSource_Stream() override;

    size_t get_bytes_read() const override { return m_total_read; }

   private:
    const std::string m_identifier;

    std::unique_ptr<std::istream> m_source_memory;
    std::istream& m_source;
    size_t m_total_read;
};

}    // namespace Botan

namespace Botan {

class BOTAN_PUBLIC_API(2, 0) SQL_Database {
   public:
    class BOTAN_PUBLIC_API(2, 0) SQL_DB_Error final : public Exception {
       public:
        explicit SQL_DB_Error(std::string_view what) : Exception("SQL database", what), m_rc(0) {}

        SQL_DB_Error(std::string_view what, int rc) : Exception("SQL database", what), m_rc(rc) {}

        ErrorType error_type() const noexcept override { return ErrorType::DatabaseError; }

        int error_code() const noexcept override { return m_rc; }

       private:
        int m_rc;
    };

    class BOTAN_PUBLIC_API(2, 0) Statement {
       public:
        /* Bind statement parameters */
        virtual void bind(int column, std::string_view str) = 0;

        virtual void bind(int column, size_t i) = 0;

        virtual void bind(int column, std::chrono::system_clock::time_point time) = 0;

        virtual void bind(int column, const std::vector<uint8_t>& blob) = 0;

        virtual void bind(int column, const uint8_t* data, size_t len) = 0;

        /* Get output */
        virtual std::pair<const uint8_t*, size_t> get_blob(int column) = 0;

        virtual std::string get_str(int column) = 0;

        virtual size_t get_size_t(int column) = 0;

        /* Run to completion */
        virtual size_t spin() = 0;

        /* Maybe update */
        virtual bool step() = 0;

        virtual ~Statement() = default;
    };

    /*
     * Create a new statement for execution.
     * Use ?1, ?2, ?3, etc for parameters to set later with bind
     */
    virtual std::shared_ptr<Statement> new_statement(std::string_view base_sql) const = 0;

    virtual size_t row_count(std::string_view table_name) = 0;

    virtual void create_table(std::string_view table_schema) = 0;

    virtual size_t rows_changed_by_last_statement() = 0;

    virtual size_t exec(std::string_view sql) { return new_statement(sql)->spin(); }

    virtual bool is_threadsafe() const { return false; }

    virtual ~SQL_Database() = default;
};

}    // namespace Botan

namespace Botan {

class Filter;
class Output_Buffers;

/**
 * This class represents pipe objects.
 * A set of filters can be placed into a pipe, and information flows
 * through the pipe until it reaches the end, where the output is
 * collected for retrieval.  If you're familiar with the Unix shell
 * environment, this design will sound quite familiar.
 */
class BOTAN_PUBLIC_API(2, 0) Pipe final : public DataSource {
   public:
    /**
     * An opaque type that identifies a message in this Pipe
     */
    typedef size_t message_id;

    /**
     * Exception if you use an invalid message as an argument to
     * read, remaining, etc
     */
    class BOTAN_PUBLIC_API(2, 0) Invalid_Message_Number final : public Invalid_Argument {
       public:
        /**
         * @param where the error occurred
         * @param msg the invalid message id that was used
         */
        Invalid_Message_Number(std::string_view where, message_id msg);
    };

    /**
     * A meta-id for whatever the last message is
     */
    static const message_id LAST_MESSAGE;

    /**
     * A meta-id for the default message (set with set_default_msg)
     */
    static const message_id DEFAULT_MESSAGE;

    /**
     * Write input to the pipe, i.e. to its first filter.
     * @param in the byte array to write
     * @param length the length of the byte array in
     */
    void write(const uint8_t in[], size_t length);

    /**
     * Write input to the pipe, i.e. to its first filter.
     * @param in the secure_vector containing the data to write
     */
    void write(const secure_vector<uint8_t>& in) { write(in.data(), in.size()); }

    /**
     * Write input to the pipe, i.e. to its first filter.
     * @param in the std::vector containing the data to write
     */
    void write(const std::vector<uint8_t>& in) { write(in.data(), in.size()); }

    /**
     * Write input to the pipe, i.e. to its first filter.
     * @param in the string containing the data to write
     */
    void write(std::string_view in);

    /**
     * Write input to the pipe, i.e. to its first filter.
     * @param in the DataSource to read the data from
     */
    void write(DataSource& in);

    /**
     * Write input to the pipe, i.e. to its first filter.
     * @param in a single byte to be written
     */
    void write(uint8_t in);

    /**
     * Perform start_msg(), write() and end_msg() sequentially.
     * @param in the byte array containing the data to write
     * @param length the length of the byte array to write
     */
    void process_msg(const uint8_t in[], size_t length);

    /**
     * Perform start_msg(), write() and end_msg() sequentially.
     * @param in the secure_vector containing the data to write
     */
    void process_msg(const secure_vector<uint8_t>& in);

    /**
     * Perform start_msg(), write() and end_msg() sequentially.
     * @param in the secure_vector containing the data to write
     */
    void process_msg(const std::vector<uint8_t>& in);

    /**
     * Perform start_msg(), write() and end_msg() sequentially.
     * @param in the string containing the data to write
     */
    void process_msg(std::string_view in);

    /**
     * Perform start_msg(), write() and end_msg() sequentially.
     * @param in the DataSource providing the data to write
     */
    void process_msg(DataSource& in);

    /**
     * Find out how many bytes are ready to read.
     * @param msg the number identifying the message
     * for which the information is desired
     * @return number of bytes that can still be read
     */
    [[nodiscard]] size_t remaining(message_id msg = DEFAULT_MESSAGE) const;

    /**
     * Read the default message from the pipe. Moves the internal
     * offset so that every call to read will return a new portion of
     * the message.
     *
     * @param output the byte array to write the read bytes to
     * @param length the length of the byte array output
     * @return number of bytes actually read into output
     */
    [[nodiscard]] size_t read(uint8_t output[], size_t length) override;

    /**
     * Read a specified message from the pipe. Moves the internal
     * offset so that every call to read will return a new portion of
     * the message.
     * @param output the byte array to write the read bytes to
     * @param length the length of the byte array output
     * @param msg the number identifying the message to read from
     * @return number of bytes actually read into output
     */
    [[nodiscard]] size_t read(uint8_t output[], size_t length, message_id msg);

    /**
     * Read a single byte from the pipe. Moves the internal offset so
     * that every call to read will return a new portion of the
     * message.
     *
     * @param output the byte to write the result to
     * @param msg the message to read from
     * @return number of bytes actually read into output
     */
    [[nodiscard]] size_t read(uint8_t& output, message_id msg = DEFAULT_MESSAGE);

    /**
     * Read the full contents of the pipe.
     * @param msg the number identifying the message to read from
     * @return secure_vector holding the contents of the pipe
     */
    [[nodiscard]] secure_vector<uint8_t> read_all(message_id msg = DEFAULT_MESSAGE);

    /**
     * Read the full contents of the pipe.
     * @param msg the number identifying the message to read from
     * @return string holding the contents of the pipe
     */
    [[nodiscard]] std::string read_all_as_string(message_id msg = DEFAULT_MESSAGE);

    /**
     * Read from the default message but do not modify the internal
     * offset. Consecutive calls to peek() will return portions of
     * the message starting at the same position.
     * @param output the byte array to write the peeked message part to
     * @param length the length of the byte array output
     * @param offset the offset from the current position in message
     * @return number of bytes actually peeked and written into output
     */
    [[nodiscard]] size_t peek(uint8_t output[], size_t length, size_t offset) const override;

    /** Read from the specified message but do not modify the
     * internal offset. Consecutive calls to peek() will return
     * portions of the message starting at the same position.
     * @param output the byte array to write the peeked message part to
     * @param length the length of the byte array output
     * @param offset the offset from the current position in message
     * @param msg the number identifying the message to peek from
     * @return number of bytes actually peeked and written into output
     */
    [[nodiscard]] size_t peek(uint8_t output[], size_t length, size_t offset, message_id msg) const;

    /** Read a single byte from the specified message but do not
     * modify the internal offset. Consecutive calls to peek() will
     * return portions of the message starting at the same position.
     * @param output the byte to write the peeked message byte to
     * @param offset the offset from the current position in message
     * @param msg the number identifying the message to peek from
     * @return number of bytes actually peeked and written into output
     */
    [[nodiscard]] size_t peek(uint8_t& output, size_t offset,
                              message_id msg = DEFAULT_MESSAGE) const;

    /**
     * @return the number of bytes read from the default message.
     */
    size_t get_bytes_read() const override;

    /**
     * @return the number of bytes read from the specified message.
     */
    size_t get_bytes_read(message_id msg) const;

    bool check_available(size_t n) override;
    bool check_available_msg(size_t n, message_id msg) const;

    /**
     * @return currently set default message
     */
    size_t default_msg() const { return m_default_read; }

    /**
     * Set the default message
     * @param msg the number identifying the message which is going to
     * be the new default message
     */
    void set_default_msg(message_id msg);

    /**
     * Get the number of messages the are in this pipe.
     * @return number of messages the are in this pipe
     */
    message_id message_count() const;

    /**
     * Test whether this pipe has any data that can be read from.
     * @return true if there is more data to read, false otherwise
     */
    bool end_of_data() const override;

    /**
     * Start a new message in the pipe. A potential other message in this pipe
     * must be closed with end_msg() before this function may be called.
     */
    void start_msg();

    /**
     * End the current message.
     */
    void end_msg();

    /**
     * Insert a new filter at the front of the pipe
     * Deprecated because runtime modification of Pipes is deprecated.
     * You can instead use prepend_filter which only works before the first
     * message is processed.
     * @param filt the new filter to insert
     */
    BOTAN_DEPRECATED("Runtime modification of Pipe deprecated") void prepend(Filter* filt);

    /**
     * Insert a new filter at the back of the pipe
     * Deprecated because runtime modification of Pipes is deprecated.
     * You can instead use append_filter which only works before the first
     * message is processed.
     * @param filt the new filter to insert
     */
    BOTAN_DEPRECATED("Runtime modification of Pipe deprecated") void append(Filter* filt);

    /**
     * Remove the first filter at the front of the pipe.
     */
    BOTAN_DEPRECATED("Runtime modification of Pipe deprecated") void pop();

    /**
     * Reset this pipe to an empty pipe.
     */
    BOTAN_DEPRECATED("Runtime modification of Pipe deprecated") void reset();

    /**
     * Append a new filter onto the filter sequence. This may only be
     * called immediately after initial construction, before _any_
     * calls to start_msg have been made.
     *
     * This function (unlike append) is not deprecated, as it allows
     * only modification of the pipe at initialization (before use)
     * rather than after messages have been processed.
     */
    void append_filter(Filter* filt);

    /**
     * Prepend a new filter onto the filter sequence. This may only be
     * called immediately after initial construction, before _any_
     * calls to start_msg have been made.
     *
     * This function (unlike prepend) is not deprecated, as it allows
     * only modification of the pipe at initialization (before use)
     * rather than after messages have been processed.
     */
    void prepend_filter(Filter* filt);

    /**
     * Construct a Pipe of up to four filters. The filters are set up
     * in the same order as the arguments.
     */
    Pipe(Filter* = nullptr, Filter* = nullptr, Filter* = nullptr, Filter* = nullptr);

    /**
     * Construct a Pipe from a list of filters
     * @param filters the set of filters to use
     */
    explicit Pipe(std::initializer_list<Filter*> filters);

    Pipe(const Pipe&) = delete;
    Pipe& operator=(const Pipe&) = delete;

    ~Pipe() override;

   private:
    void destruct(Filter*);
    void do_append(Filter* filt);
    void do_prepend(Filter* filt);
    void find_endpoints(Filter*);
    void clear_endpoints(Filter*);

    message_id get_message_no(std::string_view, message_id) const;

    Filter* m_pipe;
    std::unique_ptr<Output_Buffers> m_outputs;
    message_id m_default_read;
    bool m_inside_msg;
};

/**
 * Stream output operator; dumps the results from pipe's default
 * message to the output stream.
 * @param out an output stream
 * @param pipe the pipe
 */
BOTAN_PUBLIC_API(2, 0) std::ostream& operator<<(std::ostream& out, Pipe& pipe);

/**
 * Stream input operator; dumps the remaining bytes of input
 * to the (assumed open) pipe message.
 * @param in the input stream
 * @param pipe the pipe
 */
BOTAN_PUBLIC_API(2, 0) std::istream& operator>>(std::istream& in, Pipe& pipe);

}    // namespace Botan

#if defined(BOTAN_HAS_PIPE_UNIXFD_IO)
#endif

namespace Botan {

class RandomNumberGenerator;

/**
 * Octet String
 */
class BOTAN_PUBLIC_API(2, 0) OctetString final {
   public:
    /**
     * @return size of this octet string in bytes
     */
    size_t length() const { return m_data.size(); }

    size_t size() const { return m_data.size(); }

    bool empty() const { return m_data.empty(); }

    /**
     * @return this object as a secure_vector<uint8_t>
     */
    secure_vector<uint8_t> bits_of() const { return m_data; }

    /**
     * @return start of this string
     */
    const uint8_t* begin() const { return m_data.data(); }

    /**
     * @return end of this string
     */
    const uint8_t* end() const { return begin() + m_data.size(); }

    /**
     * @return this encoded as hex
     */
    std::string to_string() const;

    /**
     * XOR the contents of another octet string into this one
     * @param other octet string
     * @return reference to this
     */
    OctetString& operator^=(const OctetString& other);

    /**
     * Force to have odd parity
     *
     * Deprecated. There is no reason to use this outside of interacting with
     * some very old or weird system which requires DES and also which do not
     * automatically ignore the parity bits.
     */
    BOTAN_DEPRECATED("Why would you need to do this") void set_odd_parity();

    /**
     * Create a new OctetString
     * @param str is a hex encoded string
     */
    explicit OctetString(std::string_view str = "");

    /**
     * Create a new random OctetString
     * @param rng is a random number generator
     * @param len is the desired length in bytes
     */
    OctetString(RandomNumberGenerator& rng, size_t len);

    /**
     * Create a new OctetString
     * @param in is an array
     * @param len is the length of in in bytes
     */
    OctetString(const uint8_t in[], size_t len);

    /**
     * Create a new OctetString
     * @param in a bytestring
     */
    explicit OctetString(std::span<const uint8_t> in) : m_data(in.begin(), in.end()) {}

    /**
     * Create a new OctetString
     * @param in a bytestring
     */
    explicit OctetString(secure_vector<uint8_t> in) : m_data(std::move(in)) {}

   private:
    secure_vector<uint8_t> m_data;
};

/**
 * Compare two strings
 * @param x an octet string
 * @param y an octet string
 * @return if x is equal to y
 */
BOTAN_PUBLIC_API(2, 0) bool operator==(const OctetString& x, const OctetString& y);

/**
 * Compare two strings
 * @param x an octet string
 * @param y an octet string
 * @return if x is not equal to y
 */
BOTAN_PUBLIC_API(2, 0) bool operator!=(const OctetString& x, const OctetString& y);

/**
 * Concatenate two strings
 * @param x an octet string
 * @param y an octet string
 * @return x concatenated with y
 */
BOTAN_PUBLIC_API(2, 0) OctetString operator+(const OctetString& x, const OctetString& y);

/**
 * XOR two strings
 * @param x an octet string
 * @param y an octet string
 * @return x XORed with y
 */
BOTAN_PUBLIC_API(2, 0) OctetString operator^(const OctetString& x, const OctetString& y);

/**
 * Alternate name for octet string showing intent to use as a key
 */
using SymmetricKey = OctetString;

/**
 * Alternate name for octet string showing intent to use as an IV
 */
using InitializationVector = OctetString;

}    // namespace Botan

#if defined(BOTAN_TARGET_OS_HAS_THREADS)
#include <thread>
#endif

#if defined(BOTAN_HAS_STREAM_CIPHER)
#endif

#if defined(BOTAN_HAS_HASH)

namespace Botan {

/**
 * This class represents hash function (message digest) objects
 */
class BOTAN_PUBLIC_API(2, 0) HashFunction : public Buffered_Computation {
   public:
    /**
     * Create an instance based on a name, or return null if the
     * algo/provider combination cannot be found. If provider is
     * empty then best available is chosen.
     */
    static std::unique_ptr<HashFunction> create(std::string_view algo_spec,
                                                std::string_view provider = "");

    /**
     * Create an instance based on a name
     * If provider is empty then best available is chosen.
     * @param algo_spec algorithm name
     * @param provider provider implementation to use
     * Throws Lookup_Error if not found.
     */
    static std::unique_ptr<HashFunction> create_or_throw(std::string_view algo_spec,
                                                         std::string_view provider = "");

    /**
     * @return list of available providers for this algorithm, empty if not available
     * @param algo_spec algorithm name
     */
    static std::vector<std::string> providers(std::string_view algo_spec);

    /**
     * @return provider information about this implementation. Default is "base",
     * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
     */
    virtual std::string provider() const { return "base"; }

    ~HashFunction() override = default;

    /**
     * Reset the state.
     */
    virtual void clear() = 0;

    /**
     * @return the hash function name
     */
    virtual std::string name() const = 0;

    /**
     * @return hash block size as defined for this algorithm
     */
    virtual size_t hash_block_size() const { return 0; }

    /**
     * Return a new hash object with the same state as *this. This
     * allows computing the hash of several messages with a common
     * prefix more efficiently than would otherwise be possible.
     *
     * This function should be called `clone` but that was already
     * used for the case of returning an uninitialized object.
     * @return new hash object
     */
    virtual std::unique_ptr<HashFunction> copy_state() const = 0;

    /**
     * @return new object representing the same algorithm as *this
     */
    virtual std::unique_ptr<HashFunction> new_object() const = 0;

    /**
     * @return new object representing the same algorithm as *this
     */
    HashFunction* clone() const { return this->new_object().release(); }
};

}    // namespace Botan
#endif

#if defined(BOTAN_HAS_MAC)

namespace Botan {

/**
 * This class represents Message Authentication Code (MAC) objects.
 */
class BOTAN_PUBLIC_API(2, 0) MessageAuthenticationCode : public Buffered_Computation,
                                                         public SymmetricAlgorithm {
   public:
    /**
     * Create an instance based on a name
     * If provider is empty then best available is chosen.
     * @param algo_spec algorithm name
     * @param provider provider implementation to use
     * @return a null pointer if the algo/provider combination cannot be found
     */
    static std::unique_ptr<MessageAuthenticationCode> create(std::string_view algo_spec,
                                                             std::string_view provider = "");

    /*
     * Create an instance based on a name
     * If provider is empty then best available is chosen.
     * @param algo_spec algorithm name
     * @param provider provider implementation to use
     * Throws a Lookup_Error if algo/provider combination cannot be found
     */
    static std::unique_ptr<MessageAuthenticationCode> create_or_throw(
        std::string_view algo_spec, std::string_view provider = "");

    /**
     * @return list of available providers for this algorithm, empty if not available
     */
    static std::vector<std::string> providers(std::string_view algo_spec);

    ~MessageAuthenticationCode() override = default;

    /**
     * Prepare for processing a message under the specified nonce
     *
     * Most MACs neither require nor support a nonce; for these algorithms
     * calling start() is optional and calling it with anything other than
     * an empty string is an error. One MAC which *requires* a per-message
     * nonce be specified is GMAC.
     *
     * Default implementation simply rejects all non-empty nonces
     * since most hash/MAC algorithms do not support randomization
     *
     * @param nonce the message nonce bytes
     */
    void start(std::span<const uint8_t> nonce) { start_msg(nonce); }

    /**
     * Begin processing a message.
     * @param nonce the per message nonce
     * @param nonce_len length of nonce
     */
    void start(const uint8_t nonce[], size_t nonce_len) { start_msg({nonce, nonce_len}); }

    /**
     * Begin processing a message.
     */
    void start() { return start_msg({}); }

    /**
     * Verify a MAC.
     * @param in the MAC to verify as a byte array
     * @param length the length of param in
     * @return true if the MAC is valid, false otherwise
     */
    bool verify_mac(const uint8_t in[], size_t length) { return verify_mac_result({in, length}); }

    /**
     * Verify a MAC.
     * @param in the MAC to verify as a byte array
     * @return true if the MAC is valid, false otherwise
     */
    bool verify_mac(std::span<const uint8_t> in) { return verify_mac_result(in); }

    /**
     * @return new object representing the same algorithm as *this
     */
    virtual std::unique_ptr<MessageAuthenticationCode> new_object() const = 0;

    /**
     * Get a new object representing the same algorithm as *this
     */
    MessageAuthenticationCode* clone() const { return this->new_object().release(); }

    /**
     * @return provider information about this implementation. Default is "base",
     * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
     */
    virtual std::string provider() const { return "base"; }

    /**
     * @return if a fresh key must be set for each message that is processed.
     *
     * This is required for certain polynomial-based MACs which are insecure
     * if a key is ever reused for two different messages.
     */
    virtual bool fresh_key_required_per_message() const { return false; }

   protected:
    /**
     * Prepare for processing a message under the specified nonce
     *
     * If the MAC does not support nonces, it should not override the default
     * implementation.
     */
    virtual void start_msg(std::span<const uint8_t> nonce);

    /**
     * Verify the MACs final result
     */
    virtual bool verify_mac_result(std::span<const uint8_t> in);
};

typedef MessageAuthenticationCode MAC;

}    // namespace Botan
#endif

namespace Botan {

/**
 * Filter mixin that breaks input into blocks, useful for
 * cipher modes
 */
class BOTAN_PUBLIC_API(2, 0) Buffered_Filter {
   public:
    /**
     * Write bytes into the buffered filter, which will them emit them
     * in calls to buffered_block in the subclass
     * @param in the input bytes
     * @param length of in in bytes
     */
    void write(const uint8_t in[], size_t length);

    template <typename Alloc>
    void write(const std::vector<uint8_t, Alloc>& in, size_t length) {
        write(in.data(), length);
    }

    /**
     * Finish a message, emitting to buffered_block and buffered_final
     * Will throw an exception if less than final_minimum bytes were
     * written into the filter.
     */
    void end_msg();

    /**
     * Initialize a Buffered_Filter
     * @param block_size the function buffered_block will be called
     *        with inputs which are a multiple of this size
     * @param final_minimum the function buffered_final will be called
     *        with at least this many bytes.
     */
    Buffered_Filter(size_t block_size, size_t final_minimum);

    virtual ~Buffered_Filter() = default;

   protected:
    /**
     * The block processor, implemented by subclasses
     * @param input some input bytes
     * @param length the size of input, guaranteed to be a multiple
     *        of block_size
     */
    virtual void buffered_block(const uint8_t input[], size_t length) = 0;

    /**
     * The final block, implemented by subclasses
     * @param input some input bytes
     * @param length the size of input, guaranteed to be at least
     *        final_minimum bytes
     */
    virtual void buffered_final(const uint8_t input[], size_t length) = 0;

    /**
     * @return block size of inputs
     */
    size_t buffered_block_size() const { return m_main_block_mod; }

    /**
     * @return current position in the buffer
     */
    size_t current_position() const { return m_buffer_pos; }

    /**
     * Reset the buffer position
     */
    void buffer_reset() { m_buffer_pos = 0; }

   private:
    size_t m_main_block_mod, m_final_minimum;

    secure_vector<uint8_t> m_buffer;
    size_t m_buffer_pos;
};

/**
 * This class represents keyed filters, i.e. filters that have to be
 * fed with a key in order to function.
 */
class BOTAN_PUBLIC_API(2, 0) Keyed_Filter : public Filter {
   public:
    /**
     * Set the key of this filter
     * @param key the key to use
     */
    virtual void set_key(const SymmetricKey& key) = 0;

    /**
     * Set the initialization vector of this filter. Note: you should
     * call set_iv() only after you have called set_key()
     * @param iv the initialization vector to use
     */
    virtual void set_iv(const InitializationVector& iv) {
        if (iv.length() != 0) {
            throw Invalid_IV_Length(name(), iv.length());
        }
    }

    /**
     * Check whether a key length is valid for this filter
     * @param length the key length to be checked for validity
     * @return true if the key length is valid, false otherwise
     */
    bool valid_keylength(size_t length) const { return key_spec().valid_keylength(length); }

    /**
     * @return object describing limits on key size
     */
    virtual Key_Length_Specification key_spec() const = 0;

    /**
     * Check whether an IV length is valid for this filter
     * @param length the IV length to be checked for validity
     * @return true if the IV length is valid, false otherwise
     */
    virtual bool valid_iv_length(size_t length) const { return (length == 0); }
};

/**
 * Filter interface for cipher modes
 */
class BOTAN_PUBLIC_API(2, 0) Cipher_Mode_Filter final : public Keyed_Filter,
                                                        private Buffered_Filter {
   public:
    explicit Cipher_Mode_Filter(Cipher_Mode* t);

    explicit Cipher_Mode_Filter(std::unique_ptr<Cipher_Mode> t) : Cipher_Mode_Filter(t.release()) {}

    void set_iv(const InitializationVector& iv) override;

    void set_key(const SymmetricKey& key) override;

    Key_Length_Specification key_spec() const override;

    bool valid_iv_length(size_t length) const override;

    std::string name() const override;

   private:
    void write(const uint8_t input[], size_t input_length) override;
    void start_msg() override;
    void end_msg() override;

    void buffered_block(const uint8_t input[], size_t input_length) override;
    void buffered_final(const uint8_t input[], size_t input_length) override;

    std::unique_ptr<Cipher_Mode> m_mode;
    std::vector<uint8_t> m_nonce;
    secure_vector<uint8_t> m_buffer;
};

/*
 * Get a cipher object
 */

/**
 * Factory method for general symmetric cipher filters. No key will be
 * set in the filter.
 *
 * @param algo_spec the name of the desired cipher
 * @param direction determines whether the filter will be an encrypting or
 * decrypting filter
 * @return pointer to the encryption or decryption filter
 */
inline Keyed_Filter* get_cipher(std::string_view algo_spec, Cipher_Dir direction) {
    auto c = Cipher_Mode::create_or_throw(algo_spec, direction);
    return new Cipher_Mode_Filter(c.release());
}

/**
 * Factory method for general symmetric cipher filters.
 * @param algo_spec the name of the desired cipher
 * @param key the key to be used for encryption/decryption performed by
 * the filter
 * @param direction determines whether the filter will be an encrypting
 * or decrypting filter
 * @return pointer to the encryption or decryption filter
 */
inline Keyed_Filter* get_cipher(std::string_view algo_spec, const SymmetricKey& key,
                                Cipher_Dir direction) {
    Keyed_Filter* cipher = get_cipher(algo_spec, direction);
    cipher->set_key(key);
    return cipher;
}

/**
 * Factory method for general symmetric cipher filters.
 * @param algo_spec the name of the desired cipher
 * @param key the key to be used for encryption/decryption performed by
 * the filter
 * @param iv the initialization vector to be used
 * @param direction determines whether the filter will be an encrypting
 * or decrypting filter
 * @return pointer to newly allocated encryption or decryption filter
 */
inline Keyed_Filter* get_cipher(std::string_view algo_spec, const SymmetricKey& key,
                                const InitializationVector& iv, Cipher_Dir direction) {
    Keyed_Filter* cipher = get_cipher(algo_spec, key, direction);
    if (iv.length()) {
        cipher->set_iv(iv);
    }
    return cipher;
}

#if defined(BOTAN_HAS_STREAM_CIPHER)

/**
 * Stream Cipher Filter
 */
class BOTAN_PUBLIC_API(2, 0) StreamCipher_Filter final : public Keyed_Filter {
   public:
    std::string name() const override { return m_cipher->name(); }

    /**
     * Write input data
     * @param input data
     * @param input_len length of input in bytes
     */
    void write(const uint8_t input[], size_t input_len) override;

    bool valid_iv_length(size_t iv_len) const override { return m_cipher->valid_iv_length(iv_len); }

    /**
     * Set the initialization vector for this filter.
     * @param iv the initialization vector to set
     */
    void set_iv(const InitializationVector& iv) override {
        m_cipher->set_iv(iv.begin(), iv.length());
    }

    /**
     * Set the key of this filter.
     * @param key the key to set
     */
    void set_key(const SymmetricKey& key) override { m_cipher->set_key(key); }

    Key_Length_Specification key_spec() const override { return m_cipher->key_spec(); }

    /**
     * Construct a stream cipher filter.
     * @param cipher a cipher object to use
     */
    explicit StreamCipher_Filter(StreamCipher* cipher);

    /**
     * Construct a stream cipher filter.
     * @param cipher a cipher object to use
     * @param key the key to use inside this filter
     */
    StreamCipher_Filter(StreamCipher* cipher, const SymmetricKey& key);

    /**
     * Construct a stream cipher filter.
     * @param cipher the name of the desired cipher
     */
    explicit StreamCipher_Filter(std::string_view cipher);

    /**
     * Construct a stream cipher filter.
     * @param cipher the name of the desired cipher
     * @param key the key to use inside this filter
     */
    StreamCipher_Filter(std::string_view cipher, const SymmetricKey& key);

   private:
    std::unique_ptr<StreamCipher> m_cipher;
    secure_vector<uint8_t> m_buffer;
};
#endif

#if defined(BOTAN_HAS_HASH)

/**
 * Hash Filter.
 */
class BOTAN_PUBLIC_API(2, 0) Hash_Filter final : public Filter {
   public:
    void write(const uint8_t input[], size_t len) override { m_hash->update(input, len); }

    void end_msg() override;

    std::string name() const override { return m_hash->name(); }

    /**
     * Construct a hash filter.
     * @param hash the hash function to use
     * @param len the output length of this filter. Leave the default
     * value 0 if you want to use the full output of the hashfunction
     * hash. Otherwise, specify a smaller value here so that the
     * output of the hash algorithm will be cut off.
     */
    Hash_Filter(HashFunction* hash, size_t len = 0) : m_hash(hash), m_out_len(len) {}

    /**
     * Construct a hash filter.
     * @param request the name of the hash algorithm to use
     * @param len the output length of this filter. Leave the default
     * value 0 if you want to use the full output of the hashfunction
     * hash. Otherwise, specify a smaller value here so that the
     * output of the hash algorithm will be cut off.
     */
    Hash_Filter(std::string_view request, size_t len = 0);

   private:
    std::unique_ptr<HashFunction> m_hash;
    const size_t m_out_len;
};
#endif

#if defined(BOTAN_HAS_MAC)

/**
 * MessageAuthenticationCode Filter.
 */
class BOTAN_PUBLIC_API(2, 0) MAC_Filter final : public Keyed_Filter {
   public:
    void write(const uint8_t input[], size_t len) override { m_mac->update(input, len); }

    void end_msg() override;

    std::string name() const override { return m_mac->name(); }

    /**
     * Set the key of this filter.
     * @param key the key to set
     */
    void set_key(const SymmetricKey& key) override { m_mac->set_key(key); }

    Key_Length_Specification key_spec() const override { return m_mac->key_spec(); }

    /**
     * Construct a MAC filter. The MAC key will be left empty.
     * @param mac the MAC to use
     * @param out_len the output length of this filter. Leave the default
     * value 0 if you want to use the full output of the
     * MAC. Otherwise, specify a smaller value here so that the
     * output of the MAC will be cut off.
     */
    MAC_Filter(MessageAuthenticationCode* mac, size_t out_len = 0)
        : m_mac(mac), m_out_len(out_len) {}

    /**
     * Construct a MAC filter.
     * @param mac the MAC to use
     * @param key the MAC key to use
     * @param out_len the output length of this filter. Leave the default
     * value 0 if you want to use the full output of the
     * MAC. Otherwise, specify a smaller value here so that the
     * output of the MAC will be cut off.
     */
    MAC_Filter(MessageAuthenticationCode* mac, const SymmetricKey& key, size_t out_len = 0)
        : m_mac(mac), m_out_len(out_len) {
        m_mac->set_key(key);
    }

    /**
     * Construct a MAC filter. The MAC key will be left empty.
     * @param mac the name of the MAC to use
     * @param len the output length of this filter. Leave the default
     * value 0 if you want to use the full output of the
     * MAC. Otherwise, specify a smaller value here so that the
     * output of the MAC will be cut off.
     */
    MAC_Filter(std::string_view mac, size_t len = 0);

    /**
     * Construct a MAC filter.
     * @param mac the name of the MAC to use
     * @param key the MAC key to use
     * @param len the output length of this filter. Leave the default
     * value 0 if you want to use the full output of the
     * MAC. Otherwise, specify a smaller value here so that the
     * output of the MAC will be cut off.
     */
    MAC_Filter(std::string_view mac, const SymmetricKey& key, size_t len = 0);

   private:
    std::unique_ptr<MessageAuthenticationCode> m_mac;
    const size_t m_out_len;
};
#endif

#if defined(BOTAN_HAS_COMPRESSION)

class Compression_Algorithm;
class Decompression_Algorithm;

/**
 * Filter interface for compression
 */
class BOTAN_PUBLIC_API(2, 0) Compression_Filter final : public Filter {
   public:
    void start_msg() override;
    void write(const uint8_t input[], size_t input_length) override;
    void end_msg() override;

    void flush();

    std::string name() const override;

    Compression_Filter(std::string_view type, size_t compression_level, size_t buffer_size = 4096);

    ~Compression_Filter() override;

   private:
    std::unique_ptr<Compression_Algorithm> m_comp;
    size_t m_buffersize, m_level;
    secure_vector<uint8_t> m_buffer;
};

/**
 * Filter interface for decompression
 */
class BOTAN_PUBLIC_API(2, 0) Decompression_Filter final : public Filter {
   public:
    void start_msg() override;
    void write(const uint8_t input[], size_t input_length) override;
    void end_msg() override;

    std::string name() const override;

    Decompression_Filter(std::string_view type, size_t buffer_size = 4096);

    ~Decompression_Filter() override;

   private:
    std::unique_ptr<Decompression_Algorithm> m_comp;
    std::size_t m_buffersize;
    secure_vector<uint8_t> m_buffer;
};

#endif

/**
 * This class represents a Base64 encoder.
 */
class BOTAN_PUBLIC_API(2, 0) Base64_Encoder final : public Filter {
   public:
    std::string name() const override { return "Base64_Encoder"; }

    /**
     * Input a part of a message to the encoder.
     * @param input the message to input as a byte array
     * @param length the length of the byte array input
     */
    void write(const uint8_t input[], size_t length) override;

    /**
     * Inform the Encoder that the current message shall be closed.
     */
    void end_msg() override;

    /**
     * Create a base64 encoder.
     * @param line_breaks whether to use line breaks in the output
     * @param line_length the length of the lines of the output
     * @param trailing_newline whether to use a trailing newline
     */
    Base64_Encoder(bool line_breaks = false, size_t line_length = 72,
                   bool trailing_newline = false);

   private:
    void encode_and_send(const uint8_t input[], size_t length, bool final_inputs = false);
    void do_output(const uint8_t output[], size_t length);

    const size_t m_line_length;
    const bool m_trailing_newline;
    std::vector<uint8_t> m_in, m_out;
    size_t m_position, m_out_position;
};

/**
 * This object represents a Base64 decoder.
 */
class BOTAN_PUBLIC_API(2, 0) Base64_Decoder final : public Filter {
   public:
    std::string name() const override { return "Base64_Decoder"; }

    /**
     * Input a part of a message to the decoder.
     * @param input the message to input as a byte array
     * @param length the length of the byte array input
     */
    void write(const uint8_t input[], size_t length) override;

    /**
     * Finish up the current message
     */
    void end_msg() override;

    /**
     * Create a base64 decoder.
     * @param checking the type of checking that shall be performed by
     * the decoder
     */
    explicit Base64_Decoder(Decoder_Checking checking = NONE);

   private:
    const Decoder_Checking m_checking;
    std::vector<uint8_t> m_in, m_out;
    size_t m_position;
};

/**
 * Converts arbitrary binary data to hex strings, optionally with
 * newlines inserted
 */
class BOTAN_PUBLIC_API(2, 0) Hex_Encoder final : public Filter {
   public:
    /**
     * Whether to use uppercase or lowercase letters for the encoded string.
     */
    enum Case { Uppercase, Lowercase };

    std::string name() const override { return "Hex_Encoder"; }

    void write(const uint8_t in[], size_t length) override;
    void end_msg() override;

    /**
     * Create a hex encoder.
     * @param the_case the case to use in the encoded strings.
     */
    explicit Hex_Encoder(Case the_case);

    /**
     * Create a hex encoder.
     * @param newlines should newlines be used
     * @param line_length if newlines are used, how long are lines
     * @param the_case the case to use in the encoded strings
     */
    Hex_Encoder(bool newlines = false, size_t line_length = 72, Case the_case = Uppercase);

   private:
    void encode_and_send(const uint8_t[], size_t);

    const Case m_casing;
    const size_t m_line_length;
    std::vector<uint8_t> m_in, m_out;
    size_t m_position, m_counter;
};

/**
 * Converts hex strings to bytes
 */
class BOTAN_PUBLIC_API(2, 0) Hex_Decoder final : public Filter {
   public:
    std::string name() const override { return "Hex_Decoder"; }

    void write(const uint8_t[], size_t) override;
    void end_msg() override;

    /**
     * Construct a Hex Decoder using the specified
     * character checking.
     * @param checking the checking to use during decoding.
     */
    explicit Hex_Decoder(Decoder_Checking checking = NONE);

   private:
    const Decoder_Checking m_checking;
    std::vector<uint8_t> m_in, m_out;
    size_t m_position;
};

/**
 * BitBucket is a filter which simply discards all inputs
 */
class BOTAN_PUBLIC_API(2, 0) BitBucket final : public Filter {
   public:
    void write(const uint8_t[], size_t) override { /* discard */ }

    std::string name() const override { return "BitBucket"; }
};

/**
 * This class represents Filter chains. A Filter chain is an ordered
 * concatenation of Filters, the input to a Chain sequentially passes
 * through all the Filters contained in the Chain.
 */

class BOTAN_PUBLIC_API(2, 0) Chain final : public Fanout_Filter {
   public:
    void write(const uint8_t input[], size_t length) override { send(input, length); }

    std::string name() const override { return "Chain"; }

    /**
     * Construct a chain of up to four filters. The filters are set
     * up in the same order as the arguments.
     */
    Chain(Filter* = nullptr, Filter* = nullptr, Filter* = nullptr, Filter* = nullptr);

    /**
     * Construct a chain from range of filters
     * @param filter_arr the list of filters
     * @param length how many filters
     */
    Chain(Filter* filter_arr[], size_t length);
};

/**
 * This class represents a fork filter, whose purpose is to fork the
 * flow of data. It causes an input message to result in n messages at
 * the end of the filter, where n is the number of forks.
 */
class BOTAN_PUBLIC_API(2, 0) Fork : public Fanout_Filter {
   public:
    void write(const uint8_t input[], size_t length) override { send(input, length); }

    void set_port(size_t n) { Fanout_Filter::set_port(n); }

    std::string name() const override { return "Fork"; }

    /**
     * Construct a Fork filter with up to four forks.
     */
    Fork(Filter*, Filter*, Filter* = nullptr, Filter* = nullptr);

    /**
     * Construct a Fork from range of filters
     * @param filter_arr the list of filters
     * @param length how many filters
     */
    Fork(Filter* filter_arr[], size_t length);
};

#if defined(BOTAN_HAS_THREAD_UTILS)

/**
 * This class is a threaded version of the Fork filter. While this uses
 * threads, the class itself is NOT thread-safe. This is meant as a drop-
 * in replacement for Fork where performance gains are possible.
 */
class BOTAN_PUBLIC_API(2, 0) Threaded_Fork final : public Fork {
   public:
    std::string name() const override;

    /**
     * Construct a Threaded_Fork filter with up to four forks.
     */
    Threaded_Fork(Filter*, Filter*, Filter* = nullptr, Filter* = nullptr);

    /**
     * Construct a Threaded_Fork from range of filters
     * @param filter_arr the list of filters
     * @param length how many filters
     */
    Threaded_Fork(Filter* filter_arr[], size_t length);

    ~Threaded_Fork() override;

   private:
    void set_next(Filter* f[], size_t n);
    void send(const uint8_t in[], size_t length) override;
    void thread_delegate_work(const uint8_t input[], size_t length);
    void thread_entry(Filter* filter);

    std::vector<std::shared_ptr<std::thread>> m_threads;
    std::unique_ptr<struct Threaded_Fork_Data> m_thread_data;
};
#endif

}    // namespace Botan

namespace Botan {

/**
 * Perform hex encoding
 * @param output an array of at least input_length*2 bytes
 * @param input is some binary data
 * @param input_length length of input in bytes
 * @param uppercase should output be upper or lower case?
 */
void BOTAN_PUBLIC_API(2, 0)
    hex_encode(char output[], const uint8_t input[], size_t input_length, bool uppercase = true);

/**
 * Perform hex encoding
 * @param input some input
 * @param input_length length of input in bytes
 * @param uppercase should output be upper or lower case?
 * @return hexadecimal representation of input
 */
std::string BOTAN_PUBLIC_API(2, 0)
    hex_encode(const uint8_t input[], size_t input_length, bool uppercase = true);

/**
 * Perform hex encoding
 * @param input some input
 * @param uppercase should output be upper or lower case?
 * @return hexadecimal representation of input
 */
inline std::string hex_encode(std::span<const uint8_t> input, bool uppercase = true) {
    return hex_encode(input.data(), input.size(), uppercase);
}

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0) hex_decode(uint8_t output[], const char input[], size_t input_length,
                                         size_t& input_consumed, bool ignore_ws = true);

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param input_length length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
    hex_decode(uint8_t output[], const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0)
    hex_decode(uint8_t output[], std::string_view input, bool ignore_ws = true);

/**
 * Perform hex decoding
 * @param output a contiguous byte buffer of at least input_length/2 bytes
 * @param input some hex input
 * @param ignore_ws ignore whitespace on input; if false, throw an
 *                  exception if whitespace is encountered
 * @return number of bytes written to output
 */
size_t BOTAN_PUBLIC_API(3, 0)
    hex_decode(std::span<uint8_t> output, std::string_view input, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
std::vector<uint8_t> BOTAN_PUBLIC_API(2, 0)
    hex_decode(const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
std::vector<uint8_t> BOTAN_PUBLIC_API(3, 0)
    hex_decode(std::string_view input, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(2, 0)
    hex_decode_locked(const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(3, 0)
    hex_decode_locked(std::string_view input, bool ignore_ws = true);

}    // namespace Botan

#if defined(BOTAN_TARGET_OS_HAS_THREADS)
#include <mutex>
#endif

namespace Botan {

#if defined(BOTAN_TARGET_OS_HAS_THREADS)

using mutex_type = std::mutex;
using recursive_mutex_type = std::recursive_mutex;

template <typename T>
using lock_guard_type = std::lock_guard<T>;

#else

// No threads

class noop_mutex final {
   public:
    void lock() {}

    void unlock() {}
};

using mutex_type = noop_mutex;
using recursive_mutex_type = noop_mutex;

template <typename Mutex>
class lock_guard final {
   public:
    explicit lock_guard(Mutex& m) : m_mutex(m) { m_mutex.lock(); }

    ~lock_guard() { m_mutex.unlock(); }

    lock_guard(const lock_guard& other) = delete;
    lock_guard& operator=(const lock_guard& other) = delete;

   private:
    Mutex& m_mutex;
};

template <typename T>
using lock_guard_type = lock_guard<T>;

#endif

}    // namespace Botan

namespace Botan {

class Entropy_Sources;

/**
 * An interface to a cryptographic random number generator
 */
class BOTAN_PUBLIC_API(2, 0) RandomNumberGenerator {
   public:
    /**
     * Userspace RNGs like HMAC_DRBG will reseed after a specified number
     * of outputs are generated. Set to zero to disable automatic reseeding.
     */
    static constexpr size_t DefaultReseedInterval = 1024;

    /**
     * Number of entropy bits polled for reseeding userspace RNGs like HMAC_DRBG
     */
    static constexpr size_t DefaultPollBits = 256;

    /**
     * Default poll timeout
     */
    static constexpr auto DefaultPollTimeout = std::chrono::milliseconds(50);

    virtual ~RandomNumberGenerator() = default;

    RandomNumberGenerator() = default;

    /*
     * Never copy a RNG, create a new one
     */
    RandomNumberGenerator(const RandomNumberGenerator& rng) = delete;
    RandomNumberGenerator& operator=(const RandomNumberGenerator& rng) = delete;

    /**
     * Randomize a byte array.
     *
     * May block shortly if e.g. the RNG is not yet initialized
     * or a retry because of insufficient entropy is needed.
     *
     * @param output the byte array to hold the random output.
     * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
     * @throws Exception if the RNG fails
     */
    void randomize(std::span<uint8_t> output) { this->fill_bytes_with_input(output, {}); }

    void randomize(uint8_t output[], size_t length) { this->randomize(std::span(output, length)); }

    /**
     * Returns false if it is known that this RNG object is not able to accept
     * externally provided inputs (via add_entropy, randomize_with_input, etc).
     * In this case, any such provided inputs are ignored.
     *
     * If this function returns true, then inputs may or may not be accepted.
     */
    virtual bool accepts_input() const = 0;

    /**
     * Incorporate some additional data into the RNG state. For
     * example adding nonces or timestamps from a peer's protocol
     * message can help hedge against VM state rollback attacks.
     * A few RNG types do not accept any externally provided input,
     * in which case this function is a no-op.
     *
     * @param input a byte array containing the entropy to be added
     * @throws Exception may throw if the RNG accepts input, but adding the entropy failed.
     */
    void add_entropy(std::span<const uint8_t> input) { this->fill_bytes_with_input({}, input); }

    void add_entropy(const uint8_t input[], size_t length) {
        this->add_entropy(std::span(input, length));
    }

    /**
     * Incorporate some additional data into the RNG state.
     */
    template <typename T>
        requires std::is_standard_layout<T>::value && std::is_trivial<T>::value
    void add_entropy_T(const T& t) {
        this->add_entropy(reinterpret_cast<const uint8_t*>(&t), sizeof(T));
    }

    /**
     * Incorporate entropy into the RNG state then produce output.
     * Some RNG types implement this using a single operation, default
     * calls add_entropy + randomize in sequence.
     *
     * Use this to further bind the outputs to your current
     * process/protocol state. For instance if generating a new key
     * for use in a session, include a session ID or other such
     * value. See NIST SP 800-90 A, B, C series for more ideas.
     *
     * @param output buffer to hold the random output
     * @param input entropy buffer to incorporate
     * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
     * @throws Exception if the RNG fails
     * @throws Exception may throw if the RNG accepts input, but adding the entropy failed.
     */
    void randomize_with_input(std::span<uint8_t> output, std::span<const uint8_t> input) {
        this->fill_bytes_with_input(output, input);
    }

    void randomize_with_input(uint8_t output[], size_t output_len, const uint8_t input[],
                              size_t input_len) {
        this->randomize_with_input(std::span(output, output_len), std::span(input, input_len));
    }

    /**
     * This calls `randomize_with_input` using some timestamps as extra input.
     *
     * For a stateful RNG using non-random but potentially unique data the
     * extra input can help protect against problems with fork, VM state
     * rollback, or other cases where somehow an RNG state is duplicated. If
     * both of the duplicated RNG states later incorporate a timestamp (and the
     * timestamps don't themselves repeat), their outputs will diverge.
     *
     * @param output buffer to hold the random output
     * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
     * @throws Exception if the RNG fails
     * @throws Exception may throw if the RNG accepts input, but adding the entropy failed.
     */
    void randomize_with_ts_input(std::span<uint8_t> output);

    void randomize_with_ts_input(uint8_t output[], size_t output_len) {
        this->randomize_with_ts_input(std::span(output, output_len));
    }

    /**
     * @return the name of this RNG type
     */
    virtual std::string name() const = 0;

    /**
     * Clear all internally held values of this RNG
     * @post is_seeded() == false if the RNG has an internal state that can be cleared.
     */
    virtual void clear() = 0;

    /**
     * Check whether this RNG is seeded.
     * @return true if this RNG was already seeded, false otherwise.
     */
    virtual bool is_seeded() const = 0;

    /**
     * Poll provided sources for up to poll_bits bits of entropy
     * or until the timeout expires. Returns estimate of the number
     * of bits collected.
     *
     * Sets the seeded state to true if enough entropy was added.
     */
    virtual size_t reseed(
        Entropy_Sources& srcs, size_t poll_bits = RandomNumberGenerator::DefaultPollBits,
        std::chrono::milliseconds poll_timeout = RandomNumberGenerator::DefaultPollTimeout);

    /**
     * Reseed by reading specified bits from the RNG
     *
     * Sets the seeded state to true if enough entropy was added.
     *
     * @throws Exception if RNG accepts input but reseeding failed.
     */
    virtual void reseed_from_rng(RandomNumberGenerator& rng,
                                 size_t poll_bits = RandomNumberGenerator::DefaultPollBits);

    // Some utility functions built on the interface above:

    /**
     * Fill a given byte container with @p bytes random bytes
     *
     * @todo deprecate this overload (in favor of randomize())
     *
     * @param  v     the container to be filled with @p bytes random bytes
     * @throws Exception if RNG fails
     */
    void random_vec(std::span<uint8_t> v) { this->randomize(v); }

    /**
     * Resize a given byte container to @p bytes and fill it with random bytes
     *
     * @tparam T     the desired byte container type (e.g std::vector<uint8_t>)
     * @param  v     the container to be filled with @p bytes random bytes
     * @param  bytes number of random bytes to initialize the container with
     * @throws Exception if RNG or memory allocation fails
     */
    template <concepts::resizable_byte_buffer T>
    void random_vec(T& v, size_t bytes) {
        v.resize(bytes);
        random_vec(v);
    }

    /**
     * Create some byte container type and fill it with some random @p bytes.
     *
     * @tparam T     the desired byte container type (e.g std::vector<uint8_t>)
     * @param  bytes number of random bytes to initialize the container with
     * @return       a container of type T with @p bytes random bytes
     * @throws Exception if RNG or memory allocation fails
     */
    template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
        requires std::default_initializable<T>
    T random_vec(size_t bytes) {
        T result;
        random_vec(result, bytes);
        return result;
    }

    /**
     * Create a std::array of @p bytes random bytes
     */
    template <size_t bytes>
    std::array<uint8_t, bytes> random_array() {
        std::array<uint8_t, bytes> result;
        random_vec(result);
        return result;
    }

    /**
     * Return a random byte
     * @return random byte
     * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
     * @throws Exception if the RNG fails
     */
    uint8_t next_byte() {
        uint8_t b;
        this->fill_bytes_with_input(std::span(&b, 1), {});
        return b;
    }

    /**
     * @return a random byte that is greater than zero
     * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
     * @throws Exception if the RNG fails
     */
    uint8_t next_nonzero_byte() {
        uint8_t b = this->next_byte();
        while (b == 0) {
            b = this->next_byte();
        }
        return b;
    }

   protected:
    /**
     * Generic interface to provide entropy to a concrete implementation and to
     * fill a given buffer with random output. Both @p output and @p input may
     * be empty and should be ignored in that case. If both buffers are
     * non-empty implementations should typically first apply the @p input data
     * and then generate random data into @p output.
     *
     * This method must be implemented by all RandomNumberGenerator sub-classes.
     *
     * @param output  Byte buffer to write random bytes into. Implementations
     *                should not read from this buffer.
     * @param input   Byte buffer that may contain bytes to be incorporated in
     *                the RNG's internal state. Implementations may choose to
     *                ignore the bytes in this buffer.
     */
    virtual void fill_bytes_with_input(std::span<uint8_t> output,
                                       std::span<const uint8_t> input) = 0;
};

/**
 * Convenience typedef
 */
typedef RandomNumberGenerator RNG;

/**
 * Hardware_RNG exists to tag hardware RNG types (PKCS11_RNG, TPM_RNG, Processor_RNG)
 */
class BOTAN_PUBLIC_API(2, 0) Hardware_RNG : public RandomNumberGenerator {
   public:
    void clear() final { /* no way to clear state of hardware RNG */ }
};

/**
 * Null/stub RNG - fails if you try to use it for anything
 * This is not generally useful except for in certain tests
 */
class BOTAN_PUBLIC_API(2, 0) Null_RNG final : public RandomNumberGenerator {
   public:
    bool is_seeded() const override { return false; }

    bool accepts_input() const override { return false; }

    void clear() override {}

    std::string name() const override { return "Null_RNG"; }

   private:
    void fill_bytes_with_input(std::span<uint8_t> output,
                               std::span<const uint8_t> /* ignored */) override;
};

}    // namespace Botan

namespace Botan {

/**
 * Inherited by RNGs which maintain in-process state, like HMAC_DRBG.
 * On Unix these RNGs are vulnerable to problems with fork, where the
 * RNG state is duplicated, and the parent and child process RNGs will
 * produce identical output until one of them reseeds. Stateful_RNG
 * reseeds itself whenever a fork is detected, or after a set number of
 * bytes have been output.
 *
 * Not implemented by RNGs which access an external RNG, such as the
 * system PRNG or a hardware RNG.
 */
class BOTAN_PUBLIC_API(2, 0) Stateful_RNG : public RandomNumberGenerator {
   public:
    /**
     * @param rng is a reference to some RNG which will be used
     * to perform the periodic reseeding
     * @param entropy_sources will be polled to perform reseeding periodically
     * @param reseed_interval specifies a limit of how many times
     * the RNG will be called before automatic reseeding is performed
     */
    Stateful_RNG(RandomNumberGenerator& rng, Entropy_Sources& entropy_sources,
                 size_t reseed_interval)
        : m_underlying_rng(&rng),
          m_entropy_sources(&entropy_sources),
          m_reseed_interval(reseed_interval) {}

    /**
     * @param rng is a reference to some RNG which will be used
     * to perform the periodic reseeding
     * @param reseed_interval specifies a limit of how many times
     * the RNG will be called before automatic reseeding is performed
     */
    Stateful_RNG(RandomNumberGenerator& rng, size_t reseed_interval)
        : m_underlying_rng(&rng), m_reseed_interval(reseed_interval) {}

    /**
     * @param entropy_sources will be polled to perform reseeding periodically
     * @param reseed_interval specifies a limit of how many times
     * the RNG will be called before automatic reseeding is performed
     */
    Stateful_RNG(Entropy_Sources& entropy_sources, size_t reseed_interval)
        : m_entropy_sources(&entropy_sources), m_reseed_interval(reseed_interval) {}

    /**
     * In this case, automatic reseeding is impossible
     */
    Stateful_RNG() : m_reseed_interval(0) {}

    /**
     * Consume this input and mark the RNG as initialized regardless
     * of the length of the input or the current seeded state of
     * the RNG.
     */
    void initialize_with(std::span<const uint8_t> input);

    void initialize_with(const uint8_t input[], size_t length) {
        this->initialize_with(std::span(input, length));
    }

    bool is_seeded() const final;

    bool accepts_input() const final { return true; }

    /**
     * Mark state as requiring a reseed on next use
     */
    void force_reseed();

    void reseed_from_rng(RandomNumberGenerator& rng,
                         size_t poll_bits = RandomNumberGenerator::DefaultPollBits) final;

    /**
     * Poll provided sources for up to poll_bits bits of entropy
     * or until the timeout expires. Returns estimate of the number
     * of bits collected.
     */
    size_t reseed(Entropy_Sources& srcs, size_t poll_bits = RandomNumberGenerator::DefaultPollBits,
                  std::chrono::milliseconds poll_timeout =
                      RandomNumberGenerator::DefaultPollTimeout) override;

    /**
     * @return intended security level of this DRBG
     */
    virtual size_t security_level() const = 0;

    /**
     * Some DRBGs have a notion of the maximum number of bytes per
     * request.  Longer requests (to randomize) will be treated as
     * multiple requests, and may initiate reseeding multiple times,
     * depending on the values of max_number_of_bytes_per_request and
     * reseed_interval(). This function returns zero if the RNG in
     * question does not have such a notion.
     *
     * @return max number of bytes per request (or zero)
     */
    virtual size_t max_number_of_bytes_per_request() const = 0;

    size_t reseed_interval() const { return m_reseed_interval; }

    void clear() final;

   protected:
    void reseed_check();

    virtual void generate_output(std::span<uint8_t> output, std::span<const uint8_t> input) = 0;

    virtual void update(std::span<const uint8_t> input) = 0;

    virtual void clear_state() = 0;

   private:
    void generate_batched_output(std::span<uint8_t> output, std::span<const uint8_t> input);

    void fill_bytes_with_input(std::span<uint8_t> output, std::span<const uint8_t> input) final;

    void reset_reseed_counter();

    mutable recursive_mutex_type m_mutex;

    // A non-owned and possibly null pointer to shared RNG
    RandomNumberGenerator* m_underlying_rng = nullptr;

    // A non-owned and possibly null pointer to a shared Entropy_Source
    Entropy_Sources* m_entropy_sources = nullptr;

    const size_t m_reseed_interval;
    uint32_t m_last_pid = 0;

    /*
     * Set to 1 after a successful seeding, then incremented.  Reset
     * to 0 by clear() or a fork. This logic is used even if
     * automatic reseeding is disabled (via m_reseed_interval = 0)
     */
    size_t m_reseed_counter = 0;
};

}    // namespace Botan

namespace Botan {

class Entropy_Sources;

/**
 * HMAC_DRBG from NIST SP800-90A
 */
class BOTAN_PUBLIC_API(2, 0) HMAC_DRBG final : public Stateful_RNG {
   public:
    /**
     * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
     *
     * Automatic reseeding is disabled completely, as it has no access to
     * any source for seed material.
     *
     * If a fork is detected, the RNG will be unable to reseed itself
     * in response. In this case, an exception will be thrown rather
     * than generating duplicated output.
     */
    explicit HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf);

    /**
     * Constructor taking a string for the hash
     */
    explicit HMAC_DRBG(std::string_view hmac_hash);

    /**
     * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
     *
     * Automatic reseeding from @p underlying_rng will take place after
     * @p reseed_interval many requests or after a fork was detected.
     *
     * @param prf MAC to use as a PRF
     * @param underlying_rng is a reference to some RNG which will be used
     * to perform the periodic reseeding
     * @param reseed_interval specifies a limit of how many times
     * the RNG will be called before automatic reseeding is performed (max. 2^24)
     * @param max_number_of_bytes_per_request requests that are in size higher
     * than max_number_of_bytes_per_request are treated as if multiple single
     * requests of max_number_of_bytes_per_request size had been made.
     * In theory SP 800-90A requires that we reject any request for a DRBG
     * output longer than max_number_of_bytes_per_request. To avoid inconveniencing
     * the caller who wants an output larger than max_number_of_bytes_per_request,
     * instead treat these requests as if multiple requests of
     * max_number_of_bytes_per_request size had been made. NIST requires for
     * HMAC_DRBG that every implementation set a value no more than 2**19 bits
     * (or 64 KiB). Together with @p reseed_interval = 1 you can enforce that for
     * example every 512 bit automatic reseeding occurs.
     */
    HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf, RandomNumberGenerator& underlying_rng,
              size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval,
              size_t max_number_of_bytes_per_request = 64 * 1024);

    /**
     * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
     *
     * Automatic reseeding from @p entropy_sources will take place after
     * @p reseed_interval many requests or after a fork was detected.
     *
     * @param prf MAC to use as a PRF
     * @param entropy_sources will be polled to perform reseeding periodically
     * @param reseed_interval specifies a limit of how many times
     * the RNG will be called before automatic reseeding is performed (max. 2^24)
     * @param max_number_of_bytes_per_request requests that are in size higher
     * than max_number_of_bytes_per_request are treated as if multiple single
     * requests of max_number_of_bytes_per_request size had been made.
     * In theory SP 800-90A requires that we reject any request for a DRBG
     * output longer than max_number_of_bytes_per_request. To avoid inconveniencing
     * the caller who wants an output larger than max_number_of_bytes_per_request,
     * instead treat these requests as if multiple requests of
     * max_number_of_bytes_per_request size had been made. NIST requires for
     * HMAC_DRBG that every implementation set a value no more than 2**19 bits
     * (or 64 KiB). Together with @p reseed_interval = 1 you can enforce that for
     * example every 512 bit automatic reseeding occurs.
     */
    HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf, Entropy_Sources& entropy_sources,
              size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval,
              size_t max_number_of_bytes_per_request = 64 * 1024);

    /**
     * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
     *
     * Automatic reseeding from @p underlying_rng and @p entropy_sources
     * will take place after @p reseed_interval many requests or after
     * a fork was detected.
     *
     * @param prf MAC to use as a PRF
     * @param underlying_rng is a reference to some RNG which will be used
     * to perform the periodic reseeding
     * @param entropy_sources will be polled to perform reseeding periodically
     * @param reseed_interval specifies a limit of how many times
     * the RNG will be called before automatic reseeding is performed (max. 2^24)
     * @param max_number_of_bytes_per_request requests that are in size higher
     * than max_number_of_bytes_per_request are treated as if multiple single
     * requests of max_number_of_bytes_per_request size had been made.
     * In theory SP 800-90A requires that we reject any request for a DRBG
     * output longer than max_number_of_bytes_per_request. To avoid inconveniencing
     * the caller who wants an output larger than max_number_of_bytes_per_request,
     * instead treat these requests as if multiple requests of
     * max_number_of_bytes_per_request size had been made. NIST requires for
     * HMAC_DRBG that every implementation set a value no more than 2**19 bits
     * (or 64 KiB). Together with @p reseed_interval = 1 you can enforce that for
     * example every 512 bit automatic reseeding occurs.
     */
    HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf, RandomNumberGenerator& underlying_rng,
              Entropy_Sources& entropy_sources,
              size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval,
              size_t max_number_of_bytes_per_request = 64 * 1024);

    std::string name() const override;

    size_t security_level() const override;

    size_t max_number_of_bytes_per_request() const override {
        return m_max_number_of_bytes_per_request;
    }

   private:
    void update(std::span<const uint8_t> input) override;

    void generate_output(std::span<uint8_t> output, std::span<const uint8_t> input) override;

    void clear_state() override;

    std::unique_ptr<MessageAuthenticationCode> m_mac;
    secure_vector<uint8_t> m_V;
    secure_vector<uint8_t> m_T;
    const size_t m_max_number_of_bytes_per_request;
    const size_t m_security_level;
};

}    // namespace Botan

namespace Botan {

/**
 * Key Derivation Function
 */
class BOTAN_PUBLIC_API(2, 0) KDF {
   public:
    virtual ~KDF() = default;

    /**
     * Create an instance based on a name
     * If provider is empty then best available is chosen.
     * @param algo_spec algorithm name
     * @param provider provider implementation to choose
     * @return a null pointer if the algo/provider combination cannot be found
     */
    static std::unique_ptr<KDF> create(std::string_view algo_spec, std::string_view provider = "");

    /**
     * Create an instance based on a name, or throw if the
     * algo/provider combination cannot be found. If provider is
     * empty then best available is chosen.
     */
    static std::unique_ptr<KDF> create_or_throw(std::string_view algo_spec,
                                                std::string_view provider = "");

    /**
     * @return list of available providers for this algorithm, empty if not available
     */
    static std::vector<std::string> providers(std::string_view algo_spec);

    /**
     * @return KDF name
     */
    virtual std::string name() const = 0;

    /**
     * Derive a key
     * @param key buffer holding the derived key, must be of length key_len
     * @param key_len the desired output length in bytes
     * @param secret the secret input
     * @param secret_len size of secret in bytes
     * @param salt a diversifier
     * @param salt_len size of salt in bytes
     * @param label purpose for the derived keying material
     * @param label_len size of label in bytes
     */
    BOTAN_DEPRECATED("Use KDF::derive_key")
    void kdf(uint8_t key[], size_t key_len, const uint8_t secret[], size_t secret_len,
             const uint8_t salt[], size_t salt_len, const uint8_t label[], size_t label_len) const {
        derive_key({key, key_len}, {secret, secret_len}, {salt, salt_len}, {label, label_len});
    }

    /**
     * Derive a key
     * @param key_len the desired output length in bytes
     * @param secret the secret input
     * @param secret_len size of secret in bytes
     * @param salt a diversifier
     * @param salt_len size of salt in bytes
     * @param label purpose for the derived keying material
     * @param label_len size of label in bytes
     * @return the derived key
     */
    template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
    BOTAN_DEPRECATED("Use std::span or std::string_view overloads")
    T derive_key(size_t key_len, const uint8_t secret[], size_t secret_len, const uint8_t salt[],
                 size_t salt_len, const uint8_t label[] = nullptr, size_t label_len = 0) const {
        return derive_key<T>(key_len, {secret, secret_len}, {salt, salt_len}, {label, label_len});
    }

    /**
     * Derive a key
     * @param key_len the desired output length in bytes
     * @param secret the secret input
     * @param salt a diversifier
     * @param label purpose for the derived keying material
     * @return the derived key
     */
    template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
    T derive_key(size_t key_len, std::span<const uint8_t> secret, std::string_view salt = "",
                 std::string_view label = "") const {
        return derive_key<T>(key_len, secret, _as_span(salt), _as_span(label));
    }

    /**
     * Derive a key
     * @param key the output buffer for the to-be-derived key
     * @param secret the secret input
     * @param salt a diversifier
     * @param label purpose for the derived keying material
     */
    void derive_key(std::span<uint8_t> key, std::span<const uint8_t> secret,
                    std::span<const uint8_t> salt, std::span<const uint8_t> label) const {
        perform_kdf(key, secret, salt, label);
    }

    /**
     * Derive a key
     * @param key_len the desired output length in bytes
     * @param secret the secret input
     * @param salt a diversifier
     * @param label purpose for the derived keying material
     * @return the derived key
     */
    template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
    T derive_key(size_t key_len, std::span<const uint8_t> secret, std::span<const uint8_t> salt,
                 std::span<const uint8_t> label) const {
        T key(key_len);
        perform_kdf(key, secret, salt, label);
        return key;
    }

    /**
     * Derive a key
     * @param key_len the desired output length in bytes
     * @param secret the secret input
     * @param salt a diversifier
     * @param salt_len size of salt in bytes
     * @param label purpose for the derived keying material
     * @return the derived key
     */
    template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
    BOTAN_DEPRECATED("Use std::span or std::string_view overloads")
    T derive_key(size_t key_len, std::span<const uint8_t> secret, const uint8_t salt[],
                 size_t salt_len, std::string_view label = "") const {
        return derive_key<T>(key_len, secret, {salt, salt_len}, _as_span(label));
    }

    /**
     * Derive a key
     * @param key_len the desired output length in bytes
     * @param secret the secret input
     * @param secret_len size of secret in bytes
     * @param salt a diversifier
     * @param label purpose for the derived keying material
     * @return the derived key
     */
    template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
    BOTAN_DEPRECATED("Use std::span or std::string_view overloads")
    T derive_key(size_t key_len, const uint8_t secret[], size_t secret_len,
                 std::string_view salt = "", std::string_view label = "") const {
        return derive_key<T>(key_len, {secret, secret_len}, _as_span(salt), _as_span(label));
    }

    /**
     * Derive a key
     * @tparam key_len the desired output length in bytes
     * @param secret the secret input
     * @param salt a diversifier
     * @param label purpose for the derived keying material
     * @return the derived key
     */
    template <size_t key_len>
    std::array<uint8_t, key_len> derive_key(std::span<const uint8_t> secret,
                                            std::span<const uint8_t> salt = {},
                                            std::span<const uint8_t> label = {}) {
        std::array<uint8_t, key_len> key;
        perform_kdf(key, secret, salt, label);
        return key;
    }

    /**
     * Derive a key
     * @tparam key_len the desired output length in bytes
     * @param secret the secret input
     * @param salt a diversifier
     * @param label purpose for the derived keying material
     * @return the derived key
     */
    template <size_t key_len>
    std::array<uint8_t, key_len> derive_key(std::span<const uint8_t> secret,
                                            std::span<const uint8_t> salt = {},
                                            std::string_view label = "") {
        return derive_key<key_len>(secret, salt, _as_span(label));
    }

    /**
     * Derive a key
     * @tparam key_len the desired output length in bytes
     * @param secret the secret input
     * @param salt a diversifier
     * @param label purpose for the derived keying material
     * @return the derived key
     */
    template <size_t key_len>
    std::array<uint8_t, key_len> derive_key(std::span<const uint8_t> secret,
                                            std::string_view salt = "",
                                            std::string_view label = "") {
        return derive_key<key_len>(secret, _as_span(salt), _as_span(label));
    }

    /**
     * @return new object representing the same algorithm as *this
     */
    virtual std::unique_ptr<KDF> new_object() const = 0;

    /**
     * @return new object representing the same algorithm as *this
     */
    KDF* clone() const { return this->new_object().release(); }

   protected:
    /**
     * Internal customization point for subclasses
     *
     * The byte size of the @p key span is the number of bytes to be produced
     * by the concrete key derivation function.
     *
     * @param key the output buffer for the to-be-derived key
     * @param secret the secret input
     * @param salt a diversifier
     * @param label purpose for the derived keying material
     */
    virtual void perform_kdf(std::span<uint8_t> key, std::span<const uint8_t> secret,
                             std::span<const uint8_t> salt,
                             std::span<const uint8_t> label) const = 0;

   private:
    static std::span<const uint8_t> _as_span(std::string_view s) {
        return {reinterpret_cast<const uint8_t*>(s.data()), s.size()};
    }
};

/**
 * Factory method for KDF (key derivation function)
 * @param algo_spec the name of the KDF to create
 * @return pointer to newly allocated object of that type
 *
 * Prefer KDF::create
 */
BOTAN_DEPRECATED("Use KDF::create")

inline KDF* get_kdf(std::string_view algo_spec) {
    if (algo_spec == "Raw") {
        return nullptr;
    }

    return KDF::create_or_throw(algo_spec).release();
}

}    // namespace Botan

BOTAN_FUTURE_INTERNAL_HEADER(mem_ops.h)

/*
The header mem_ops.h previously included the contents of allocator.h

Library code should always include allocator.h to see these
declarations; however when we are not building the library continue to
include the header here to avoid breaking application code.
*/
#if !defined(BOTAN_IS_BEING_BUILT)
#endif

namespace Botan {

/**
 * Scrub memory contents in a way that a compiler should not elide,
 * using some system specific technique. Note that this function might
 * not zero the memory (for example, in some hypothetical
 * implementation it might combine the memory contents with the output
 * of a system PRNG), but if you can detect any difference in behavior
 * at runtime then the clearing is side-effecting and you can just
 * use `clear_mem`.
 *
 * Use this function to scrub memory just before deallocating it, or on
 * a stack buffer before returning from the function.
 *
 * @param ptr a pointer to memory to scrub
 * @param n the number of bytes pointed to by ptr
 */
BOTAN_PUBLIC_API(2, 0) void secure_scrub_memory(void* ptr, size_t n);

/**
 * Scrub memory contents in a way that a compiler should not elide,
 * using some system specific technique. Note that this function might
 * not zero the memory.
 *
 * @param data  the data region to be scrubbed
 */
void secure_scrub_memory(ranges::contiguous_output_range auto&& data) {
    secure_scrub_memory(std::ranges::data(data), ranges::size_bytes(data));
}

#if !defined(BOTAN_IS_BEGIN_BUILT)

/**
 * Memory comparison, input insensitive
 * @param x a pointer to an array
 * @param y a pointer to another array
 * @param len the number of Ts in x and y
 * @return 0xFF iff x[i] == y[i] forall i in [0...n) or 0x00 otherwise
 */
BOTAN_DEPRECATED("This function is deprecated, use constant_time_compare()")
BOTAN_PUBLIC_API(2, 9) uint8_t ct_compare_u8(const uint8_t x[], const uint8_t y[], size_t len);

#endif

/**
 * Memory comparison, input insensitive
 * @param x a range of bytes
 * @param y another range of bytes
 * @return true iff x and y have equal lengths and x[i] == y[i] forall i in [0...n)
 */
BOTAN_PUBLIC_API(3, 3)
bool constant_time_compare(std::span<const uint8_t> x, std::span<const uint8_t> y);

/**
 * Memory comparison, input insensitive
 * @param x a pointer to an array
 * @param y a pointer to another array
 * @param len the number of Ts in x and y
 * @return true iff x[i] == y[i] forall i in [0...n)
 */
inline bool constant_time_compare(const uint8_t x[], const uint8_t y[], size_t len) {
    // simply assumes that *x and *y point to len allocated bytes at least
    return constant_time_compare({x, len}, {y, len});
}

/**
 * Zero out some bytes. Warning: use secure_scrub_memory instead if the
 * memory is about to be freed or otherwise the compiler thinks it can
 * elide the writes.
 *
 * @param ptr a pointer to memory to zero
 * @param bytes the number of bytes to zero in ptr
 */
inline constexpr void clear_bytes(void* ptr, size_t bytes) {
    if (bytes > 0) {
        std::memset(ptr, 0, bytes);
    }
}

/**
 * Zero memory before use. This simply calls memset and should not be
 * used in cases where the compiler cannot see the call as a
 * side-effecting operation (for example, if calling clear_mem before
 * deallocating memory, the compiler would be allowed to omit the call
 * to memset entirely under the as-if rule.)
 *
 * @param ptr a pointer to an array of Ts to zero
 * @param n the number of Ts pointed to by ptr
 */
template <typename T>
inline constexpr void clear_mem(T* ptr, size_t n) {
    clear_bytes(ptr, sizeof(T) * n);
}

/**
 * Zero memory before use. This simply calls memset and should not be
 * used in cases where the compiler cannot see the call as a
 * side-effecting operation.
 *
 * @param mem a contiguous range of Ts to zero
 */
template <ranges::contiguous_output_range R>
inline constexpr void clear_mem(R&& mem)
    requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
{
    clear_bytes(std::ranges::data(mem), ranges::size_bytes(mem));
}

/**
 * Copy memory
 * @param out the destination array
 * @param in the source array
 * @param n the number of elements of in/out
 */
template <typename T>
    requires std::is_trivial<typename std::decay<T>::type>::value
inline constexpr void copy_mem(T* out, const T* in, size_t n) {
    BOTAN_ASSERT_IMPLICATION(n > 0, in != nullptr && out != nullptr,
                             "If n > 0 then args are not null");

    if (in != nullptr && out != nullptr && n > 0) {
        std::memmove(out, in, sizeof(T) * n);
    }
}

/**
 * Copy memory
 * @param out the destination array
 * @param in the source array
 */
template <ranges::contiguous_output_range OutR, ranges::contiguous_range InR>
    requires std::is_same_v<std::ranges::range_value_t<OutR>, std::ranges::range_value_t<InR>> &&
             std::is_trivially_copyable_v<std::ranges::range_value_t<InR>>
inline constexpr void copy_mem(OutR&& out, InR&& in) {
    ranges::assert_equal_byte_lengths(out, in);
    if (std::is_constant_evaluated()) {
        std::copy(std::ranges::begin(in), std::ranges::end(in), std::ranges::begin(out));
    } else if (ranges::size_bytes(out) > 0) {
        std::memmove(std::ranges::data(out), std::ranges::data(in), ranges::size_bytes(out));
    }
}

/**
 * Copy a range of a trivially copyable type into another range of trivially
 * copyable type of matching byte length.
 */
template <ranges::contiguous_output_range ToR, ranges::contiguous_range FromR>
    requires std::is_trivially_copyable_v<std::ranges::range_value_t<FromR>> &&
             std::is_trivially_copyable_v<std::ranges::range_value_t<ToR>>
inline constexpr void typecast_copy(ToR&& out, FromR&& in) {
    ranges::assert_equal_byte_lengths(out, in);
    std::memcpy(std::ranges::data(out), std::ranges::data(in), ranges::size_bytes(out));
}

/**
 * Copy a range of trivially copyable type into an instance of trivially
 * copyable type with matching length.
 */
template <typename ToT, ranges::contiguous_range FromR>
    requires std::is_trivially_copyable_v<std::ranges::range_value_t<FromR>> &&
             std::is_trivially_copyable_v<ToT> && (!std::ranges::range<ToT>)
inline constexpr void typecast_copy(ToT& out, FromR&& in) noexcept {
    typecast_copy(std::span<ToT, 1>(&out, 1), in);
}

/**
 * Copy an instance of trivially copyable type into a range of trivially
 * copyable type with matching length.
 */
template <ranges::contiguous_output_range ToR, typename FromT>
    requires std::is_trivially_copyable_v<FromT> && (!std::ranges::range<FromT>) &&
             std::is_trivially_copyable_v<std::ranges::range_value_t<ToR>>
inline constexpr void typecast_copy(ToR&& out, const FromT& in) {
    typecast_copy(out, std::span<const FromT, 1>(&in, 1));
}

/**
 * Create a trivial type by bit-casting a range of trivially copyable type with
 * matching length into it.
 */
template <typename ToT, ranges::contiguous_range FromR>
    requires std::is_default_constructible_v<ToT> && std::is_trivially_copyable_v<ToT> &&
             std::is_trivially_copyable_v<std::ranges::range_value_t<FromR>>
inline constexpr ToT typecast_copy(FromR&& src) noexcept {
    ToT dst;
    typecast_copy(dst, src);
    return dst;
}

// TODO: deprecate and replace
template <typename T>
inline constexpr void typecast_copy(uint8_t out[], T in[], size_t N)
    requires std::is_trivially_copyable<T>::value
{
    // asserts that *in and *out point to the correct amount of memory
    typecast_copy(std::span<uint8_t>(out, sizeof(T) * N), std::span<const T>(in, N));
}

// TODO: deprecate and replace
template <typename T>
inline constexpr void typecast_copy(T out[], const uint8_t in[], size_t N)
    requires std::is_trivial<T>::value
{
    // asserts that *in and *out point to the correct amount of memory
    typecast_copy(std::span<T>(out, N), std::span<const uint8_t>(in, N * sizeof(T)));
}

// TODO: deprecate and replace
template <typename T>
inline constexpr void typecast_copy(uint8_t out[], const T& in) {
    // asserts that *out points to the correct amount of memory
    typecast_copy(std::span<uint8_t, sizeof(T)>(out, sizeof(T)), in);
}

// TODO: deprecate and replace
template <typename T>
    requires std::is_trivial<typename std::decay<T>::type>::value
inline constexpr void typecast_copy(T& out, const uint8_t in[]) {
    // asserts that *in points to the correct amount of memory
    typecast_copy(out, std::span<const uint8_t, sizeof(T)>(in, sizeof(T)));
}

// TODO: deprecate and replace
template <typename To>
    requires std::is_trivial<To>::value
inline constexpr To typecast_copy(const uint8_t src[]) noexcept {
    // asserts that *src points to the correct amount of memory
    return typecast_copy<To>(std::span<const uint8_t, sizeof(To)>(src, sizeof(To)));
}

#if !defined(BOTAN_IS_BEGIN_BUILT)
/**
 * Set memory to a fixed value
 * @param ptr a pointer to an array of bytes
 * @param n the number of Ts pointed to by ptr
 * @param val the value to set each byte to
 */
BOTAN_DEPRECATED("This function is deprecated")
inline constexpr void set_mem(uint8_t* ptr, size_t n, uint8_t val) {
    if (n > 0) {
        std::memset(ptr, val, n);
    }
}
#endif

inline const uint8_t* cast_char_ptr_to_uint8(const char* s) {
    return reinterpret_cast<const uint8_t*>(s);
}

inline const char* cast_uint8_ptr_to_char(const uint8_t* b) {
    return reinterpret_cast<const char*>(b);
}

inline uint8_t* cast_char_ptr_to_uint8(char* s) { return reinterpret_cast<uint8_t*>(s); }

inline char* cast_uint8_ptr_to_char(uint8_t* b) { return reinterpret_cast<char*>(b); }

#if !defined(BOTAN_IS_BEING_BUILT)
/**
 * Memory comparison, input insensitive
 * @param p1 a pointer to an array
 * @param p2 a pointer to another array
 * @param n the number of Ts in p1 and p2
 * @return true iff p1[i] == p2[i] forall i in [0...n)
 */
template <typename T>
BOTAN_DEPRECATED("This function is deprecated")
inline bool same_mem(const T* p1, const T* p2, size_t n) {
    volatile T difference = 0;

    for (size_t i = 0; i != n; ++i) {
        difference = difference | (p1[i] ^ p2[i]);
    }

    return difference == 0;
}
#endif

#if !defined(BOTAN_IS_BEING_BUILT)

template <typename T, typename Alloc>
BOTAN_DEPRECATED("The buffer_insert functions are deprecated")
size_t buffer_insert(std::vector<T, Alloc>& buf, size_t buf_offset, const T input[],
                     size_t input_length) {
    BOTAN_ASSERT_NOMSG(buf_offset <= buf.size());
    const size_t to_copy = std::min(input_length, buf.size() - buf_offset);
    if (to_copy > 0) {
        copy_mem(&buf[buf_offset], input, to_copy);
    }
    return to_copy;
}

template <typename T, typename Alloc, typename Alloc2>
BOTAN_DEPRECATED("The buffer_insert functions are deprecated")
size_t buffer_insert(std::vector<T, Alloc>& buf, size_t buf_offset,
                     const std::vector<T, Alloc2>& input) {
    BOTAN_ASSERT_NOMSG(buf_offset <= buf.size());
    const size_t to_copy = std::min(input.size(), buf.size() - buf_offset);
    if (to_copy > 0) {
        copy_mem(&buf[buf_offset], input.data(), to_copy);
    }
    return to_copy;
}

#endif

/**
 * XOR arrays. Postcondition out[i] = in[i] ^ out[i] forall i = 0...length
 * @param out the input/output range
 * @param in the read-only input range
 */
inline constexpr void xor_buf(ranges::contiguous_output_range<uint8_t> auto&& out,
                              ranges::contiguous_range<uint8_t> auto&& in) {
    ranges::assert_equal_byte_lengths(out, in);

    std::span<uint8_t> o(out);
    std::span<const uint8_t> i(in);

    for (; o.size_bytes() >= 32; o = o.subspan(32), i = i.subspan(32)) {
        auto x = typecast_copy<std::array<uint64_t, 4>>(o.template first<32>());
        const auto y = typecast_copy<std::array<uint64_t, 4>>(i.template first<32>());

        x[0] ^= y[0];
        x[1] ^= y[1];
        x[2] ^= y[2];
        x[3] ^= y[3];

        typecast_copy(o.template first<32>(), x);
    }

    for (size_t off = 0; off != o.size_bytes(); ++off) {
        o[off] ^= i[off];
    }
}

/**
 * XOR arrays. Postcondition out[i] = in1[i] ^ in2[i] forall i = 0...length
 * @param out the output range
 * @param in1 the first input range
 * @param in2 the second input range
 */
inline constexpr void xor_buf(ranges::contiguous_output_range<uint8_t> auto&& out,
                              ranges::contiguous_range<uint8_t> auto&& in1,
                              ranges::contiguous_range<uint8_t> auto&& in2) {
    ranges::assert_equal_byte_lengths(out, in1, in2);

    std::span o{out};
    std::span i1{in1};
    std::span i2{in2};

    for (; o.size_bytes() >= 32; o = o.subspan(32), i1 = i1.subspan(32), i2 = i2.subspan(32)) {
        auto x = typecast_copy<std::array<uint64_t, 4>>(i1.template first<32>());
        const auto y = typecast_copy<std::array<uint64_t, 4>>(i2.template first<32>());

        x[0] ^= y[0];
        x[1] ^= y[1];
        x[2] ^= y[2];
        x[3] ^= y[3];

        typecast_copy(o.template first<32>(), x);
    }

    for (size_t off = 0; off != o.size_bytes(); ++off) {
        o[off] = i1[off] ^ i2[off];
    }
}

/**
 * XOR arrays. Postcondition out[i] = in[i] ^ out[i] forall i = 0...length
 * @param out the input/output buffer
 * @param in the read-only input buffer
 * @param length the length of the buffers
 */
inline void xor_buf(uint8_t out[], const uint8_t in[], size_t length) {
    // simply assumes that *out and *in point to "length" allocated bytes at least
    xor_buf(std::span{out, length}, std::span{in, length});
}

/**
 * XOR arrays. Postcondition out[i] = in[i] ^ in2[i] forall i = 0...length
 * @param out the output buffer
 * @param in the first input buffer
 * @param in2 the second input buffer
 * @param length the length of the three buffers
 */
inline void xor_buf(uint8_t out[], const uint8_t in[], const uint8_t in2[], size_t length) {
    // simply assumes that *out, *in, and *in2 point to "length" allocated bytes at least
    xor_buf(std::span{out, length}, std::span{in, length}, std::span{in2, length});
}

// TODO: deprecate and replace, use .subspan()
inline void xor_buf(std::span<uint8_t> out, std::span<const uint8_t> in, size_t n) {
    BOTAN_ARG_CHECK(out.size() >= n, "output span is too small");
    BOTAN_ARG_CHECK(in.size() >= n, "input span is too small");
    xor_buf(out.first(n), in.first(n));
}

// TODO: deprecate and replace, use .subspan()
template <typename Alloc>
void xor_buf(std::vector<uint8_t, Alloc>& out, const uint8_t* in, size_t n) {
    BOTAN_ARG_CHECK(out.size() >= n, "output vector is too small");
    // simply assumes that *in points to "n" allocated bytes at least
    xor_buf(std::span{out}.first(n), std::span{in, n});
}

// TODO: deprecate and replace
template <typename Alloc, typename Alloc2>
void xor_buf(std::vector<uint8_t, Alloc>& out, const uint8_t* in,
             const std::vector<uint8_t, Alloc2>& in2, size_t n) {
    BOTAN_ARG_CHECK(out.size() >= n, "output vector is too small");
    BOTAN_ARG_CHECK(in2.size() >= n, "input vector is too small");
    // simply assumes that *in points to "n" allocated bytes at least
    xor_buf(std::span{out}.first(n), std::span{in, n}, std::span{in2}.first(n));
}

template <typename Alloc, typename Alloc2>
std::vector<uint8_t, Alloc>& operator^=(std::vector<uint8_t, Alloc>& out,
                                        const std::vector<uint8_t, Alloc2>& in) {
    if (out.size() < in.size()) {
        out.resize(in.size());
    }

    xor_buf(std::span{out}.first(in.size()), in);
    return out;
}

}    // namespace Botan

/*
 * This entire interface is deprecated. Use the interface in pwdhash.h
 */
BOTAN_DEPRECATED_HEADER("pbkdf.h")

namespace Botan {

/**
 * Base class for PBKDF (password based key derivation function)
 * implementations. Converts a password into a key using a salt
 * and iterated hashing to make brute force attacks harder.
 *
 * Starting in 2.8 this functionality is also offered by PasswordHash.
 * The PBKDF interface may be removed in a future release.
 */
class BOTAN_PUBLIC_API(2, 0) PBKDF {
   public:
    /**
     * Create an instance based on a name
     * If provider is empty then best available is chosen.
     * @param algo_spec algorithm name
     * @param provider provider implementation to choose
     * @return a null pointer if the algo/provider combination cannot be found
     */
    BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
    static std::unique_ptr<PBKDF> create(std::string_view algo_spec,
                                         std::string_view provider = "");

    /**
     * Create an instance based on a name, or throw if the
     * algo/provider combination cannot be found. If provider is
     * empty then best available is chosen.
     */
    BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
    static std::unique_ptr<PBKDF> create_or_throw(std::string_view algo_spec,
                                                  std::string_view provider = "");

    /**
     * @return list of available providers for this algorithm, empty if not available
     */
    static std::vector<std::string> providers(std::string_view algo_spec);

    /**
     * @return new instance of this same algorithm
     */
    virtual std::unique_ptr<PBKDF> new_object() const = 0;

    /**
     * @return new instance of this same algorithm
     */
    PBKDF* clone() const { return this->new_object().release(); }

    /**
     * @return name of this PBKDF
     */
    virtual std::string name() const = 0;

    virtual ~PBKDF() = default;

    /**
     * Derive a key from a passphrase for a number of iterations
     * specified by either iterations or if iterations == 0 then
     * running until msec time has elapsed.
     *
     * @param out buffer to store the derived key, must be of out_len bytes
     * @param out_len the desired length of the key to produce
     * @param passphrase the password to derive the key from
     * @param salt a randomly chosen salt
     * @param salt_len length of salt in bytes
     * @param iterations the number of iterations to use (use 10K or more)
     * @param msec if iterations is zero, then instead the PBKDF is
     *        run until msec milliseconds has passed.
     * @return the number of iterations performed
     */
    virtual size_t pbkdf(uint8_t out[], size_t out_len, std::string_view passphrase,
                         const uint8_t salt[], size_t salt_len, size_t iterations,
                         std::chrono::milliseconds msec) const = 0;

    /**
     * Derive a key from a passphrase for a number of iterations.
     *
     * @param out buffer to store the derived key, must be of out_len bytes
     * @param out_len the desired length of the key to produce
     * @param passphrase the password to derive the key from
     * @param salt a randomly chosen salt
     * @param salt_len length of salt in bytes
     * @param iterations the number of iterations to use (use 10K or more)
     */
    void pbkdf_iterations(uint8_t out[], size_t out_len, std::string_view passphrase,
                          const uint8_t salt[], size_t salt_len, size_t iterations) const;

    /**
     * Derive a key from a passphrase, running until msec time has elapsed.
     *
     * @param out buffer to store the derived key, must be of out_len bytes
     * @param out_len the desired length of the key to produce
     * @param passphrase the password to derive the key from
     * @param salt a randomly chosen salt
     * @param salt_len length of salt in bytes
     * @param msec if iterations is zero, then instead the PBKDF is
     *        run until msec milliseconds has passed.
     * @param iterations set to the number iterations executed
     */
    void pbkdf_timed(uint8_t out[], size_t out_len, std::string_view passphrase,
                     const uint8_t salt[], size_t salt_len, std::chrono::milliseconds msec,
                     size_t& iterations) const;

    /**
     * Derive a key from a passphrase for a number of iterations.
     *
     * @param out_len the desired length of the key to produce
     * @param passphrase the password to derive the key from
     * @param salt a randomly chosen salt
     * @param salt_len length of salt in bytes
     * @param iterations the number of iterations to use (use 10K or more)
     * @return the derived key
     */
    secure_vector<uint8_t> pbkdf_iterations(size_t out_len, std::string_view passphrase,
                                            const uint8_t salt[], size_t salt_len,
                                            size_t iterations) const;

    /**
     * Derive a key from a passphrase, running until msec time has elapsed.
     *
     * @param out_len the desired length of the key to produce
     * @param passphrase the password to derive the key from
     * @param salt a randomly chosen salt
     * @param salt_len length of salt in bytes
     * @param msec if iterations is zero, then instead the PBKDF is
     *        run until msec milliseconds has passed.
     * @param iterations set to the number iterations executed
     * @return the derived key
     */
    secure_vector<uint8_t> pbkdf_timed(size_t out_len, std::string_view passphrase,
                                       const uint8_t salt[], size_t salt_len,
                                       std::chrono::milliseconds msec, size_t& iterations) const;

    // Following kept for compat with 1.10:

    /**
     * Derive a key from a passphrase
     * @param out_len the desired length of the key to produce
     * @param passphrase the password to derive the key from
     * @param salt a randomly chosen salt
     * @param salt_len length of salt in bytes
     * @param iterations the number of iterations to use (use 10K or more)
     */
    OctetString derive_key(size_t out_len, std::string_view passphrase, const uint8_t salt[],
                           size_t salt_len, size_t iterations) const {
        return OctetString(pbkdf_iterations(out_len, passphrase, salt, salt_len, iterations));
    }

    /**
     * Derive a key from a passphrase
     * @param out_len the desired length of the key to produce
     * @param passphrase the password to derive the key from
     * @param salt a randomly chosen salt
     * @param iterations the number of iterations to use (use 10K or more)
     */
    template <typename Alloc>
    OctetString derive_key(size_t out_len, std::string_view passphrase,
                           const std::vector<uint8_t, Alloc>& salt, size_t iterations) const {
        return OctetString(
            pbkdf_iterations(out_len, passphrase, salt.data(), salt.size(), iterations));
    }

    /**
     * Derive a key from a passphrase
     * @param out_len the desired length of the key to produce
     * @param passphrase the password to derive the key from
     * @param salt a randomly chosen salt
     * @param salt_len length of salt in bytes
     * @param msec is how long to run the PBKDF
     * @param iterations is set to the number of iterations used
     */
    OctetString derive_key(size_t out_len, std::string_view passphrase, const uint8_t salt[],
                           size_t salt_len, std::chrono::milliseconds msec,
                           size_t& iterations) const {
        return OctetString(pbkdf_timed(out_len, passphrase, salt, salt_len, msec, iterations));
    }

    /**
     * Derive a key from a passphrase using a certain amount of time
     * @param out_len the desired length of the key to produce
     * @param passphrase the password to derive the key from
     * @param salt a randomly chosen salt
     * @param msec is how long to run the PBKDF
     * @param iterations is set to the number of iterations used
     */
    template <typename Alloc>
    OctetString derive_key(size_t out_len, std::string_view passphrase,
                           const std::vector<uint8_t, Alloc>& salt, std::chrono::milliseconds msec,
                           size_t& iterations) const {
        return OctetString(
            pbkdf_timed(out_len, passphrase, salt.data(), salt.size(), msec, iterations));
    }
};

/*
 * Compatibility typedef
 */
typedef PBKDF S2K;

/**
 * Password based key derivation function factory method
 * @param algo_spec the name of the desired PBKDF algorithm
 * @param provider the provider to use
 * @return pointer to newly allocated object of that type
 */
BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
inline PBKDF* get_pbkdf(std::string_view algo_spec, std::string_view provider = "") {
    return PBKDF::create_or_throw(algo_spec, provider).release();
}

BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
inline PBKDF* get_s2k(std::string_view algo_spec) {
    return PBKDF::create_or_throw(algo_spec).release();
}

}    // namespace Botan

namespace Botan {

/**
 * Base class for password based key derivation functions.
 *
 * Converts a password into a key using a salt and iterated hashing to
 * make brute force attacks harder.
 */
class BOTAN_PUBLIC_API(2, 8) PasswordHash {
   public:
    virtual ~PasswordHash() = default;

    virtual std::string to_string() const = 0;

    /**
     * Most password hashes have some notion of iterations.
     */
    virtual size_t iterations() const = 0;

    /**
     * Some password hashing algorithms have a parameter which controls how
     * much memory is used. If not supported by some algorithm, returns 0.
     */
    virtual size_t memory_param() const { return 0; }

    /**
     * Some password hashing algorithms have a parallelism parameter.
     * If the algorithm does not support this notion, then the
     * function returns zero. This allows distinguishing between a
     * password hash which just does not support parallel operation,
     * vs one that does support parallel operation but which has been
     * configured to use a single lane.
     */
    virtual size_t parallelism() const { return 0; }

    /**
     * Returns an estimate of the total number of bytes required to perform this
     * key derivation.
     *
     * If this algorithm uses a small and constant amount of memory, with no
     * effort made towards being memory hard, this function returns 0.
     */
    virtual size_t total_memory_usage() const { return 0; }

    /**
     * @returns true if this password hash supports supplying a key
     */
    virtual bool supports_keyed_operation() const { return false; }

    /**
     * @returns true if this password hash supports supplying associated data
     */
    virtual bool supports_associated_data() const { return false; }

    /**
     * Hash a password into a bitstring
     *
     * Derive a key from the specified @p password and  @p salt, placing it into
     * @p out.
     *
     * @param out a span where the derived key will be placed
     * @param password the password to derive the key from
     * @param salt a randomly chosen salt
     *
     * This function is const, but is not thread safe. Different threads should
     * either use unique objects, or serialize all access.
     */
    void hash(std::span<uint8_t> out, std::string_view password,
              std::span<const uint8_t> salt) const {
        this->derive_key(out.data(), out.size(), password.data(), password.size(), salt.data(),
                         salt.size());
    }

    /**
     * Hash a password into a bitstring
     *
     * Derive a key from the specified @p password, @p salt, @p
     * associated_data, and secret @p key, placing it into @p out. The
     * @p associated_data and @p key are both allowed to be empty. Currently
     * non-empty AD/key is only supported with Argon2.
     *
     * @param out a span where the derived key will be placed
     * @param password the password to derive the key from
     * @param salt a randomly chosen salt
     * @param associated_data some additional data
     * @param key a secret key
     *
     * This function is const, but is not thread safe. Different threads should
     * either use unique objects, or serialize all access.
     */
    void hash(std::span<uint8_t> out, std::string_view password, std::span<const uint8_t> salt,
              std::span<const uint8_t> associated_data, std::span<const uint8_t> key) const {
        this->derive_key(out.data(), out.size(), password.data(), password.size(), salt.data(),
                         salt.size(), associated_data.data(), associated_data.size(), key.data(),
                         key.size());
    }

    /**
     * Derive a key from a password
     *
     * @param out buffer to store the derived key, must be of out_len bytes
     * @param out_len the desired length of the key to produce
     * @param password the password to derive the key from
     * @param password_len the length of password in bytes
     * @param salt a randomly chosen salt
     * @param salt_len length of salt in bytes
     *
     * This function is const, but is not thread safe. Different threads should
     * either use unique objects, or serialize all access.
     */
    virtual void derive_key(uint8_t out[], size_t out_len, const char* password,
                            size_t password_len, const uint8_t salt[], size_t salt_len) const = 0;

    /**
     * Derive a key from a password plus additional data and/or a secret key
     *
     * Currently this is only supported for Argon2. Using a non-empty AD or key
     * with other algorithms will cause a Not_Implemented exception.
     *
     * @param out buffer to store the derived key, must be of out_len bytes
     * @param out_len the desired length of the key to produce
     * @param password the password to derive the key from
     * @param password_len the length of password in bytes
     * @param salt a randomly chosen salt
     * @param salt_len length of salt in bytes
     * @param ad some additional data
     * @param ad_len length of ad in bytes
     * @param key a secret key
     * @param key_len length of key in bytes
     *
     * This function is const, but is not thread safe. Different threads should
     * either use unique objects, or serialize all access.
     */
    virtual void derive_key(uint8_t out[], size_t out_len, const char* password,
                            size_t password_len, const uint8_t salt[], size_t salt_len,
                            const uint8_t ad[], size_t ad_len, const uint8_t key[],
                            size_t key_len) const;
};

class BOTAN_PUBLIC_API(2, 8) PasswordHashFamily {
   public:
    /**
     * Create an instance based on a name
     * If provider is empty then best available is chosen.
     * @param algo_spec algorithm name
     * @param provider provider implementation to choose
     * @return a null pointer if the algo/provider combination cannot be found
     */
    static std::unique_ptr<PasswordHashFamily> create(std::string_view algo_spec,
                                                      std::string_view provider = "");

    /**
     * Create an instance based on a name, or throw if the
     * algo/provider combination cannot be found. If provider is
     * empty then best available is chosen.
     */
    static std::unique_ptr<PasswordHashFamily> create_or_throw(std::string_view algo_spec,
                                                               std::string_view provider = "");

    /**
     * @return list of available providers for this algorithm, empty if not available
     */
    static std::vector<std::string> providers(std::string_view algo_spec);

    virtual ~PasswordHashFamily() = default;

    /**
     * @return name of this PasswordHash
     */
    virtual std::string name() const = 0;

    /**
     * Return a new parameter set tuned for this machine
     *
     * Return a password hash instance tuned to run for approximately @p msec
     * milliseconds when producing an output of length @p output_length.
     * (Accuracy may vary, use the command line utility ``botan pbkdf_tune`` to
     * check.)
     *
     * The parameters will be selected to use at most @p max_memory_usage_mb
     * megabytes of memory, or if left as zero any size is allowed.
     *
     * This function works by runing a short tuning loop to estimate the
     * performance of the algorithm, then scaling the parameters appropriately
     * to hit the target size. The length of time the tuning loop runs can be
     * controlled using the @p tuning_msec parameter.
     *
     * @param output_length how long the output length will be
     * @param msec the desired execution time in milliseconds
     *
     * @param max_memory_usage_mb some password hash functions can use a
     * tunable amount of memory, in this case max_memory_usage limits the
     * amount of RAM the returned parameters will require, in mebibytes (2**20
     * bytes). It may require some small amount above the request. Set to zero
     * to place no limit at all.
     * @param tuning_msec how long to run the tuning loop
     */
    virtual std::unique_ptr<PasswordHash> tune(
        size_t output_length, std::chrono::milliseconds msec, size_t max_memory_usage_mb = 0,
        std::chrono::milliseconds tuning_msec = std::chrono::milliseconds(10)) const = 0;

    /**
     * Return some default parameter set for this PBKDF that should be good
     * enough for most users. The value returned may change over time as
     * processing power and attacks improve.
     */
    virtual std::unique_ptr<PasswordHash> default_params() const = 0;

    /**
     * Return a parameter chosen based on a rough approximation with the
     * specified iteration count. The exact value this returns for a particular
     * algorithm may change from over time. Think of it as an alternative to
     * tune, where time is expressed in terms of PBKDF2 iterations rather than
     * milliseconds.
     */
    virtual std::unique_ptr<PasswordHash> from_iterations(size_t iterations) const = 0;

    /**
     * Create a password hash using some scheme specific format. Parameters are as follows:
     * - For PBKDF2, PGP-S2K, and Bcrypt-PBKDF, i1 is iterations
     * - Scrypt uses N, r, p for i{1-3}
     * - Argon2 family uses memory (in KB), iterations, and parallelism for i{1-3}
     *
     * All unneeded parameters should be set to 0 or left blank.
     */
    virtual std::unique_ptr<PasswordHash> from_params(size_t i1, size_t i2 = 0,
                                                      size_t i3 = 0) const = 0;
};

}    // namespace Botan

// Use pwdhash.h
BOTAN_FUTURE_INTERNAL_HEADER(pbkdf2.h)

namespace Botan {

BOTAN_PUBLIC_API(2, 0)
size_t pbkdf2(MessageAuthenticationCode& prf, uint8_t out[], size_t out_len,
              std::string_view passphrase, const uint8_t salt[], size_t salt_len, size_t iterations,
              std::chrono::milliseconds msec);

/**
 * Perform PBKDF2. The prf is assumed to be keyed already.
 */
BOTAN_PUBLIC_API(2, 8)
void pbkdf2(MessageAuthenticationCode& prf, uint8_t out[], size_t out_len, const uint8_t salt[],
            size_t salt_len, size_t iterations);

/**
 * PBKDF2
 */
class BOTAN_PUBLIC_API(2, 8) PBKDF2 final : public PasswordHash {
   public:
    PBKDF2(const MessageAuthenticationCode& prf, size_t iter)
        : m_prf(prf.new_object()), m_iterations(iter) {}

    BOTAN_DEPRECATED("For runtime tuning use PBKDF2_Family::tune")
    PBKDF2(const MessageAuthenticationCode& prf, size_t olen, std::chrono::milliseconds msec);

    size_t iterations() const override { return m_iterations; }

    std::string to_string() const override;

    void derive_key(uint8_t out[], size_t out_len, const char* password, size_t password_len,
                    const uint8_t salt[], size_t salt_len) const override;

   private:
    std::unique_ptr<MessageAuthenticationCode> m_prf;
    size_t m_iterations;
};

/**
 * Family of PKCS #5 PBKDF2 operations
 */
class BOTAN_PUBLIC_API(2, 8) PBKDF2_Family final : public PasswordHashFamily {
   public:
    PBKDF2_Family(std::unique_ptr<MessageAuthenticationCode> prf) : m_prf(std::move(prf)) {}

    std::string name() const override;

    std::unique_ptr<PasswordHash> tune(size_t output_len, std::chrono::milliseconds msec,
                                       size_t max_memory,
                                       std::chrono::milliseconds tune_msec) const override;

    /**
     * Return some default parameter set for this PBKDF that should be good
     * enough for most users. The value returned may change over time as
     * processing power and attacks improve.
     */
    std::unique_ptr<PasswordHash> default_params() const override;

    std::unique_ptr<PasswordHash> from_iterations(size_t iter) const override;

    std::unique_ptr<PasswordHash> from_params(size_t iter, size_t, size_t) const override;

   private:
    std::unique_ptr<MessageAuthenticationCode> m_prf;
};

/**
 * PKCS #5 PBKDF2 (old interface)
 */
class BOTAN_PUBLIC_API(2, 0) PKCS5_PBKDF2 final : public PBKDF {
   public:
    std::string name() const override;

    std::unique_ptr<PBKDF> new_object() const override;

    size_t pbkdf(uint8_t output_buf[], size_t output_len, std::string_view passphrase,
                 const uint8_t salt[], size_t salt_len, size_t iterations,
                 std::chrono::milliseconds msec) const override;

    /**
     * Create a PKCS #5 instance using the specified message auth code
     * @param mac_fn the MAC object to use as PRF
     */
    BOTAN_DEPRECATED("Use version taking unique_ptr")
    explicit PKCS5_PBKDF2(MessageAuthenticationCode* mac_fn) : m_mac(mac_fn) {}

    /**
     * Create a PKCS #5 instance using the specified message auth code
     * @param mac_fn the MAC object to use as PRF
     */
    BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
    explicit PKCS5_PBKDF2(std::unique_ptr<MessageAuthenticationCode> mac_fn)
        : m_mac(std::move(mac_fn)) {}

   private:
    std::unique_ptr<MessageAuthenticationCode> m_mac;
};

}    // namespace Botan

namespace Botan {

/**
 * Added as an additional "capability tag" to enable arithmetic operators with
 * plain numbers for Strong<> types that wrap a number.
 */
struct EnableArithmeticWithPlainNumber {};

namespace detail {

/**
 * Checks whether the @p CapabilityT is included in the @p Tags type pack.
 */
template <typename CapabilityT, typename... Tags>
constexpr bool has_capability = (std::is_same_v<CapabilityT, Tags> || ...);

template <typename T>
class Strong_Base {
   private:
    T m_value;

   public:
    using wrapped_type = T;

   public:
    Strong_Base() = default;
    Strong_Base(const Strong_Base&) = default;
    Strong_Base(Strong_Base&&) noexcept = default;
    Strong_Base& operator=(const Strong_Base&) = default;
    Strong_Base& operator=(Strong_Base&&) noexcept = default;

    constexpr explicit Strong_Base(T v) : m_value(std::move(v)) {}

    constexpr T& get() & { return m_value; }

    constexpr const T& get() const& { return m_value; }

    constexpr T&& get() && { return std::move(m_value); }

    constexpr const T&& get() const&& { return std::move(m_value); }
};

template <typename T>
class Strong_Adapter : public Strong_Base<T> {
   public:
    using Strong_Base<T>::Strong_Base;
};

template <std::integral T>
class Strong_Adapter<T> : public Strong_Base<T> {
   public:
    using Strong_Base<T>::Strong_Base;
};

template <concepts::container T>
class Container_Strong_Adapter_Base : public Strong_Base<T> {
   public:
    using value_type = typename T::value_type;
    using size_type = typename T::size_type;
    using iterator = typename T::iterator;
    using const_iterator = typename T::const_iterator;

   public:
    using Strong_Base<T>::Strong_Base;

    explicit Container_Strong_Adapter_Base(size_t size)
        requires(concepts::resizable_container<T>)
        : Container_Strong_Adapter_Base(T(size)) {}

    template <typename InputIt>
    Container_Strong_Adapter_Base(InputIt begin, InputIt end)
        : Container_Strong_Adapter_Base(T(begin, end)) {}

   public:
    decltype(auto) begin() noexcept(noexcept(this->get().begin())) { return this->get().begin(); }

    decltype(auto) begin() const noexcept(noexcept(this->get().begin())) {
        return this->get().begin();
    }

    decltype(auto) end() noexcept(noexcept(this->get().end())) { return this->get().end(); }

    decltype(auto) end() const noexcept(noexcept(this->get().end())) { return this->get().end(); }

    decltype(auto) cbegin() noexcept(noexcept(this->get().cbegin())) {
        return this->get().cbegin();
    }

    decltype(auto) cbegin() const noexcept(noexcept(this->get().cbegin())) {
        return this->get().cbegin();
    }

    decltype(auto) cend() noexcept(noexcept(this->get().cend())) { return this->get().cend(); }

    decltype(auto) cend() const noexcept(noexcept(this->get().cend())) {
        return this->get().cend();
    }

    size_type size() const noexcept(noexcept(this->get().size())) { return this->get().size(); }

    bool empty() const noexcept(noexcept(this->get().empty()))
        requires(concepts::has_empty<T>)
    {
        return this->get().empty();
    }

    void resize(size_type size) noexcept(noexcept(this->get().resize(size)))
        requires(concepts::resizable_container<T>)
    {
        this->get().resize(size);
    }

    void reserve(size_type size) noexcept(noexcept(this->get().reserve(size)))
        requires(concepts::reservable_container<T>)
    {
        this->get().reserve(size);
    }

    template <typename U>
    decltype(auto) operator[](U&& i) const noexcept(noexcept(this->get().operator[](i))) {
        return this->get()[std::forward<U>(i)];
    }

    template <typename U>
    decltype(auto) operator[](U&& i) noexcept(noexcept(this->get().operator[](i))) {
        return this->get()[std::forward<U>(i)];
    }

    template <typename U>
    decltype(auto) at(U&& i) const noexcept(noexcept(this->get().at(i)))
        requires(concepts::has_bounds_checked_accessors<T>)
    {
        return this->get().at(std::forward<U>(i));
    }

    template <typename U>
    decltype(auto) at(U&& i) noexcept(noexcept(this->get().at(i)))
        requires(concepts::has_bounds_checked_accessors<T>)
    {
        return this->get().at(std::forward<U>(i));
    }
};

template <concepts::container T>
class Strong_Adapter<T> : public Container_Strong_Adapter_Base<T> {
   public:
    using Container_Strong_Adapter_Base<T>::Container_Strong_Adapter_Base;
};

template <concepts::contiguous_container T>
class Strong_Adapter<T> : public Container_Strong_Adapter_Base<T> {
   public:
    using pointer = typename T::pointer;
    using const_pointer = typename T::const_pointer;

   public:
    using Container_Strong_Adapter_Base<T>::Container_Strong_Adapter_Base;

    explicit Strong_Adapter(
        std::span<const typename Container_Strong_Adapter_Base<T>::value_type> span)
        : Strong_Adapter(T(span.begin(), span.end())) {}

    // Disambiguates the usage of string literals, otherwise:
    // Strong_Adapter(std::span<>) and Strong_Adapter(const char*)
    // would be ambiguous.
    explicit Strong_Adapter(const char* str)
        requires(std::same_as<T, std::string>)
        : Strong_Adapter(std::string(str)) {}

   public:
    decltype(auto) data() noexcept(noexcept(this->get().data())) { return this->get().data(); }

    decltype(auto) data() const noexcept(noexcept(this->get().data())) {
        return this->get().data();
    }
};

}    // namespace detail

/**
 * Strong types can be used as wrappers around common types to provide
 * compile time semantics. They usually contribute to more maintainable and
 * less error-prone code especially when dealing with function parameters.
 *
 * Internally, this provides adapters so that the wrapping strong type behaves
 * as much as the underlying type as possible and desirable.
 *
 * This implementation was inspired by:
 *   https://stackoverflow.com/a/69030899
 */
template <typename T, typename TagTypeT, typename... Capabilities>
class Strong : public detail::Strong_Adapter<T> {
   public:
    using detail::Strong_Adapter<T>::Strong_Adapter;

    template <typename CapabilityT>
    constexpr static bool has_capability() {
        return (std::is_same_v<CapabilityT, Capabilities> || ...);
    }

   private:
    using Tag = TagTypeT;
};

/**
 * @brief Generically unwraps a strong type to its underlying type.
 *
 * If the provided type is not a strong type, it is returned as is.
 *
 * @note This is meant as a helper for generic code that needs to deal with both
 *       wrapped strong types and bare objects. Use the ordinary `get()` method
 *       if you know that you are dealing with a strong type.
 *
 * @param t  value to be unwrapped
 * @return   the unwrapped value
 */
template <typename T>
[[nodiscard]] constexpr decltype(auto) unwrap_strong_type(T&& t) {
    if constexpr (!concepts::strong_type<std::remove_cvref_t<T>>) {
        // If the parameter type isn't a strong type, return it as is.
        return std::forward<T>(t);
    } else {
        // Unwrap the strong type and return the underlying value.
        return std::forward<T>(t).get();
    }
}

/**
 * @brief Wraps a value into a caller-defined (strong) type.
 *
 * If the provided object @p t is already of type @p T, it is returned as is.
 *
 * @note This is meant as a helper for generic code that needs to deal with both
 *       wrapped strong types and bare objects. Use the ordinary constructor if
 *       you know that you are dealing with a bare value type.
 *
 * @param t  value to be wrapped
 * @return   the wrapped value
 */
template <typename T, typename ParamT>
    requires std::constructible_from<T, ParamT> ||
             (concepts::strong_type<T> && std::constructible_from<typename T::wrapped_type, ParamT>)
[[nodiscard]] constexpr decltype(auto) wrap_strong_type(ParamT&& t) {
    if constexpr (std::same_as<std::remove_cvref_t<ParamT>, T>) {
        // Noop, if the parameter type already is the desired return type.
        return std::forward<ParamT>(t);
    } else if constexpr (std::constructible_from<T, ParamT>) {
        // Implicit conversion from the parameter type to the return type.
        return T{std::forward<ParamT>(t)};
    } else {
        // Explicitly calling the wrapped type's constructor to support
        // implicit conversions on types that mark their constructors as explicit.
        static_assert(concepts::strong_type<T> &&
                      std::constructible_from<typename T::wrapped_type, ParamT>);
        return T{typename T::wrapped_type{std::forward<ParamT>(t)}};
    }
}

namespace detail {

template <typename T>
struct wrapped_type_helper {
    using type = T;
};

template <concepts::strong_type T>
struct wrapped_type_helper<T> {
    using type = typename T::wrapped_type;
};

}    // namespace detail

/**
 * @brief Extracts the wrapped type from a strong type.
 *
 * If the provided type is not a strong type, it is returned as is.
 *
 * @note This is meant as a helper for generic code that needs to deal with both
 *       wrapped strong types and bare objects. Use the ordinary `::wrapped_type`
 *       declaration if you know that you are dealing with a strong type.
 */
template <typename T>
using strong_type_wrapped_type = typename detail::wrapped_type_helper<std::remove_cvref_t<T>>::type;

template <typename T, typename... Tags>
    requires(concepts::streamable<T>)
decltype(auto) operator<<(std::ostream& os, const Strong<T, Tags...>& v) {
    return os << v.get();
}

template <typename T, typename... Tags>
    requires(std::equality_comparable<T>)
bool operator==(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs) {
    return lhs.get() == rhs.get();
}

template <typename T, typename... Tags>
    requires(std::three_way_comparable<T>)
auto operator<=>(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs) {
    return lhs.get() <=> rhs.get();
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator<=>(T1 a, Strong<T2, Tags...> b) {
    return a <=> b.get();
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator<=>(Strong<T1, Tags...> a, T2 b) {
    return a.get() <=> b;
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator==(T1 a, Strong<T2, Tags...> b) {
    return a == b.get();
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator==(Strong<T1, Tags...> a, T2 b) {
    return a.get() == b;
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator+(T1 a, Strong<T2, Tags...> b) {
    return Strong<T2, Tags...>(a + b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator+(Strong<T1, Tags...> a, T2 b) {
    return Strong<T1, Tags...>(a.get() + b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator+(Strong<T, Tags...> a, Strong<T, Tags...> b) {
    return Strong<T, Tags...>(a.get() + b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator-(T1 a, Strong<T2, Tags...> b) {
    return Strong<T2, Tags...>(a - b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator-(Strong<T1, Tags...> a, T2 b) {
    return Strong<T1, Tags...>(a.get() - b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator-(Strong<T, Tags...> a, Strong<T, Tags...> b) {
    return Strong<T, Tags...>(a.get() - b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator*(T1 a, Strong<T2, Tags...> b) {
    return Strong<T2, Tags...>(a * b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator*(Strong<T1, Tags...> a, T2 b) {
    return Strong<T1, Tags...>(a.get() * b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator*(Strong<T, Tags...> a, Strong<T, Tags...> b) {
    return Strong<T, Tags...>(a.get() * b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator/(T1 a, Strong<T2, Tags...> b) {
    return Strong<T2, Tags...>(a / b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator/(Strong<T1, Tags...> a, T2 b) {
    return Strong<T1, Tags...>(a.get() / b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator/(Strong<T, Tags...> a, Strong<T, Tags...> b) {
    return Strong<T, Tags...>(a.get() / b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator^(T1 a, Strong<T2, Tags...> b) {
    return Strong<T2, Tags...>(a ^ b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator^(Strong<T1, Tags...> a, T2 b) {
    return Strong<T1, Tags...>(a.get() ^ b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator^(Strong<T, Tags...> a, Strong<T, Tags...> b) {
    return Strong<T, Tags...>(a.get() ^ b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator&(T1 a, Strong<T2, Tags...> b) {
    return Strong<T2, Tags...>(a & b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator&(Strong<T1, Tags...> a, T2 b) {
    return Strong<T1, Tags...>(a.get() & b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator&(Strong<T, Tags...> a, Strong<T, Tags...> b) {
    return Strong<T, Tags...>(a.get() & b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator|(T1 a, Strong<T2, Tags...> b) {
    return Strong<T2, Tags...>(a | b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator|(Strong<T1, Tags...> a, T2 b) {
    return Strong<T1, Tags...>(a.get() | b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator|(Strong<T, Tags...> a, Strong<T, Tags...> b) {
    return Strong<T, Tags...>(a.get() | b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator>>(T1 a, Strong<T2, Tags...> b) {
    return Strong<T2, Tags...>(a >> b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator>>(Strong<T1, Tags...> a, T2 b) {
    return Strong<T1, Tags...>(a.get() >> b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator>>(Strong<T, Tags...> a, Strong<T, Tags...> b) {
    return Strong<T, Tags...>(a.get() >> b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator<<(T1 a, Strong<T2, Tags...> b) {
    return Strong<T2, Tags...>(a << b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator<<(Strong<T1, Tags...> a, T2 b) {
    return Strong<T1, Tags...>(a.get() << b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator<<(Strong<T, Tags...> a, Strong<T, Tags...> b) {
    return Strong<T, Tags...>(a.get() << b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator+=(Strong<T1, Tags...>& a, T2 b) {
    a.get() += b;
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator+=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
    a.get() += b.get();
    return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator-=(Strong<T1, Tags...>& a, T2 b) {
    a.get() -= b;
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator-=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
    a.get() -= b.get();
    return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator*=(Strong<T1, Tags...>& a, T2 b) {
    a.get() *= b;
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator*=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
    a.get() *= b.get();
    return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator/=(Strong<T1, Tags...>& a, T2 b) {
    a.get() /= b;
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator/=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
    a.get() /= b.get();
    return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator^=(Strong<T1, Tags...>& a, T2 b) {
    a.get() ^= b;
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator^=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
    a.get() ^= b.get();
    return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator&=(Strong<T1, Tags...>& a, T2 b) {
    a.get() &= b;
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator&=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
    a.get() &= b.get();
    return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator|=(Strong<T1, Tags...>& a, T2 b) {
    a.get() |= b;
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator|=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
    a.get() |= b.get();
    return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator>>=(Strong<T1, Tags...>& a, T2 b) {
    a.get() >>= b;
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator>>=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
    a.get() >>= b.get();
    return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
    requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator<<=(Strong<T1, Tags...>& a, T2 b) {
    a.get() <<= b;
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator<<=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
    a.get() <<= b.get();
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator++(Strong<T, Tags...>& a, int) {
    auto tmp = a;
    ++a.get();
    return tmp;
}

template <std::integral T, typename... Tags>
constexpr auto operator++(Strong<T, Tags...>& a) {
    ++a.get();
    return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator--(Strong<T, Tags...>& a, int) {
    auto tmp = a;
    --a.get();
    return tmp;
}

template <std::integral T, typename... Tags>
constexpr auto operator--(Strong<T, Tags...>& a) {
    --a.get();
    return a;
}

/**
 * This mimmicks a std::span but keeps track of the strong-type information. Use
 * this when you would want to use `const Strong<...>&` as a parameter
 * declaration. In particular this allows assigning strong-type information to
 * slices of a bigger buffer without copying the bytes. E.g:
 *
 *    using Foo = Strong<std::vector<uint8_t>, Foo_>;
 *
 *    void bar(StrongSpan<Foo> foo) { ... }
 *
 *    std::vector<uint8_t> buffer;
 *    BufferSlicer slicer(buffer);
 *    bar(slicer.take<Foo>());  // This does not copy the data from buffer but
 *                              // just annotates the 'Foo' strong-type info.
 */
template <concepts::contiguous_strong_type T>
class StrongSpan {
    using underlying_span =
        std::conditional_t<std::is_const_v<T>, std::span<const typename T::value_type>,
                           std::span<typename T::value_type>>;

   public:
    using value_type = typename underlying_span::value_type;
    using size_type = typename underlying_span::size_type;
    using iterator = typename underlying_span::iterator;
    using pointer = typename underlying_span::pointer;
    using const_pointer = typename underlying_span::const_pointer;

    StrongSpan() = default;

    explicit StrongSpan(underlying_span span) : m_span(span) {}

    StrongSpan(T& strong) : m_span(strong) {}

    // Allows implicit conversion from `StrongSpan<T>` to `StrongSpan<const T>`.
    // Note that this is not bi-directional. Conversion from `StrongSpan<const T>`
    // to `StrongSpan<T>` is not allowed.
    //
    // TODO: Technically, we should be able to phrase this with a `requires std::is_const_v<T>`
    //       instead of the `std::enable_if` constructions. clang-tidy (14 or 15) doesn't seem
    //       to pick up on that (yet?). As a result, for a non-const T it assumes this to be
    //       a declaration of an ordinary copy constructor. The existance of a copy constructor
    //       is interpreted as "not cheap to copy", setting off the
    //       `performance-unnecessary-value-param` check. See also:
    //       https://github.com/randombit/botan/issues/3591
    template <concepts::contiguous_strong_type T2,
              typename = std::enable_if_t<std::is_same_v<T2, std::remove_const_t<T>>>>
    StrongSpan(const StrongSpan<T2>& other) : m_span(other.get()) {}

    StrongSpan(const StrongSpan& other) = default;

    ~StrongSpan() = default;

    /**
     * @returns the underlying std::span without any type constraints
     */
    underlying_span get() const { return m_span; }

    /**
     * @returns the underlying std::span without any type constraints
     */
    underlying_span get() { return m_span; }

    decltype(auto) data() noexcept(noexcept(this->m_span.data())) { return this->m_span.data(); }

    decltype(auto) data() const noexcept(noexcept(this->m_span.data())) {
        return this->m_span.data();
    }

    decltype(auto) size() const noexcept(noexcept(this->m_span.size())) {
        return this->m_span.size();
    }

    bool empty() const noexcept(noexcept(this->m_span.empty())) { return this->m_span.empty(); }

    decltype(auto) begin() noexcept(noexcept(this->m_span.begin())) { return this->m_span.begin(); }

    decltype(auto) begin() const noexcept(noexcept(this->m_span.begin())) {
        return this->m_span.begin();
    }

    decltype(auto) end() noexcept(noexcept(this->m_span.end())) { return this->m_span.end(); }

    decltype(auto) end() const noexcept(noexcept(this->m_span.end())) { return this->m_span.end(); }

    decltype(auto) operator[](typename underlying_span::size_type i) const noexcept {
        return this->m_span[i];
    }

   private:
    underlying_span m_span;
};

template <typename>
struct is_strong_span : std::false_type {};

template <typename T>
struct is_strong_span<StrongSpan<T>> : std::true_type {};

template <typename T>
constexpr bool is_strong_span_v = is_strong_span<T>::value;

}    // namespace Botan

namespace Botan {

/*
 * Get information describing the version
 */

/**
 * Get a human-readable single-line string identifying the version of Botan.
 * No particular format should be assumed.
 * @return version string
 */
BOTAN_PUBLIC_API(2, 0) std::string version_string();

/**
 * Same as version_string() except returning a pointer to a statically
 * allocated string.
 * @return version string
 */
BOTAN_PUBLIC_API(2, 0) const char* version_cstr();

/**
 * Return a version string of the form "MAJOR.MINOR.PATCH" where
 * each of the values is an integer.
 */
BOTAN_PUBLIC_API(2, 4) std::string short_version_string();

/**
 * Same as version_short_string except returning a pointer to the string.
 */
BOTAN_PUBLIC_API(2, 4) const char* short_version_cstr();

/**
 * Return the date this version of botan was released, in an integer of
 * the form YYYYMMDD. For instance a version released on May 21, 2013
 * would return the integer 20130521. If the currently running version
 * is not an official release, this function will return 0 instead.
 *
 * @return release date, or zero if unreleased
 */
BOTAN_PUBLIC_API(2, 0) uint32_t version_datestamp();

/**
 * Get the major version number.
 * @return major version number
 */
BOTAN_PUBLIC_API(2, 0) uint32_t version_major();

/**
 * Get the minor version number.
 * @return minor version number
 */
BOTAN_PUBLIC_API(2, 0) uint32_t version_minor();

/**
 * Get the patch number.
 * @return patch number
 */
BOTAN_PUBLIC_API(2, 0) uint32_t version_patch();

/**
 * Returns a string that is set to a revision identifier corresponding to the
 * source, or `nullopt` if this could not be determined. It is set for all
 * official releases, and for builds that originated from within a git checkout.
 *
 * @return VC revision
 */
BOTAN_PUBLIC_API(3, 8) std::optional<std::string> version_vc_revision();

/**
 * Return any string that is set at build time using the `--distribution-info`
 * option. It allows a packager of the library to specify any distribution-specific
 * patches. If no value is given at build time, returns `nullopt`.
 *
 * @return distribution info
 */
BOTAN_PUBLIC_API(3, 8) std::optional<std::string> version_distribution_info();

/**
 * Usable for checking that the DLL version loaded at runtime exactly matches the
 * compile-time version. Call using BOTAN_VERSION_* macro values, like so:
 *
 * ```
 * Botan::runtime_version_check(BOTAN_VERSION_MAJOR, BOTAN_VERSION_MINOR, BOTAN_VERSION_PATCH);
 * ```
 *
 * It will return an empty string if the versions match, or otherwise an error
 * message indicating the discrepancy. This only is useful in dynamic libraries,
 * where it is possible to compile and run against different versions.
 */
BOTAN_PUBLIC_API(2, 0)
std::string runtime_version_check(uint32_t major, uint32_t minor, uint32_t patch);

/**
 * Certain build-time options, used for testing, result in a binary which is not
 * safe for use in a production system. This function can be used to test for such
 * a configuration at runtime.
 *
 * Currently these unsafe conditions include:
 *
 * - Unsafe fuzzer mode (--unsafe-fuzzer-mode) which intentionally disables various
 *   checks in order to improve the effectiveness of fuzzing.
 * - Terminate on asserts (--unsafe-terminate-on-asserts) which intentionally aborts
 *   if any internal assertion failure occurs, rather than throwing an exception.
 */
BOTAN_PUBLIC_API(3, 8) bool unsafe_for_production_build();

/*
 * Macros for compile-time version checks
 *
 * Return a value that can be used to compare versions. The current
 * (compile-time) version is available as the macro BOTAN_VERSION_CODE. For
 * instance, to choose one code path for version 3.1.0 and later, and another
 * code path for older releases:
 *
 * ```
 * #if BOTAN_VERSION_CODE >= BOTAN_VERSION_CODE_FOR(3,1,0)
 *    // 3.1+ code path
 * #else
 *    // code path for older versions
 * #endif
 * ```
 */
#define BOTAN_VERSION_CODE_FOR(a, b, c) ((a << 16) | (b << 8) | (c))

/**
 * Compare using BOTAN_VERSION_CODE_FOR, as in
 *  # if BOTAN_VERSION_CODE < BOTAN_VERSION_CODE_FOR(1,8,0)
 *  #    error "Botan version too old"
 *  # endif
 */
#define BOTAN_VERSION_CODE \
    BOTAN_VERSION_CODE_FOR(BOTAN_VERSION_MAJOR, BOTAN_VERSION_MINOR, BOTAN_VERSION_PATCH)

}    // namespace Botan

#endif    // BOTAN_AMALGAMATION_H_
