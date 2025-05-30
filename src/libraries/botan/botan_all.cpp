/*
 * Botan 3.8.1 Amalgamation
 * (C) 1999-2023 The Botan Authors
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include "botan_all.h"

#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <concepts>
#include <ctime>
#include <deque>
#include <functional>
#include <istream>
#include <locale>
#include <map>
#include <memory>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace Botan {

/**
 * AES-128
 */
class AES_128 final : public Block_Cipher_Fixed_Params<16, 16> {
   public:
    void encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const override;
    void decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const override;

    void clear() override;

    std::string provider() const override;

    std::string name() const override { return "AES-128"; }

    std::unique_ptr<BlockCipher> new_object() const override { return std::make_unique<AES_128>(); }

    size_t parallelism() const override;

    bool has_keying_material() const override;

   private:
    void key_schedule(std::span<const uint8_t> key) override;

#if defined(BOTAN_HAS_AES_VPERM)
    void vperm_encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void vperm_decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void vperm_key_schedule(const uint8_t key[], size_t length);
#endif

#if defined(BOTAN_HAS_AES_NI)
    void aesni_key_schedule(const uint8_t key[], size_t length);
#endif

#if defined(BOTAN_HAS_AES_POWER8) || defined(BOTAN_HAS_AES_ARMV8) || defined(BOTAN_HAS_AES_NI)
    void hw_aes_encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void hw_aes_decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
#endif

#if defined(BOTAN_HAS_AES_VAES)
    void x86_vaes_encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void x86_vaes_decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
#endif

    secure_vector<uint32_t> m_EK, m_DK;
};

/**
 * AES-192
 */
class AES_192 final : public Block_Cipher_Fixed_Params<16, 24> {
   public:
    void encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const override;
    void decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const override;

    void clear() override;

    std::string provider() const override;

    std::string name() const override { return "AES-192"; }

    std::unique_ptr<BlockCipher> new_object() const override { return std::make_unique<AES_192>(); }

    size_t parallelism() const override;
    bool has_keying_material() const override;

   private:
#if defined(BOTAN_HAS_AES_VPERM)
    void vperm_encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void vperm_decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void vperm_key_schedule(const uint8_t key[], size_t length);
#endif

#if defined(BOTAN_HAS_AES_NI)
    void aesni_key_schedule(const uint8_t key[], size_t length);
#endif

#if defined(BOTAN_HAS_AES_POWER8) || defined(BOTAN_HAS_AES_ARMV8) || defined(BOTAN_HAS_AES_NI)
    void hw_aes_encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void hw_aes_decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
#endif

#if defined(BOTAN_HAS_AES_VAES)
    void x86_vaes_encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void x86_vaes_decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
#endif

    void key_schedule(std::span<const uint8_t> key) override;

    secure_vector<uint32_t> m_EK, m_DK;
};

/**
 * AES-256
 */
class AES_256 final : public Block_Cipher_Fixed_Params<16, 32> {
   public:
    void encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const override;
    void decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const override;

    void clear() override;

    std::string provider() const override;

    std::string name() const override { return "AES-256"; }

    std::unique_ptr<BlockCipher> new_object() const override { return std::make_unique<AES_256>(); }

    size_t parallelism() const override;
    bool has_keying_material() const override;

   private:
#if defined(BOTAN_HAS_AES_VPERM)
    void vperm_encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void vperm_decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void vperm_key_schedule(const uint8_t key[], size_t length);
#endif

#if defined(BOTAN_HAS_AES_NI)
    void aesni_key_schedule(const uint8_t key[], size_t length);
#endif

#if defined(BOTAN_HAS_AES_POWER8) || defined(BOTAN_HAS_AES_ARMV8) || defined(BOTAN_HAS_AES_NI)
    void hw_aes_encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void hw_aes_decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
#endif

#if defined(BOTAN_HAS_AES_VAES)
    void x86_vaes_encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
    void x86_vaes_decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const;
#endif

    void key_schedule(std::span<const uint8_t> key) override;

    secure_vector<uint32_t> m_EK, m_DK;
};

}    // namespace Botan

namespace Botan {

template <concepts::contiguous_container T = std::vector<uint8_t>>
inline T to_byte_vector(std::string_view s) {
    return T(s.cbegin(), s.cend());
}

inline std::string to_string(std::span<const uint8_t> bytes) {
    return std::string(bytes.begin(), bytes.end());
}

/**
 * Reduce the values of @p keys into an accumulator initialized with @p acc using
 * the reducer function @p reducer.
 *
 * The @p reducer is a function taking the accumulator and a single key to return the
 * new accumulator. Keys are consecutively reduced into the accumulator.
 *
 * @return the accumulator containing the reduction of @p keys
 */
template <typename RetT, typename KeyT, typename ReducerT>
RetT reduce(const std::vector<KeyT>& keys, RetT acc, ReducerT reducer)
    requires std::is_convertible_v<ReducerT, std::function<RetT(RetT, const KeyT&)>>
{
    for (const KeyT& key : keys) {
        acc = reducer(std::move(acc), key);
    }
    return acc;
}

/**
 * Existence check for values
 */
template <typename T, typename OT>
bool value_exists(const std::vector<T>& vec, const OT& val) {
    for (size_t i = 0; i != vec.size(); ++i) {
        if (vec[i] == val) {
            return true;
        }
    }
    return false;
}

template <typename T, typename Pred>
void map_remove_if(Pred pred, T& assoc) {
    auto i = assoc.begin();
    while (i != assoc.end()) {
        if (pred(i->first)) {
            assoc.erase(i++);
        } else {
            i++;
        }
    }
}

/**
 * Helper class to ease unmarshalling of concatenated fixed-length values
 */
class BufferSlicer final {
   public:
    BufferSlicer(std::span<const uint8_t> buffer) : m_remaining(buffer) {}

    template <concepts::contiguous_container ContainerT>
    auto copy(const size_t count) {
        const auto result = take(count);
        return ContainerT(result.begin(), result.end());
    }

    auto copy_as_vector(const size_t count) { return copy<std::vector<uint8_t>>(count); }

    auto copy_as_secure_vector(const size_t count) { return copy<secure_vector<uint8_t>>(count); }

    std::span<const uint8_t> take(const size_t count) {
        BOTAN_STATE_CHECK(remaining() >= count);
        auto result = m_remaining.first(count);
        m_remaining = m_remaining.subspan(count);
        return result;
    }

    template <size_t count>
    std::span<const uint8_t, count> take() {
        BOTAN_STATE_CHECK(remaining() >= count);
        auto result = m_remaining.first<count>();
        m_remaining = m_remaining.subspan(count);
        return result;
    }

    template <concepts::contiguous_strong_type T>
    StrongSpan<const T> take(const size_t count) {
        return StrongSpan<const T>(take(count));
    }

    uint8_t take_byte() { return take(1)[0]; }

    void copy_into(std::span<uint8_t> sink) {
        const auto data = take(sink.size());
        std::copy(data.begin(), data.end(), sink.begin());
    }

    void skip(const size_t count) { take(count); }

    size_t remaining() const { return m_remaining.size(); }

    bool empty() const { return m_remaining.empty(); }

   private:
    std::span<const uint8_t> m_remaining;
};

/**
 * @brief Helper class to ease in-place marshalling of concatenated fixed-length
 *        values.
 *
 * The size of the final buffer must be known from the start, reallocations are
 * not performed.
 */
class BufferStuffer final {
   public:
    constexpr BufferStuffer(std::span<uint8_t> buffer) : m_buffer(buffer) {}

    /**
     * @returns a span for the next @p bytes bytes in the concatenated buffer.
     *          Checks that the buffer is not exceded.
     */
    constexpr std::span<uint8_t> next(size_t bytes) {
        BOTAN_STATE_CHECK(m_buffer.size() >= bytes);

        auto result = m_buffer.first(bytes);
        m_buffer = m_buffer.subspan(bytes);
        return result;
    }

    template <size_t bytes>
    constexpr std::span<uint8_t, bytes> next() {
        BOTAN_STATE_CHECK(m_buffer.size() >= bytes);

        auto result = m_buffer.first<bytes>();
        m_buffer = m_buffer.subspan(bytes);
        return result;
    }

    template <concepts::contiguous_strong_type StrongT>
    StrongSpan<StrongT> next(size_t bytes) {
        return StrongSpan<StrongT>(next(bytes));
    }

    /**
     * @returns a reference to the next single byte in the buffer
     */
    constexpr uint8_t& next_byte() { return next(1)[0]; }

    constexpr void append(std::span<const uint8_t> buffer) {
        auto sink = next(buffer.size());
        std::copy(buffer.begin(), buffer.end(), sink.begin());
    }

    constexpr void append(uint8_t b, size_t repeat = 1) {
        auto sink = next(repeat);
        std::fill(sink.begin(), sink.end(), b);
    }

    constexpr bool full() const { return m_buffer.empty(); }

    constexpr size_t remaining_capacity() const { return m_buffer.size(); }

   private:
    std::span<uint8_t> m_buffer;
};

namespace detail {

/**
 * Helper function that performs range size-checks as required given the
 * selected output and input range types. If all lengths are known at compile
 * time, this check will be performed at compile time as well. It will then
 * instantiate an output range and concatenate the input ranges' contents.
 */
template <ranges::spanable_range OutR, ranges::spanable_range... Rs>
constexpr OutR concatenate(Rs&&... ranges)
    requires(concepts::reservable_container<OutR> || ranges::statically_spanable_range<OutR>)
{
    OutR result;

    // Prepare and validate the output range and construct a lambda that does the
    // actual filling of the result buffer.
    // (if no input ranges are given, GCC claims that fill_fn is unused)
    [[maybe_unused]] auto fill_fn = [&] {
        if constexpr (concepts::reservable_container<OutR>) {
            // dynamically allocate the correct result byte length
            const size_t total_size = (ranges.size() + ... + 0);
            result.reserve(total_size);

            // fill the result buffer using a back-inserter
            return [&result](auto&& range) {
                std::copy(std::ranges::begin(range), std::ranges::end(range),
                          std::back_inserter(unwrap_strong_type(result)));
            };
        } else {
            if constexpr ((ranges::statically_spanable_range<Rs> && ... && true)) {
                // all input ranges have a static extent, so check the total size at compile time
                // (work around an issue in MSVC that warns `total_size` is unused)
                [[maybe_unused]] constexpr size_t total_size =
                    (decltype(std::span{ranges})::extent + ... + 0);
                static_assert(result.size() == total_size,
                              "size of result buffer does not match the sum of input buffers");
            } else {
                // at least one input range has a dynamic extent, so check the total size at runtime
                const size_t total_size = (ranges.size() + ... + 0);
                BOTAN_ARG_CHECK(
                    result.size() == total_size,
                    "result buffer has static extent that does not match the sum of input buffers");
            }

            // fill the result buffer and hold the current output-iterator position
            return [itr = std::ranges::begin(result)](auto&& range) mutable {
                std::copy(std::ranges::begin(range), std::ranges::end(range), itr);
                std::advance(itr, std::ranges::size(range));
            };
        }
    }();

    // perform the actual concatenation
    (fill_fn(std::forward<Rs>(ranges)), ...);

    return result;
}

}    // namespace detail

/**
 * Concatenate an arbitrary number of buffers. Performs range-checks as needed.
 *
 * The output type can be auto-detected based on the input ranges, or explicitly
 * specified by the caller. If all input ranges have a static extent, the total
 * size is calculated at compile time and a statically sized std::array<> is used.
 * Otherwise this tries to use the type of the first input range as output type.
 *
 * Alternatively, the output container type can be specified explicitly.
 */
template <typename OutR = detail::AutoDetect, ranges::spanable_range... Rs>
constexpr auto concat(Rs&&... ranges)
    requires(all_same_v<std::ranges::range_value_t<Rs>...>)
{
    if constexpr (std::same_as<detail::AutoDetect, OutR>) {
        // Try to auto-detect a reasonable output type given the input ranges
        static_assert(
            sizeof...(Rs) > 0,
            "Cannot auto-detect the output type if not a single input range is provided.");
        using candidate_result_t = std::remove_cvref_t<std::tuple_element_t<0, std::tuple<Rs...>>>;
        using result_range_value_t =
            std::remove_cvref_t<std::ranges::range_value_t<candidate_result_t>>;

        if constexpr ((ranges::statically_spanable_range<Rs> && ...)) {
            // If all input ranges have a static extent, we can calculate the total size at compile
            // time and therefore can use a statically sized output container. This is constexpr.
            constexpr size_t total_size = (decltype(std::span{ranges})::extent + ... + 0);
            using out_array_t = std::array<result_range_value_t, total_size>;
            return detail::concatenate<out_array_t>(std::forward<Rs>(ranges)...);
        } else {
            // If at least one input range has a dynamic extent, we must use a dynamically allocated
            // output container. We assume that the user wants to use the first input range's
            // container type as output type.
            static_assert(
                concepts::reservable_container<candidate_result_t>,
                "First input range has static extent, but a dynamically allocated output range is "
                "required. Please explicitly specify a dynamically allocatable output type.");
            return detail::concatenate<candidate_result_t>(std::forward<Rs>(ranges)...);
        }
    } else {
        // The caller has explicitly specified the output type
        return detail::concatenate<OutR>(std::forward<Rs>(ranges)...);
    }
}

template <typename... Alts, typename... Ts>
constexpr bool holds_any_of(const std::variant<Ts...>& v) noexcept {
    return (std::holds_alternative<Alts>(v) || ...);
}

template <typename GeneralVariantT, typename SpecialT>
constexpr bool is_generalizable_to(const SpecialT&) noexcept {
    return std::is_constructible_v<GeneralVariantT, SpecialT>;
}

template <typename GeneralVariantT, typename... SpecialTs>
constexpr bool is_generalizable_to(const std::variant<SpecialTs...>&) noexcept {
    return (std::is_constructible_v<GeneralVariantT, SpecialTs> && ...);
}

/**
 * @brief Converts a given variant into another variant-ish whose type states
 *        are a super set of the given variant.
 *
 * This is useful to convert restricted variant types into more general
 * variants types.
 */
template <typename GeneralVariantT, typename SpecialT>
constexpr GeneralVariantT generalize_to(SpecialT&& specific) noexcept
    requires(std::is_constructible_v<GeneralVariantT, std::decay_t<SpecialT>>)
{
    return std::forward<SpecialT>(specific);
}

/**
 * @brief Converts a given variant into another variant-ish whose type states
 *        are a super set of the given variant.
 *
 * This is useful to convert restricted variant types into more general
 * variants types.
 */
template <typename GeneralVariantT, typename... SpecialTs>
constexpr GeneralVariantT generalize_to(std::variant<SpecialTs...> specific) noexcept {
    static_assert(is_generalizable_to<GeneralVariantT>(specific),
                  "Desired general type must be implicitly constructible by all types of the "
                  "specialized std::variant<>");
    return std::visit([](auto s) -> GeneralVariantT { return s; }, std::move(specific));
}

// This is a helper utility to emulate pattern matching with std::visit.
// See https://en.cppreference.com/w/cpp/utility/variant/visit for more info.
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/**
 * @brief Helper class to create a RAII-style cleanup callback
 *
 * Ensures that the cleanup callback given in the object's constructor is called
 * when the object is destroyed. Use this to ensure some cleanup code runs when
 * leaving the current scope.
 */
template <std::invocable FunT>
class scoped_cleanup final {
   public:
    explicit scoped_cleanup(FunT cleanup) : m_cleanup(std::move(cleanup)) {}

    scoped_cleanup(const scoped_cleanup&) = delete;
    scoped_cleanup& operator=(const scoped_cleanup&) = delete;

    scoped_cleanup(scoped_cleanup&& other) : m_cleanup(std::move(other.m_cleanup)) {
        other.disengage();
    }

    scoped_cleanup& operator=(scoped_cleanup&& other) {
        if (this != &other) {
            m_cleanup = std::move(other.m_cleanup);
            other.disengage();
        }
        return *this;
    }

    ~scoped_cleanup() {
        if (m_cleanup.has_value()) {
            m_cleanup.value()();
        }
    }

    /**
     * Disengage the cleanup callback, i.e., prevent it from being called
     */
    void disengage() { m_cleanup.reset(); }

   private:
    std::optional<FunT> m_cleanup;
};

/**
 * Define BOTAN_ASSERT_IS_SOME
 */
template <typename T>
T assert_is_some(std::optional<T> v, const char* expr, const char* func, const char* file,
                 int line) {
    if (v) {
        return *v;
    } else {
        Botan::assertion_failure(expr, "optional had value", func, file, line);
    }
}

#define BOTAN_ASSERT_IS_SOME(v) assert_is_some(v, #v, __func__, __FILE__, __LINE__)

/*
 * @brief Helper class to pass literal strings to C++ templates
 */
template <size_t N>
class StringLiteral final {
   public:
    constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }

    char value[N];
};

// TODO: C++23: replace with std::to_underlying
template <typename T>
    requires std::is_enum_v<T>
auto to_underlying(T e) noexcept {
    return static_cast<std::underlying_type_t<T>>(e);
}

// TODO: C++23 - use std::out_ptr
template <typename T>
[[nodiscard]] constexpr auto out_ptr(T& outptr) noexcept {
    class out_ptr_t {
       public:
        constexpr ~out_ptr_t() noexcept {
            m_ptr.reset(m_rawptr);
            m_rawptr = nullptr;
        }

        constexpr out_ptr_t(T& outptr) noexcept : m_ptr(outptr), m_rawptr(nullptr) {}

        out_ptr_t(const out_ptr_t&) = delete;
        out_ptr_t(out_ptr_t&&) = delete;
        out_ptr_t& operator=(const out_ptr_t&) = delete;
        out_ptr_t& operator=(out_ptr_t&&) = delete;

        [[nodiscard]] constexpr operator typename T::element_type **() && noexcept {
            return &m_rawptr;
        }

       private:
        T& m_ptr;
        typename T::element_type* m_rawptr;
    };

    return out_ptr_t{outptr};
}

template <typename T>
    requires std::is_default_constructible_v<T>
[[nodiscard]] constexpr auto out_opt(std::optional<T>& outopt) noexcept {
    class out_opt_t {
       public:
        constexpr ~out_opt_t() noexcept { m_opt = m_raw; }

        constexpr out_opt_t(std::optional<T>& outopt) noexcept : m_opt(outopt) {}

        out_opt_t(const out_opt_t&) = delete;
        out_opt_t(out_opt_t&&) = delete;
        out_opt_t& operator=(const out_opt_t&) = delete;
        out_opt_t& operator=(out_opt_t&&) = delete;

        [[nodiscard]] constexpr operator T*() && noexcept { return &m_raw; }

       private:
        std::optional<T>& m_opt;
        T m_raw;
    };

    return out_opt_t{outopt};
}

}    // namespace Botan

namespace Botan {

/**
 * Defines the strategy for handling the final block of input data in the
 * handle_unaligned_data() method of the AlignmentBuffer<>.
 *
 * - is_not_special:   the final block is treated like any other block
 * - must_be_deferred: the final block is not emitted while bulk processing (typically add_data())
 *                     but is deferred until manually consumed (typically final_result())
 *
 * The AlignmentBuffer<> assumes data to be "the final block" if no further
 * input data is available in the BufferSlicer<>. This might result in some
 * performance overhead when using the must_be_deferred strategy.
 */
enum class AlignmentBufferFinalBlock : size_t {
    is_not_special = 0,
    must_be_deferred = 1,
};

/**
 * @brief Alignment buffer helper
 *
 * Many algorithms have an intrinsic block size in which they consume input
 * data. When streaming arbitrary data chunks to such algorithms we must store
 * some data intermittently to honor the algorithm's alignment requirements.
 *
 * This helper encapsulates such an alignment buffer. The API of this class is
 * designed to minimize user errors in the algorithm implementations. Therefore,
 * it is strongly opinionated on its use case. Don't try to use it for anything
 * but the described circumstance.
 *
 * @tparam T                     the element type of the internal buffer
 * @tparam BLOCK_SIZE            the buffer size to use for the alignment buffer
 * @tparam FINAL_BLOCK_STRATEGY  defines whether the final input data block is
 *                               retained in handle_unaligned_data() and must be
 *                               manually consumed
 */
template <typename T, size_t BLOCK_SIZE,
          AlignmentBufferFinalBlock FINAL_BLOCK_STRATEGY =
              AlignmentBufferFinalBlock::is_not_special>
    requires(BLOCK_SIZE > 0)
class AlignmentBuffer {
   public:
    AlignmentBuffer() : m_position(0) {}

    ~AlignmentBuffer() { secure_scrub_memory(m_buffer.data(), m_buffer.size()); }

    AlignmentBuffer(const AlignmentBuffer& other) = default;
    AlignmentBuffer(AlignmentBuffer&& other) noexcept = default;
    AlignmentBuffer& operator=(const AlignmentBuffer& other) = default;
    AlignmentBuffer& operator=(AlignmentBuffer&& other) noexcept = default;

    void clear() {
        clear_mem(m_buffer.data(), m_buffer.size());
        m_position = 0;
    }

    /**
     * Fills the currently unused bytes of the buffer with zero bytes
     */
    void fill_up_with_zeros() {
        if (!ready_to_consume()) {
            clear_mem(&m_buffer[m_position], elements_until_alignment());
            m_position = m_buffer.size();
        }
    }

    /**
     * Appends the provided @p elements to the buffer. The user has to make
     * sure that @p elements fits in the remaining capacity of the buffer.
     */
    void append(std::span<const T> elements) {
        BOTAN_ASSERT_NOMSG(elements.size() <= elements_until_alignment());
        std::copy(elements.begin(), elements.end(), m_buffer.begin() + m_position);
        m_position += elements.size();
    }

    /**
     * Allows direct modification of the first @p elements in the buffer.
     * This is a low-level accessor that neither takes the buffer's current
     * capacity into account nor does it change the internal cursor.
     * Beware not to overwrite unconsumed bytes.
     */
    std::span<T> directly_modify_first(size_t elements) {
        BOTAN_ASSERT_NOMSG(size() >= elements);
        return std::span(m_buffer).first(elements);
    }

    /**
     * Allows direct modification of the last @p elements in the buffer.
     * This is a low-level accessor that neither takes the buffer's current
     * capacity into account nor does it change the internal cursor.
     * Beware not to overwrite unconsumed bytes.
     */
    std::span<T> directly_modify_last(size_t elements) {
        BOTAN_ASSERT_NOMSG(size() >= elements);
        return std::span(m_buffer).last(elements);
    }

    /**
     * Once the buffer reached alignment, this can be used to consume as many
     * input bytes from the given @p slider as possible. The output always
     * contains data elements that are a multiple of the intrinsic block size.
     *
     * @returns a view onto the aligned data from @p slicer and the number of
     *          full blocks that are represented by this view.
     */
    [[nodiscard]] std::tuple<std::span<const uint8_t>, size_t> aligned_data_to_process(
        BufferSlicer& slicer) const {
        BOTAN_ASSERT_NOMSG(in_alignment());

        // When the final block is to be deferred, the last block must not be
        // selected for processing if there is no (unaligned) extra input data.
        const size_t defer = (defers_final_block()) ? 1 : 0;
        const size_t full_blocks_to_process = (slicer.remaining() - defer) / m_buffer.size();
        return {slicer.take(full_blocks_to_process * m_buffer.size()), full_blocks_to_process};
    }

    /**
     * Once the buffer reached alignment, this can be used to consume full
     * blocks from the input data represented by @p slicer.
     *
     * @returns a view onto the next full block from @p slicer or std::nullopt
     *          if not enough data is available in @p slicer.
     */
    [[nodiscard]] std::optional<std::span<const uint8_t>> next_aligned_block_to_process(
        BufferSlicer& slicer) const {
        BOTAN_ASSERT_NOMSG(in_alignment());

        // When the final block is to be deferred, the last block must not be
        // selected for processing if there is no (unaligned) extra input data.
        const size_t defer = (defers_final_block()) ? 1 : 0;
        if (slicer.remaining() < m_buffer.size() + defer) {
            return std::nullopt;
        }

        return slicer.take(m_buffer.size());
    }

    /**
     * Intermittently buffers potentially unaligned data provided in @p
     * slicer. If the internal buffer already contains some elements, data is
     * appended. Once a full block is collected, it is returned to the caller
     * for processing.
     *
     * @param slicer the input data source to be (partially) consumed
     * @returns a view onto a full block once enough data was collected, or
     *          std::nullopt if no full block is available yet
     */
    [[nodiscard]] std::optional<std::span<const T>> handle_unaligned_data(BufferSlicer& slicer) {
        // When the final block is to be deferred, we would need to store and
        // hold a buffer that contains exactly one block until more data is
        // passed or it is explicitly consumed.
        const size_t defer = (defers_final_block()) ? 1 : 0;

        if (in_alignment() && slicer.remaining() >= m_buffer.size() + defer) {
            // We are currently in alignment and the passed-in data source
            // contains enough data to benefit from aligned processing.
            // Therefore, we don't copy anything into the intermittent buffer.
            return std::nullopt;
        }

        // Fill the buffer with as much input data as needed to reach alignment
        // or until the input source is depleted.
        const auto elements_to_consume = std::min(m_buffer.size() - m_position, slicer.remaining());
        append(slicer.take(elements_to_consume));

        // If we collected enough data, we push out one full block. When
        // deferring the final block is enabled, we additionally check that
        // more input data is available to continue processing a consecutive
        // block.
        if (ready_to_consume() && (!defers_final_block() || !slicer.empty())) {
            return consume();
        } else {
            return std::nullopt;
        }
    }

    /**
     * Explicitly consume the currently collected block. It is the caller's
     * responsibility to ensure that the buffer is filled fully. After
     * consumption, the buffer is cleared and ready to collect new data.
     */
    [[nodiscard]] std::span<const T> consume() {
        BOTAN_ASSERT_NOMSG(ready_to_consume());
        m_position = 0;
        return m_buffer;
    }

    /**
     * Explicitly consumes however many bytes are currently stored in the
     * buffer. After consumption, the buffer is cleared and ready to collect
     * new data.
     */
    [[nodiscard]] std::span<const T> consume_partial() {
        const auto elements = elements_in_buffer();
        m_position = 0;
        return std::span(m_buffer).first(elements);
    }

    constexpr size_t size() const { return m_buffer.size(); }

    size_t elements_in_buffer() const { return m_position; }

    size_t elements_until_alignment() const { return m_buffer.size() - m_position; }

    /**
     * @returns true if the buffer is empty (i.e. contains no unaligned data)
     */
    bool in_alignment() const { return m_position == 0; }

    /**
     * @returns true if the buffer is full (i.e. a block is ready to be consumed)
     */
    bool ready_to_consume() const { return m_position == m_buffer.size(); }

    constexpr bool defers_final_block() const {
        return FINAL_BLOCK_STRATEGY == AlignmentBufferFinalBlock::must_be_deferred;
    }

   private:
    std::array<T, BLOCK_SIZE> m_buffer;
    size_t m_position;
};

}    // namespace Botan

namespace Botan {

/**
 * Swap the byte order of an unsigned integer
 */
template <std::unsigned_integral T>
    requires(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8)
inline constexpr T reverse_bytes(T x) {
    if constexpr (sizeof(T) == 1) {
        return x;
    } else if constexpr (sizeof(T) == 2) {
#if BOTAN_COMPILER_HAS_BUILTIN(__builtin_bswap16)
        return static_cast<T>(__builtin_bswap16(x));
#else
        return static_cast<T>((x << 8) | (x >> 8));
#endif
    } else if constexpr (sizeof(T) == 4) {
#if BOTAN_COMPILER_HAS_BUILTIN(__builtin_bswap32)
        return static_cast<T>(__builtin_bswap32(x));
#else
        // MSVC at least recognizes this as a bswap
        return static_cast<T>(((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) |
                              ((x & 0x00FF0000) >> 8) | ((x & 0xFF000000) >> 24));
#endif
    } else if constexpr (sizeof(T) == 8) {
#if BOTAN_COMPILER_HAS_BUILTIN(__builtin_bswap64)
        return static_cast<T>(__builtin_bswap64(x));
#else
        uint32_t hi = static_cast<uint32_t>(x >> 32);
        uint32_t lo = static_cast<uint32_t>(x);

        hi = reverse_bytes(hi);
        lo = reverse_bytes(lo);

        return (static_cast<T>(lo) << 32) | hi;
#endif
    }
}

}    // namespace Botan

namespace Botan {

/**
 * If top bit of arg is set, return ~0. Otherwise return 0.
 */
template <typename T>
inline constexpr T expand_top_bit(T a)
    requires(std::is_integral<T>::value)
{
    return static_cast<T>(0) - (a >> (sizeof(T) * 8 - 1));
}

/**
 * If arg is zero, return ~0. Otherwise return 0
 */
template <typename T>
inline constexpr T ct_is_zero(T x)
    requires(std::is_integral<T>::value)
{
    return expand_top_bit<T>(~x & (x - 1));
}

/**
 * Power of 2 test. T should be an unsigned integer type
 * @param arg an integer value
 * @return true iff arg is 2^n for some n > 0
 */
template <typename T>
inline constexpr bool is_power_of_2(T arg)
    requires(std::is_unsigned<T>::value)
{
    return (arg != 0) && (arg != 1) && ((arg & static_cast<T>(arg - 1)) == 0);
}

/**
 * Return the index of the highest set bit
 * T is an unsigned integer type
 * @param n an integer value
 * @return index of the highest set bit in n
 */
template <typename T>
inline constexpr size_t high_bit(T n)
    requires(std::is_unsigned<T>::value)
{
    size_t hb = 0;

    for (size_t s = 8 * sizeof(T) / 2; s > 0; s /= 2) {
        const size_t z = s * ((~ct_is_zero(n >> s)) & 1);
        hb += z;
        n >>= z;
    }

    hb += n;

    return hb;
}

/**
 * Return the number of significant bytes in n
 * @param n an integer value
 * @return number of significant bytes in n
 */
template <typename T>
inline constexpr size_t significant_bytes(T n)
    requires(std::is_integral<T>::value)
{
    size_t b = 0;

    for (size_t s = 8 * sizeof(n) / 2; s >= 8; s /= 2) {
        const size_t z = s * (~ct_is_zero(n >> s) & 1);
        b += z / 8;
        n >>= z;
    }

    b += (n != 0);

    return b;
}

/**
 * Count the trailing zero bits in n
 * @param n an integer value
 * @return maximum x st 2^x divides n
 */
template <typename T>
inline constexpr size_t ctz(T n)
    requires(std::is_integral<T>::value)
{
    /*
     * If n == 0 then this function will compute 8*sizeof(T)-1, so
     * initialize lb to 1 if n == 0 to produce the expected result.
     */
    size_t lb = ct_is_zero(n) & 1;

    for (size_t s = 8 * sizeof(T) / 2; s > 0; s /= 2) {
        const T mask = (static_cast<T>(1) << s) - 1;
        const size_t z = s * (ct_is_zero(n & mask) & 1);
        lb += z;
        n >>= z;
    }

    return lb;
}

template <typename T>
inline constexpr T floor_log2(T n)
    requires(std::is_unsigned<T>::value)
{
    BOTAN_ARG_CHECK(n != 0, "log2(0) is not defined");
    return static_cast<T>(high_bit(n) - 1);
}

template <typename T>
constexpr uint8_t ceil_log2(T x)
    requires(std::is_integral<T>::value && sizeof(T) < 32)
{
    if (x >> (sizeof(T) * 8 - 1)) {
        return sizeof(T) * 8;
    }

    uint8_t result = 0;
    T compare = 1;

    while (compare < x) {
        compare <<= 1;
        result++;
    }

    return result;
}

/**
 * Ceil of an unsigned integer division. @p b must not be zero.
 *
 * @param a divident
 * @param b divisor
 *
 * @returns ceil(a/b)
 */
template <std::unsigned_integral T>
inline constexpr T ceil_division(T a, T b) {
    return (a + b - 1) / b;
}

/**
 * Return the number of bytes necessary to contain @p bits bits.
 */
template <typename T>
inline constexpr T ceil_tobytes(T bits)
    requires(std::is_integral<T>::value)
{
    return (bits + 7) / 8;
}

// Potentially variable time ctz used for OCB
inline constexpr size_t var_ctz32(uint32_t n) {
#if BOTAN_COMPILER_HAS_BUILTIN(__builtin_ctz)
    if (n == 0) {
        return 32;
    }
    return __builtin_ctz(n);
#else
    return ctz<uint32_t>(n);
#endif
}

template <typename T>
inline constexpr T bit_permute_step(T x, T mask, size_t shift) {
    /*
    See
    https://reflectionsonsecurity.wordpress.com/2014/05/11/efficient-bit-permutation-using-delta-swaps/
    and http://programming.sirrida.de/bit_perm.html
    */
    const T swap = ((x >> shift) ^ x) & mask;
    return (x ^ swap) ^ (swap << shift);
}

template <typename T>
inline constexpr void swap_bits(T& x, T& y, T mask, size_t shift) {
    const T swap = ((x >> shift) ^ y) & mask;
    x ^= swap << shift;
    y ^= swap;
}

template <typename T>
inline constexpr T choose(T mask, T a, T b) {
    // return (mask & a) | (~mask & b);
    return (b ^ (mask & (a ^ b)));
}

template <typename T>
inline constexpr T majority(T a, T b, T c) {
    /*
    Considering each bit of a, b, c individually

    If a xor b is set, then c is the deciding vote.

    If a xor b is not set then either a and b are both set or both unset.
    In either case the value of c doesn't matter, and examining b (or a)
    allows us to determine which case we are in.
    */
    return choose(a ^ b, c, b);
}

/**
 * @returns the reversed bits in @p b.
 */
template <std::unsigned_integral T>
constexpr T ct_reverse_bits(T b) {
    auto extend = [](uint8_t m) -> T {
        T mask = 0;
        for (size_t i = 0; i < sizeof(T); ++i) {
            mask |= T(m) << i * 8;
        }
        return mask;
    };

    // First reverse bits in each byte...
    // From: https://stackoverflow.com/a/2602885
    b = (b & extend(0xF0)) >> 4 | (b & extend(0x0F)) << 4;
    b = (b & extend(0xCC)) >> 2 | (b & extend(0x33)) << 2;
    b = (b & extend(0xAA)) >> 1 | (b & extend(0x55)) << 1;

    // ... then swap the bytes
    return reverse_bytes(b);
}

/**
 * Calculates the number of 1-bits in an unsigned integer in constant-time.
 * This operation is also known as "population count" or hamming weight.
 *
 * Modern compilers will recognize this pattern and replace it by a hardware
 * instruction, if available. This is the SWAR (SIMD within a register)
 * algorithm. See: https://nimrod.blog/posts/algorithms-behind-popcount/#swar-algorithm
 *
 * Note: C++20 provides std::popcount(), but there's no guarantee that this
 *       is implemented in constant-time.
 *
 * @param x an unsigned integer
 * @returns the number of 1-bits in the provided value
 */
template <std::unsigned_integral T>
inline constexpr uint8_t ct_popcount(T x) {
    constexpr size_t s = sizeof(T);
    static_assert(s <= 8, "T is not a suitable unsigned integer value");
    if constexpr (s == 8) {
        x = x - ((x >> 1) & 0x5555555555555555);
        x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
        x = (x + (x >> 4)) & 0xF0F0F0F0F0F0F0F;
        return (x * 0x101010101010101) >> 56;
    } else if constexpr (s == 4) {
        x = x - ((x >> 1) & 0x55555555);
        x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
        x = (x + (x >> 4)) & 0x0F0F0F0F;
        return (x * 0x01010101) >> 24;
    } else {
        // s < 4
        return ct_popcount(static_cast<uint32_t>(x));
    }
}

}    // namespace Botan

namespace Botan {

/**
 * Struct representing a particular date and time
 */
class BOTAN_TEST_API calendar_point {
   public:
    /** The year */
    uint32_t year() const { return m_year; }

    /** The month, 1 through 12 for Jan to Dec */
    uint32_t month() const { return m_month; }

    /** The day of the month, 1 through 31 (or 28 or 30 based on month */
    uint32_t day() const { return m_day; }

    /** Hour in 24-hour form, 0 to 23 */
    uint32_t hour() const { return m_hour; }

    /** Minutes in the hour, 0 to 60 */
    uint32_t minutes() const { return m_minutes; }

    /** Seconds in the minute, 0 to 60, but might be slightly
    larger to deal with leap seconds on some systems
    */
    uint32_t seconds() const { return m_seconds; }

    /**
     * Initialize a calendar_point
     * @param y the year
     * @param mon the month
     * @param d the day
     * @param h the hour
     * @param min the minute
     * @param sec the second
     */
    calendar_point(uint32_t y, uint32_t mon, uint32_t d, uint32_t h, uint32_t min, uint32_t sec)
        : m_year(y), m_month(mon), m_day(d), m_hour(h), m_minutes(min), m_seconds(sec) {}

    /**
     * Convert a time_point to a calendar_point
     * @param time_point a time point from the system clock
     */
    calendar_point(const std::chrono::system_clock::time_point& time_point);

    /**
     * Return seconds since epoch
     */
    uint64_t seconds_since_epoch() const;

    /**
     * Returns an STL timepoint object
     *
     * Note this throws an exception if the time is not representable
     * in the system time_t
     */
    std::chrono::system_clock::time_point to_std_timepoint() const;

    /**
     * Returns a human readable string of the struct's components.
     * Formatting might change over time. Currently it is RFC339 'iso-date-time'.
     */
    std::string to_string() const;

   private:
    uint32_t m_year;
    uint32_t m_month;
    uint32_t m_day;
    uint32_t m_hour;
    uint32_t m_minutes;
    uint32_t m_seconds;
};

}    // namespace Botan

namespace Botan {

/**
 * Block Cipher Mode Padding Method
 * This class is pretty limited, it cannot deal well with
 * randomized padding methods, or any padding method that
 * wants to add more than one block. For instance, it should
 * be possible to define cipher text stealing mode as simply
 * a padding mode for CBC, which happens to consume the last
 * two block (and requires use of the block cipher).
 */
class BOTAN_TEST_API BlockCipherModePaddingMethod {
   public:
    /**
     * Get a block cipher padding mode by name (eg "NoPadding" or "PKCS7")
     * @param algo_spec block cipher padding mode name
     */
    static std::unique_ptr<BlockCipherModePaddingMethod> create(std::string_view algo_spec);

    /**
     * Add padding bytes to buffer.
     * @param buffer data to pad
     * @param final_block_bytes size of the final block in bytes
     * @param block_size size of each block in bytes
     */
    virtual void add_padding(secure_vector<uint8_t>& buffer, size_t final_block_bytes,
                             size_t block_size) const = 0;

    /**
     * Remove padding bytes from block
     * @param block the last block
     * @param len the size of the block in bytes
     * @return number of data bytes, or if the padding is invalid returns len
     */
    virtual size_t unpad(const uint8_t block[], size_t len) const = 0;

    /**
     * @param block_size of the cipher
     * @return valid block size for this padding mode
     */
    virtual bool valid_blocksize(size_t block_size) const = 0;

    /**
     * @return name of the mode
     */
    virtual std::string name() const = 0;

    /**
     * virtual destructor
     */
    virtual ~BlockCipherModePaddingMethod() = default;
};

/**
 * PKCS#7 Padding
 */
class BOTAN_FUZZER_API PKCS7_Padding final : public BlockCipherModePaddingMethod {
   public:
    void add_padding(secure_vector<uint8_t>& buffer, size_t final_block_bytes,
                     size_t block_size) const override;

    size_t unpad(const uint8_t[], size_t) const override;

    bool valid_blocksize(size_t bs) const override { return (bs > 2 && bs < 256); }

    std::string name() const override { return "PKCS7"; }
};

/**
 * ANSI X9.23 Padding
 */
class BOTAN_FUZZER_API ANSI_X923_Padding final : public BlockCipherModePaddingMethod {
   public:
    void add_padding(secure_vector<uint8_t>& buffer, size_t final_block_bytes,
                     size_t block_size) const override;

    size_t unpad(const uint8_t[], size_t) const override;

    bool valid_blocksize(size_t bs) const override { return (bs > 2 && bs < 256); }

    std::string name() const override { return "X9.23"; }
};

/**
 * One And Zeros Padding (ISO/IEC 9797-1, padding method 2)
 */
class BOTAN_FUZZER_API OneAndZeros_Padding final : public BlockCipherModePaddingMethod {
   public:
    void add_padding(secure_vector<uint8_t>& buffer, size_t final_block_bytes,
                     size_t block_size) const override;

    size_t unpad(const uint8_t[], size_t) const override;

    bool valid_blocksize(size_t bs) const override { return (bs > 2); }

    std::string name() const override { return "OneAndZeros"; }
};

/**
 * ESP Padding (RFC 4303)
 */
class BOTAN_FUZZER_API ESP_Padding final : public BlockCipherModePaddingMethod {
   public:
    void add_padding(secure_vector<uint8_t>& buffer, size_t final_block_bytes,
                     size_t block_size) const override;

    size_t unpad(const uint8_t[], size_t) const override;

    bool valid_blocksize(size_t bs) const override { return (bs > 2 && bs < 256); }

    std::string name() const override { return "ESP"; }
};

/**
 * Null Padding
 */
class Null_Padding final : public BlockCipherModePaddingMethod {
   public:
    void add_padding(secure_vector<uint8_t>&, size_t, size_t) const override { /* no padding */ }

    size_t unpad(const uint8_t[], size_t size) const override { return size; }

    bool valid_blocksize(size_t) const override { return true; }

    std::string name() const override { return "NoPadding"; }
};

}    // namespace Botan

namespace Botan {

/**
 * CBC Mode
 */
class CBC_Mode : public Cipher_Mode {
   public:
    std::string name() const final;

    size_t update_granularity() const final;

    size_t ideal_granularity() const final;

    Key_Length_Specification key_spec() const final;

    size_t default_nonce_length() const final;

    bool valid_nonce_length(size_t n) const override;

    void clear() final;

    void reset() override;

    bool has_keying_material() const final;

   protected:
    CBC_Mode(std::unique_ptr<BlockCipher> cipher,
             std::unique_ptr<BlockCipherModePaddingMethod> padding);

    const BlockCipher& cipher() const { return *m_cipher; }

    const BlockCipherModePaddingMethod& padding() const {
        BOTAN_ASSERT_NONNULL(m_padding);
        return *m_padding;
    }

    size_t block_size() const { return m_block_size; }

    secure_vector<uint8_t>& state() { return m_state; }

    uint8_t* state_ptr() { return m_state.data(); }

   private:
    void start_msg(const uint8_t nonce[], size_t nonce_len) override;

    void key_schedule(std::span<const uint8_t> key) override;

    std::unique_ptr<BlockCipher> m_cipher;
    std::unique_ptr<BlockCipherModePaddingMethod> m_padding;
    secure_vector<uint8_t> m_state;
    size_t m_block_size;
};

/**
 * CBC Encryption
 */
class CBC_Encryption : public CBC_Mode {
   public:
    /**
     * @param cipher block cipher to use
     * @param padding padding method to use
     */
    CBC_Encryption(std::unique_ptr<BlockCipher> cipher,
                   std::unique_ptr<BlockCipherModePaddingMethod> padding)
        : CBC_Mode(std::move(cipher), std::move(padding)) {}

    size_t output_length(size_t input_length) const override;

    size_t minimum_final_size() const override;

   private:
    size_t process_msg(uint8_t buf[], size_t size) override;
    void finish_msg(secure_vector<uint8_t>& final_block, size_t offset = 0) override;
};

/**
 * CBC Encryption with ciphertext stealing (CBC-CS3 variant)
 */
class CTS_Encryption final : public CBC_Encryption {
   public:
    /**
     * @param cipher block cipher to use
     */
    explicit CTS_Encryption(std::unique_ptr<BlockCipher> cipher)
        : CBC_Encryption(std::move(cipher), nullptr) {}

    size_t output_length(size_t input_length) const override;

    size_t minimum_final_size() const override;

    bool valid_nonce_length(size_t n) const override;

   private:
    void finish_msg(secure_vector<uint8_t>& final_block, size_t offset = 0) override;
};

/**
 * CBC Decryption
 */
class CBC_Decryption : public CBC_Mode {
   public:
    /**
     * @param cipher block cipher to use
     * @param padding padding method to use
     */
    CBC_Decryption(std::unique_ptr<BlockCipher> cipher,
                   std::unique_ptr<BlockCipherModePaddingMethod> padding)
        : CBC_Mode(std::move(cipher), std::move(padding)), m_tempbuf(ideal_granularity()) {}

    size_t output_length(size_t input_length) const override;

    size_t minimum_final_size() const override;

    void reset() override;

   private:
    size_t process_msg(uint8_t buf[], size_t size) override;
    void finish_msg(secure_vector<uint8_t>& final_block, size_t offset = 0) override;

    secure_vector<uint8_t> m_tempbuf;
};

/**
 * CBC Decryption with ciphertext stealing (CBC-CS3 variant)
 */
class CTS_Decryption final : public CBC_Decryption {
   public:
    /**
     * @param cipher block cipher to use
     */
    explicit CTS_Decryption(std::unique_ptr<BlockCipher> cipher)
        : CBC_Decryption(std::move(cipher), nullptr) {}

    size_t minimum_final_size() const override;

    bool valid_nonce_length(size_t n) const override;

   private:
    void finish_msg(secure_vector<uint8_t>& final_block, size_t offset = 0) override;
};

}    // namespace Botan

namespace Botan {

/**
 * Convert a sequence of UCS-2 (big endian) characters to a UTF-8 string
 * This is used for ASN.1 BMPString type
 * @param ucs2 the sequence of UCS-2 characters
 * @param len length of ucs2 in bytes, must be a multiple of 2
 */
BOTAN_TEST_API std::string ucs2_to_utf8(const uint8_t ucs2[], size_t len);

/**
 * Convert a sequence of UCS-4 (big endian) characters to a UTF-8 string
 * This is used for ASN.1 UniversalString type
 * @param ucs4 the sequence of UCS-4 characters
 * @param len length of ucs4 in bytes, must be a multiple of 4
 */
BOTAN_TEST_API std::string ucs4_to_utf8(const uint8_t ucs4[], size_t len);

BOTAN_TEST_API std::string latin1_to_utf8(const uint8_t latin1[], size_t len);

/**
 * Return a string containing 'c', quoted and possibly escaped
 *
 * This is used when creating an error message nothing an invalid character
 * in some codex (for example during hex decoding)
 *
 * Currently this function escapes tab, newlines and carriage return
 * as "\t", "\n", and "\r", and also escapes characters > 0x7F as
 * "\xHH" where HH is the hex code.
 */
std::string format_char_for_display(char c);

}    // namespace Botan

namespace Botan {

/**
 * CMAC, also known as OMAC1
 */
class CMAC final : public MessageAuthenticationCode {
   public:
    std::string name() const override;

    size_t output_length() const override { return m_block_size; }

    std::unique_ptr<MessageAuthenticationCode> new_object() const override;

    void clear() override;
    bool has_keying_material() const override;

    Key_Length_Specification key_spec() const override { return m_cipher->key_spec(); }

    /**
     * @param cipher the block cipher to use
     */
    explicit CMAC(std::unique_ptr<BlockCipher> cipher);

    CMAC(const CMAC&) = delete;
    CMAC& operator=(const CMAC&) = delete;

   private:
    void add_data(std::span<const uint8_t>) override;
    void final_result(std::span<uint8_t>) override;
    void key_schedule(std::span<const uint8_t>) override;

    std::unique_ptr<BlockCipher> m_cipher;
    secure_vector<uint8_t> m_buffer, m_state, m_B, m_P;
    const size_t m_block_size;
    size_t m_position;
};

}    // namespace Botan

namespace Botan {

/**
* Perform encoding using the base provided
* @param base object giving access to the encodings specifications
* @param output an array of at least base.encode_max_output bytes
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
template <class Base>
size_t base_encode(Base&& base, char output[], const uint8_t input[], size_t input_length,
                   size_t& input_consumed, bool final_inputs) {
    input_consumed = 0;

    // TODO(Botan4) Check if we can use just base. or Base:: here instead
    constexpr size_t encoding_bytes_in = std::remove_reference_t<Base>::encoding_bytes_in();
    constexpr size_t encoding_bytes_out = std::remove_reference_t<Base>::encoding_bytes_out();

    size_t input_remaining = input_length;
    size_t output_produced = 0;

    while (input_remaining >= encoding_bytes_in) {
        base.encode(output + output_produced, input + input_consumed);

        input_consumed += encoding_bytes_in;
        output_produced += encoding_bytes_out;
        input_remaining -= encoding_bytes_in;
    }

    if (final_inputs && input_remaining) {
        std::array<uint8_t, encoding_bytes_in> remainder{};
        for (size_t i = 0; i != input_remaining; ++i) {
            remainder[i] = input[input_consumed + i];
        }

        base.encode(output + output_produced, remainder.data());

        const size_t bits_consumed = base.bits_consumed();
        const size_t remaining_bits_before_padding = base.remaining_bits_before_padding();

        size_t empty_bits = 8 * (encoding_bytes_in - input_remaining);
        size_t index = output_produced + encoding_bytes_out - 1;
        while (empty_bits >= remaining_bits_before_padding) {
            output[index--] = '=';
            empty_bits -= bits_consumed;
        }

        input_consumed += input_remaining;
        output_produced += encoding_bytes_out;
    }

    return output_produced;
}

template <typename Base>
std::string base_encode_to_string(Base&& base, const uint8_t input[], size_t input_length) {
    const size_t output_length = base.encode_max_output(input_length);
    std::string output(output_length, 0);

    size_t consumed = 0;
    size_t produced = 0;

    if (output_length > 0) {
        produced = base_encode(base, &output.front(), input, input_length, consumed, true);
    }

    BOTAN_ASSERT_EQUAL(consumed, input_length, "Consumed the entire input");
    BOTAN_ASSERT_EQUAL(produced, output.size(), "Produced expected size");

    return output;
}

/**
* Perform decoding using the base provided
* @param base object giving access to the encodings specifications
* @param output an array of at least Base::decode_max_output bytes
* @param input some base input
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
template <typename Base>
size_t base_decode(Base&& base, uint8_t output[], const char input[], size_t input_length,
                   size_t& input_consumed, bool final_inputs, bool ignore_ws = true) {
    // TODO(Botan4) Check if we can use just base. or Base:: here instead
    constexpr size_t decoding_bytes_in = std::remove_reference_t<Base>::decoding_bytes_in();
    constexpr size_t decoding_bytes_out = std::remove_reference_t<Base>::decoding_bytes_out();

    uint8_t* out_ptr = output;
    std::array<uint8_t, decoding_bytes_in> decode_buf{};
    size_t decode_buf_pos = 0;
    size_t final_truncate = 0;

    clear_mem(output, base.decode_max_output(input_length));

    for (size_t i = 0; i != input_length; ++i) {
        const uint8_t bin = base.lookup_binary_value(input[i]);

        // This call might throw Invalid_Argument
        if (base.check_bad_char(bin, input[i], ignore_ws)) {
            decode_buf[decode_buf_pos] = bin;
            ++decode_buf_pos;
        }

        /*
         * If we're at the end of the input, pad with 0s and truncate
         */
        if (final_inputs && (i == input_length - 1)) {
            if (decode_buf_pos) {
                for (size_t j = decode_buf_pos; j < decoding_bytes_in; ++j) {
                    decode_buf[j] = 0;
                }

                final_truncate = decoding_bytes_in - decode_buf_pos;
                decode_buf_pos = decoding_bytes_in;
            }
        }

        if (decode_buf_pos == decoding_bytes_in) {
            base.decode(out_ptr, decode_buf.data());

            out_ptr += decoding_bytes_out;
            decode_buf_pos = 0;
            input_consumed = i + 1;
        }
    }

    while (input_consumed < input_length &&
           base.lookup_binary_value(input[input_consumed]) == 0x80) {
        ++input_consumed;
    }

    size_t written = (out_ptr - output) - base.bytes_to_remove(final_truncate);

    return written;
}

template <typename Base>
size_t base_decode_full(Base&& base, uint8_t output[], const char input[], size_t input_length,
                        bool ignore_ws) {
    size_t consumed = 0;
    const size_t written =
        base_decode(base, output, input, input_length, consumed, true, ignore_ws);

    if (consumed != input_length) {
        throw Invalid_Argument(base.name() + " decoding failed, input did not have full bytes");
    }

    return written;
}

template <typename Vector, typename Base>
Vector base_decode_to_vec(Base&& base, const char input[], size_t input_length, bool ignore_ws) {
    const size_t output_length = base.decode_max_output(input_length);
    Vector bin(output_length);

    const size_t written = base_decode_full(base, bin.data(), input, input_length, ignore_ws);

    bin.resize(written);
    return bin;
}

}    // namespace Botan

/**
 * @file  target_info.h
 *
 * Automatically generated from
 * 'configure.py --cc=gcc --cc-bin=g++ --cpu=x86_64 --amalgamation --minimized-build
 * --disable-shared
 * --enable-modules=aes,base,base64,block,cbc,checksum,cmac,filters,hash,hash_id,hex,hmac,hmac_drbg,kdf,kdf2,pbkdf,pbkdf2,sha1
 * --without-documentation --disable-sse2 --disable-ssse3 --disable-sse4.1 --disable-sse4.2
 * --disable-aes-ni --disable-sha-ni --cxxflags=-pipe
 * --prefix=/nix/store/rhwvqgj2ggrhlygdmb8gnza1nci76f8i-botan3-amalgamation-3.8.1'
 *
 * Target
 *  - Compiler: g++ -fstack-protector -m64 -pthread -std=c++20 -D_REENTRANT -pipe
 *  - Arch: x86_64
 *  - OS: linux
 */

/*
 * Configuration
 */
#define BOTAN_CT_VALUE_BARRIER_USE_ASM

[[maybe_unused]] static constexpr bool OptimizeForSize = false;

/*
 * Compiler Information
 */
#define BOTAN_BUILD_COMPILER_IS_GCC

#define BOTAN_COMPILER_INVOCATION_STRING "g++ -fstack-protector -m64 -pthread -pipe"

#define BOTAN_USE_GCC_INLINE_ASM

/*
 * External tool settings
 */

/*
 * CPU feature information
 */
#define BOTAN_TARGET_ARCH "x86_64"

#define BOTAN_TARGET_ARCH_IS_X86_64

#define BOTAN_TARGET_CPU_IS_X86_FAMILY

#define BOTAN_TARGET_CPU_SUPPORTS_AVX2
#define BOTAN_TARGET_CPU_SUPPORTS_AVX512
#define BOTAN_TARGET_CPU_SUPPORTS_BMI2
#define BOTAN_TARGET_CPU_SUPPORTS_GFNI
#define BOTAN_TARGET_CPU_SUPPORTS_RDRAND
#define BOTAN_TARGET_CPU_SUPPORTS_RDSEED
#define BOTAN_TARGET_CPU_SUPPORTS_SHA
#define BOTAN_TARGET_CPU_SUPPORTS_SHA512
#define BOTAN_TARGET_CPU_SUPPORTS_SM3
#define BOTAN_TARGET_CPU_SUPPORTS_SM4
#define BOTAN_TARGET_CPU_SUPPORTS_VAES

/*
 * Operating system information
 */
#define BOTAN_TARGET_OS_IS_LINUX

#define BOTAN_TARGET_OS_HAS_ATOMICS
#define BOTAN_TARGET_OS_HAS_CLOCK_GETTIME
#define BOTAN_TARGET_OS_HAS_DEV_RANDOM
#define BOTAN_TARGET_OS_HAS_EXPLICIT_BZERO
#define BOTAN_TARGET_OS_HAS_GETAUXVAL
#define BOTAN_TARGET_OS_HAS_GETENTROPY
#define BOTAN_TARGET_OS_HAS_GETRANDOM
#define BOTAN_TARGET_OS_HAS_POSIX1
#define BOTAN_TARGET_OS_HAS_POSIX_MLOCK
#define BOTAN_TARGET_OS_HAS_PRCTL
#define BOTAN_TARGET_OS_HAS_SOCKETS
#define BOTAN_TARGET_OS_HAS_SYSTEM_CLOCK
#define BOTAN_TARGET_OS_HAS_THREAD_LOCAL

/*
 * System paths
 */
#define BOTAN_INSTALL_PREFIX \
    R"(/nix/store/rhwvqgj2ggrhlygdmb8gnza1nci76f8i-botan3-amalgamation-3.8.1)"
#define BOTAN_INSTALL_HEADER_DIR R"(include/botan-3)"
#define BOTAN_INSTALL_LIB_DIR \
    R"(/nix/store/rhwvqgj2ggrhlygdmb8gnza1nci76f8i-botan3-amalgamation-3.8.1/lib)"
#define BOTAN_LIB_LINK ""
#define BOTAN_LINK_FLAGS "-fstack-protector -m64 -pthread"

#if defined(BOTAN_HAS_VALGRIND)
#include <valgrind/memcheck.h>
#endif

namespace Botan::CT {

/// @name Constant Time Check Annotation Helpers
/// @{

/**
 * Use valgrind to mark the contents of memory as being undefined.
 * Valgrind will accept operations which manipulate undefined values,
 * but will warn if an undefined value is used to decided a conditional
 * jump or a load/store address. So if we poison all of our inputs we
 * can confirm that the operations in question are truly const time
 * when compiled by whatever compiler is in use.
 *
 * Even better, the VALGRIND_MAKE_MEM_* macros work even when the
 * program is not run under valgrind (though with a few cycles of
 * overhead, which is unfortunate in final binaries as these
 * annotations tend to be used in fairly important loops).
 *
 * This approach was first used in ctgrind (https://github.com/agl/ctgrind)
 * but calling the valgrind mecheck API directly works just as well and
 * doesn't require a custom patched valgrind.
 */
template <typename T>
constexpr inline void poison(const T* p, size_t n) {
#if defined(BOTAN_HAS_VALGRIND)
    if (!std::is_constant_evaluated()) {
        VALGRIND_MAKE_MEM_UNDEFINED(p, n * sizeof(T));
    }
#endif

    BOTAN_UNUSED(p, n);
}

template <typename T>
constexpr inline void unpoison(const T* p, size_t n) {
#if defined(BOTAN_HAS_VALGRIND)
    if (!std::is_constant_evaluated()) {
        VALGRIND_MAKE_MEM_DEFINED(p, n * sizeof(T));
    }
#endif

    BOTAN_UNUSED(p, n);
}

/**
 * Checks whether CT::poison() and CT::unpoison() actually have an effect.
 *
 * If the build is not instrumented and/or not run using an analysis tool like
 * valgrind, the functions are no-ops and the return value is false.
 *
 * @returns true if CT::poison() and CT::unpoison() are effective
 */
inline bool poison_has_effect() {
#if defined(BOTAN_HAS_VALGRIND)
    return RUNNING_ON_VALGRIND;
#else
    return false;
#endif
}

/// @}

/// @name Constant Time Check Annotation Convenience overloads
/// @{

/**
 * Poison a single integral object
 */
template <std::integral T>
constexpr void poison(T& p) {
    poison(&p, 1);
}

template <std::integral T>
constexpr void unpoison(T& p) {
    unpoison(&p, 1);
}

/**
 * Poison a contiguous buffer of trivial objects (e.g. integers and such)
 */
template <ranges::spanable_range R>
    requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
constexpr void poison(R&& r) {
    std::span s{r};
    poison(s.data(), s.size());
}

template <ranges::spanable_range R>
    requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
constexpr void unpoison(R&& r) {
    std::span s{r};
    unpoison(s.data(), s.size());
}

/**
 * Poison a class type that provides a public `_const_time_poison()` method
 * For instance: BigInt, CT::Mask<>, FrodoMatrix, ...
 */
template <typename T>
    requires requires(const T& x) { x._const_time_poison(); }
constexpr void poison(const T& x) {
    x._const_time_poison();
}

template <typename T>
    requires requires(const T& x) { x._const_time_unpoison(); }
constexpr void unpoison(const T& x) {
    x._const_time_unpoison();
}

/**
 * Poison an optional object if it has a value.
 */
template <typename T>
    requires requires(const T& v) { ::Botan::CT::poison(v); }
constexpr void poison(const std::optional<T>& x) {
    if (x.has_value()) {
        poison(x.value());
    }
}

template <typename T>
    requires requires(const T& v) { ::Botan::CT::unpoison(v); }
constexpr void unpoison(const std::optional<T>& x) {
    if (x.has_value()) {
        unpoison(x.value());
    }
}

/// @}

/// @name Higher-level Constant Time Check Annotation Helpers
/// @{

template <typename T>
concept poisonable = requires(const T& v) { ::Botan::CT::poison(v); };
template <typename T>
concept unpoisonable = requires(const T& v) { ::Botan::CT::unpoison(v); };

/**
 * Poison a range of objects by calling `poison` on each element.
 */
template <std::ranges::range R>
    requires poisonable<std::ranges::range_value_t<R>>
constexpr void poison_range(R&& r) {
    for (const auto& v : r) {
        poison(v);
    }
}

template <std::ranges::range R>
    requires unpoisonable<std::ranges::range_value_t<R>>
constexpr void unpoison_range(R&& r) {
    for (const auto& v : r) {
        unpoison(v);
    }
}

/**
 * Poisons an arbitrary number of values in a single call.
 * Mostly syntactic sugar to save clutter (i.e. lines-of-code).
 */
template <poisonable... Ts>
    requires(sizeof...(Ts) > 0)
constexpr void poison_all(Ts&&... ts) {
    (poison(ts), ...);
}

template <unpoisonable... Ts>
    requires(sizeof...(Ts) > 0)
constexpr void unpoison_all(Ts&&... ts) {
    (unpoison(ts), ...);
}

/**
 * Poisons an arbitrary number of poisonable values, and unpoisons them when the
 * returned object runs out-of-scope
 *
 * Use this when you want to poison a value that remains valid longer than the
 * scope you are currently in. For instance, a private key structure that is a
 * member of a Signature_Operation object, that may be used for multiple
 * signatures.
 */
template <typename... Ts>
    requires(sizeof...(Ts) > 0) && (poisonable<Ts> && ...) && (unpoisonable<Ts> && ...)
[[nodiscard]] constexpr auto scoped_poison(const Ts&... xs) {
    auto scope = scoped_cleanup([&] { unpoison_all(xs...); });
    poison_all(xs...);
    return scope;
}

/**
 * Poisons an r-value @p v and forwards it as the return value.
 */
template <poisonable T>
[[nodiscard]] decltype(auto) driveby_poison(T&& v)
    requires(std::is_rvalue_reference_v<decltype(v)>)
{
    poison(v);
    return std::forward<T>(v);
}

/**
 * Unpoisons an r-value @p v and forwards it as the return value.
 */
template <unpoisonable T>
[[nodiscard]] decltype(auto) driveby_unpoison(T&& v)
    requires(std::is_rvalue_reference_v<decltype(v)>)
{
    unpoison(v);
    return std::forward<T>(v);
}

/// @}

/**
 * This function returns its argument, but (if called in a non-constexpr context)
 * attempts to prevent the compiler from reasoning about the value or the possible
 * range of values. Such optimizations have a way of breaking constant time code.
 *
 * The method that is use is decided at configuration time based on the target
 * compiler and architecture (see `ct_value_barrier` blocks in `src/build-data/cc`).
 * The decision can be overridden by the user with the configure.py option
 * `--ct-value-barrier-type=`
 *
 * There are three options currently possible in the data files and with the
 * option:
 *
 *  * `asm`: Use an inline assembly expression which (currently) prevents Clang
 *    and GCC from optimizing based on the possible value of the input expression.
 *
 *  * `volatile`: Launder the input through a volatile variable. This is likely
 *    to cause significant performance regressions since the value must be
 *    actually stored and loaded back from memory each time.
 *
 *  * `none`: disable constant time barriers entirely. This is used
 *    with MSVC, which is not known to perform optimizations that break
 *    constant time code and which does not support GCC-style inline asm.
 *
 */
template <typename T>
constexpr inline T value_barrier(T x)
    requires std::unsigned_integral<T> && (!std::same_as<bool, T>)
{
    if (std::is_constant_evaluated()) {
        return x;
    } else {
#if defined(BOTAN_CT_VALUE_BARRIER_USE_ASM)
        /*
         * We may want a "stronger" statement such as
         *     asm volatile("" : "+r,m"(x) : : "memory);
         * (see https://theunixzoo.co.uk/blog/2021-10-14-preventing-optimisations.html)
         * however the current approach seems sufficient with current compilers,
         * and is minimally damaging with regards to degrading code generation.
         */
        asm("" : "+r"(x) : /* no input */);
        return x;
#elif defined(BOTAN_CT_VALUE_BARRIER_USE_VOLATILE)
        volatile T vx = x;
        return vx;
#else
        return x;
#endif
    }
}

/**
 * A Choice is used for constant-time conditionals.
 *
 * Internally it always is either |0| (all 0 bits) or |1| (all 1 bits)
 * and measures are taken to block compilers from reasoning about the
 * expected value of a Choice.
 */
class Choice final {
   public:
    /**
     * If v == 0 return an unset (false) Choice, otherwise a set Choice
     */
    template <typename T>
        requires std::unsigned_integral<T> && (!std::same_as<bool, T>)
    constexpr static Choice from_int(T v) {
        // Mask of T that is either |0| or |1|
        const T v_is_0 = ct_is_zero<T>(value_barrier<T>(v));

        // We want the mask to be set if v != 0 so we must check that
        // v_is_0 is itself zero.
        //
        // Also sizeof(T) may not equal sizeof(uint32_t) so we must
        // use ct_is_zero<uint32_t>. It's ok to either truncate or
        // zero extend v_is_0 to 32 bits since we know it is |0| or |1|
        // so even just the low bit is sufficient.
        return Choice(ct_is_zero<uint32_t>(static_cast<uint32_t>(v_is_0)));
    }

    constexpr static Choice yes() { return Choice(static_cast<uint32_t>(-1)); }

    constexpr static Choice no() { return Choice(0); }

    constexpr Choice operator!() const { return Choice(~value()); }

    constexpr Choice operator&&(const Choice& other) const {
        return Choice(value() & other.value());
    }

    constexpr Choice operator||(const Choice& other) const {
        return Choice(value() | other.value());
    }

    constexpr Choice operator!=(const Choice& other) const {
        return Choice(value() ^ other.value());
    }

    constexpr Choice operator==(const Choice& other) const { return !(*this != other); }

    /**
     * Unsafe conversion to bool
     *
     * This conversion itself is (probably) constant time, but once the
     * choice is reduced to a simple bool, it's entirely possible for the
     * compiler to perform range analysis on the values, since there are just
     * the two. As a consequence even if the caller is not using this in an
     * obviously branchy way (`if(choice.as_bool()) ...`) a smart compiler
     * may introduce branches depending on the value.
     */
    constexpr bool as_bool() const { return m_value != 0; }

    /// Return the masked value
    constexpr uint32_t value() const { return value_barrier(m_value); }

    constexpr Choice(const Choice& other) = default;
    constexpr Choice(Choice&& other) = default;
    constexpr Choice& operator=(const Choice& other) noexcept = default;
    constexpr Choice& operator=(Choice&& other) noexcept = default;

   private:
    constexpr explicit Choice(uint32_t v) : m_value(v) {}

    uint32_t m_value;
};

/**
 * A concept for a type which is conditionally assignable
 */
template <typename T>
concept ct_conditional_assignable =
    requires(T lhs, const T& rhs, Choice c) { lhs.conditional_assign(c, rhs); };

/**
 * A Mask type used for constant-time operations. A Mask<T> always has value
 * either |0| (all bits cleared) or |1| (all bits set). All operations in a Mask<T>
 * are intended to compile to code which does not contain conditional jumps.
 * This must be verified with tooling (eg binary disassembly or using valgrind)
 * since you never know what a compiler might do.
 */
template <typename T>
class Mask final {
   public:
    static_assert(std::is_unsigned<T>::value && !std::is_same<bool, T>::value,
                  "Only unsigned integer types are supported by CT::Mask");

    Mask(const Mask<T>& other) = default;
    Mask<T>& operator=(const Mask<T>& other) = default;

    /**
     * Derive a Mask from a Mask of a larger type
     */
    template <typename U>
    constexpr Mask(Mask<U> o) : m_mask(static_cast<T>(o.value())) {
        static_assert(sizeof(U) > sizeof(T), "sizes ok");
    }

    /**
     * Return a Mask<T> of |1| (all bits set)
     */
    static constexpr Mask<T> set() { return Mask<T>(static_cast<T>(~0)); }

    /**
     * Return a Mask<T> of |0| (all bits cleared)
     */
    static constexpr Mask<T> cleared() { return Mask<T>(0); }

    /**
     * Return a Mask<T> which is set if v is != 0
     */
    static constexpr Mask<T> expand(T v) { return ~Mask<T>::is_zero(value_barrier<T>(v)); }

    /**
     * Return a Mask<T> which is set if choice is set
     */
    static constexpr Mask<T> from_choice(Choice c) {
        if constexpr (sizeof(T) <= sizeof(uint32_t)) {
            // Take advantage of the fact that Choice's mask is always
            // either |0| or |1|
            return Mask<T>(static_cast<T>(c.value()));
        } else {
            return ~Mask<T>::is_zero(c.value());
        }
    }

    /**
     * Return a Mask<T> which is set if the top bit of v is set
     */
    static constexpr Mask<T> expand_top_bit(T v) {
        return Mask<T>(Botan::expand_top_bit<T>(value_barrier<T>(v)));
    }

    /**
     * Return a Mask<T> which is set if the given @p bit of @p v is set.
     * @p bit must be from 0 (LSB) to (sizeof(T) * 8 - 1) (MSB).
     */
    static constexpr Mask<T> expand_bit(T v, size_t bit) {
        return CT::Mask<T>::expand_top_bit(v << (sizeof(v) * 8 - 1 - bit));
    }

    /**
     * Return a Mask<T> which is set if m is set
     */
    template <typename U>
    static constexpr Mask<T> expand(Mask<U> m) {
        static_assert(sizeof(U) < sizeof(T), "sizes ok");
        return ~Mask<T>::is_zero(m.value());
    }

    /**
     * Return a Mask<T> which is set if v is == 0 or cleared otherwise
     */
    static constexpr Mask<T> is_zero(T x) { return Mask<T>(ct_is_zero<T>(value_barrier<T>(x))); }

    /**
     * Return a Mask<T> which is set if x == y
     */
    static constexpr Mask<T> is_equal(T x, T y) {
        const T diff = value_barrier(x) ^ value_barrier(y);
        return Mask<T>::is_zero(diff);
    }

    /**
     * Return a Mask<T> which is set if x < y
     */
    static constexpr Mask<T> is_lt(T x, T y) {
        T u = x ^ ((x ^ y) | ((x - y) ^ x));
        return Mask<T>::expand_top_bit(u);
    }

    /**
     * Return a Mask<T> which is set if x > y
     */
    static constexpr Mask<T> is_gt(T x, T y) { return Mask<T>::is_lt(y, x); }

    /**
     * Return a Mask<T> which is set if x <= y
     */
    static constexpr Mask<T> is_lte(T x, T y) { return ~Mask<T>::is_gt(x, y); }

    /**
     * Return a Mask<T> which is set if x >= y
     */
    static constexpr Mask<T> is_gte(T x, T y) { return ~Mask<T>::is_lt(x, y); }

    static constexpr Mask<T> is_within_range(T v, T l, T u) {
        // return Mask<T>::is_gte(v, l) & Mask<T>::is_lte(v, u);

        const T v_lt_l = v ^ ((v ^ l) | ((v - l) ^ v));
        const T v_gt_u = u ^ ((u ^ v) | ((u - v) ^ u));
        const T either = value_barrier(v_lt_l) | value_barrier(v_gt_u);
        return ~Mask<T>::expand_top_bit(either);
    }

    static constexpr Mask<T> is_any_of(T v, std::initializer_list<T> accepted) {
        T accept = 0;

        for (auto a : accepted) {
            const T diff = a ^ v;
            const T eq_zero = value_barrier<T>(~diff & (diff - 1));
            accept |= eq_zero;
        }

        return Mask<T>::expand_top_bit(accept);
    }

    /**
     * AND-combine two masks
     */
    Mask<T>& operator&=(Mask<T> o) {
        m_mask &= o.value();
        return (*this);
    }

    /**
     * XOR-combine two masks
     */
    Mask<T>& operator^=(Mask<T> o) {
        m_mask ^= o.value();
        return (*this);
    }

    /**
     * OR-combine two masks
     */
    Mask<T>& operator|=(Mask<T> o) {
        m_mask |= o.value();
        return (*this);
    }

    /**
     * AND-combine two masks
     */
    friend Mask<T> operator&(Mask<T> x, Mask<T> y) { return Mask<T>(x.value() & y.value()); }

    /**
     * XOR-combine two masks
     */
    friend Mask<T> operator^(Mask<T> x, Mask<T> y) { return Mask<T>(x.value() ^ y.value()); }

    /**
     * OR-combine two masks
     */
    friend Mask<T> operator|(Mask<T> x, Mask<T> y) { return Mask<T>(x.value() | y.value()); }

    /**
     * Negate this mask
     */
    constexpr Mask<T> operator~() const { return Mask<T>(~value()); }

    /**
     * Return x if the mask is set, or otherwise zero
     */
    constexpr T if_set_return(T x) const { return value() & x; }

    /**
     * Return x if the mask is cleared, or otherwise zero
     */
    constexpr T if_not_set_return(T x) const { return ~value() & x; }

    /**
     * If this mask is set, return x, otherwise return y
     */
    constexpr T select(T x, T y) const { return choose(value(), x, y); }

    constexpr T select_and_unpoison(T x, T y) const {
        T r = this->select(x, y);
        CT::unpoison(r);
        return r;
    }

    /**
     * If this mask is set, return x, otherwise return y
     */
    Mask<T> select_mask(Mask<T> x, Mask<T> y) const {
        return Mask<T>(select(x.value(), y.value()));
    }

    /**
     * Conditionally set output to x or y, depending on if mask is set or
     * cleared (resp)
     */
    constexpr void select_n(T output[], const T x[], const T y[], size_t len) const {
        const T mask = value();
        for (size_t i = 0; i != len; ++i) {
            output[i] = choose(mask, x[i], y[i]);
        }
    }

    /**
     * If this mask is set, zero out buf, otherwise do nothing
     */
    constexpr void if_set_zero_out(T buf[], size_t elems) {
        for (size_t i = 0; i != elems; ++i) {
            buf[i] = this->if_not_set_return(buf[i]);
        }
    }

    /**
     * If this mask is set, swap x and y
     */
    template <typename U>
    void conditional_swap(U& x, U& y) const
        requires(sizeof(U) <= sizeof(T))
    {
        auto cnd = Mask<U>(*this);
        U t0 = cnd.select(y, x);
        U t1 = cnd.select(x, y);
        x = t0;
        y = t1;
    }

    /**
     * Return the value of the mask, unpoisoned
     */
    constexpr T unpoisoned_value() const {
        T r = value();
        CT::unpoison(r);
        return r;
    }

    /**
     * Unsafe conversion to bool
     *
     * This conversion itself is (probably) constant time, but once the
     * mask is reduced to a simple bool, it's entirely possible for the
     * compiler to perform range analysis on the values, since there are just
     * the two. As a consequence even if the caller is not using this in an
     * obviously branchy way (`if(mask.as_bool()) ...`) a smart compiler
     * may introduce branches depending on the value.
     */
    constexpr bool as_bool() const { return unpoisoned_value() != 0; }

    /**
     * Return a Choice based on this mask
     */
    constexpr CT::Choice as_choice() const { return CT::Choice::from_int(unpoisoned_value()); }

    /**
     * Return the underlying value of the mask
     */
    constexpr T value() const { return value_barrier<T>(m_mask); }

    constexpr void _const_time_poison() const { CT::poison(m_mask); }

    constexpr void _const_time_unpoison() const { CT::unpoison(m_mask); }

   private:
    constexpr Mask(T m) : m_mask(m) {}

    T m_mask;
};

/**
 * A CT::Option<T> is either a valid T, or not
 *
 * To maintain constant time behavior a value must always be stored.
 * A CT::Choice tracks if the value is valid or not. It is not possible
 * to access the inner value if the Choice is unset.
 */
template <typename T>
class Option final {
   public:
    /// Construct an Option which contains the specified value, and is set or not
    constexpr Option(T v, Choice valid) : m_has_value(valid), m_value(std::move(v)) {}

    /// Construct a set option with the provided value
    constexpr Option(T v) : Option(std::move(v), Choice::yes()) {}

    /// Construct an unset option with a default inner value
    constexpr Option()
        requires std::default_initializable<T>
        : Option(T(), Choice::no()) {}

    /// Return true if this Option contains a value
    constexpr Choice has_value() const { return m_has_value; }

    /**
     * Apply a function to the inner value and return a new Option
     * which contains that value. This is constant time only if @p f is.
     *
     * @note The function will always be called, even if the Option is None. It
     *       must be prepared to handle any possible state of T.
     */
    template <std::invocable<const T&> F>
    constexpr auto transform(F f) const
        -> Option<std::remove_cvref_t<std::invoke_result_t<F, const T&>>> {
        return {f(m_value), m_has_value};
    }

    /// Either returns the value or throws an exception
    constexpr const T& value() const {
        BOTAN_STATE_CHECK(m_has_value.as_bool());
        return m_value;
    }

    /// Returns either the inner value or the alternative, in constant time
    ///
    /// This variant is used for types which explicitly define a function
    /// conditional_assign which takes a CT::Choice as the conditional.
    constexpr T value_or(T other) const
        requires ct_conditional_assignable<T>
    {
        other.conditional_assign(m_has_value, m_value);
        return other;
    }

    /// Returns either the inner value or the alternative, in constant time
    ///
    /// This variant is used for integer types where CT::Mask can perform
    /// a constant time selection
    constexpr T value_or(T other) const
        requires std::unsigned_integral<T>
    {
        auto mask = CT::Mask<T>::from_choice(m_has_value);
        return mask.select(m_value, other);
    }

    /// Convert this Option into a std::optional
    ///
    /// This is not constant time, leaking if the Option had a
    /// value or not
    constexpr std::optional<T> as_optional_vartime() const {
        if (m_has_value.as_bool()) {
            return {m_value};
        } else {
            return {};
        }
    }

    /// Return a new CT::Option that is set if @p also is set as well
    constexpr CT::Option<T> operator&&(CT::Choice also) {
        return CT::Option<T>(m_value, m_has_value && also);
    }

   private:
    Choice m_has_value;
    T m_value;
};

template <typename T>
constexpr inline Mask<T> conditional_copy_mem(Mask<T> mask, T* to, const T* from0, const T* from1,
                                              size_t elems) {
    mask.select_n(to, from0, from1, elems);
    return mask;
}

template <typename T>
constexpr inline Mask<T> conditional_copy_mem(T cnd, T* to, const T* from0, const T* from1,
                                              size_t elems) {
    const auto mask = CT::Mask<T>::expand(cnd);
    return CT::conditional_copy_mem(mask, to, from0, from1, elems);
}

template <typename T>
constexpr inline Mask<T> conditional_assign_mem(T cnd, T* sink, const T* src, size_t elems) {
    const auto mask = CT::Mask<T>::expand(cnd);
    mask.select_n(sink, src, sink, elems);
    return mask;
}

template <typename T>
constexpr inline Mask<T> conditional_assign_mem(Choice cnd, T* sink, const T* src, size_t elems) {
    const auto mask = CT::Mask<T>::from_choice(cnd);
    mask.select_n(sink, src, sink, elems);
    return mask;
}

template <typename T>
constexpr inline void conditional_swap(bool cnd, T& x, T& y) {
    const auto swap = CT::Mask<T>::expand(cnd);
    swap.conditional_swap(x, y);
}

template <typename T>
constexpr inline void conditional_swap_ptr(bool cnd, T& x, T& y) {
    uintptr_t xp = reinterpret_cast<uintptr_t>(x);
    uintptr_t yp = reinterpret_cast<uintptr_t>(y);

    conditional_swap<uintptr_t>(cnd, xp, yp);

    x = reinterpret_cast<T>(xp);
    y = reinterpret_cast<T>(yp);
}

template <typename T>
constexpr inline CT::Mask<T> all_zeros(const T elem[], size_t len) {
    T sum = 0;
    for (size_t i = 0; i != len; ++i) {
        sum |= elem[i];
    }
    return CT::Mask<T>::is_zero(sum);
}

/**
 * Compare two arrays of equal size and return a Mask indicating if
 * they are equal or not. The mask is set if they are identical.
 */
template <typename T>
constexpr inline CT::Mask<T> is_equal(const T x[], const T y[], size_t len) {
    if (std::is_constant_evaluated()) {
        T difference = 0;

        for (size_t i = 0; i != len; ++i) {
            difference = difference | (x[i] ^ y[i]);
        }

        return CT::Mask<T>::is_zero(difference);
    } else {
        volatile T difference = 0;

        for (size_t i = 0; i != len; ++i) {
            difference = difference | (x[i] ^ y[i]);
        }

        return CT::Mask<T>::is_zero(difference);
    }
}

/**
 * Compare two arrays of equal size and return a Mask indicating if
 * they are equal or not. The mask is set if they differ.
 */
template <typename T>
constexpr inline CT::Mask<T> is_not_equal(const T x[], const T y[], size_t len) {
    return ~CT::is_equal(x, y, len);
}

/**
 * Constant time conditional copy out with offset
 *
 * If accept is set and offset <= input_length, sets output[0..] to
 * input[offset:input_length] and returns input_length - offset. The
 * remaining bytes of output are zeroized.
 *
 * Otherwise, output is zeroized, and returns an empty Ct::Option
 *
 * The input and output spans may not overlap, and output must be at
 * least as large as input.
 *
 * This function attempts to avoid leaking the following to side channels
 *  - if accept was set or not
 *  - the value of offset
 *  - the value of input
 *
 * This function leaks the length of the input
 */
BOTAN_TEST_API
CT::Option<size_t> copy_output(CT::Choice accept, std::span<uint8_t> output,
                               std::span<const uint8_t> input, size_t offset);

size_t count_leading_zero_bytes(std::span<const uint8_t> input);

secure_vector<uint8_t> strip_leading_zeros(std::span<const uint8_t> input);

}    // namespace Botan::CT

#if defined(BOTAN_BUILD_COMPILER_IS_MSVC)
#include <intrin.h>
#endif

namespace Botan {

/**
 * Perform a 64x64->128 bit multiplication
 */
constexpr inline void mul64x64_128(uint64_t a, uint64_t b, uint64_t* lo, uint64_t* hi) {
    if (!std::is_constant_evaluated()) {
#if defined(BOTAN_BUILD_COMPILER_IS_MSVC) && defined(BOTAN_TARGET_ARCH_IS_X86_64)
        *lo = _umul128(a, b, hi);
        return;

#elif defined(BOTAN_BUILD_COMPILER_IS_MSVC) && defined(BOTAN_TARGET_ARCH_IS_ARM64)
        *lo = a * b;
        *hi = __umulh(a, b);
        return;
#endif
    }

#if defined(BOTAN_TARGET_HAS_NATIVE_UINT128)
    const uint128_t r = static_cast<uint128_t>(a) * b;
    *hi = (r >> 64) & 0xFFFFFFFFFFFFFFFF;
    *lo = (r) & 0xFFFFFFFFFFFFFFFF;
#else

    /*
     * Do a 64x64->128 multiply using four 32x32->64 multiplies plus
     * some adds and shifts.
     */
    const size_t HWORD_BITS = 32;
    const uint32_t HWORD_MASK = 0xFFFFFFFF;

    const uint32_t a_hi = (a >> HWORD_BITS);
    const uint32_t a_lo = (a & HWORD_MASK);
    const uint32_t b_hi = (b >> HWORD_BITS);
    const uint32_t b_lo = (b & HWORD_MASK);

    const uint64_t x0 = static_cast<uint64_t>(a_hi) * b_hi;
    const uint64_t x1 = static_cast<uint64_t>(a_lo) * b_hi;
    const uint64_t x2 = static_cast<uint64_t>(a_hi) * b_lo;
    const uint64_t x3 = static_cast<uint64_t>(a_lo) * b_lo;

    // this cannot overflow as (2^32-1)^2 + 2^32-1 + 2^32-1 = 2^64-1
    const uint64_t middle = x2 + (x3 >> HWORD_BITS) + (x1 & HWORD_MASK);

    // likewise these cannot overflow
    *hi = x0 + (middle >> HWORD_BITS) + (x1 >> HWORD_BITS);
    *lo = (middle << HWORD_BITS) + (x3 & HWORD_MASK);
#endif
}

}    // namespace Botan

namespace Botan {

class donna128 final {
   public:
    constexpr donna128(uint64_t ll = 0, uint64_t hh = 0) {
        l = ll;
        h = hh;
    }

    donna128(const donna128&) = default;
    donna128& operator=(const donna128&) = default;

    template <typename T>
    constexpr friend donna128 operator>>(const donna128& x, T shift) {
        donna128 z = x;

        if (shift > 64) {
            z.l = z.h >> (shift - 64);
            z.h = 0;
        } else if (shift == 64) {
            z.l = z.h;
            z.h = 0;
        } else if (shift > 0) {
            const uint64_t carry = z.h << static_cast<size_t>(64 - shift);
            z.h >>= shift;
            z.l >>= shift;
            z.l |= carry;
        }

        return z;
    }

    template <typename T>
    constexpr friend donna128 operator<<(const donna128& x, T shift) {
        donna128 z = x;
        if (shift > 64) {
            z.h = z.l << (shift - 64);
            z.l = 0;
        } else if (shift == 64) {
            z.h = z.l;
            z.l = 0;
        } else if (shift > 0) {
            const uint64_t carry = z.l >> static_cast<size_t>(64 - shift);
            z.l = (z.l << shift);
            z.h = (z.h << shift) | carry;
        }

        return z;
    }

    constexpr friend uint64_t operator&(const donna128& x, uint64_t mask) { return x.l & mask; }

    constexpr uint64_t operator&=(uint64_t mask) {
        h = 0;
        l &= mask;
        return l;
    }

    constexpr donna128& operator+=(const donna128& x) {
        l += x.l;
        h += x.h;

        const uint64_t carry = CT::Mask<uint64_t>::is_lt(l, x.l).if_set_return(1);
        h += carry;
        return *this;
    }

    constexpr donna128& operator+=(uint64_t x) {
        l += x;
        const uint64_t carry = CT::Mask<uint64_t>::is_lt(l, x).if_set_return(1);
        h += carry;
        return *this;
    }

    constexpr uint64_t lo() const { return l; }

    constexpr uint64_t hi() const { return h; }

    constexpr operator uint64_t() const { return l; }

   private:
    uint64_t h = 0, l = 0;
};

template <std::unsigned_integral T>
constexpr inline donna128 operator*(const donna128& x, T y) {
    BOTAN_ARG_CHECK(x.hi() == 0, "High 64 bits of donna128 set to zero during multiply");

    uint64_t lo = 0, hi = 0;
    mul64x64_128(x.lo(), static_cast<uint64_t>(y), &lo, &hi);
    return donna128(lo, hi);
}

template <std::unsigned_integral T>
constexpr inline donna128 operator*(T y, const donna128& x) {
    return x * y;
}

constexpr inline donna128 operator+(const donna128& x, const donna128& y) {
    donna128 z = x;
    z += y;
    return z;
}

constexpr inline donna128 operator+(const donna128& x, uint64_t y) {
    donna128 z = x;
    z += y;
    return z;
}

constexpr inline donna128 operator|(const donna128& x, const donna128& y) {
    return donna128(x.lo() | y.lo(), x.hi() | y.hi());
}

constexpr inline donna128 operator|(const donna128& x, uint64_t y) {
    return donna128(x.lo() | y, x.hi());
}

constexpr inline uint64_t carry_shift(const donna128& a, size_t shift) { return (a >> shift).lo(); }

constexpr inline uint64_t combine_lower(const donna128& a, size_t s1, const donna128& b,
                                        size_t s2) {
    donna128 z = (a >> s1) | (b << s2);
    return z.lo();
}

#if defined(BOTAN_TARGET_HAS_NATIVE_UINT128)
inline uint64_t carry_shift(const uint128_t a, size_t shift) {
    return static_cast<uint64_t>(a >> shift);
}

inline uint64_t combine_lower(const uint128_t a, size_t s1, const uint128_t b, size_t s2) {
    return static_cast<uint64_t>((a >> s1) | (b << s2));
}
#endif

}    // namespace Botan

namespace Botan {

class RandomNumberGenerator;

/**
 * Encoding Method for Encryption
 */
class BOTAN_TEST_API EME {
   public:
    virtual ~EME();

    /**
     * Factory method for EME (message-encoding methods for encryption) objects
     * @param algo_spec the name of the EME to create
     * @return pointer to newly allocated object of that type
     */
    static std::unique_ptr<EME> create(std::string_view algo_spec);

    /**
     * Return the maximum input size in bytes we can support
     * @param keybits the size of the key in bits
     * @return upper bound of input in bytes
     */
    virtual size_t maximum_input_size(size_t keybits) const = 0;

    /**
     * Encode an input
     * @param output buffer that is written to
     * @param input the plaintext
     * @param key_length length of the key in bits
     * @param rng a random number generator
     * @return number of bytes written to output
     */
    virtual size_t pad(std::span<uint8_t> output, std::span<const uint8_t> input, size_t key_length,
                       RandomNumberGenerator& rng) const = 0;

    /**
     * Decode an input
     * @param output buffer where output is placed
     * @param input the encoded plaintext
     * @return number of bytes written to output if valid,
     *  or an empty option if invalid. If an empty option is
     *  returned the contents of output are undefined
     */
    virtual CT::Option<size_t> unpad(std::span<uint8_t> output,
                                     std::span<const uint8_t> input) const = 0;
};

}    // namespace Botan

namespace Botan {

class RandomNumberGenerator;

/**
 * EMSA, from IEEE 1363s Encoding Method for Signatures, Appendix
 *
 * Any way of encoding/padding signatures
 */
class BOTAN_TEST_API EMSA {
   public:
    virtual ~EMSA() = default;

    /**
     * Factory method for EMSA (message-encoding methods for signatures
     * with appendix) objects
     * @param algo_spec the name of the EMSA to create
     * @return pointer to newly allocated object of that type, or nullptr
     */
    static std::unique_ptr<EMSA> create(std::string_view algo_spec);

    /**
     * Factory method for EMSA (message-encoding methods for signatures
     * with appendix) objects
     * @param algo_spec the name of the EMSA to create
     * @return pointer to newly allocated object of that type, or throws
     */
    static std::unique_ptr<EMSA> create_or_throw(std::string_view algo_spec);

    /**
     * Add more data to the signature computation
     * @param input some data
     * @param length length of input in bytes
     */
    virtual void update(const uint8_t input[], size_t length) = 0;

    /**
     * @return raw hash
     */
    virtual std::vector<uint8_t> raw_data() = 0;

    /**
     * Return the encoding of a message
     * @param msg the result of raw_data()
     * @param output_bits the desired output bit size
     * @param rng a random number generator
     * @return encoded signature
     */
    virtual std::vector<uint8_t> encoding_of(std::span<const uint8_t> msg, size_t output_bits,
                                             RandomNumberGenerator& rng) = 0;

    /**
     * Verify the encoding
     * @param encoding the received (coded) message representative
     * @param raw_hash the computed (local, uncoded) message representative
     * @param key_bits the size of the key in bits
     * @return true if coded is a valid encoding of raw, otherwise false
     */
    virtual bool verify(std::span<const uint8_t> encoding, std::span<const uint8_t> raw_hash,
                        size_t key_bits) = 0;

    /**
     * Return the hash function being used by this padding scheme
     */
    virtual std::string hash_function() const = 0;

    /**
     * @return the SCAN name of the encoding/padding scheme
     */
    virtual std::string name() const = 0;
};

}    // namespace Botan

namespace Botan {

/**
 * No_Filesystem_Access Exception
 */
class No_Filesystem_Access final : public Exception {
   public:
    No_Filesystem_Access() : Exception("No filesystem access enabled.") {}
};

BOTAN_TEST_API bool has_filesystem_impl();

BOTAN_TEST_API std::vector<std::string> get_files_recursive(std::string_view dir);

}    // namespace Botan

namespace Botan {

namespace fmt_detail {

inline void do_fmt(std::ostringstream& oss, std::string_view format) { oss << format; }

template <typename T, typename... Ts>
void do_fmt(std::ostringstream& oss, std::string_view format, const T& val, const Ts&... rest) {
    size_t i = 0;

    while (i < format.size()) {
        if (format[i] == '{' && (format.size() > (i + 1)) && format.at(i + 1) == '}') {
            oss << val;
            return do_fmt(oss, format.substr(i + 2), rest...);
        } else {
            oss << format[i];
        }

        i += 1;
    }
}

}    // namespace fmt_detail

/**
 * Simple formatter utility.
 *
 * Should be replaced with std::format once that's available on all our
 * supported compilers.
 *
 * '{}' markers in the format string are replaced by the arguments.
 * Unlike std::format, there is no support for escaping or for any kind
 * of conversion flags.
 */
template <typename... T>
std::string fmt(std::string_view format, const T&... args) {
    std::ostringstream oss;
    oss.imbue(std::locale::classic());
    fmt_detail::do_fmt(oss, format, args...);
    return oss.str();
}

}    // namespace Botan

namespace Botan {

/**
 * Return the PKCS #1 hash identifier
 * @see RFC 3447 section 9.2
 * @param hash_name the name of the hash function
 * @return uint8_t sequence identifying the hash
 * @throw Invalid_Argument if the hash has no known PKCS #1 hash id
 */
std::vector<uint8_t> BOTAN_TEST_API pkcs_hash_id(std::string_view hash_name);

/**
 * Return the IEEE 1363 hash identifier
 * @param hash_name the name of the hash function
 * @return uint8_t code identifying the hash, or 0 if not known
 */
uint8_t ieee1363_hash_id(std::string_view hash_name);

}    // namespace Botan

namespace Botan {

/**
 * HMAC
 */
class HMAC final : public MessageAuthenticationCode {
   public:
    void clear() override;
    std::string name() const override;
    std::unique_ptr<MessageAuthenticationCode> new_object() const override;

    size_t output_length() const override;

    Key_Length_Specification key_spec() const override;

    bool has_keying_material() const override;

    /**
     * @param hash the hash to use for HMACing
     */
    explicit HMAC(std::unique_ptr<HashFunction> hash);

    HMAC(const HMAC&) = delete;
    HMAC& operator=(const HMAC&) = delete;

   private:
    void add_data(std::span<const uint8_t>) override;
    void final_result(std::span<uint8_t>) override;
    void key_schedule(std::span<const uint8_t>) override;

    std::unique_ptr<HashFunction> m_hash;
    secure_vector<uint8_t> m_ikey, m_okey;
    size_t m_hash_output_length;
    size_t m_hash_block_size;
};

}    // namespace Botan

namespace Botan {

template <std::unsigned_integral T>
constexpr inline std::optional<T> checked_add(T a, T b) {
    const T r = a + b;
    if (r < a || r < b) {
        return {};
    }
    return r;
}

template <std::unsigned_integral T>
constexpr std::optional<T> checked_sub(T a, T b) {
    if (b > a) {
        return {};
    }
    return a - b;
}

template <std::unsigned_integral T, std::unsigned_integral... Ts>
    requires all_same_v<T, Ts...>
constexpr inline std::optional<T> checked_add(T a, T b, Ts... rest) {
    if (auto r = checked_add(a, b)) {
        return checked_add(r.value(), rest...);
    } else {
        return {};
    }
}

template <std::unsigned_integral T>
constexpr inline std::optional<T> checked_mul(T a, T b) {
    // Multiplication by 1U is a hack to work around C's insane
    // integer promotion rules.
    // https://stackoverflow.com/questions/24795651
    const T r = (1U * a) * b;
    // If a == 0 then the multiply certainly did not overflow
    // Otherwise r / a == b unless overflow occured
    if (a != 0 && r / a != b) {
        return {};
    }
    return r;
}

template <typename RT, typename ExceptionType, typename AT>
    requires std::integral<strong_type_wrapped_type<RT>> &&
             std::integral<strong_type_wrapped_type<AT>>
constexpr RT checked_cast_to_or_throw(AT i, std::string_view error_msg_on_fail) {
    const auto unwrapped_input = unwrap_strong_type(i);

    const auto unwrapped_result = static_cast<strong_type_wrapped_type<RT>>(unwrapped_input);
    if (unwrapped_input != static_cast<strong_type_wrapped_type<AT>>(unwrapped_result))
        [[unlikely]] {
        throw ExceptionType(error_msg_on_fail);
    }

    return wrap_strong_type<RT>(unwrapped_result);
}

template <typename RT, typename AT>
    requires std::integral<strong_type_wrapped_type<RT>> &&
             std::integral<strong_type_wrapped_type<AT>>
constexpr RT checked_cast_to(AT i) {
    return checked_cast_to_or_throw<RT, Internal_Error>(i, "Error during integer conversion");
}

/**
 * SWAR (SIMD within a word) byte-by-byte comparison
 *
 * This individually compares each byte of the provided words.
 * It returns a mask which contains, for each byte, 0xFF if
 * the byte in @p a was less than the byte in @p b. Otherwise the
 * mask is 00.
 *
 * This implementation assumes that the high bits of each byte
 * in both @p a and @p b are clear! It is possible to support the
 * full range of bytes, but this requires additional comparisons.
 */
template <std::unsigned_integral T>
constexpr T swar_lt(T a, T b) {
    // The constant 0x808080... as a T
    constexpr T hi1 = (static_cast<T>(-1) / 255) << 7;
    // The constant 0x7F7F7F... as a T
    constexpr T lo7 = static_cast<T>(~hi1);
    T r = (lo7 - a + b) & hi1;
    // Currently the mask is 80 if lt, otherwise 00. Convert to FF/00
    return (r << 1) - (r >> 7);
}

/**
 * SWAR (SIMD within a word) byte-by-byte comparison
 *
 * This individually compares each byte of the provided words.
 * It returns a mask which contains, for each byte, 0x80 if
 * the byte in @p a was less than the byte in @p b. Otherwise the
 * mask is 00.
 *
 * This implementation assumes that the high bits of each byte
 * in both @p lower and @p upper are clear! It is possible to support the
 * full range of bytes, but this requires additional comparisons.
 */
template <std::unsigned_integral T>
constexpr T swar_in_range(T v, T lower, T upper) {
    // The constant 0x808080... as a T
    constexpr T hi1 = (static_cast<T>(-1) / 255) << 7;
    // The constant 0x7F7F7F... as a T
    constexpr T lo7 = ~hi1;

    const T sub = ((v | hi1) - (lower & lo7)) ^ ((v ^ (~lower)) & hi1);
    const T a_lo = sub & lo7;
    const T a_hi = sub & hi1;
    return (lo7 - a_lo + upper) & hi1 & ~a_hi;
}

/**
 * Return the index of the first byte with the high bit set
 */
template <std::unsigned_integral T>
constexpr size_t index_of_first_set_byte(T v) {
    // The constant 0x010101... as a T
    constexpr T lo1 = (static_cast<T>(-1) / 255);
    // The constant 0x808080... as a T
    constexpr T hi1 = lo1 << 7;
    // How many bits to shift in order to get the top byte
    constexpr size_t bits = (sizeof(T) * 8) - 8;

    return static_cast<size_t>((((((v & hi1) - 1) & lo1) * lo1) >> bits) - 1);
}

}    // namespace Botan

/*
 * GCC and Clang use string identifiers to tag ISA extensions (eg using the
 * `target` function attribute).
 *
 * This file consolidates the actual definition of such target attributes
 */

#if defined(BOTAN_TARGET_CPU_IS_X86_FAMILY)

#define BOTAN_FN_ISA_SIMD_4X32 BOTAN_FUNC_ISA("ssse3")
#define BOTAN_FN_ISA_SIMD_2X64 BOTAN_FUNC_ISA("ssse3")
#define BOTAN_FN_ISA_SIMD_4X64 BOTAN_FUNC_ISA("avx2")
#define BOTAN_FN_ISA_SIMD_8X64 BOTAN_FN_ISA_AVX512
#define BOTAN_FN_ISA_CLMUL BOTAN_FUNC_ISA("pclmul,ssse3")
#define BOTAN_FN_ISA_AESNI BOTAN_FUNC_ISA("aes,ssse3")
#define BOTAN_FN_ISA_SHANI BOTAN_FUNC_ISA("sha,ssse3,sse4.1")
#define BOTAN_FN_ISA_SHA512 BOTAN_FUNC_ISA("sha512,avx2")
#define BOTAN_FN_ISA_SSE2 BOTAN_FUNC_ISA("sse2")
#define BOTAN_FN_ISA_AVX2 BOTAN_FUNC_ISA("avx2")
#define BOTAN_FN_ISA_AVX2_BMI2 BOTAN_FUNC_ISA("avx2,bmi,bmi2")
#define BOTAN_FN_ISA_AVX2_VAES BOTAN_FUNC_ISA("vaes,avx2")
#define BOTAN_FN_ISA_AVX2_SM4 BOTAN_FUNC_ISA("sm4,avx2")
#define BOTAN_FN_ISA_AVX2_GFNI BOTAN_FUNC_ISA("gfni,avx2")
#define BOTAN_FN_ISA_AVX512 BOTAN_FUNC_ISA("avx512f,avx512dq,avx512bw")
#define BOTAN_FN_ISA_AVX512_BMI2 BOTAN_FUNC_ISA("avx512f,avx512dq,avx512bw,bmi,bmi2")

#endif

#if defined(BOTAN_TARGET_ARCH_IS_ARM64)

#define BOTAN_FN_ISA_SIMD_4X32 BOTAN_FUNC_ISA("+simd")
#define BOTAN_FN_ISA_CLMUL BOTAN_FUNC_ISA("+crypto+aes")
#define BOTAN_FN_ISA_AES BOTAN_FUNC_ISA("+crypto+aes")
#define BOTAN_FN_ISA_SHA2 BOTAN_FUNC_ISA("+crypto+sha2")
#define BOTAN_FN_ISA_SM4 BOTAN_FUNC_ISA("arch=armv8.2-a+sm4")
#define BOTAN_FN_ISA_SHA512 BOTAN_FUNC_ISA("arch=armv8.2-a+sha3")

#endif

#if defined(BOTAN_TARGET_ARCH_IS_ARM32)
#define BOTAN_FN_ISA_SIMD_4X32 BOTAN_FUNC_ISA("fpu=neon")
#endif

#if defined(BOTAN_TARGET_CPU_IS_PPC_FAMILY)

#define BOTAN_FN_ISA_SIMD_4X32 BOTAN_FUNC_ISA("altivec")
#define BOTAN_FN_ISA_CLMUL BOTAN_FUNC_ISA("vsx,crypto")
#define BOTAN_FN_ISA_AES BOTAN_FUNC_ISA("vsx,crypto")

#endif

#if defined(BOTAN_TARGET_ARCH_IS_LOONGARCH64)

#define BOTAN_FN_ISA_SIMD_4X32 BOTAN_FUNC_ISA("lsx")

#endif

namespace Botan {

/**
 * KDF2, from IEEE 1363
 */
class KDF2 final : public KDF {
   public:
    std::string name() const override;

    std::unique_ptr<KDF> new_object() const override;

    /**
     * @param hash the hash function to use
     */
    explicit KDF2(std::unique_ptr<HashFunction> hash) : m_hash(std::move(hash)) {}

   private:
    void perform_kdf(std::span<uint8_t> key, std::span<const uint8_t> secret,
                     std::span<const uint8_t> salt, std::span<const uint8_t> label) const override;

   private:
    std::unique_ptr<HashFunction> m_hash;
};

}    // namespace Botan

/**
 * @file loadstor.h
 *
 * @brief This header contains various helper functions to load and store
 *        unsigned integers in big- or little-endian byte order.
 *
 * Storing integer values in various ways (same for BE and LE):
 * @code {.cpp}
 *
 *   std::array<uint8_t, 8> bytes = store_le(some_uint64);
 *   std::array<uint8_t, 12> bytes = store_le(some_uint32_1, some_uint32_2, some_uint32_3, ...);
 *   auto bytes = store_le<std::vector<uint8_t>>(some_uint64);
 *   auto bytes = store_le<MyContainerStrongType>(some_uint64);
 *   auto bytes = store_le<std::vector<uint8_t>>(vector_of_ints);
 *   auto bytes = store_le<secure_vector<uint8_t>>(some_uint32_1, some_uint32_2, some_uint32_3,
 * ...); store_le(bytes, some_uint64); store_le(concatenated_bytes, some_uint64_1, some_uint64_2,
 * some_uint64_3, ...); store_le(concatenated_bytes, vector_of_ints);
 *   copy_out_le(short_concated_bytes, vector_of_ints); // stores as many bytes as required in the
 * output buffer
 *
 * @endcode
 *
 * Loading integer values in various ways (same for BE and LE):
 * @code {.cpp}
 *
 *   uint64_t some_uint64 = load_le(bytes_8);
 *   auto some_int32s = load_le<std::vector<uint32_t>>(concatenated_bytes);
 *   auto some_int32s = load_le<std::vector<MyIntStrongType>>(concatenated_bytes);
 *   auto some_int32s = load_le(some_strong_typed_bytes);
 *   auto strong_int  = load_le<MyStrongTypedInteger>(concatenated_bytes);
 *   load_le(concatenated_bytes, out_some_uint64);
 *   load_le(concatenated_bytes, out_some_uint64_1, out_some_uint64_2, out_some_uint64_3, ...);
 *   load_le(out_vector_of_ints, concatenated_bytes);
 *
 * @endcode
 */

namespace Botan {

static_assert(std::endian::native == std::endian::big || std::endian::native == std::endian::little,
              "Mixed endian systems are not supported");

/**
 * Byte extraction
 * @param byte_num which byte to extract, 0 == highest byte
 * @param input the value to extract from
 * @return byte byte_num of input
 */
template <typename T>
inline constexpr uint8_t get_byte_var(size_t byte_num, T input) {
    return static_cast<uint8_t>(input >> (((~byte_num) & (sizeof(T) - 1)) << 3));
}

/**
 * Byte extraction
 * @param input the value to extract from
 * @return byte byte number B of input
 */
template <size_t B, typename T>
inline constexpr uint8_t get_byte(T input)
    requires(B < sizeof(T))
{
    const size_t shift = ((~B) & (sizeof(T) - 1)) << 3;
    return static_cast<uint8_t>((input >> shift) & 0xFF);
}

/**
 * Make a uint16_t from two bytes
 * @param i0 the first byte
 * @param i1 the second byte
 * @return i0 || i1
 */
inline constexpr uint16_t make_uint16(uint8_t i0, uint8_t i1) {
    return static_cast<uint16_t>((static_cast<uint16_t>(i0) << 8) | i1);
}

/**
 * Make a uint32_t from four bytes
 * @param i0 the first byte
 * @param i1 the second byte
 * @param i2 the third byte
 * @param i3 the fourth byte
 * @return i0 || i1 || i2 || i3
 */
inline constexpr uint32_t make_uint32(uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3) {
    return ((static_cast<uint32_t>(i0) << 24) | (static_cast<uint32_t>(i1) << 16) |
            (static_cast<uint32_t>(i2) << 8) | (static_cast<uint32_t>(i3)));
}

/**
 * Make a uint64_t from eight bytes
 * @param i0 the first byte
 * @param i1 the second byte
 * @param i2 the third byte
 * @param i3 the fourth byte
 * @param i4 the fifth byte
 * @param i5 the sixth byte
 * @param i6 the seventh byte
 * @param i7 the eighth byte
 * @return i0 || i1 || i2 || i3 || i4 || i5 || i6 || i7
 */
inline constexpr uint64_t make_uint64(uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3, uint8_t i4,
                                      uint8_t i5, uint8_t i6, uint8_t i7) {
    return ((static_cast<uint64_t>(i0) << 56) | (static_cast<uint64_t>(i1) << 48) |
            (static_cast<uint64_t>(i2) << 40) | (static_cast<uint64_t>(i3) << 32) |
            (static_cast<uint64_t>(i4) << 24) | (static_cast<uint64_t>(i5) << 16) |
            (static_cast<uint64_t>(i6) << 8) | (static_cast<uint64_t>(i7)));
}

namespace detail {

/**
 * @returns the opposite endianness of the specified endianness
 *
 * Note this assumes that there are only two endian orderings; we
 * do not supported mixed endian systems
 */
consteval std::endian opposite(std::endian endianness) {
    if (endianness == std::endian::big) {
        return std::endian::little;
    } else {
        // We already verified via static assert earlier in this file that we are
        // running on either a big endian or little endian system
        return std::endian::big;
    }
}

/**
 * Models a custom type that provides factory methods to be loaded in big- or
 * little-endian byte order.
 */
template <typename T>
concept custom_loadable = requires(std::span<const uint8_t, sizeof(T)> data) {
    { T::load_be(data) } -> std::same_as<T>;
    { T::load_le(data) } -> std::same_as<T>;
};

/**
 * Models a custom type that provides store methods to be stored in big- or
 * little-endian byte order.
 */
template <typename T>
concept custom_storable = requires(std::span<uint8_t, sizeof(T)> data, const T value) {
    { value.store_be(data) };
    { value.store_le(data) };
};

/**
 * Models a type that can be loaded/stored from/to a byte range.
 */
template <typename T>
concept unsigned_integralish =
    std::unsigned_integral<strong_type_wrapped_type<T>> ||
    (std::is_enum_v<T> && std::unsigned_integral<std::underlying_type_t<T>>) ||
    (custom_loadable<strong_type_wrapped_type<T>> || custom_storable<strong_type_wrapped_type<T>>);

template <typename T>
struct wrapped_type_helper_with_enum {
    using type = strong_type_wrapped_type<T>;
};

template <typename T>
    requires std::is_enum_v<T>
struct wrapped_type_helper_with_enum<T> {
    using type = std::underlying_type_t<T>;
};

template <unsigned_integralish T>
using wrapped_type = typename wrapped_type_helper_with_enum<T>::type;

template <unsigned_integralish InT>
constexpr auto unwrap_strong_type_or_enum(InT t) {
    if constexpr (std::is_enum_v<InT>) {
        // TODO: C++23: use std::to_underlying(in) instead
        return static_cast<std::underlying_type_t<InT>>(t);
    } else {
        return Botan::unwrap_strong_type(t);
    }
}

template <unsigned_integralish OutT, std::unsigned_integral T>
constexpr auto wrap_strong_type_or_enum(T t) {
    if constexpr (std::is_enum_v<OutT>) {
        return static_cast<OutT>(t);
    } else {
        return Botan::wrap_strong_type<OutT>(t);
    }
}

/**
 * Manually load a word from a range in either big or little endian byte order.
 *
 * This is only used at compile time.
 */
template <std::endian endianness, std::unsigned_integral OutT,
          ranges::contiguous_range<uint8_t> InR>
inline constexpr OutT fallback_load_any(InR&& in_range) {
    std::span in{in_range};
    // clang-format off
   if constexpr(endianness == std::endian::big) {
      return [&]<size_t... i>(std::index_sequence<i...>) {
         return static_cast<OutT>(((static_cast<OutT>(in[i]) << ((sizeof(OutT) - i - 1) * 8)) | ...));
      } (std::make_index_sequence<sizeof(OutT)>());
   } else {
      static_assert(endianness == std::endian::little);
      return [&]<size_t... i>(std::index_sequence<i...>) {
         return static_cast<OutT>(((static_cast<OutT>(in[i]) << (i * 8)) | ...));
      } (std::make_index_sequence<sizeof(OutT)>());
   }
    // clang-format on
}

/**
 * Manually store a word into a range in either big or little endian byte order.
 *
 * This will be used only at compile time.
 */
template <std::endian endianness, std::unsigned_integral InT,
          ranges::contiguous_output_range<uint8_t> OutR>
inline constexpr void fallback_store_any(InT in, OutR&& out_range) {
    std::span out{out_range};
    // clang-format off
   if constexpr(endianness == std::endian::big) {
      [&]<size_t... i>(std::index_sequence<i...>) {
         ((out[i] = get_byte<i>(in)), ...);
      } (std::make_index_sequence<sizeof(InT)>());
   } else {
      static_assert(endianness == std::endian::little);
      [&]<size_t... i>(std::index_sequence<i...>) {
         ((out[i] = get_byte<sizeof(InT) - i - 1>(in)), ...);
      } (std::make_index_sequence<sizeof(InT)>());
   }
    // clang-format on
}

/**
 * Load a word from a range in either big or little endian byte order
 *
 * This is the base implementation, all other overloads are just convenience
 * wrappers. It is assumed that the range has the correct size for the word.
 *
 * Template arguments of all overloads of load_any() share the same semantics:
 *
 *   1.  std::endian     Either `std::endian::big` or `std::endian::little`, that
 *                       will eventually select the byte order translation mode
 *                       implemented in this base function.
 *
 *   2.  Output type     Either `AutoDetect`, an unsigned integer or a container
 *                       holding an unsigned integer type. `AutoDetect` means
 *                       that the caller did not explicitly specify the type and
 *                       expects the type to be inferred from the input.
 *
 *   3+. Argument types  Typically, those are input and output ranges of bytes
 *                       or unsigned integers. Or one or more unsigned integers
 *                       acting as output parameters.
 *
 * @param in_range a fixed-length byte range
 * @return T loaded from @p in_range, as a big-endian value
 */
template <std::endian endianness, unsigned_integralish WrappedOutT,
          ranges::contiguous_range<uint8_t> InR>
    requires(!custom_loadable<strong_type_wrapped_type<WrappedOutT>>)
inline constexpr WrappedOutT load_any(InR&& in_range) {
    using OutT = detail::wrapped_type<WrappedOutT>;
    ranges::assert_exact_byte_length<sizeof(OutT)>(in_range);

    return detail::wrap_strong_type_or_enum<WrappedOutT>([&]() -> OutT {
        // At compile time we cannot use `typecast_copy` as it uses `std::memcpy`
        // internally to copy ranges on a byte-by-byte basis, which is not allowed
        // in a `constexpr` context.
        if (std::is_constant_evaluated()) /* TODO: C++23: if consteval {} */ {
            return fallback_load_any<endianness, OutT>(std::forward<InR>(in_range));
        } else {
            std::span in{in_range};
            if constexpr (sizeof(OutT) == 1) {
                return static_cast<OutT>(in[0]);
            } else if constexpr (endianness == std::endian::native) {
                return typecast_copy<OutT>(in);
            } else {
                static_assert(opposite(endianness) == std::endian::native);
                return reverse_bytes(typecast_copy<OutT>(in));
            }
        }
    }());
}

/**
 * Load a custom object from a range in either big or little endian byte order
 *
 * This is the base implementation for custom objects (e.g. SIMD type wrappres),
 * all other overloads are just convenience overloads.
 *
 * @param in_range a fixed-length byte range
 * @return T loaded from @p in_range, as a big-endian value
 */
template <std::endian endianness, unsigned_integralish WrappedOutT,
          ranges::contiguous_range<uint8_t> InR>
    requires(custom_loadable<strong_type_wrapped_type<WrappedOutT>>)
inline constexpr WrappedOutT load_any(InR&& in_range) {
    using OutT = detail::wrapped_type<WrappedOutT>;
    ranges::assert_exact_byte_length<sizeof(OutT)>(in_range);
    std::span<const uint8_t, sizeof(OutT)> ins{in_range};
    if constexpr (endianness == std::endian::big) {
        return wrap_strong_type<WrappedOutT>(OutT::load_be(ins));
    } else {
        return wrap_strong_type<WrappedOutT>(OutT::load_le(ins));
    }
}

/**
 * Load many unsigned integers
 * @param in   a fixed-length span to some bytes
 * @param outs a arbitrary-length parameter list of unsigned integers to be loaded
 */
template <std::endian endianness, typename OutT, ranges::contiguous_range<uint8_t> InR,
          unsigned_integralish... Ts>
    requires(sizeof...(Ts) > 0) && ((std::same_as<AutoDetect, OutT> && all_same_v<Ts...>) ||
                                    (unsigned_integralish<OutT> && all_same_v<OutT, Ts...>))
inline constexpr void load_any(InR&& in, Ts&... outs) {
    ranges::assert_exact_byte_length<(sizeof(Ts) + ...)>(in);
    auto load_one = [off = 0]<typename T>(auto i, T& o) mutable {
        o = load_any<endianness, T>(i.subspan(off).template first<sizeof(T)>());
        off += sizeof(T);
    };

    (load_one(std::span{in}, outs), ...);
}

/**
 * Load a variable number of words from @p in into @p out.
 * The byte length of the @p out and @p in ranges must match.
 *
 * @param out the output range of words
 * @param in the input range of bytes
 */
template <std::endian endianness, typename OutT, ranges::contiguous_output_range OutR,
          ranges::contiguous_range<uint8_t> InR>
    requires(unsigned_integralish<std::ranges::range_value_t<OutR>> &&
             (std::same_as<AutoDetect, OutT> ||
              std::same_as<OutT, std::ranges::range_value_t<OutR>>))
inline constexpr void load_any(OutR&& out, InR&& in) {
    ranges::assert_equal_byte_lengths(out, in);
    using element_type = std::ranges::range_value_t<OutR>;

    auto load_elementwise = [&] {
        constexpr size_t bytes_per_element = sizeof(element_type);
        std::span<const uint8_t> in_s(in);
        for (auto& out_elem : out) {
            out_elem = load_any<endianness, element_type>(in_s.template first<bytes_per_element>());
            in_s = in_s.subspan(bytes_per_element);
        }
    };

    // At compile time we cannot use `typecast_copy` as it uses `std::memcpy`
    // internally to copy ranges on a byte-by-byte basis, which is not allowed
    // in a `constexpr` context.
    if (std::is_constant_evaluated()) /* TODO: C++23: if consteval {} */ {
        load_elementwise();
    } else {
        if constexpr (endianness == std::endian::native && !custom_loadable<element_type>) {
            typecast_copy(out, in);
        } else {
            load_elementwise();
        }
    }
}

//
// Type inference overloads
//

/**
 * Load one or more unsigned integers, auto-detect the output type if
 * possible. Otherwise, use the specified integer or integer container type.
 *
 * @param in_range a statically-sized range with some bytes
 * @return T loaded from in
 */
template <std::endian endianness, typename OutT, ranges::contiguous_range<uint8_t> InR>
    requires(std::same_as<AutoDetect, OutT> ||
             ((ranges::statically_spanable_range<OutT> || concepts::resizable_container<OutT>) &&
              unsigned_integralish<typename OutT::value_type>))
inline constexpr auto load_any(InR&& in_range) {
    auto out = []([[maybe_unused]] const auto& in) {
        if constexpr (std::same_as<AutoDetect, OutT>) {
            if constexpr (ranges::statically_spanable_range<InR>) {
                constexpr size_t extent = decltype(std::span{in})::extent;

                // clang-format off
            using type =
               std::conditional_t<extent == 1, uint8_t,
               std::conditional_t<extent == 2, uint16_t,
               std::conditional_t<extent == 4, uint32_t,
               std::conditional_t<extent == 8, uint64_t, void>>>>;
                // clang-format on

                static_assert(!std::is_void_v<type>,
                              "Cannot determine the output type based on a statically sized "
                              "bytearray with length other than those: 1, 2, 4, 8");

                return type{};
            } else {
                static_assert(!std::same_as<AutoDetect, OutT>,
                              "cannot infer return type from a dynamic range at compile time, "
                              "please specify it explicitly");
            }
        } else if constexpr (concepts::resizable_container<OutT>) {
            const size_t in_bytes = std::span{in}.size_bytes();
            constexpr size_t out_elem_bytes = sizeof(typename OutT::value_type);
            BOTAN_ARG_CHECK(in_bytes % out_elem_bytes == 0,
                            "Input range is not word-aligned with the requested output range");
            return OutT(in_bytes / out_elem_bytes);
        } else {
            return OutT{};
        }
    }(in_range);

    using out_type = decltype(out);
    if constexpr (unsigned_integralish<out_type>) {
        out = load_any<endianness, out_type>(std::forward<InR>(in_range));
    } else {
        static_assert(ranges::contiguous_range<out_type>);
        using out_range_type = std::ranges::range_value_t<out_type>;
        load_any<endianness, out_range_type>(out, std::forward<InR>(in_range));
    }
    return out;
}

//
// Legacy load functions that work on raw pointers and arrays
//

/**
 * Load a word from @p in at some offset @p off
 * @param in a pointer to some bytes
 * @param off an offset into the array
 * @return off'th T of in, as a big-endian value
 */
template <std::endian endianness, unsigned_integralish OutT>
inline constexpr OutT load_any(const uint8_t in[], size_t off) {
    // asserts that *in points to enough bytes to read at offset off
    constexpr size_t out_size = sizeof(OutT);
    return load_any<endianness, OutT>(
        std::span<const uint8_t, out_size>(in + off * out_size, out_size));
}

/**
 * Load many words from @p in
 * @param in   a pointer to some bytes
 * @param outs a arbitrary-length parameter list of unsigned integers to be loaded
 */
template <std::endian endianness, typename OutT, unsigned_integralish... Ts>
    requires(sizeof...(Ts) > 0 && all_same_v<Ts...> &&
             ((std::same_as<AutoDetect, OutT> && all_same_v<Ts...>) ||
              (unsigned_integralish<OutT> && all_same_v<OutT, Ts...>)))
inline constexpr void load_any(const uint8_t in[], Ts&... outs) {
    constexpr auto bytes = (sizeof(outs) + ...);
    // asserts that *in points to the correct amount of memory
    load_any<endianness, OutT>(std::span<const uint8_t, bytes>(in, bytes), outs...);
}

/**
 * Load a variable number of words from @p in into @p out.
 * @param out the output array of words
 * @param in the input array of bytes
 * @param count how many words are in in
 */
template <std::endian endianness, typename OutT, unsigned_integralish T>
    requires(std::same_as<AutoDetect, OutT> || std::same_as<T, OutT>)
inline constexpr void load_any(T out[], const uint8_t in[], size_t count) {
    // asserts that *in and *out point to the correct amount of memory
    load_any<endianness, OutT>(std::span<T>(out, count),
                               std::span<const uint8_t>(in, count * sizeof(T)));
}

}    // namespace detail

/**
 * Load "something" in little endian byte order
 * See the documentation of this file for more details.
 */
template <typename OutT = detail::AutoDetect, typename... ParamTs>
inline constexpr auto load_le(ParamTs&&... params) {
    return detail::load_any<std::endian::little, OutT>(std::forward<ParamTs>(params)...);
}

/**
 * Load "something" in big endian byte order
 * See the documentation of this file for more details.
 */
template <typename OutT = detail::AutoDetect, typename... ParamTs>
inline constexpr auto load_be(ParamTs&&... params) {
    return detail::load_any<std::endian::big, OutT>(std::forward<ParamTs>(params)...);
}

namespace detail {

/**
 * Store a word in either big or little endian byte order into a range
 *
 * This is the base implementation, all other overloads are just convenience
 * wrappers. It is assumed that the range has the correct size for the word.
 *
 * Template arguments of all overloads of store_any() share the same semantics
 * as those of load_any(). See the documentation of this function for more
 * details.
 *
 * @param wrapped_in an unsigned integral to be stored
 * @param out_range  a byte range to store the word into
 */
template <std::endian endianness, unsigned_integralish WrappedInT,
          ranges::contiguous_output_range<uint8_t> OutR>
    requires(!custom_storable<strong_type_wrapped_type<WrappedInT>>)
inline constexpr void store_any(WrappedInT wrapped_in, OutR&& out_range) {
    const auto in = detail::unwrap_strong_type_or_enum(wrapped_in);
    using InT = decltype(in);
    ranges::assert_exact_byte_length<sizeof(in)>(out_range);
    std::span out{out_range};

    // At compile time we cannot use `typecast_copy` as it uses `std::memcpy`
    // internally to copy ranges on a byte-by-byte basis, which is not allowed
    // in a `constexpr` context.
    if (std::is_constant_evaluated()) /* TODO: C++23: if consteval {} */ {
        return fallback_store_any<endianness, InT>(in, std::forward<OutR>(out_range));
    } else {
        if constexpr (sizeof(InT) == 1) {
            out[0] = static_cast<uint8_t>(in);
        } else if constexpr (endianness == std::endian::native) {
            typecast_copy(out, in);
        } else {
            static_assert(opposite(endianness) == std::endian::native);
            typecast_copy(out, reverse_bytes(in));
        }
    }
}

/**
 * Store a custom word in either big or little endian byte order into a range
 *
 * This is the base implementation for storing custom objects, all other
 * overloads are just convenience overloads.
 *
 * @param wrapped_in a custom object to be stored
 * @param out_range  a byte range to store the word into
 */
template <std::endian endianness, unsigned_integralish WrappedInT,
          ranges::contiguous_output_range<uint8_t> OutR>
    requires(custom_storable<strong_type_wrapped_type<WrappedInT>>)
inline constexpr void store_any(WrappedInT wrapped_in, OutR&& out_range) {
    const auto in = detail::unwrap_strong_type_or_enum(wrapped_in);
    using InT = decltype(in);
    ranges::assert_exact_byte_length<sizeof(in)>(out_range);
    std::span<uint8_t, sizeof(InT)> outs{out_range};
    if constexpr (endianness == std::endian::big) {
        in.store_be(outs);
    } else {
        in.store_le(outs);
    }
}

/**
 * Store many unsigned integers words into a byte range
 * @param out a sized range of some bytes
 * @param ins a arbitrary-length parameter list of unsigned integers to be stored
 */
template <std::endian endianness, typename InT, ranges::contiguous_output_range<uint8_t> OutR,
          unsigned_integralish... Ts>
    requires(sizeof...(Ts) > 0) && ((std::same_as<AutoDetect, InT> && all_same_v<Ts...>) ||
                                    (unsigned_integralish<InT> && all_same_v<InT, Ts...>))
inline constexpr void store_any(OutR&& out, Ts... ins) {
    ranges::assert_exact_byte_length<(sizeof(Ts) + ...)>(out);
    auto store_one = [off = 0]<typename T>(auto o, T i) mutable {
        store_any<endianness, T>(i, o.subspan(off).template first<sizeof(T)>());
        off += sizeof(T);
    };

    (store_one(std::span{out}, ins), ...);
}

/**
 * Store a variable number of words given in @p in into @p out.
 * The byte lengths of @p in and @p out must be consistent.
 * @param out the output range of bytes
 * @param in the input range of words
 */
template <std::endian endianness, typename InT, ranges::contiguous_output_range<uint8_t> OutR,
          ranges::spanable_range InR>
    requires(std::same_as<AutoDetect, InT> || std::same_as<InT, std::ranges::range_value_t<InR>>)
inline constexpr void store_any(OutR&& out, InR&& in) {
    ranges::assert_equal_byte_lengths(out, in);
    using element_type = std::ranges::range_value_t<InR>;

    auto store_elementwise = [&] {
        constexpr size_t bytes_per_element = sizeof(element_type);
        std::span<uint8_t> out_s(out);
        for (auto in_elem : in) {
            store_any<endianness, element_type>(out_s.template first<bytes_per_element>(), in_elem);
            out_s = out_s.subspan(bytes_per_element);
        }
    };

    // At compile time we cannot use `typecast_copy` as it uses `std::memcpy`
    // internally to copy ranges on a byte-by-byte basis, which is not allowed
    // in a `constexpr` context.
    if (std::is_constant_evaluated()) /* TODO: C++23: if consteval {} */ {
        store_elementwise();
    } else {
        if constexpr (endianness == std::endian::native && !custom_storable<element_type>) {
            typecast_copy(out, in);
        } else {
            store_elementwise();
        }
    }
}

//
// Type inference overloads
//

/**
 * Infer InT from a single unsigned integer input parameter.
 *
 * TODO: we might consider dropping this overload (i.e. out-range as second
 *       parameter) and make this a "special case" of the overload below, that
 *       takes a variadic number of input parameters.
 *
 * @param in an unsigned integer to be stored
 * @param out_range a range of bytes to store the word into
 */
template <std::endian endianness, typename InT, unsigned_integralish T,
          ranges::contiguous_output_range<uint8_t> OutR>
    requires std::same_as<AutoDetect, InT>
inline constexpr void store_any(T in, OutR&& out_range) {
    store_any<endianness, T>(in, std::forward<OutR>(out_range));
}

/**
 * The caller provided some integer values in a collection but did not provide
 * the output container. Let's create one for them, fill it with one of the
 * overloads above and return it. This will default to a std::array if the
 * caller did not specify the desired output container type.
 *
 * @param in_range a range of words that should be stored
 * @return a container of bytes that contains the stored words
 */
template <std::endian endianness, typename OutR, ranges::spanable_range InR>
    requires(std::same_as<AutoDetect, OutR> ||
             (ranges::statically_spanable_range<OutR> && std::default_initializable<OutR>) ||
             concepts::resizable_byte_buffer<OutR>)
inline constexpr auto store_any(InR&& in_range) {
    auto out = []([[maybe_unused]] const auto& in) {
        if constexpr (std::same_as<AutoDetect, OutR>) {
            if constexpr (ranges::statically_spanable_range<InR>) {
                constexpr size_t bytes =
                    decltype(std::span{in})::extent * sizeof(std::ranges::range_value_t<InR>);
                return std::array<uint8_t, bytes>();
            } else {
                static_assert(!std::same_as<AutoDetect, OutR>,
                              "cannot infer a suitable result container type from the given "
                              "parameters at compile time, please specify it explicitly");
            }
        } else if constexpr (concepts::resizable_byte_buffer<OutR>) {
            return OutR(std::span{in}.size_bytes());
        } else {
            return OutR{};
        }
    }(in_range);

    store_any<endianness, std::ranges::range_value_t<InR>>(out, std::forward<InR>(in_range));
    return out;
}

/**
 * The caller provided some integer values but did not provide the output
 * container. Let's create one for them, fill it with one of the overloads above
 * and return it. This will default to a std::array if the caller did not
 * specify the desired output container type.
 *
 * @param ins some words that should be stored
 * @return a container of bytes that contains the stored words
 */
template <std::endian endianness, typename OutR, unsigned_integralish... Ts>
    requires all_same_v<Ts...>
inline constexpr auto store_any(Ts... ins) {
    return store_any<endianness, OutR>(std::array{ins...});
}

//
// Legacy store functions that work on raw pointers and arrays
//

/**
 * Store a single unsigned integer into a raw pointer
 * @param in the input unsigned integer
 * @param out the byte array to write to
 */
template <std::endian endianness, typename InT, unsigned_integralish T>
    requires(std::same_as<AutoDetect, InT> || std::same_as<T, InT>)
inline constexpr void store_any(T in, uint8_t out[]) {
    // asserts that *out points to enough bytes to write into
    store_any<endianness, InT>(in, std::span<uint8_t, sizeof(T)>(out, sizeof(T)));
}

/**
 * Store many unsigned integers words into a raw pointer
 * @param ins a arbitrary-length parameter list of unsigned integers to be stored
 * @param out the byte array to write to
 */
template <std::endian endianness, typename InT, unsigned_integralish T0, unsigned_integralish... Ts>
    requires(std::same_as<AutoDetect, InT> || std::same_as<T0, InT>) && all_same_v<T0, Ts...>
inline constexpr void store_any(uint8_t out[], T0 in0, Ts... ins) {
    constexpr auto bytes = sizeof(in0) + (sizeof(ins) + ... + 0);
    // asserts that *out points to the correct amount of memory
    store_any<endianness, T0>(std::span<uint8_t, bytes>(out, bytes), in0, ins...);
}

}    // namespace detail

/**
 * Store "something" in little endian byte order
 * See the documentation of this file for more details.
 */
template <typename ModifierT = detail::AutoDetect, typename... ParamTs>
inline constexpr auto store_le(ParamTs&&... params) {
    return detail::store_any<std::endian::little, ModifierT>(std::forward<ParamTs>(params)...);
}

/**
 * Store "something" in big endian byte order
 * See the documentation of this file for more details.
 */
template <typename ModifierT = detail::AutoDetect, typename... ParamTs>
inline constexpr auto store_be(ParamTs&&... params) {
    return detail::store_any<std::endian::big, ModifierT>(std::forward<ParamTs>(params)...);
}

namespace detail {

template <std::endian endianness, unsigned_integralish T>
inline size_t copy_out_any_word_aligned_portion(std::span<uint8_t>& out, std::span<const T>& in) {
    const size_t full_words = out.size() / sizeof(T);
    const size_t full_word_bytes = full_words * sizeof(T);
    const size_t remaining_bytes = out.size() - full_word_bytes;
    BOTAN_ASSERT_NOMSG(in.size_bytes() >= full_word_bytes + remaining_bytes);

    // copy full words
    store_any<endianness, T>(out.first(full_word_bytes), in.first(full_words));
    out = out.subspan(full_word_bytes);
    in = in.subspan(full_words);

    return remaining_bytes;
}

}    // namespace detail

/**
 * Partially copy a subset of @p in into @p out using big-endian
 * byte order.
 */
template <ranges::spanable_range InR>
inline void copy_out_be(std::span<uint8_t> out, InR&& in) {
    using T = std::ranges::range_value_t<InR>;
    std::span<const T> in_s{in};
    const auto remaining_bytes =
        detail::copy_out_any_word_aligned_portion<std::endian::big>(out, in_s);

    // copy remaining bytes as a partial word
    for (size_t i = 0; i < remaining_bytes; ++i) {
        out[i] = get_byte_var(i, in_s.front());
    }
}

/**
 * Partially copy a subset of @p in into @p out using little-endian
 * byte order.
 */
template <ranges::spanable_range InR>
inline void copy_out_le(std::span<uint8_t> out, InR&& in) {
    using T = std::ranges::range_value_t<InR>;
    std::span<const T> in_s{in};
    const auto remaining_bytes =
        detail::copy_out_any_word_aligned_portion<std::endian::little>(out, in_s);

    // copy remaining bytes as a partial word
    for (size_t i = 0; i < remaining_bytes; ++i) {
        out[i] = get_byte_var(sizeof(T) - 1 - i, in_s.front());
    }
}

}    // namespace Botan

namespace Botan {

enum class MD_Endian {
    Little,
    Big,
};

template <typename T>
concept md_hash_implementation =
    concepts::contiguous_container<typename T::digest_type> &&
    requires(typename T::digest_type& digest, std::span<const uint8_t> input, size_t blocks) {
        { T::init(digest) } -> std::same_as<void>;
        { T::compress_n(digest, input, blocks) } -> std::same_as<void>;
        T::bit_endianness;
        T::byte_endianness;
        T::block_bytes;
        T::output_bytes;
        T::ctr_bytes;
    } && T::block_bytes >= 64 && is_power_of_2(T::block_bytes) && T::output_bytes >= 16 &&
    T::ctr_bytes >= 8 && is_power_of_2(T::ctr_bytes) && T::ctr_bytes < T::block_bytes;

template <md_hash_implementation MD>
class MerkleDamgard_Hash final {
   public:
    MerkleDamgard_Hash() { clear(); }

    void update(std::span<const uint8_t> input) {
        BufferSlicer in(input);

        while (!in.empty()) {
            if (const auto one_block = m_buffer.handle_unaligned_data(in)) {
                MD::compress_n(m_digest, one_block.value(), 1);
            }

            if (m_buffer.in_alignment()) {
                const auto [aligned_data, full_blocks] = m_buffer.aligned_data_to_process(in);
                if (full_blocks > 0) {
                    MD::compress_n(m_digest, aligned_data, full_blocks);
                }
            }
        }

        m_count += input.size();
    }

    void final(std::span<uint8_t> output) {
        append_padding_bit();
        append_counter_and_finalize();
        copy_output(output);
        clear();
    }

    void clear() {
        MD::init(m_digest);
        m_buffer.clear();
        m_count = 0;
    }

   private:
    void append_padding_bit() {
        BOTAN_ASSERT_NOMSG(!m_buffer.ready_to_consume());
        if constexpr (MD::bit_endianness == MD_Endian::Big) {
            const uint8_t final_byte = 0x80;
            m_buffer.append({&final_byte, 1});
        } else {
            const uint8_t final_byte = 0x01;
            m_buffer.append({&final_byte, 1});
        }
    }

    void append_counter_and_finalize() {
        // Compress the remaining data if the final data block does not provide
        // enough space for the counter bytes.
        if (m_buffer.elements_until_alignment() < MD::ctr_bytes) {
            m_buffer.fill_up_with_zeros();
            MD::compress_n(m_digest, m_buffer.consume(), 1);
        }

        // Make sure that any remaining bytes in the very last block are zero.
        BOTAN_ASSERT_NOMSG(m_buffer.elements_until_alignment() >= MD::ctr_bytes);
        m_buffer.fill_up_with_zeros();

        // Replace a bunch of the right-most zero-padding with the counter bytes.
        const uint64_t bit_count = m_count * 8;
        auto last_bytes = m_buffer.directly_modify_last(sizeof(bit_count));
        if constexpr (MD::byte_endianness == MD_Endian::Big) {
            store_be(bit_count, last_bytes.data());
        } else {
            store_le(bit_count, last_bytes.data());
        }

        // Compress the very last block.
        MD::compress_n(m_digest, m_buffer.consume(), 1);
    }

    void copy_output(std::span<uint8_t> output) {
        BOTAN_ASSERT_NOMSG(output.size() >= MD::output_bytes);

        if constexpr (MD::byte_endianness == MD_Endian::Big) {
            copy_out_be(output.first(MD::output_bytes), m_digest);
        } else {
            copy_out_le(output.first(MD::output_bytes), m_digest);
        }
    }

   private:
    typename MD::digest_type m_digest;
    uint64_t m_count;

    AlignmentBuffer<uint8_t, MD::block_bytes> m_buffer;
};

}    // namespace Botan

#if defined(BOTAN_TARGET_OS_HAS_THREADS)
#include <thread>
#endif

namespace Botan::OS {

/*
 * This header is internal (not installed) and these functions are not
 * intended to be called by applications. However they are given public
 * visibility (using BOTAN_TEST_API macro) for the tests. This also probably
 * allows them to be overridden by the application on ELF systems, but
 * this hasn't been tested.
 */

/**
 * @return process ID assigned by the operating system.
 *
 * On Unix and Windows systems, this always returns a result
 *
 * On systems where there is no processes to speak of (for example on baremetal
 * systems or within a unikernel), this function returns zero.
 */
uint32_t BOTAN_TEST_API get_process_id();

/**
 * @return CPU processor clock, if available
 *
 * On Windows, calls QueryPerformanceCounter.
 *
 * Under GCC or Clang on supported platforms the hardware cycle counter is queried.
 * Currently supported processors are x86, PPC, Alpha, SPARC, IA-64, S/390x, and HP-PA.
 * If no CPU cycle counter is available on this system, returns zero.
 */
uint64_t BOTAN_TEST_API get_cpu_cycle_counter();

size_t BOTAN_TEST_API get_cpu_available();

/**
 * If this system supports getauxval (or an equivalent interface,
 * like FreeBSD's elf_aux_info) queries AT_HWCAP and AT_HWCAP2
 * and returns both.
 *
 * Otherwise returns nullopt.
 */
std::optional<std::pair<unsigned long, unsigned long>> get_auxval_hwcap();

/*
 * @return best resolution timestamp available
 *
 * The epoch and update rate of this clock is arbitrary and depending
 * on the hardware it may not tick at a constant rate.
 *
 * Uses hardware cycle counter, if available.
 * On POSIX platforms clock_gettime is used with a monotonic timer
 *
 * As a final fallback std::chrono::high_resolution_clock is used.
 *
 * On systems that are lacking a real time clock, this may return 0
 */
uint64_t BOTAN_TEST_API get_high_resolution_clock();

/**
 * @return system clock (reflecting wall clock) with best resolution
 * available, normalized to nanoseconds resolution, using Unix epoch.
 *
 * If the system does not have a real time clock this function will throw
 * Not_Implemented
 */
uint64_t BOTAN_TEST_API get_system_timestamp_ns();

/**
 * Format a time
 *
 * Converts the time_t to a local time representation,
 * then invokes std::put_time with the specified format.
 */
std::string BOTAN_TEST_API format_time(time_t time, const std::string& format);

/**
 * @return maximum amount of memory (in bytes) Botan could/should
 * hyptothetically allocate for the memory poool. Reads environment
 * variable "BOTAN_MLOCK_POOL_SIZE", set to "0" to disable pool.
 */
size_t get_memory_locking_limit();

/**
 * Return the size of a memory page, if that can be derived on the
 * current system. Otherwise returns some default value (eg 4096)
 */
size_t system_page_size();

/**
 * Read the value of an environment variable, setting it to value_out if it
 * exists.  Returns false and sets value_out to empty string if no such variable
 * is set. If the process seems to be running in a privileged state (such as
 * setuid) then always returns false and does not examine the environment.
 */
bool read_env_variable(std::string& value_out, std::string_view var_name);

/**
 * Read the value of an environment variable and convert it to an
 * integer. If not set or conversion fails, returns the default value.
 *
 * If the process seems to be running in a privileged state (such as setuid)
 * then always returns nullptr, similiar to glibc's secure_getenv.
 */
size_t read_env_variable_sz(std::string_view var_name, size_t def_value = 0);

/**
 * Request count pages of RAM which are locked into memory using mlock,
 * VirtualLock, or some similar OS specific API. Free it with free_locked_pages.
 *
 * Returns an empty list on failure. This function is allowed to return fewer
 * than count pages.
 *
 * The contents of the allocated pages are undefined.
 *
 * Each page is preceded by and followed by a page which is marked
 * as noaccess, such that accessing it will cause a crash. This turns
 * out of bound reads/writes into crash events.
 *
 * @param count requested number of locked pages
 */
std::vector<void*> allocate_locked_pages(size_t count);

/**
 * Free memory allocated by allocate_locked_pages
 * @param pages a list of pages returned by allocate_locked_pages
 */
void free_locked_pages(const std::vector<void*>& pages);

/**
 * Set the MMU to prohibit access to this page
 */
void page_prohibit_access(void* page);

/**
 * Set the MMU to allow R/W access to this page
 */
void page_allow_access(void* page);

/**
 * Set a ID to a page's range expressed by size bytes
 */
void page_named(void* page, size_t size);

#if defined(BOTAN_TARGET_OS_HAS_THREADS)
void set_thread_name(std::thread& thread, const std::string& name);
#endif

/**
 * Run a probe instruction to test for support for a CPU instruction.
 * Runs in system-specific env that catches illegal instructions; this
 * function always fails if the OS doesn't provide this.
 * Returns value of probe_fn, if it could run.
 * If error occurs, returns negative number.
 * This allows probe_fn to indicate errors of its own, if it wants.
 * For example the instruction might not only be only available on some
 * CPUs, but also buggy on some subset of these - the probe function
 * can test to make sure the instruction works properly before
 * indicating that the instruction is available.
 *
 * @warning on Unix systems uses signal handling in a way that is not
 * thread safe. It should only be called in a single-threaded context
 * (ie, at static init time).
 *
 * If probe_fn throws an exception the result is undefined.
 *
 * Return codes:
 * -1 illegal instruction detected
 */
int BOTAN_TEST_API run_cpu_instruction_probe(const std::function<int()>& probe_fn);

/**
 * Represents a terminal state
 */
class BOTAN_UNSTABLE_API Echo_Suppression {
   public:
    /**
     * Reenable echo on this terminal. Can be safely called
     * multiple times. May throw if an error occurs.
     */
    virtual void reenable_echo() = 0;

    /**
     * Implicitly calls reenable_echo, but swallows/ignored all
     * errors which would leave the terminal in an invalid state.
     */
    virtual ~Echo_Suppression() = default;
};

/**
 * Suppress echo on the terminal
 * Returns null if this operation is not supported on the current system.
 */
std::unique_ptr<Echo_Suppression> BOTAN_UNSTABLE_API suppress_echo_on_terminal();

}    // namespace Botan::OS

namespace Botan {

class SecureQueue;

/**
 * Container of output buffers for Pipe
 */
class Output_Buffers final {
   public:
    size_t read(uint8_t[], size_t, Pipe::message_id);
    size_t peek(uint8_t[], size_t, size_t, Pipe::message_id) const;
    size_t get_bytes_read(Pipe::message_id) const;
    size_t remaining(Pipe::message_id) const;

    void add(SecureQueue*);
    void retire();

    Pipe::message_id message_count() const;

    Output_Buffers();

   private:
    class SecureQueue* get(Pipe::message_id) const;

    std::deque<std::unique_ptr<SecureQueue>> m_buffers;
    Pipe::message_id m_offset;
};

}    // namespace Botan

namespace Botan {

/**
 * Parse a SCAN-style algorithm name
 * @param scan_name the name
 * @return the name components
 */
std::vector<std::string> parse_algorithm_name(std::string_view scan_name);

/**
 * Split a string
 * @param str the input string
 * @param delim the delimitor
 * @return string split by delim
 */
BOTAN_TEST_API std::vector<std::string> split_on(std::string_view str, char delim);

/**
 * Join a string
 * @param strs strings to join
 * @param delim the delimitor
 * @return string joined by delim
 */
std::string string_join(const std::vector<std::string>& strs, char delim);

/**
 * Convert a decimal string to a number
 * @param str the string to convert
 * @return number value of the string
 */
BOTAN_TEST_API uint32_t to_u32bit(std::string_view str);

/**
 * Convert a decimal string to a number
 * @param str the string to convert
 * @return number value of the string
 */
uint16_t to_uint16(std::string_view str);

/**
 * Convert a string representation of an IPv4 address to a number
 * @param ip_str the string representation
 * @return integer IPv4 address
 */
std::optional<uint32_t> BOTAN_TEST_API string_to_ipv4(std::string_view ip_str);

/**
 * Convert an IPv4 address to a string
 * @param ip_addr the IPv4 address to convert
 * @return string representation of the IPv4 address
 */
std::string BOTAN_TEST_API ipv4_to_string(uint32_t ip_addr);

std::map<std::string, std::string> read_cfg(std::istream& is);

/**
 * Accepts key value pairs deliminated by commas:
 *
 * "" (returns empty map)
 * "K=V" (returns map {'K': 'V'})
 * "K1=V1,K2=V2"
 * "K1=V1,K2=V2,K3=V3"
 * "K1=V1,K2=V2,K3=a_value\,with\,commas_and_\=equals"
 *
 * Values may be empty, keys must be non-empty and unique. Duplicate
 * keys cause an exception.
 *
 * Within both key and value, comma and equals can be escaped with
 * backslash. Backslash can also be escaped.
 */
BOTAN_TEST_API
std::map<std::string, std::string> read_kv(std::string_view kv);

std::string tolower_string(std::string_view s);

/**
 * Check if the given hostname is a match for the specified wildcard
 */
BOTAN_TEST_API
bool host_wildcard_match(std::string_view wildcard, std::string_view host);

/**
 * If name is a valid DNS name, return it canonicalized
 *
 * Otherwise throws Decoding_Error
 */
std::string check_and_canonicalize_dns_name(std::string_view name);

}    // namespace Botan

namespace Botan {

/**
 * Polynomial doubling in GF(2^n)
 */
void BOTAN_TEST_API poly_double_n(uint8_t out[], const uint8_t in[], size_t n);

/**
 * Returns true iff poly_double_n is implemented for this size.
 */
inline bool poly_double_supported_size(size_t n) {
    return (n == 8 || n == 16 || n == 24 || n == 32 || n == 64 || n == 128);
}

inline void poly_double_n(uint8_t buf[], size_t n) { return poly_double_n(buf, buf, n); }

/*
 * Little endian convention - used for XTS
 */
void BOTAN_TEST_API poly_double_n_le(uint8_t out[], const uint8_t in[], size_t n);

/*
 * Tweak block update step for XTS
 */
void xts_update_tweak_block(uint8_t tweak[], size_t BS, size_t n);

}    // namespace Botan

namespace Botan {

/**
 * Prefetch an array
 *
 * This function returns a uint64_t which is accumulated from values
 * read from the array. This may help confuse the compiler sufficiently
 * to not elide otherwise "useless" reads. The return value will always
 * be zero.
 */
uint64_t prefetch_array_raw(size_t bytes, const void* array) noexcept;

/**
 * Prefetch several arrays
 *
 * This function returns a uint64_t which is accumulated from values
 * read from the array. This may help confuse the compiler sufficiently
 * to not elide otherwise "useless" reads. The return value will always
 * be zero.
 */
template <typename T, size_t... Ns>
T prefetch_arrays(T (&... arr)[Ns]) noexcept
    requires std::is_integral<T>::value
{
    return (static_cast<T>(prefetch_array_raw(sizeof(T) * Ns, arr)) & ...);
}

}    // namespace Botan

namespace Botan {

/**
 * Bit rotation left by a compile-time constant amount
 * @param input the input word
 * @return input rotated left by ROT bits
 */
template <size_t ROT, typename T>
inline constexpr T rotl(T input)
    requires(ROT > 0 && ROT < 8 * sizeof(T))
{
    return static_cast<T>((input << ROT) | (input >> (8 * sizeof(T) - ROT)));
}

/**
 * Bit rotation right by a compile-time constant amount
 * @param input the input word
 * @return input rotated right by ROT bits
 */
template <size_t ROT, typename T>
inline constexpr T rotr(T input)
    requires(ROT > 0 && ROT < 8 * sizeof(T))
{
    return static_cast<T>((input >> ROT) | (input << (8 * sizeof(T) - ROT)));
}

/**
 * SHA-2 Sigma style function
 */
template <size_t R1, size_t R2, size_t S, typename T>
inline constexpr T sigma(T x) {
    return rotr<R1>(x) ^ rotr<R2>(x) ^ (x >> S);
}

/**
 * SHA-2 Sigma style function
 */
template <size_t R1, size_t R2, size_t R3, typename T>
inline constexpr T rho(T x) {
    return rotr<R1>(x) ^ rotr<R2>(x) ^ rotr<R3>(x);
}

/**
 * Bit rotation left, variable rotation amount
 * @param input the input word
 * @param rot the number of bits to rotate, must be between 0 and sizeof(T)*8-1
 * @return input rotated left by rot bits
 */
template <typename T>
inline constexpr T rotl_var(T input, size_t rot) {
    return rot ? static_cast<T>((input << rot) | (input >> (sizeof(T) * 8 - rot))) : input;
}

/**
 * Bit rotation right, variable rotation amount
 * @param input the input word
 * @param rot the number of bits to rotate, must be between 0 and sizeof(T)*8-1
 * @return input rotated right by rot bits
 */
template <typename T>
inline constexpr T rotr_var(T input, size_t rot) {
    return rot ? static_cast<T>((input >> rot) | (input << (sizeof(T) * 8 - rot))) : input;
}

}    // namespace Botan

namespace Botan {

/**
 * Integer rounding
 *
 * Returns an integer z such that n <= z <= n + align_to
 * and z % align_to == 0
 *
 * @param n an integer
 * @param align_to the alignment boundary
 * @return n rounded up to a multiple of align_to
 */
constexpr inline size_t round_up(size_t n, size_t align_to) {
    // Arguably returning n in this case would also be sensible
    BOTAN_ARG_CHECK(align_to != 0, "align_to must not be 0");

    if (n % align_to > 0) {
        const size_t adj = align_to - (n % align_to);
        BOTAN_ARG_CHECK(n + adj >= n, "Integer overflow during rounding");
        n += adj;
    }
    return n;
}

}    // namespace Botan

namespace Botan {

/**
A class encapsulating a SCAN name (similar to JCE conventions)
http://www.users.zetnet.co.uk/hopwood/crypto/scan/
*/
class SCAN_Name final {
   public:
    /**
     * Create a SCAN_Name
     * @param algo_spec A SCAN-format name
     */
    explicit SCAN_Name(const char* algo_spec);

    /**
     * Create a SCAN_Name
     * @param algo_spec A SCAN-format name
     */
    explicit SCAN_Name(std::string_view algo_spec);

    /**
     * @return original input string
     */
    const std::string& to_string() const { return m_orig_algo_spec; }

    /**
     * @return algorithm name
     */
    const std::string& algo_name() const { return m_alg_name; }

    /**
     * @return number of arguments
     */
    size_t arg_count() const { return m_args.size(); }

    /**
     * @param lower is the lower bound
     * @param upper is the upper bound
     * @return if the number of arguments is between lower and upper
     */
    bool arg_count_between(size_t lower, size_t upper) const {
        return ((arg_count() >= lower) && (arg_count() <= upper));
    }

    /**
     * @param i which argument
     * @return ith argument
     */
    std::string arg(size_t i) const;

    /**
     * @param i which argument
     * @param def_value the default value
     * @return ith argument or the default value
     */
    std::string arg(size_t i, std::string_view def_value) const;

    /**
     * @param i which argument
     * @param def_value the default value
     * @return ith argument as an integer, or the default value
     */
    size_t arg_as_integer(size_t i, size_t def_value) const;

    /**
     * @param i which argument
     * @return ith argument as an integer
     */
    size_t arg_as_integer(size_t i) const;

    /**
     * @return cipher mode (if any)
     */
    std::string cipher_mode() const { return (!m_mode_info.empty()) ? m_mode_info[0] : ""; }

    /**
     * @return cipher mode padding (if any)
     */
    std::string cipher_mode_pad() const { return (m_mode_info.size() >= 2) ? m_mode_info[1] : ""; }

   private:
    std::string m_orig_algo_spec;
    std::string m_alg_name;
    std::vector<std::string> m_args;
    std::vector<std::string> m_mode_info;
};

// This is unrelated but it is convenient to stash it here
template <typename T>
std::vector<std::string> probe_providers_of(std::string_view algo_spec,
                                            const std::vector<std::string>& possible = {"base"}) {
    std::vector<std::string> providers;
    for (auto&& prov : possible) {
        auto o = T::create(algo_spec, prov);
        if (o) {
            providers.push_back(prov);    // available
        }
    }
    return providers;
}

}    // namespace Botan

namespace Botan {

/**
 * A queue that knows how to zeroize itself
 */
class BOTAN_TEST_API SecureQueue final : public Fanout_Filter, public DataSource {
   public:
    std::string name() const override { return "Queue"; }

    void write(const uint8_t[], size_t) override;

    size_t read(uint8_t[], size_t) override;
    size_t peek(uint8_t[], size_t, size_t = 0) const override;
    size_t get_bytes_read() const override;

    bool end_of_data() const override;

    bool empty() const;

    bool check_available(size_t n) override { return n <= size(); }

    /**
     * @return number of bytes available in the queue
     */
    size_t size() const;

    bool attachable() override { return false; }

    /**
     * SecureQueue assignment
     * @param other the queue to copy
     */
    SecureQueue& operator=(const SecureQueue& other);

    SecureQueue& operator=(SecureQueue&& other) = delete;
    SecureQueue(SecureQueue&& other) = delete;

    /**
     * SecureQueue default constructor (creates empty queue)
     */
    SecureQueue();

    /**
     * SecureQueue copy constructor
     * @param other the queue to copy
     */
    SecureQueue(const SecureQueue& other);

    ~SecureQueue() override { destroy(); }

   private:
    void destroy();
    size_t m_bytes_read;
    class SecureQueueNode* m_head;
    class SecureQueueNode* m_tail;
};

}    // namespace Botan

namespace Botan {

/**
 * NIST's SHA-1
 */
class SHA_1 final : public HashFunction {
   public:
    using digest_type = secure_vector<uint32_t>;

    static constexpr MD_Endian byte_endianness = MD_Endian::Big;
    static constexpr MD_Endian bit_endianness = MD_Endian::Big;
    static constexpr size_t block_bytes = 64;
    static constexpr size_t output_bytes = 20;
    static constexpr size_t ctr_bytes = 8;

    static void compress_n(digest_type& digest, std::span<const uint8_t> input, size_t blocks);
    static void init(digest_type& digest);

   public:
    std::string name() const override { return "SHA-1"; }

    size_t output_length() const override { return 20; }

    size_t hash_block_size() const override { return block_bytes; }

    std::unique_ptr<HashFunction> new_object() const override;

    std::unique_ptr<HashFunction> copy_state() const override;

    std::string provider() const override;

    void clear() override { m_md.clear(); }

#if defined(BOTAN_HAS_SHA1_ARMV8)
    static void sha1_armv8_compress_n(digest_type& digest, std::span<const uint8_t> blocks,
                                      size_t block_count);
#endif

#if defined(BOTAN_HAS_SHA1_SIMD_4X32)
    static void simd_compress_n(digest_type& digest, std::span<const uint8_t> blocks,
                                size_t block_count);
#endif

#if defined(BOTAN_HAS_SHA1_X86_SHA_NI)
    // Using x86 SHA instructions in Intel Goldmont and Cannonlake
    static void sha1_compress_x86(digest_type& digest, std::span<const uint8_t> blocks,
                                  size_t block_count);
#endif

   private:
    void add_data(std::span<const uint8_t> input) override;

    void final_result(std::span<uint8_t> output) override;

   private:
    MerkleDamgard_Hash<SHA_1> m_md;
};

}    // namespace Botan

#if defined(BOTAN_HAS_STREAM_CIPHER)
#endif

namespace Botan {

#if defined(BOTAN_HAS_STREAM_CIPHER)

class Stream_Cipher_Mode final : public Cipher_Mode {
   public:
    /**
     * @param cipher underyling stream cipher
     */
    explicit Stream_Cipher_Mode(std::unique_ptr<StreamCipher> cipher)
        : m_cipher(std::move(cipher)) {}

    size_t output_length(size_t input_length) const override { return input_length; }

    size_t update_granularity() const override { return 1; }

    size_t ideal_granularity() const override {
        const size_t buf_size = m_cipher->buffer_size();
        BOTAN_ASSERT_NOMSG(buf_size > 0);
        if (buf_size >= 256) {
            return buf_size;
        }
        return buf_size * (256 / buf_size);
    }

    size_t minimum_final_size() const override { return 0; }

    size_t default_nonce_length() const override { return 0; }

    bool valid_nonce_length(size_t nonce_len) const override {
        return m_cipher->valid_iv_length(nonce_len);
    }

    Key_Length_Specification key_spec() const override { return m_cipher->key_spec(); }

    std::string name() const override { return m_cipher->name(); }

    void clear() override {
        m_cipher->clear();
        reset();
    }

    void reset() override { /* no msg state */ }

    bool has_keying_material() const override { return m_cipher->has_keying_material(); }

   private:
    void start_msg(const uint8_t nonce[], size_t nonce_len) override {
        if (nonce_len > 0) {
            m_cipher->set_iv(nonce, nonce_len);
        }
    }

    size_t process_msg(uint8_t buf[], size_t sz) override {
        m_cipher->cipher1(buf, sz);
        return sz;
    }

    void finish_msg(secure_vector<uint8_t>& buf, size_t offset) override {
        return update(buf, offset);
    }

    void key_schedule(std::span<const uint8_t> key) override { m_cipher->set_key(key); }

    std::unique_ptr<StreamCipher> m_cipher;
};

#endif

}    // namespace Botan

#if defined(BOTAN_HAS_OS_UTILS)
#endif

namespace Botan {

template <typename F>
uint64_t measure_cost(std::chrono::milliseconds trial_msec, F func) {
#if defined(BOTAN_HAS_OS_UTILS)
    const uint64_t trial_nsec =
        std::chrono::duration_cast<std::chrono::nanoseconds>(trial_msec).count();

    uint64_t total_nsec = 0;
    uint64_t trials = 0;

    auto trial_start = OS::get_system_timestamp_ns();

    for (;;) {
        const auto start = OS::get_system_timestamp_ns();
        func();
        const auto end = OS::get_system_timestamp_ns();

        if (end >= start) {
            total_nsec += (end - start);
            trials += 1;

            if ((end - trial_start) >= trial_nsec) {
                return (total_nsec / trials);
            }
        }
    }

#else
    BOTAN_UNUSED(trial_msec, func);
    throw Not_Implemented("No system clock available");
#endif
}

}    // namespace Botan
#define BOTAN_FULL_VERSION_STRING \
    "Botan 3.8.1 (release, dated 20250507, revision git:c06967092edd9baa2b42f89135380912113ae209)"

#define BOTAN_SHORT_VERSION_STRING "3.8.1"

#define BOTAN_VC_REVISION "git:c06967092edd9baa2b42f89135380912113ae209"
/*
 * (C) 1999-2010,2015,2017,2018,2020 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_CPUID)
#endif

#if defined(BOTAN_HAS_AES_POWER8) || defined(BOTAN_HAS_AES_ARMV8) || defined(BOTAN_HAS_AES_NI)
#define BOTAN_HAS_HW_AES_SUPPORT
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
#include <bit>
#endif

namespace Botan {

/*
 * One of three AES implementation strategies are used to get a constant time
 * implementation which is immune to common cache/timing based side channels:
 *
 * - If AES hardware support is available (AES-NI, POWER8, Aarch64) use that
 *
 * - If 128-bit SIMD with byte shuffles are available (SSSE3, NEON, or Altivec),
 *   use the vperm technique published by Mike Hamburg at CHES 2009.
 *
 * - If no hardware or SIMD support, fall back to a constant time bitsliced
 *   implementation. This uses 32-bit words resulting in 2 blocks being processed
 *   in parallel. Moving to 4 blocks (with 64-bit words) would approximately
 *   double performance on 64-bit CPUs. Likewise moving to 128 bit SIMD would
 *   again approximately double performance vs 64-bit. However the assumption is
 *   that most 64-bit CPUs either have hardware AES or SIMD shuffle support and
 *   that the majority of users falling back to this code will be 32-bit cores.
 *   If this assumption proves to be unsound, the bitsliced code can easily be
 *   extended to operate on either 32 or 64 bit words depending on the native
 *   wordsize of the target processor.
 *
 * Useful references
 *
 * - "Accelerating AES with Vector Permute Instructions" Mike Hamburg
 *   https://www.shiftleft.org/papers/vector_aes/vector_aes.pdf
 *
 * - "Faster and Timing-Attack Resistant AES-GCM" Käsper and Schwabe
 *   https://eprint.iacr.org/2009/129.pdf
 *
 * - "A new combinational logic minimization technique with applications to cryptology."
 *   Boyar and Peralta https://eprint.iacr.org/2009/191.pdf
 *
 * - "A depth-16 circuit for the AES S-box" Boyar and Peralta
 *    https://eprint.iacr.org/2011/332.pdf
 *
 * - "A Very Compact S-box for AES" Canright
 *   https://www.iacr.org/archive/ches2005/032.pdf
 *   https://core.ac.uk/download/pdf/36694529.pdf (extended)
 */

namespace {

/*
This is an AES sbox circuit which can execute in bitsliced mode up to 32x in
parallel.

The circuit is from the "Circuit Minimization Team" group
http://www.cs.yale.edu/homes/peralta/CircuitStuff/CMT.html
http://www.cs.yale.edu/homes/peralta/CircuitStuff/SLP_AES_113.txt

This circuit has size 113 and depth 27. In software it is much faster than
circuits which are considered faster for hardware purposes (where circuit depth
is the critical constraint), because unlike in hardware, on common CPUs we can
only execute - at best - 3 or 4 logic operations per cycle. So a smaller circuit
is superior. On an x86-64 machine this circuit is about 15% faster than the
circuit of size 128 and depth 16 given in "A depth-16 circuit for the AES S-box".

Another circuit for AES Sbox of size 102 and depth 24 is describted in "New
Circuit Minimization Techniques for Smaller and Faster AES SBoxes"
[https://eprint.iacr.org/2019/802] however it relies on "non-standard" gates
like MUX, NOR, NAND, etc and so in practice in bitsliced software, its size is
actually a bit larger than this circuit, as few CPUs have such instructions and
otherwise they must be emulated using a sequence of available bit operations.
*/
void AES_SBOX(uint32_t V[8]) {
    const uint32_t U0 = V[0];
    const uint32_t U1 = V[1];
    const uint32_t U2 = V[2];
    const uint32_t U3 = V[3];
    const uint32_t U4 = V[4];
    const uint32_t U5 = V[5];
    const uint32_t U6 = V[6];
    const uint32_t U7 = V[7];

    const uint32_t y14 = U3 ^ U5;
    const uint32_t y13 = U0 ^ U6;
    const uint32_t y9 = U0 ^ U3;
    const uint32_t y8 = U0 ^ U5;
    const uint32_t t0 = U1 ^ U2;
    const uint32_t y1 = t0 ^ U7;
    const uint32_t y4 = y1 ^ U3;
    const uint32_t y12 = y13 ^ y14;
    const uint32_t y2 = y1 ^ U0;
    const uint32_t y5 = y1 ^ U6;
    const uint32_t y3 = y5 ^ y8;
    const uint32_t t1 = U4 ^ y12;
    const uint32_t y15 = t1 ^ U5;
    const uint32_t y20 = t1 ^ U1;
    const uint32_t y6 = y15 ^ U7;
    const uint32_t y10 = y15 ^ t0;
    const uint32_t y11 = y20 ^ y9;
    const uint32_t y7 = U7 ^ y11;
    const uint32_t y17 = y10 ^ y11;
    const uint32_t y19 = y10 ^ y8;
    const uint32_t y16 = t0 ^ y11;
    const uint32_t y21 = y13 ^ y16;
    const uint32_t y18 = U0 ^ y16;
    const uint32_t t2 = y12 & y15;
    const uint32_t t3 = y3 & y6;
    const uint32_t t4 = t3 ^ t2;
    const uint32_t t5 = y4 & U7;
    const uint32_t t6 = t5 ^ t2;
    const uint32_t t7 = y13 & y16;
    const uint32_t t8 = y5 & y1;
    const uint32_t t9 = t8 ^ t7;
    const uint32_t t10 = y2 & y7;
    const uint32_t t11 = t10 ^ t7;
    const uint32_t t12 = y9 & y11;
    const uint32_t t13 = y14 & y17;
    const uint32_t t14 = t13 ^ t12;
    const uint32_t t15 = y8 & y10;
    const uint32_t t16 = t15 ^ t12;
    const uint32_t t17 = t4 ^ y20;
    const uint32_t t18 = t6 ^ t16;
    const uint32_t t19 = t9 ^ t14;
    const uint32_t t20 = t11 ^ t16;
    const uint32_t t21 = t17 ^ t14;
    const uint32_t t22 = t18 ^ y19;
    const uint32_t t23 = t19 ^ y21;
    const uint32_t t24 = t20 ^ y18;
    const uint32_t t25 = t21 ^ t22;
    const uint32_t t26 = t21 & t23;
    const uint32_t t27 = t24 ^ t26;
    const uint32_t t28 = t25 & t27;
    const uint32_t t29 = t28 ^ t22;
    const uint32_t t30 = t23 ^ t24;
    const uint32_t t31 = t22 ^ t26;
    const uint32_t t32 = t31 & t30;
    const uint32_t t33 = t32 ^ t24;
    const uint32_t t34 = t23 ^ t33;
    const uint32_t t35 = t27 ^ t33;
    const uint32_t t36 = t24 & t35;
    const uint32_t t37 = t36 ^ t34;
    const uint32_t t38 = t27 ^ t36;
    const uint32_t t39 = t29 & t38;
    const uint32_t t40 = t25 ^ t39;
    const uint32_t t41 = t40 ^ t37;
    const uint32_t t42 = t29 ^ t33;
    const uint32_t t43 = t29 ^ t40;
    const uint32_t t44 = t33 ^ t37;
    const uint32_t t45 = t42 ^ t41;
    const uint32_t z0 = t44 & y15;
    const uint32_t z1 = t37 & y6;
    const uint32_t z2 = t33 & U7;
    const uint32_t z3 = t43 & y16;
    const uint32_t z4 = t40 & y1;
    const uint32_t z5 = t29 & y7;
    const uint32_t z6 = t42 & y11;
    const uint32_t z7 = t45 & y17;
    const uint32_t z8 = t41 & y10;
    const uint32_t z9 = t44 & y12;
    const uint32_t z10 = t37 & y3;
    const uint32_t z11 = t33 & y4;
    const uint32_t z12 = t43 & y13;
    const uint32_t z13 = t40 & y5;
    const uint32_t z14 = t29 & y2;
    const uint32_t z15 = t42 & y9;
    const uint32_t z16 = t45 & y14;
    const uint32_t z17 = t41 & y8;
    const uint32_t tc1 = z15 ^ z16;
    const uint32_t tc2 = z10 ^ tc1;
    const uint32_t tc3 = z9 ^ tc2;
    const uint32_t tc4 = z0 ^ z2;
    const uint32_t tc5 = z1 ^ z0;
    const uint32_t tc6 = z3 ^ z4;
    const uint32_t tc7 = z12 ^ tc4;
    const uint32_t tc8 = z7 ^ tc6;
    const uint32_t tc9 = z8 ^ tc7;
    const uint32_t tc10 = tc8 ^ tc9;
    const uint32_t tc11 = tc6 ^ tc5;
    const uint32_t tc12 = z3 ^ z5;
    const uint32_t tc13 = z13 ^ tc1;
    const uint32_t tc14 = tc4 ^ tc12;
    const uint32_t S3 = tc3 ^ tc11;
    const uint32_t tc16 = z6 ^ tc8;
    const uint32_t tc17 = z14 ^ tc10;
    const uint32_t tc18 = ~tc13 ^ tc14;
    const uint32_t S7 = z12 ^ tc18;
    const uint32_t tc20 = z15 ^ tc16;
    const uint32_t tc21 = tc2 ^ z11;
    const uint32_t S0 = tc3 ^ tc16;
    const uint32_t S6 = tc10 ^ tc18;
    const uint32_t S4 = tc14 ^ S3;
    const uint32_t S1 = ~(S3 ^ tc16);
    const uint32_t tc26 = tc17 ^ tc20;
    const uint32_t S2 = ~(tc26 ^ z17);
    const uint32_t S5 = tc21 ^ tc17;

    V[0] = S0;
    V[1] = S1;
    V[2] = S2;
    V[3] = S3;
    V[4] = S4;
    V[5] = S5;
    V[6] = S6;
    V[7] = S7;
}

/*
A circuit for inverse AES Sbox of size 121 and depth 21 from
http://www.cs.yale.edu/homes/peralta/CircuitStuff/CMT.html
http://www.cs.yale.edu/homes/peralta/CircuitStuff/Sinv.txt
*/
void AES_INV_SBOX(uint32_t V[8]) {
    const uint32_t U0 = V[0];
    const uint32_t U1 = V[1];
    const uint32_t U2 = V[2];
    const uint32_t U3 = V[3];
    const uint32_t U4 = V[4];
    const uint32_t U5 = V[5];
    const uint32_t U6 = V[6];
    const uint32_t U7 = V[7];

    const uint32_t Y0 = U0 ^ U3;
    const uint32_t Y2 = ~(U1 ^ U3);
    const uint32_t Y4 = U0 ^ Y2;
    const uint32_t RTL0 = U6 ^ U7;
    const uint32_t Y1 = Y2 ^ RTL0;
    const uint32_t Y7 = ~(U2 ^ Y1);
    const uint32_t RTL1 = U3 ^ U4;
    const uint32_t Y6 = ~(U7 ^ RTL1);
    const uint32_t Y3 = Y1 ^ RTL1;
    const uint32_t RTL2 = ~(U0 ^ U2);
    const uint32_t Y5 = U5 ^ RTL2;
    const uint32_t sa1 = Y0 ^ Y2;
    const uint32_t sa0 = Y1 ^ Y3;
    const uint32_t sb1 = Y4 ^ Y6;
    const uint32_t sb0 = Y5 ^ Y7;
    const uint32_t ah = Y0 ^ Y1;
    const uint32_t al = Y2 ^ Y3;
    const uint32_t aa = sa0 ^ sa1;
    const uint32_t bh = Y4 ^ Y5;
    const uint32_t bl = Y6 ^ Y7;
    const uint32_t bb = sb0 ^ sb1;
    const uint32_t ab20 = sa0 ^ sb0;
    const uint32_t ab22 = al ^ bl;
    const uint32_t ab23 = Y3 ^ Y7;
    const uint32_t ab21 = sa1 ^ sb1;
    const uint32_t abcd1 = ah & bh;
    const uint32_t rr1 = Y0 & Y4;
    const uint32_t ph11 = ab20 ^ abcd1;
    const uint32_t t01 = Y1 & Y5;
    const uint32_t ph01 = t01 ^ abcd1;
    const uint32_t abcd2 = al & bl;
    const uint32_t r1 = Y2 & Y6;
    const uint32_t pl11 = ab22 ^ abcd2;
    const uint32_t r2 = Y3 & Y7;
    const uint32_t pl01 = r2 ^ abcd2;
    const uint32_t r3 = sa0 & sb0;
    const uint32_t vr1 = aa & bb;
    const uint32_t pr1 = vr1 ^ r3;
    const uint32_t wr1 = sa1 & sb1;
    const uint32_t qr1 = wr1 ^ r3;
    const uint32_t ab0 = ph11 ^ rr1;
    const uint32_t ab1 = ph01 ^ ab21;
    const uint32_t ab2 = pl11 ^ r1;
    const uint32_t ab3 = pl01 ^ qr1;
    const uint32_t cp1 = ab0 ^ pr1;
    const uint32_t cp2 = ab1 ^ qr1;
    const uint32_t cp3 = ab2 ^ pr1;
    const uint32_t cp4 = ab3 ^ ab23;
    const uint32_t tinv1 = cp3 ^ cp4;
    const uint32_t tinv2 = cp3 & cp1;
    const uint32_t tinv3 = cp2 ^ tinv2;
    const uint32_t tinv4 = cp1 ^ cp2;
    const uint32_t tinv5 = cp4 ^ tinv2;
    const uint32_t tinv6 = tinv5 & tinv4;
    const uint32_t tinv7 = tinv3 & tinv1;
    const uint32_t d2 = cp4 ^ tinv7;
    const uint32_t d0 = cp2 ^ tinv6;
    const uint32_t tinv8 = cp1 & cp4;
    const uint32_t tinv9 = tinv4 & tinv8;
    const uint32_t tinv10 = tinv4 ^ tinv2;
    const uint32_t d1 = tinv9 ^ tinv10;
    const uint32_t tinv11 = cp2 & cp3;
    const uint32_t tinv12 = tinv1 & tinv11;
    const uint32_t tinv13 = tinv1 ^ tinv2;
    const uint32_t d3 = tinv12 ^ tinv13;
    const uint32_t sd1 = d1 ^ d3;
    const uint32_t sd0 = d0 ^ d2;
    const uint32_t dl = d0 ^ d1;
    const uint32_t dh = d2 ^ d3;
    const uint32_t dd = sd0 ^ sd1;
    const uint32_t abcd3 = dh & bh;
    const uint32_t rr2 = d3 & Y4;
    const uint32_t t02 = d2 & Y5;
    const uint32_t abcd4 = dl & bl;
    const uint32_t r4 = d1 & Y6;
    const uint32_t r5 = d0 & Y7;
    const uint32_t r6 = sd0 & sb0;
    const uint32_t vr2 = dd & bb;
    const uint32_t wr2 = sd1 & sb1;
    const uint32_t abcd5 = dh & ah;
    const uint32_t r7 = d3 & Y0;
    const uint32_t r8 = d2 & Y1;
    const uint32_t abcd6 = dl & al;
    const uint32_t r9 = d1 & Y2;
    const uint32_t r10 = d0 & Y3;
    const uint32_t r11 = sd0 & sa0;
    const uint32_t vr3 = dd & aa;
    const uint32_t wr3 = sd1 & sa1;
    const uint32_t ph12 = rr2 ^ abcd3;
    const uint32_t ph02 = t02 ^ abcd3;
    const uint32_t pl12 = r4 ^ abcd4;
    const uint32_t pl02 = r5 ^ abcd4;
    const uint32_t pr2 = vr2 ^ r6;
    const uint32_t qr2 = wr2 ^ r6;
    const uint32_t p0 = ph12 ^ pr2;
    const uint32_t p1 = ph02 ^ qr2;
    const uint32_t p2 = pl12 ^ pr2;
    const uint32_t p3 = pl02 ^ qr2;
    const uint32_t ph13 = r7 ^ abcd5;
    const uint32_t ph03 = r8 ^ abcd5;
    const uint32_t pl13 = r9 ^ abcd6;
    const uint32_t pl03 = r10 ^ abcd6;
    const uint32_t pr3 = vr3 ^ r11;
    const uint32_t qr3 = wr3 ^ r11;
    const uint32_t p4 = ph13 ^ pr3;
    const uint32_t S7 = ph03 ^ qr3;
    const uint32_t p6 = pl13 ^ pr3;
    const uint32_t p7 = pl03 ^ qr3;
    const uint32_t S3 = p1 ^ p6;
    const uint32_t S6 = p2 ^ p6;
    const uint32_t S0 = p3 ^ p6;
    const uint32_t X11 = p0 ^ p2;
    const uint32_t S5 = S0 ^ X11;
    const uint32_t X13 = p4 ^ p7;
    const uint32_t X14 = X11 ^ X13;
    const uint32_t S1 = S3 ^ X14;
    const uint32_t X16 = p1 ^ S7;
    const uint32_t S2 = X14 ^ X16;
    const uint32_t X18 = p0 ^ p4;
    const uint32_t X19 = S5 ^ X16;
    const uint32_t S4 = X18 ^ X19;

    V[0] = S0;
    V[1] = S1;
    V[2] = S2;
    V[3] = S3;
    V[4] = S4;
    V[5] = S5;
    V[6] = S6;
    V[7] = S7;
}

inline void bit_transpose(uint32_t B[8]) {
    swap_bits<uint32_t>(B[1], B[0], 0x55555555, 1);
    swap_bits<uint32_t>(B[3], B[2], 0x55555555, 1);
    swap_bits<uint32_t>(B[5], B[4], 0x55555555, 1);
    swap_bits<uint32_t>(B[7], B[6], 0x55555555, 1);

    swap_bits<uint32_t>(B[2], B[0], 0x33333333, 2);
    swap_bits<uint32_t>(B[3], B[1], 0x33333333, 2);
    swap_bits<uint32_t>(B[6], B[4], 0x33333333, 2);
    swap_bits<uint32_t>(B[7], B[5], 0x33333333, 2);

    swap_bits<uint32_t>(B[4], B[0], 0x0F0F0F0F, 4);
    swap_bits<uint32_t>(B[5], B[1], 0x0F0F0F0F, 4);
    swap_bits<uint32_t>(B[6], B[2], 0x0F0F0F0F, 4);
    swap_bits<uint32_t>(B[7], B[3], 0x0F0F0F0F, 4);
}

inline void ks_expand(uint32_t B[8], const uint32_t K[], size_t r) {
    /*
    This is bit_transpose of K[r..r+4] || K[r..r+4], we can save some computation
    due to knowing the first and second halves are the same data.
    */
    for (size_t i = 0; i != 4; ++i) {
        B[i] = K[r + i];
    }

    swap_bits<uint32_t>(B[1], B[0], 0x55555555, 1);
    swap_bits<uint32_t>(B[3], B[2], 0x55555555, 1);

    swap_bits<uint32_t>(B[2], B[0], 0x33333333, 2);
    swap_bits<uint32_t>(B[3], B[1], 0x33333333, 2);

    B[4] = B[0];
    B[5] = B[1];
    B[6] = B[2];
    B[7] = B[3];

    swap_bits<uint32_t>(B[4], B[0], 0x0F0F0F0F, 4);
    swap_bits<uint32_t>(B[5], B[1], 0x0F0F0F0F, 4);
    swap_bits<uint32_t>(B[6], B[2], 0x0F0F0F0F, 4);
    swap_bits<uint32_t>(B[7], B[3], 0x0F0F0F0F, 4);
}

inline void shift_rows(uint32_t B[8]) {
    // 3 0 1 2 7 4 5 6 10 11 8 9 14 15 12 13 17 18 19 16 21 22 23 20 24 25 26 27 28 29 30 31
    if constexpr (HasNative64BitRegisters) {
        for (size_t i = 0; i != 8; i += 2) {
            uint64_t x = (static_cast<uint64_t>(B[i]) << 32) | B[i + 1];
            x = bit_permute_step<uint64_t>(x, 0x0022331100223311, 2);
            x = bit_permute_step<uint64_t>(x, 0x0055005500550055, 1);
            B[i] = static_cast<uint32_t>(x >> 32);
            B[i + 1] = static_cast<uint32_t>(x);
        }
    } else {
        for (size_t i = 0; i != 8; ++i) {
            uint32_t x = B[i];
            x = bit_permute_step<uint32_t>(x, 0x00223311, 2);
            x = bit_permute_step<uint32_t>(x, 0x00550055, 1);
            B[i] = x;
        }
    }
}

inline void inv_shift_rows(uint32_t B[8]) {
    // Inverse of shift_rows, just inverting the steps

    if constexpr (HasNative64BitRegisters) {
        for (size_t i = 0; i != 8; i += 2) {
            uint64_t x = (static_cast<uint64_t>(B[i]) << 32) | B[i + 1];
            x = bit_permute_step<uint64_t>(x, 0x0055005500550055, 1);
            x = bit_permute_step<uint64_t>(x, 0x0022331100223311, 2);
            B[i] = static_cast<uint32_t>(x >> 32);
            B[i + 1] = static_cast<uint32_t>(x);
        }
    } else {
        for (size_t i = 0; i != 8; ++i) {
            uint32_t x = B[i];
            x = bit_permute_step<uint32_t>(x, 0x00550055, 1);
            x = bit_permute_step<uint32_t>(x, 0x00223311, 2);
            B[i] = x;
        }
    }
}

inline void mix_columns(uint32_t B[8]) {
    // carry high bits in B[0] to positions in 0x1b == 0b11011
    const uint32_t X2[8] = {
        B[1], B[2], B[3], B[4] ^ B[0], B[5] ^ B[0], B[6], B[7] ^ B[0], B[0],
    };

    for (size_t i = 0; i != 8; i++) {
        const uint32_t X3 = B[i] ^ X2[i];
        B[i] = X2[i] ^ rotr<8>(B[i]) ^ rotr<16>(B[i]) ^ rotr<24>(X3);
    }
}

void inv_mix_columns(uint32_t B[8]) {
    /*
    OpenSSL's bsaes implementation credits Jussi Kivilinna with the lovely
    matrix decomposition

    | 0e 0b 0d 09 |   | 02 03 01 01 |   | 05 00 04 00 |
    | 09 0e 0b 0d | = | 01 02 03 01 | x | 00 05 00 04 |
    | 0d 09 0e 0b |   | 01 01 02 03 |   | 04 00 05 00 |
    | 0b 0d 09 0e |   | 03 01 01 02 |   | 00 04 00 05 |

    Notice the first component is simply the MixColumns matrix. So we can
    multiply first by (05,00,04,00) then perform MixColumns to get the equivalent
    of InvMixColumn.
    */
    const uint32_t X4[8] = {
        B[2], B[3], B[4] ^ B[0], B[5] ^ B[0] ^ B[1], B[6] ^ B[1], B[7] ^ B[0], B[0] ^ B[1], B[1],
    };

    for (size_t i = 0; i != 8; i++) {
        const uint32_t X5 = X4[i] ^ B[i];
        B[i] = X5 ^ rotr<16>(X4[i]);
    }

    mix_columns(B);
}

/*
 * AES Encryption
 */
void aes_encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks,
                   const secure_vector<uint32_t>& EK) {
    BOTAN_ASSERT(EK.size() == 44 || EK.size() == 52 || EK.size() == 60, "Key was set");

    const size_t rounds = (EK.size() - 4) / 4;

    uint32_t KS[13 * 8] = {0};    // actual maximum is (rounds - 1) * 8
    for (size_t i = 0; i < rounds - 1; i += 1) {
        ks_expand(&KS[8 * i], EK.data(), 4 * i + 4);
    }

    const size_t BLOCK_SIZE = 16;
    const size_t BITSLICED_BLOCKS = 8 * sizeof(uint32_t) / BLOCK_SIZE;

    while (blocks > 0) {
        const size_t this_loop = std::min(blocks, BITSLICED_BLOCKS);

        uint32_t B[8] = {0};

        load_be(B, in, this_loop * 4);

        CT::poison(B, 8);

        for (size_t i = 0; i != 8; ++i) {
            B[i] ^= EK[i % 4];
        }

        bit_transpose(B);

        for (size_t r = 0; r != rounds - 1; ++r) {
            AES_SBOX(B);
            shift_rows(B);
            mix_columns(B);

            for (size_t i = 0; i != 8; ++i) {
                B[i] ^= KS[8 * r + i];
            }
        }

        // Final round:
        AES_SBOX(B);
        shift_rows(B);
        bit_transpose(B);

        for (size_t i = 0; i != 8; ++i) {
            B[i] ^= EK[4 * rounds + i % 4];
        }

        CT::unpoison(B, 8);

        copy_out_be(std::span(out, this_loop * 4 * sizeof(uint32_t)), B);

        in += this_loop * BLOCK_SIZE;
        out += this_loop * BLOCK_SIZE;
        blocks -= this_loop;
    }
}

/*
 * AES Decryption
 */
void aes_decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks,
                   const secure_vector<uint32_t>& DK) {
    BOTAN_ASSERT(DK.size() == 44 || DK.size() == 52 || DK.size() == 60, "Key was set");

    const size_t rounds = (DK.size() - 4) / 4;

    uint32_t KS[13 * 8] = {0};    // actual maximum is (rounds - 1) * 8
    for (size_t i = 0; i < rounds - 1; i += 1) {
        ks_expand(&KS[8 * i], DK.data(), 4 * i + 4);
    }

    const size_t BLOCK_SIZE = 16;
    const size_t BITSLICED_BLOCKS = 8 * sizeof(uint32_t) / BLOCK_SIZE;

    while (blocks > 0) {
        const size_t this_loop = std::min(blocks, BITSLICED_BLOCKS);

        uint32_t B[8] = {0};

        CT::poison(B, 8);

        load_be(B, in, this_loop * 4);

        for (size_t i = 0; i != 8; ++i) {
            B[i] ^= DK[i % 4];
        }

        bit_transpose(B);

        for (size_t r = 0; r != rounds - 1; ++r) {
            AES_INV_SBOX(B);
            inv_shift_rows(B);
            inv_mix_columns(B);

            for (size_t i = 0; i != 8; ++i) {
                B[i] ^= KS[8 * r + i];
            }
        }

        // Final round:
        AES_INV_SBOX(B);
        inv_shift_rows(B);
        bit_transpose(B);

        for (size_t i = 0; i != 8; ++i) {
            B[i] ^= DK[4 * rounds + i % 4];
        }

        CT::unpoison(B, 8);

        copy_out_be(std::span(out, this_loop * 4 * sizeof(uint32_t)), B);

        in += this_loop * BLOCK_SIZE;
        out += this_loop * BLOCK_SIZE;
        blocks -= this_loop;
    }
}

inline uint32_t xtime32(uint32_t s) {
    const uint32_t lo_bit = 0x01010101;
    const uint32_t mask = 0x7F7F7F7F;
    const uint32_t poly = 0x1B;

    return ((s & mask) << 1) ^ (((s >> 7) & lo_bit) * poly);
}

inline uint32_t InvMixColumn(uint32_t s1) {
    const uint32_t s2 = xtime32(s1);
    const uint32_t s4 = xtime32(s2);
    const uint32_t s8 = xtime32(s4);
    const uint32_t s9 = s8 ^ s1;
    const uint32_t s11 = s9 ^ s2;
    const uint32_t s13 = s9 ^ s4;
    const uint32_t s14 = s8 ^ s4 ^ s2;

    return s14 ^ rotr<8>(s9) ^ rotr<16>(s13) ^ rotr<24>(s11);
}

void InvMixColumn_x4(uint32_t x[4]) {
    x[0] = InvMixColumn(x[0]);
    x[1] = InvMixColumn(x[1]);
    x[2] = InvMixColumn(x[2]);
    x[3] = InvMixColumn(x[3]);
}

uint32_t SE_word(uint32_t x) {
    uint32_t I[8] = {0};

    for (size_t i = 0; i != 8; ++i) {
        I[i] = (x >> (7 - i)) & 0x01010101;
    }

    AES_SBOX(I);

    x = 0;

    for (size_t i = 0; i != 8; ++i) {
        x |= ((I[i] & 0x01010101) << (7 - i));
    }

    return x;
}

void aes_key_schedule(const uint8_t key[], size_t length, secure_vector<uint32_t>& EK,
                      secure_vector<uint32_t>& DK, bool bswap_keys = false) {
    static const uint32_t RC[10] = {0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
                                    0x20000000, 0x40000000, 0x80000000, 0x1B000000, 0x36000000};

    const size_t X = length / 4;

    // Can't happen, but make static analyzers happy
    BOTAN_ASSERT_NOMSG(X == 4 || X == 6 || X == 8);

    const size_t rounds = (length / 4) + 6;

    // Help the optimizer
    BOTAN_ASSERT_NOMSG(rounds == 10 || rounds == 12 || rounds == 14);

    CT::poison(key, length);

    EK.resize(length + 28);
    DK.resize(length + 28);

    for (size_t i = 0; i != X; ++i) {
        EK[i] = load_be<uint32_t>(key, i);
    }

    for (size_t i = X; i < 4 * (rounds + 1); i += X) {
        EK[i] = EK[i - X] ^ RC[(i - X) / X] ^ rotl<8>(SE_word(EK[i - 1]));

        for (size_t j = 1; j != X && (i + j) < EK.size(); ++j) {
            EK[i + j] = EK[i + j - X];

            if (X == 8 && j == 4) {
                EK[i + j] ^= SE_word(EK[i + j - 1]);
            } else {
                EK[i + j] ^= EK[i + j - 1];
            }
        }
    }

    for (size_t i = 0; i != 4 * (rounds + 1); i += 4) {
        DK[i] = EK[4 * rounds - i];
        DK[i + 1] = EK[4 * rounds - i + 1];
        DK[i + 2] = EK[4 * rounds - i + 2];
        DK[i + 3] = EK[4 * rounds - i + 3];
    }

    for (size_t i = 4; i != 4 * rounds; i += 4) {
        InvMixColumn_x4(&DK[i]);
    }

    if (bswap_keys) {
        // HW AES on little endian needs the subkeys to be byte reversed
        for (size_t i = 0; i != EK.size(); ++i) {
            EK[i] = reverse_bytes(EK[i]);
        }
        for (size_t i = 0; i != DK.size(); ++i) {
            DK[i] = reverse_bytes(DK[i]);
        }
    }

    CT::unpoison(EK.data(), EK.size());
    CT::unpoison(DK.data(), DK.size());
    CT::unpoison(key, length);
}

size_t aes_parallelism() {
#if defined(BOTAN_HAS_AES_VAES)
    if (CPUID::has(CPUID::Feature::AVX2_AES)) {
        return 8;    // pipelined
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (CPUID::has(CPUID::Feature::HW_AES)) {
        return 4;    // pipelined
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return 2;    // pipelined
    }
#endif

    // bitsliced:
    return 2;
}

std::string aes_provider() {
#if defined(BOTAN_HAS_AES_VAES)
    if (auto feat = CPUID::check(CPUID::Feature::AVX2_AES)) {
        return *feat;
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (auto feat = CPUID::check(CPUID::Feature::HW_AES)) {
        return *feat;
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (auto feat = CPUID::check(CPUID::Feature::SIMD_4X32)) {
        return *feat;
    }
#endif

    return "base";
}

}    // namespace

std::string AES_128::provider() const { return aes_provider(); }

std::string AES_192::provider() const { return aes_provider(); }

std::string AES_256::provider() const { return aes_provider(); }

size_t AES_128::parallelism() const { return aes_parallelism(); }

size_t AES_192::parallelism() const { return aes_parallelism(); }

size_t AES_256::parallelism() const { return aes_parallelism(); }

bool AES_128::has_keying_material() const { return !m_EK.empty(); }

bool AES_192::has_keying_material() const { return !m_EK.empty(); }

bool AES_256::has_keying_material() const { return !m_EK.empty(); }

void AES_128::encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const {
    assert_key_material_set();

#if defined(BOTAN_HAS_AES_VAES)
    if (CPUID::has(CPUID::Feature::AVX2_AES)) {
        return x86_vaes_encrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (CPUID::has(CPUID::Feature::HW_AES)) {
        return hw_aes_encrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return vperm_encrypt_n(in, out, blocks);
    }
#endif

    aes_encrypt_n(in, out, blocks, m_EK);
}

void AES_128::decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const {
    assert_key_material_set();

#if defined(BOTAN_HAS_AES_VAES)
    if (CPUID::has(CPUID::Feature::AVX2_AES)) {
        return x86_vaes_decrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (CPUID::has(CPUID::Feature::HW_AES)) {
        return hw_aes_decrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return vperm_decrypt_n(in, out, blocks);
    }
#endif

    aes_decrypt_n(in, out, blocks, m_DK);
}

void AES_128::key_schedule(std::span<const uint8_t> key) {
#if defined(BOTAN_HAS_AES_NI)
    if (CPUID::has(CPUID::Feature::AESNI)) {
        return aesni_key_schedule(key.data(), key.size());
    }
#endif

#if defined(BOTAN_HAS_AES_VAES)
    if (CPUID::has(CPUID::Feature::AVX2_AES)) {
        return aes_key_schedule(key.data(), key.size(), m_EK, m_DK, true);
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (CPUID::has(CPUID::Feature::HW_AES)) {
        constexpr bool is_little_endian = std::endian::native == std::endian::little;
        return aes_key_schedule(key.data(), key.size(), m_EK, m_DK, is_little_endian);
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return vperm_key_schedule(key.data(), key.size());
    }
#endif

    aes_key_schedule(key.data(), key.size(), m_EK, m_DK);
}

void AES_128::clear() {
    zap(m_EK);
    zap(m_DK);
}

void AES_192::encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const {
    assert_key_material_set();

#if defined(BOTAN_HAS_AES_VAES)
    if (CPUID::has(CPUID::Feature::AVX2_AES)) {
        return x86_vaes_encrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (CPUID::has(CPUID::Feature::HW_AES)) {
        return hw_aes_encrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return vperm_encrypt_n(in, out, blocks);
    }
#endif

    aes_encrypt_n(in, out, blocks, m_EK);
}

void AES_192::decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const {
    assert_key_material_set();

#if defined(BOTAN_HAS_AES_VAES)
    if (CPUID::has(CPUID::Feature::AVX2_AES)) {
        return x86_vaes_decrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (CPUID::has(CPUID::Feature::HW_AES)) {
        return hw_aes_decrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return vperm_decrypt_n(in, out, blocks);
    }
#endif

    aes_decrypt_n(in, out, blocks, m_DK);
}

void AES_192::key_schedule(std::span<const uint8_t> key) {
#if defined(BOTAN_HAS_AES_NI)
    if (CPUID::has(CPUID::Feature::AESNI)) {
        return aesni_key_schedule(key.data(), key.size());
    }
#endif

#if defined(BOTAN_HAS_AES_VAES)
    if (CPUID::has(CPUID::Feature::AVX2_AES)) {
        return aes_key_schedule(key.data(), key.size(), m_EK, m_DK, true);
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (CPUID::has(CPUID::Feature::HW_AES)) {
        constexpr bool is_little_endian = std::endian::native == std::endian::little;
        return aes_key_schedule(key.data(), key.size(), m_EK, m_DK, is_little_endian);
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return vperm_key_schedule(key.data(), key.size());
    }
#endif

    aes_key_schedule(key.data(), key.size(), m_EK, m_DK);
}

void AES_192::clear() {
    zap(m_EK);
    zap(m_DK);
}

void AES_256::encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const {
    assert_key_material_set();

#if defined(BOTAN_HAS_AES_VAES)
    if (CPUID::has(CPUID::Feature::AVX2_AES)) {
        return x86_vaes_encrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (CPUID::has(CPUID::Feature::HW_AES)) {
        return hw_aes_encrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return vperm_encrypt_n(in, out, blocks);
    }
#endif

    aes_encrypt_n(in, out, blocks, m_EK);
}

void AES_256::decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const {
    assert_key_material_set();

#if defined(BOTAN_HAS_AES_VAES)
    if (CPUID::has(CPUID::Feature::AVX2_AES)) {
        return x86_vaes_decrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (CPUID::has(CPUID::Feature::HW_AES)) {
        return hw_aes_decrypt_n(in, out, blocks);
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return vperm_decrypt_n(in, out, blocks);
    }
#endif

    aes_decrypt_n(in, out, blocks, m_DK);
}

void AES_256::key_schedule(std::span<const uint8_t> key) {
#if defined(BOTAN_HAS_AES_NI)
    if (CPUID::has(CPUID::Feature::AESNI)) {
        return aesni_key_schedule(key.data(), key.size());
    }
#endif

#if defined(BOTAN_HAS_AES_VAES)
    if (CPUID::has(CPUID::Feature::AVX2_AES)) {
        return aes_key_schedule(key.data(), key.size(), m_EK, m_DK, true);
    }
#endif

#if defined(BOTAN_HAS_HW_AES_SUPPORT)
    if (CPUID::has(CPUID::Feature::HW_AES)) {
        constexpr bool is_little_endian = std::endian::native == std::endian::little;
        return aes_key_schedule(key.data(), key.size(), m_EK, m_DK, is_little_endian);
    }
#endif

#if defined(BOTAN_HAS_AES_VPERM)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return vperm_key_schedule(key.data(), key.size());
    }
#endif

    aes_key_schedule(key.data(), key.size(), m_EK, m_DK);
}

void AES_256::clear() {
    zap(m_EK);
    zap(m_DK);
}

}    // namespace Botan
/*
 * (C) 2019 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

void Buffered_Computation::update(std::string_view str) {
    add_data({cast_char_ptr_to_uint8(str.data()), str.size()});
}

void Buffered_Computation::update_be(uint16_t val) {
    uint8_t inb[sizeof(val)];
    store_be(val, inb);
    add_data({inb, sizeof(inb)});
}

void Buffered_Computation::update_be(uint32_t val) {
    uint8_t inb[sizeof(val)];
    store_be(val, inb);
    add_data({inb, sizeof(inb)});
}

void Buffered_Computation::update_be(uint64_t val) {
    uint8_t inb[sizeof(val)];
    store_be(val, inb);
    add_data({inb, sizeof(inb)});
}

void Buffered_Computation::update_le(uint16_t val) {
    uint8_t inb[sizeof(val)];
    store_le(val, inb);
    add_data({inb, sizeof(inb)});
}

void Buffered_Computation::update_le(uint32_t val) {
    uint8_t inb[sizeof(val)];
    store_le(val, inb);
    add_data({inb, sizeof(inb)});
}

void Buffered_Computation::update_le(uint64_t val) {
    uint8_t inb[sizeof(val)];
    store_le(val, inb);
    add_data({inb, sizeof(inb)});
}

void Buffered_Computation::final(std::span<uint8_t> out) {
    BOTAN_ARG_CHECK(out.size() >= output_length(),
                    "provided output buffer has insufficient capacity");
    final_result(out);
}

}    // namespace Botan
/*
 * (C) 2018 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

void SymmetricAlgorithm::set_key(const OctetString& key) {
    set_key(std::span{key.begin(), key.length()});
}

void SymmetricAlgorithm::throw_key_not_set_error() const { throw Key_Not_Set(name()); }

void SymmetricAlgorithm::set_key(std::span<const uint8_t> key) {
    if (!valid_keylength(key.size())) {
        throw Invalid_Key_Length(name(), key.size());
    }
    key_schedule(key);
}

}    // namespace Botan
/*
 * OctetString
 * (C) 1999-2007 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include <algorithm>

namespace Botan {

/*
 * Create an OctetString from RNG output
 */
OctetString::OctetString(RandomNumberGenerator& rng, size_t len) { rng.random_vec(m_data, len); }

/*
 * Create an OctetString from a hex string
 */
OctetString::OctetString(std::string_view hex_string) {
    if (!hex_string.empty()) {
        m_data.resize(1 + hex_string.length() / 2);
        m_data.resize(hex_decode(m_data.data(), hex_string));
    }
}

/*
 * Create an OctetString from a byte string
 */
OctetString::OctetString(const uint8_t in[], size_t n) { m_data.assign(in, in + n); }

namespace {

uint8_t odd_parity_of(uint8_t x) {
    uint8_t f = x | 0x01;
    f ^= (f >> 4);
    f ^= (f >> 2);
    f ^= (f >> 1);

    return (x & 0xFE) ^ (f & 0x01);
}

}    // namespace

/*
 * Set the parity of each key byte to odd
 */
void OctetString::set_odd_parity() {
    for (size_t j = 0; j != m_data.size(); ++j) {
        m_data[j] = odd_parity_of(m_data[j]);
    }
}

/*
 * Hex encode an OctetString
 */
std::string OctetString::to_string() const { return hex_encode(m_data.data(), m_data.size()); }

/*
 * XOR Operation for OctetStrings
 */
OctetString& OctetString::operator^=(const OctetString& k) {
    if (&k == this) {
        zeroise(m_data);
        return (*this);
    }
    xor_buf(m_data.data(), k.begin(), std::min(length(), k.length()));
    return (*this);
}

/*
 * Equality Operation for OctetStrings
 */
bool operator==(const OctetString& s1, const OctetString& s2) {
    return (s1.bits_of() == s2.bits_of());
}

/*
 * Unequality Operation for OctetStrings
 */
bool operator!=(const OctetString& s1, const OctetString& s2) { return !(s1 == s2); }

/*
 * Append Operation for OctetStrings
 */
OctetString operator+(const OctetString& k1, const OctetString& k2) {
    secure_vector<uint8_t> out;
    out += k1.bits_of();
    out += k2.bits_of();
    return OctetString(out);
}

/*
 * XOR Operation for OctetStrings
 */
OctetString operator^(const OctetString& k1, const OctetString& k2) {
    secure_vector<uint8_t> out(std::max(k1.length(), k2.length()));

    copy_mem(out.data(), k1.begin(), k1.length());
    xor_buf(out.data(), k2.begin(), k2.length());
    return OctetString(out);
}

}    // namespace Botan
/*
 * Base64 Encoding and Decoding
 * (C) 2010,2015,2020 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

namespace {

class Base64 final {
   public:
    static std::string name() noexcept { return "base64"; }

    static constexpr size_t encoding_bytes_in() noexcept { return m_encoding_bytes_in; }

    static constexpr size_t encoding_bytes_out() noexcept { return m_encoding_bytes_out; }

    static constexpr size_t decoding_bytes_in() noexcept { return m_encoding_bytes_out; }

    static constexpr size_t decoding_bytes_out() noexcept { return m_encoding_bytes_in; }

    static constexpr size_t bits_consumed() noexcept { return m_encoding_bits; }

    static constexpr size_t remaining_bits_before_padding() noexcept {
        return m_remaining_bits_before_padding;
    }

    static constexpr size_t encode_max_output(size_t input_length) {
        return (round_up(input_length, m_encoding_bytes_in) / m_encoding_bytes_in) *
               m_encoding_bytes_out;
    }

    static constexpr size_t decode_max_output(size_t input_length) {
        return (round_up(input_length, m_encoding_bytes_out) * m_encoding_bytes_in) /
               m_encoding_bytes_out;
    }

    static void encode(char out[4], const uint8_t in[3]) noexcept;

    static uint8_t lookup_binary_value(char input) noexcept;

    static bool check_bad_char(uint8_t bin, char input, bool ignore_ws);

    static void decode(uint8_t* out_ptr, const uint8_t decode_buf[4]) {
        out_ptr[0] = (decode_buf[0] << 2) | (decode_buf[1] >> 4);
        out_ptr[1] = (decode_buf[1] << 4) | (decode_buf[2] >> 2);
        out_ptr[2] = (decode_buf[2] << 6) | decode_buf[3];
    }

    static size_t bytes_to_remove(size_t final_truncate) { return final_truncate; }

   private:
    static constexpr size_t m_encoding_bits = 6;
    static constexpr size_t m_remaining_bits_before_padding = 8;

    static constexpr size_t m_encoding_bytes_in = 3;
    static constexpr size_t m_encoding_bytes_out = 4;
};

uint32_t lookup_base64_chars(uint32_t x32) {
    /*
     * The basic insight of this approach is that our goal is computing
     * f(x) = y where x is in [0,63) and y is the correct base64 encoding.
     *
     * Instead of doing this directly, we compute
     * offset(x) such that f(x) = x + offset(x)
     *
     * This is described in
     * http://0x80.pl/notesen/2016-01-12-sse-base64-encoding.html#improved-version
     *
     * Here we do a SWAR (simd within a register) implementation of Wojciech's lookup_version2_swar
     */

    uint32_t r = x32 + 0x41414141;

    r += (~swar_lt<uint32_t>(x32, 0x1A1A1A1A)) & 0x06060606;
    r -= (~swar_lt<uint32_t>(x32, 0x34343434)) & 0x4B4B4B4B;
    r -= (~swar_lt<uint32_t>(x32, 0x3E3E3E3E)) & 0x0F0F0F0F;
    r += (~swar_lt<uint32_t>(x32, 0x3F3F3F3F)) & 0x03030303;

    return r;
}

// static
void Base64::encode(char out[4], const uint8_t in[3]) noexcept {
    const uint32_t b0 = (in[0] & 0xFC) >> 2;
    const uint32_t b1 = ((in[0] & 0x03) << 4) | (in[1] >> 4);
    const uint32_t b2 = ((in[1] & 0x0F) << 2) | (in[2] >> 6);
    const uint32_t b3 = in[2] & 0x3F;

    const uint32_t z = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;

    const uint32_t b64 = lookup_base64_chars(z);

    out[0] = static_cast<char>(get_byte<0>(b64));
    out[1] = static_cast<char>(get_byte<1>(b64));
    out[2] = static_cast<char>(get_byte<2>(b64));
    out[3] = static_cast<char>(get_byte<3>(b64));
}

// static
uint8_t Base64::lookup_binary_value(char input) noexcept {
    auto has_zero_byte = [](uint64_t v) {
        return ((v - 0x0101010101010101) & ~(v) & 0x8080808080808080);
    };

    // Assumes each byte is either 0x00 or 0x80
    auto index_of_first_set_byte = [](uint64_t v) {
        return ((((v - 1) & 0x0101010101010101) * 0x0101010101010101) >> 56) - 1;
    };

    constexpr uint64_t lo = 0x0101010101010101;

    const uint8_t x = static_cast<uint8_t>(input);

    const uint64_t x8 = x * lo;

    // Defines the valid ASCII ranges of base64, except the special chars (below)
    constexpr uint64_t val_l = make_uint64(0, 0, 0, 0, 0, 'A', 'a', '0');
    constexpr uint64_t val_u = make_uint64(0, 0, 0, 0, 0, 26, 26, 10);

    // If x is in one of the ranges return a mask. Otherwise we xor in at the
    // high word which will be our invalid marker
    auto v_mask = swar_in_range<uint64_t>(x8, val_l, val_u) ^ 0x80000000;

    // This is the offset added to x to get the value
    const uint64_t val_v = 0xbfb904 ^ (0xFF000000 - (x << 24));

    uint8_t z = x + static_cast<uint8_t>(val_v >> (8 * index_of_first_set_byte(v_mask)));

    // Valid base64 special characters, and some whitespace chars
    constexpr uint64_t specials_i = make_uint64(0, '+', '/', '=', ' ', '\n', '\t', '\r');

    const uint64_t specials_v = 0x3e3f8180808080 ^ (static_cast<uint64_t>(z) << 56);

    const uint64_t smask = has_zero_byte(x8 ^ specials_i) ^ 0x8000000000000000;

    return static_cast<uint8_t>(specials_v >> (8 * index_of_first_set_byte(smask)));
}

// static
bool Base64::check_bad_char(uint8_t bin, char input, bool ignore_ws) {
    if (bin <= 0x3F) {
        return true;
    } else if (!(bin == 0x81 || (bin == 0x80 && ignore_ws))) {
        throw Invalid_Argument(
            fmt("base64_decode: invalid character '{}'", format_char_for_display(input)));
    }
    return false;
}

}    // namespace

size_t base64_encode(char out[], const uint8_t in[], size_t input_length, size_t& input_consumed,
                     bool final_inputs) {
    return base_encode(Base64(), out, in, input_length, input_consumed, final_inputs);
}

std::string base64_encode(const uint8_t input[], size_t input_length) {
    return base_encode_to_string(Base64(), input, input_length);
}

size_t base64_decode(uint8_t out[], const char in[], size_t input_length, size_t& input_consumed,
                     bool final_inputs, bool ignore_ws) {
    return base_decode(Base64(), out, in, input_length, input_consumed, final_inputs, ignore_ws);
}

size_t base64_decode(uint8_t output[], const char input[], size_t input_length, bool ignore_ws) {
    return base_decode_full(Base64(), output, input, input_length, ignore_ws);
}

size_t base64_decode(uint8_t output[], std::string_view input, bool ignore_ws) {
    return base64_decode(output, input.data(), input.length(), ignore_ws);
}

size_t base64_decode(std::span<uint8_t> output, std::string_view input, bool ignore_ws) {
    if (output.size() < base64_decode_max_output(input.size())) {
        throw Invalid_Argument("base64_decode: output buffer is too short");
    }
    return base64_decode(output.data(), input.data(), input.length(), ignore_ws);
}

secure_vector<uint8_t> base64_decode(const char input[], size_t input_length, bool ignore_ws) {
    return base_decode_to_vec<secure_vector<uint8_t>>(Base64(), input, input_length, ignore_ws);
}

secure_vector<uint8_t> base64_decode(std::string_view input, bool ignore_ws) {
    return base64_decode(input.data(), input.size(), ignore_ws);
}

size_t base64_encode_max_output(size_t input_length) {
    return Base64::encode_max_output(input_length);
}

size_t base64_decode_max_output(size_t input_length) {
    return Base64::decode_max_output(input_length);
}

}    // namespace Botan
/*
 * Block Ciphers
 * (C) 2015 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_AES)
#endif

#if defined(BOTAN_HAS_ARIA)
#endif

#if defined(BOTAN_HAS_BLOWFISH)
#endif

#if defined(BOTAN_HAS_CAMELLIA)
#endif

#if defined(BOTAN_HAS_CAST_128)
#endif

#if defined(BOTAN_HAS_CASCADE)
#endif

#if defined(BOTAN_HAS_DES)
#endif

#if defined(BOTAN_HAS_GOST_28147_89)
#endif

#if defined(BOTAN_HAS_IDEA)
#endif

#if defined(BOTAN_HAS_KUZNYECHIK)
#endif

#if defined(BOTAN_HAS_LION)
#endif

#if defined(BOTAN_HAS_NOEKEON)
#endif

#if defined(BOTAN_HAS_SEED)
#endif

#if defined(BOTAN_HAS_SERPENT)
#endif

#if defined(BOTAN_HAS_SHACAL2)
#endif

#if defined(BOTAN_HAS_SM4)
#endif

#if defined(BOTAN_HAS_TWOFISH)
#endif

#if defined(BOTAN_HAS_THREEFISH_512)
#endif

#if defined(BOTAN_HAS_COMMONCRYPTO)
#endif

namespace Botan {

std::unique_ptr<BlockCipher> BlockCipher::create(std::string_view algo, std::string_view provider) {
#if defined(BOTAN_HAS_COMMONCRYPTO)
    if (provider.empty() || provider == "commoncrypto") {
        if (auto bc = make_commoncrypto_block_cipher(algo)) return bc;

        if (!provider.empty()) return nullptr;
    }
#endif

    // TODO: CryptoAPI
    // TODO: /dev/crypto

    // Only base providers from here on out
    if (provider.empty() == false && provider != "base") {
        return nullptr;
    }

#if defined(BOTAN_HAS_AES)
    if (algo == "AES-128") {
        return std::make_unique<AES_128>();
    }

    if (algo == "AES-192") {
        return std::make_unique<AES_192>();
    }

    if (algo == "AES-256") {
        return std::make_unique<AES_256>();
    }
#endif

#if defined(BOTAN_HAS_ARIA)
    if (algo == "ARIA-128") {
        return std::make_unique<ARIA_128>();
    }

    if (algo == "ARIA-192") {
        return std::make_unique<ARIA_192>();
    }

    if (algo == "ARIA-256") {
        return std::make_unique<ARIA_256>();
    }
#endif

#if defined(BOTAN_HAS_SERPENT)
    if (algo == "Serpent") {
        return std::make_unique<Serpent>();
    }
#endif

#if defined(BOTAN_HAS_SHACAL2)
    if (algo == "SHACAL2") {
        return std::make_unique<SHACAL2>();
    }
#endif

#if defined(BOTAN_HAS_TWOFISH)
    if (algo == "Twofish") {
        return std::make_unique<Twofish>();
    }
#endif

#if defined(BOTAN_HAS_THREEFISH_512)
    if (algo == "Threefish-512") {
        return std::make_unique<Threefish_512>();
    }
#endif

#if defined(BOTAN_HAS_BLOWFISH)
    if (algo == "Blowfish") {
        return std::make_unique<Blowfish>();
    }
#endif

#if defined(BOTAN_HAS_CAMELLIA)
    if (algo == "Camellia-128") {
        return std::make_unique<Camellia_128>();
    }

    if (algo == "Camellia-192") {
        return std::make_unique<Camellia_192>();
    }

    if (algo == "Camellia-256") {
        return std::make_unique<Camellia_256>();
    }
#endif

#if defined(BOTAN_HAS_DES)
    if (algo == "DES") {
        return std::make_unique<DES>();
    }

    if (algo == "TripleDES" || algo == "3DES" || algo == "DES-EDE") {
        return std::make_unique<TripleDES>();
    }
#endif

#if defined(BOTAN_HAS_NOEKEON)
    if (algo == "Noekeon") {
        return std::make_unique<Noekeon>();
    }
#endif

#if defined(BOTAN_HAS_CAST_128)
    if (algo == "CAST-128" || algo == "CAST5") {
        return std::make_unique<CAST_128>();
    }
#endif

#if defined(BOTAN_HAS_IDEA)
    if (algo == "IDEA") {
        return std::make_unique<IDEA>();
    }
#endif

#if defined(BOTAN_HAS_KUZNYECHIK)
    if (algo == "Kuznyechik") {
        return std::make_unique<Kuznyechik>();
    }
#endif

#if defined(BOTAN_HAS_SEED)
    if (algo == "SEED") {
        return std::make_unique<SEED>();
    }
#endif

#if defined(BOTAN_HAS_SM4)
    if (algo == "SM4") {
        return std::make_unique<SM4>();
    }
#endif

    const SCAN_Name req(algo);

#if defined(BOTAN_HAS_GOST_28147_89)
    if (req.algo_name() == "GOST-28147-89") {
        return std::make_unique<GOST_28147_89>(req.arg(0, "R3411_94_TestParam"));
    }
#endif

#if defined(BOTAN_HAS_CASCADE)
    if (req.algo_name() == "Cascade" && req.arg_count() == 2) {
        auto c1 = BlockCipher::create(req.arg(0));
        auto c2 = BlockCipher::create(req.arg(1));

        if (c1 && c2) {
            return std::make_unique<Cascade_Cipher>(std::move(c1), std::move(c2));
        }
    }
#endif

#if defined(BOTAN_HAS_LION)
    if (req.algo_name() == "Lion" && req.arg_count_between(2, 3)) {
        auto hash = HashFunction::create(req.arg(0));
        auto stream = StreamCipher::create(req.arg(1));

        if (hash && stream) {
            const size_t block_size = req.arg_as_integer(2, 1024);
            return std::make_unique<Lion>(std::move(hash), std::move(stream), block_size);
        }
    }
#endif

    BOTAN_UNUSED(req);
    BOTAN_UNUSED(provider);

    return nullptr;
}

// static
std::unique_ptr<BlockCipher> BlockCipher::create_or_throw(std::string_view algo,
                                                          std::string_view provider) {
    if (auto bc = BlockCipher::create(algo, provider)) {
        return bc;
    }
    throw Lookup_Error("Block cipher", algo, provider);
}

std::vector<std::string> BlockCipher::providers(std::string_view algo) {
    return probe_providers_of<BlockCipher>(algo, {"base", "commoncrypto"});
}

}    // namespace Botan
/*
 * CBC Mode
 * (C) 1999-2007,2013,2017 Jack Lloyd
 * (C) 2016 Daniel Neus, Rohde & Schwarz Cybersecurity
 * (C) 2018 Ribose Inc
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

CBC_Mode::CBC_Mode(std::unique_ptr<BlockCipher> cipher,
                   std::unique_ptr<BlockCipherModePaddingMethod> padding)
    : m_cipher(std::move(cipher)),
      m_padding(std::move(padding)),
      m_block_size(m_cipher->block_size()) {
    if (m_padding && !m_padding->valid_blocksize(m_block_size)) {
        throw Invalid_Argument(fmt("Padding {} cannot be used with {} in CBC mode",
                                   m_padding->name(), m_cipher->name()));
    }
}

void CBC_Mode::clear() {
    m_cipher->clear();
    reset();
}

void CBC_Mode::reset() { m_state.clear(); }

std::string CBC_Mode::name() const {
    if (m_padding) {
        return fmt("{}/CBC/{}", cipher().name(), padding().name());
    } else {
        return fmt("{}/CBC/CTS", cipher().name());
    }
}

size_t CBC_Mode::update_granularity() const { return cipher().block_size(); }

size_t CBC_Mode::ideal_granularity() const { return cipher().parallel_bytes(); }

Key_Length_Specification CBC_Mode::key_spec() const { return cipher().key_spec(); }

size_t CBC_Mode::default_nonce_length() const { return block_size(); }

bool CBC_Mode::valid_nonce_length(size_t n) const { return (n == 0 || n == block_size()); }

bool CBC_Mode::has_keying_material() const { return m_cipher->has_keying_material(); }

void CBC_Mode::key_schedule(std::span<const uint8_t> key) {
    m_cipher->set_key(key);
    m_state.clear();
}

void CBC_Mode::start_msg(const uint8_t nonce[], size_t nonce_len) {
    if (!valid_nonce_length(nonce_len)) {
        throw Invalid_IV_Length(name(), nonce_len);
    }

    /*
     * A nonce of zero length means carry the last ciphertext value over
     * as the new IV, as unfortunately some protocols require this. If
     * this is the first message then we use an IV of all zeros.
     */
    if (nonce_len) {
        m_state.assign(nonce, nonce + nonce_len);
    } else if (m_state.empty()) {
        m_state.resize(m_cipher->block_size());
    }
    // else leave the state alone
}

size_t CBC_Encryption::minimum_final_size() const { return 0; }

size_t CBC_Encryption::output_length(size_t input_length) const {
    if (input_length == 0) {
        return block_size();
    } else {
        return round_up(input_length, block_size());
    }
}

size_t CBC_Encryption::process_msg(uint8_t buf[], size_t sz) {
    BOTAN_STATE_CHECK(state().empty() == false);
    const size_t BS = block_size();

    BOTAN_ARG_CHECK(sz % BS == 0, "CBC input is not full blocks");
    const size_t blocks = sz / BS;

    if (blocks > 0) {
        xor_buf(&buf[0], state_ptr(), BS);
        cipher().encrypt(&buf[0]);

        for (size_t i = 1; i != blocks; ++i) {
            xor_buf(&buf[BS * i], &buf[BS * (i - 1)], BS);
            cipher().encrypt(&buf[BS * i]);
        }

        state().assign(&buf[BS * (blocks - 1)], &buf[BS * blocks]);
    }

    return sz;
}

void CBC_Encryption::finish_msg(secure_vector<uint8_t>& buffer, size_t offset) {
    BOTAN_STATE_CHECK(state().empty() == false);
    BOTAN_ARG_CHECK(buffer.size() >= offset, "Offset is out of range");

    const size_t BS = block_size();

    const size_t bytes_in_final_block = (buffer.size() - offset) % BS;

    padding().add_padding(buffer, bytes_in_final_block, BS);

    BOTAN_ASSERT_EQUAL(buffer.size() % BS, offset % BS, "Padded to block boundary");

    update(buffer, offset);
}

bool CTS_Encryption::valid_nonce_length(size_t n) const { return (n == block_size()); }

size_t CTS_Encryption::minimum_final_size() const { return block_size() + 1; }

size_t CTS_Encryption::output_length(size_t input_length) const {
    return input_length;    // no ciphertext expansion in CTS
}

void CTS_Encryption::finish_msg(secure_vector<uint8_t>& buffer, size_t offset) {
    BOTAN_STATE_CHECK(state().empty() == false);
    BOTAN_ARG_CHECK(buffer.size() >= offset, "Offset is out of range");
    uint8_t* buf = buffer.data() + offset;
    const size_t sz = buffer.size() - offset;

    const size_t BS = block_size();

    if (sz < BS + 1) {
        throw Encoding_Error(name() + ": insufficient data to encrypt");
    }

    if (sz % BS == 0) {
        update(buffer, offset);

        // swap last two blocks
        for (size_t i = 0; i != BS; ++i) {
            std::swap(buffer[buffer.size() - BS + i], buffer[buffer.size() - 2 * BS + i]);
        }
    } else {
        const size_t full_blocks = ((sz / BS) - 1) * BS;
        const size_t final_bytes = sz - full_blocks;
        BOTAN_ASSERT(final_bytes > BS && final_bytes < 2 * BS, "Left over size in expected range");

        secure_vector<uint8_t> last(buf + full_blocks, buf + full_blocks + final_bytes);
        buffer.resize(full_blocks + offset);
        update(buffer, offset);

        xor_buf(last.data(), state_ptr(), BS);
        cipher().encrypt(last.data());

        for (size_t i = 0; i != final_bytes - BS; ++i) {
            last[i] ^= last[i + BS];
            last[i + BS] ^= last[i];
        }

        cipher().encrypt(last.data());

        buffer += last;
    }
}

size_t CBC_Decryption::output_length(size_t input_length) const {
    return input_length;    // precise for CTS, worst case otherwise
}

size_t CBC_Decryption::minimum_final_size() const { return block_size(); }

size_t CBC_Decryption::process_msg(uint8_t buf[], size_t sz) {
    BOTAN_STATE_CHECK(state().empty() == false);

    const size_t BS = block_size();

    BOTAN_ARG_CHECK(sz % BS == 0, "Input is not full blocks");
    size_t blocks = sz / BS;

    while (blocks) {
        const size_t to_proc = std::min(BS * blocks, m_tempbuf.size());

        cipher().decrypt_n(buf, m_tempbuf.data(), to_proc / BS);

        xor_buf(m_tempbuf.data(), state_ptr(), BS);
        xor_buf(&m_tempbuf[BS], buf, to_proc - BS);
        copy_mem(state_ptr(), buf + (to_proc - BS), BS);

        copy_mem(buf, m_tempbuf.data(), to_proc);

        buf += to_proc;
        blocks -= to_proc / BS;
    }

    return sz;
}

void CBC_Decryption::finish_msg(secure_vector<uint8_t>& buffer, size_t offset) {
    BOTAN_STATE_CHECK(state().empty() == false);
    BOTAN_ARG_CHECK(buffer.size() >= offset, "Offset is out of range");
    const size_t sz = buffer.size() - offset;

    const size_t BS = block_size();

    if (sz == 0 || sz % BS) {
        throw Decoding_Error(name() + ": Ciphertext not a multiple of block size");
    }

    update(buffer, offset);

    const size_t pad_bytes = BS - padding().unpad(&buffer[buffer.size() - BS], BS);
    buffer.resize(buffer.size() - pad_bytes);    // remove padding
    if (pad_bytes == 0 && padding().name() != "NoPadding") {
        throw Decoding_Error("Invalid CBC padding");
    }
}

void CBC_Decryption::reset() {
    CBC_Mode::reset();
    zeroise(m_tempbuf);
}

bool CTS_Decryption::valid_nonce_length(size_t n) const { return (n == block_size()); }

size_t CTS_Decryption::minimum_final_size() const { return block_size() + 1; }

void CTS_Decryption::finish_msg(secure_vector<uint8_t>& buffer, size_t offset) {
    BOTAN_STATE_CHECK(state().empty() == false);
    BOTAN_ARG_CHECK(buffer.size() >= offset, "Offset is out of range");
    const size_t sz = buffer.size() - offset;
    uint8_t* buf = buffer.data() + offset;

    const size_t BS = block_size();

    if (sz < BS + 1) {
        throw Encoding_Error(name() + ": insufficient data to decrypt");
    }

    if (sz % BS == 0) {
        // swap last two blocks

        for (size_t i = 0; i != BS; ++i) {
            std::swap(buffer[buffer.size() - BS + i], buffer[buffer.size() - 2 * BS + i]);
        }

        update(buffer, offset);
    } else {
        const size_t full_blocks = ((sz / BS) - 1) * BS;
        const size_t final_bytes = sz - full_blocks;
        BOTAN_ASSERT(final_bytes > BS && final_bytes < 2 * BS, "Left over size in expected range");

        secure_vector<uint8_t> last(buf + full_blocks, buf + full_blocks + final_bytes);
        buffer.resize(full_blocks + offset);
        update(buffer, offset);

        cipher().decrypt(last.data());

        xor_buf(last.data(), &last[BS], final_bytes - BS);

        for (size_t i = 0; i != final_bytes - BS; ++i) {
            std::swap(last[i], last[i + BS]);
        }

        cipher().decrypt(last.data());
        xor_buf(last.data(), state_ptr(), BS);

        buffer += last;
    }
}

}    // namespace Botan
/*
 * CMAC
 * (C) 1999-2007,2014 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/*
 * Update an CMAC Calculation
 */
void CMAC::add_data(std::span<const uint8_t> input) {
    const size_t bs = output_length();

    const size_t initial_fill = std::min(m_buffer.size() - m_position, input.size());
    copy_mem(m_buffer.data() + m_position, input.data(), initial_fill);

    if (m_position + input.size() > bs) {
        xor_buf(m_state, m_buffer, bs);
        m_cipher->encrypt(m_state);

        BufferSlicer in(input);
        in.skip(bs - m_position);
        while (in.remaining() > bs) {
            xor_buf(m_state, in.take(bs), bs);
            m_cipher->encrypt(m_state);
        }

        const auto remaining = in.take(in.remaining());
        copy_mem(m_buffer.data(), remaining.data(), remaining.size());
        m_position = remaining.size();
    } else {
        m_position += input.size();
    }
}

/*
 * Finalize an CMAC Calculation
 */
void CMAC::final_result(std::span<uint8_t> mac) {
    xor_buf(m_state, m_buffer, m_position);

    if (m_position == output_length()) {
        xor_buf(m_state, m_B, output_length());
    } else {
        m_state[m_position] ^= 0x80;
        xor_buf(m_state, m_P, output_length());
    }

    m_cipher->encrypt(m_state);

    copy_mem(mac.data(), m_state.data(), output_length());

    zeroise(m_state);
    zeroise(m_buffer);
    m_position = 0;
}

bool CMAC::has_keying_material() const { return m_cipher->has_keying_material(); }

/*
 * CMAC Key Schedule
 */
void CMAC::key_schedule(std::span<const uint8_t> key) {
    clear();
    m_cipher->set_key(key);
    m_cipher->encrypt(m_B);
    poly_double_n(m_B.data(), m_B.size());
    poly_double_n(m_P.data(), m_B.data(), m_P.size());
}

/*
 * Clear memory of sensitive data
 */
void CMAC::clear() {
    m_cipher->clear();
    zeroise(m_state);
    zeroise(m_buffer);
    zeroise(m_B);
    zeroise(m_P);
    m_position = 0;
}

/*
 * Return the name of this type
 */
std::string CMAC::name() const { return fmt("CMAC({})", m_cipher->name()); }

/*
 * Return a new_object of this object
 */
std::unique_ptr<MessageAuthenticationCode> CMAC::new_object() const {
    return std::make_unique<CMAC>(m_cipher->new_object());
}

/*
 * CMAC Constructor
 */
CMAC::CMAC(std::unique_ptr<BlockCipher> cipher)
    : m_cipher(std::move(cipher)), m_block_size(m_cipher->block_size()) {
    if (poly_double_supported_size(m_block_size) == false) {
        throw Invalid_Argument(
            fmt("CMAC cannot use the {} bit cipher {}", m_block_size * 8, m_cipher->name()));
    }

    m_state.resize(output_length());
    m_buffer.resize(output_length());
    m_B.resize(output_length());
    m_P.resize(output_length());
    m_position = 0;
}

}    // namespace Botan
/*
 * Filters
 * (C) 1999-2007,2015 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

#if defined(BOTAN_HAS_STREAM_CIPHER)

StreamCipher_Filter::StreamCipher_Filter(StreamCipher* cipher)
    : m_cipher(cipher), m_buffer(m_cipher->buffer_size()) {}

StreamCipher_Filter::StreamCipher_Filter(StreamCipher* cipher, const SymmetricKey& key)
    : StreamCipher_Filter(cipher) {
    m_cipher->set_key(key);
}

StreamCipher_Filter::StreamCipher_Filter(std::string_view sc_name)
    : m_cipher(StreamCipher::create_or_throw(sc_name)), m_buffer(m_cipher->buffer_size()) {}

StreamCipher_Filter::StreamCipher_Filter(std::string_view sc_name, const SymmetricKey& key)
    : StreamCipher_Filter(sc_name) {
    m_cipher->set_key(key);
}

void StreamCipher_Filter::write(const uint8_t input[], size_t length) {
    while (length) {
        size_t copied = std::min<size_t>(length, m_buffer.size());
        m_cipher->cipher(input, m_buffer.data(), copied);
        send(m_buffer, copied);
        input += copied;
        length -= copied;
    }
}

#endif

#if defined(BOTAN_HAS_HASH)

Hash_Filter::Hash_Filter(std::string_view hash_name, size_t len)
    : m_hash(HashFunction::create_or_throw(hash_name)), m_out_len(len) {}

void Hash_Filter::end_msg() {
    secure_vector<uint8_t> output = m_hash->final();
    if (m_out_len) {
        send(output, std::min<size_t>(m_out_len, output.size()));
    } else {
        send(output);
    }
}
#endif

#if defined(BOTAN_HAS_MAC)

MAC_Filter::MAC_Filter(std::string_view mac_name, size_t len)
    : m_mac(MessageAuthenticationCode::create_or_throw(mac_name)), m_out_len(len) {}

MAC_Filter::MAC_Filter(std::string_view mac_name, const SymmetricKey& key, size_t len)
    : MAC_Filter(mac_name, len) {
    m_mac->set_key(key);
}

void MAC_Filter::end_msg() {
    secure_vector<uint8_t> output = m_mac->final();
    if (m_out_len) {
        send(output, std::min<size_t>(m_out_len, output.size()));
    } else {
        send(output);
    }
}

#endif

}    // namespace Botan
/*
 * Base64 Encoder/Decoder
 * (C) 1999-2010 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/*
 * Base64_Encoder Constructor
 */
Base64_Encoder::Base64_Encoder(bool line_breaks, size_t line_length, bool trailing_newline)
    : m_line_length(line_breaks ? line_length : 0),
      m_trailing_newline(trailing_newline && line_breaks),
      m_in(48),
      m_out(64),
      m_position(0),
      m_out_position(0) {}

/*
 * Encode and send a block
 */
void Base64_Encoder::encode_and_send(const uint8_t input[], size_t length, bool final_inputs) {
    while (length) {
        const size_t proc = std::min(length, m_in.size());

        size_t consumed = 0;
        size_t produced = base64_encode(cast_uint8_ptr_to_char(m_out.data()), input, proc, consumed,
                                        final_inputs);

        do_output(m_out.data(), produced);

        // FIXME: s/proc/consumed/?
        input += proc;
        length -= proc;
    }
}

/*
 * Handle the output
 */
void Base64_Encoder::do_output(const uint8_t input[], size_t length) {
    if (m_line_length == 0) {
        send(input, length);
    } else {
        size_t remaining = length, offset = 0;
        while (remaining) {
            size_t sent = std::min(m_line_length - m_out_position, remaining);
            send(input + offset, sent);
            m_out_position += sent;
            remaining -= sent;
            offset += sent;
            if (m_out_position == m_line_length) {
                send('\n');
                m_out_position = 0;
            }
        }
    }
}

/*
 * Convert some data into Base64
 */
void Base64_Encoder::write(const uint8_t input[], size_t length) {
    const size_t initial_fill = std::min(m_in.size() - m_position, length);
    copy_mem(&m_in[m_position], input, initial_fill);

    if (m_position + length >= m_in.size()) {
        encode_and_send(m_in.data(), m_in.size());
        input += (m_in.size() - m_position);
        length -= (m_in.size() - m_position);
        while (length >= m_in.size()) {
            encode_and_send(input, m_in.size());
            input += m_in.size();
            length -= m_in.size();
        }
        copy_mem(m_in.data(), input, length);
        m_position = 0;
    }
    m_position += length;
}

/*
 * Flush buffers
 */
void Base64_Encoder::end_msg() {
    encode_and_send(m_in.data(), m_position, true);

    if (m_trailing_newline || (m_out_position && m_line_length)) {
        send('\n');
    }

    m_out_position = m_position = 0;
}

/*
 * Base64_Decoder Constructor
 */
Base64_Decoder::Base64_Decoder(Decoder_Checking c)
    : m_checking(c), m_in(64), m_out(48), m_position(0) {}

/*
 * Convert some data from Base64
 */
void Base64_Decoder::write(const uint8_t input[], size_t length) {
    while (length) {
        size_t to_copy = std::min<size_t>(length, m_in.size() - m_position);
        if (to_copy == 0) {
            m_in.resize(m_in.size() * 2);
            m_out.resize(m_out.size() * 2);
        }
        copy_mem(&m_in[m_position], input, to_copy);
        m_position += to_copy;

        size_t consumed = 0;
        size_t written = base64_decode(m_out.data(), cast_uint8_ptr_to_char(m_in.data()),
                                       m_position, consumed, false, m_checking != FULL_CHECK);

        send(m_out, written);

        if (consumed != m_position) {
            copy_mem(m_in.data(), m_in.data() + consumed, m_position - consumed);
            m_position = m_position - consumed;
        } else {
            m_position = 0;
        }

        length -= to_copy;
        input += to_copy;
    }
}

/*
 * Flush buffers
 */
void Base64_Decoder::end_msg() {
    size_t consumed = 0;
    size_t written = base64_decode(m_out.data(), cast_uint8_ptr_to_char(m_in.data()), m_position,
                                   consumed, true, m_checking != FULL_CHECK);

    send(m_out, written);

    const bool not_full_bytes = consumed != m_position;

    m_position = 0;

    if (not_full_bytes) {
        throw Invalid_Argument("Base64_Decoder: Input not full bytes");
    }
}

}    // namespace Botan
/*
 * (C) 1999-2007 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/*
 * Chain Constructor
 */
Chain::Chain(Filter* f1, Filter* f2, Filter* f3, Filter* f4) {
    if (f1) {
        attach(f1);
        incr_owns();
    }
    if (f2) {
        attach(f2);
        incr_owns();
    }
    if (f3) {
        attach(f3);
        incr_owns();
    }
    if (f4) {
        attach(f4);
        incr_owns();
    }
}

/*
 * Chain Constructor
 */
Chain::Chain(Filter* filters[], size_t count) {
    for (size_t j = 0; j != count; ++j) {
        if (filters[j]) {
            attach(filters[j]);
            incr_owns();
        }
    }
}

/*
 * Fork Constructor
 */
Fork::Fork(Filter* f1, Filter* f2, Filter* f3, Filter* f4) {
    Filter* filters[4] = {f1, f2, f3, f4};
    set_next(filters, 4);
}

/*
 * Fork Constructor
 */
Fork::Fork(Filter* filters[], size_t count) { set_next(filters, count); }

}    // namespace Botan
/*
 * Buffered Filter
 * (C) 1999-2007 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/*
 * Buffered_Filter Constructor
 */
Buffered_Filter::Buffered_Filter(size_t b, size_t f) : m_main_block_mod(b), m_final_minimum(f) {
    if (m_main_block_mod == 0) {
        throw Invalid_Argument("m_main_block_mod == 0");
    }

    if (m_final_minimum > m_main_block_mod) {
        throw Invalid_Argument("m_final_minimum > m_main_block_mod");
    }

    m_buffer.resize(2 * m_main_block_mod);
    m_buffer_pos = 0;
}

/*
 * Buffer input into blocks, trying to minimize copying
 */
void Buffered_Filter::write(const uint8_t input[], size_t input_size) {
    if (!input_size) {
        return;
    }

    if (m_buffer_pos + input_size >= m_main_block_mod + m_final_minimum) {
        size_t to_copy = std::min<size_t>(m_buffer.size() - m_buffer_pos, input_size);

        copy_mem(&m_buffer[m_buffer_pos], input, to_copy);
        m_buffer_pos += to_copy;

        input += to_copy;
        input_size -= to_copy;

        const size_t available =
            std::min(m_buffer_pos, m_buffer_pos + input_size - m_final_minimum);

        // Size down to available block size
        const size_t total_to_consume = available - (available % m_main_block_mod);

        buffered_block(m_buffer.data(), total_to_consume);

        m_buffer_pos -= total_to_consume;

        copy_mem(m_buffer.data(), m_buffer.data() + total_to_consume, m_buffer_pos);
    }

    if (input_size >= m_final_minimum) {
        size_t full_blocks = (input_size - m_final_minimum) / m_main_block_mod;
        size_t to_copy = full_blocks * m_main_block_mod;

        if (to_copy) {
            buffered_block(input, to_copy);

            input += to_copy;
            input_size -= to_copy;
        }
    }

    copy_mem(&m_buffer[m_buffer_pos], input, input_size);
    m_buffer_pos += input_size;
}

/*
 * Finish/flush operation
 */
void Buffered_Filter::end_msg() {
    if (m_buffer_pos < m_final_minimum) {
        throw Invalid_State("Buffered filter end_msg without enough input");
    }

    size_t spare_blocks = (m_buffer_pos - m_final_minimum) / m_main_block_mod;

    if (spare_blocks) {
        size_t spare_bytes = m_main_block_mod * spare_blocks;
        buffered_block(m_buffer.data(), spare_bytes);
        buffered_final(&m_buffer[spare_bytes], m_buffer_pos - spare_bytes);
    } else {
        buffered_final(m_buffer.data(), m_buffer_pos);
    }

    m_buffer_pos = 0;
}

}    // namespace Botan
/*
 * Filter interface for Cipher_Modes
 * (C) 2013,2014,2017 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

namespace {

size_t choose_update_size(size_t update_granularity) {
    const size_t target_size = 1024;

    if (update_granularity >= target_size) {
        return update_granularity;
    }

    return round_up(target_size, update_granularity);
}

}    // namespace

Cipher_Mode_Filter::Cipher_Mode_Filter(Cipher_Mode* mode)
    : Buffered_Filter(choose_update_size(mode->ideal_granularity()), mode->minimum_final_size()),
      m_mode(mode),
      m_nonce(mode->default_nonce_length()),
      m_buffer(m_mode->ideal_granularity()) {}

std::string Cipher_Mode_Filter::name() const { return m_mode->name(); }

void Cipher_Mode_Filter::set_iv(const InitializationVector& iv) { m_nonce = unlock(iv.bits_of()); }

void Cipher_Mode_Filter::set_key(const SymmetricKey& key) { m_mode->set_key(key); }

Key_Length_Specification Cipher_Mode_Filter::key_spec() const { return m_mode->key_spec(); }

bool Cipher_Mode_Filter::valid_iv_length(size_t length) const {
    return m_mode->valid_nonce_length(length);
}

void Cipher_Mode_Filter::write(const uint8_t input[], size_t input_length) {
    Buffered_Filter::write(input, input_length);
}

void Cipher_Mode_Filter::end_msg() { Buffered_Filter::end_msg(); }

void Cipher_Mode_Filter::start_msg() {
    if (m_nonce.empty() && !m_mode->valid_nonce_length(0)) {
        throw Invalid_State("Cipher " + m_mode->name() +
                            " requires a fresh nonce for each message");
    }

    m_mode->start(m_nonce);
    m_nonce.clear();
}

void Cipher_Mode_Filter::buffered_block(const uint8_t input[], size_t input_length) {
    while (input_length) {
        const size_t take = std::min(m_mode->ideal_granularity(), input_length);

        m_buffer.assign(input, input + take);
        m_mode->update(m_buffer);

        send(m_buffer);

        input += take;
        input_length -= take;
    }
}

void Cipher_Mode_Filter::buffered_final(const uint8_t input[], size_t input_length) {
    secure_vector<uint8_t> buf(input, input + input_length);
    m_mode->finish(buf);
    send(buf);
}

}    // namespace Botan
/*
 * Filter interface for compression
 * (C) 2014,2015,2016 Jack Lloyd
 * (C) 2015 Matej Kenda
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_COMPRESSION)
#endif

namespace Botan {

#if defined(BOTAN_HAS_COMPRESSION)

Compression_Filter::Compression_Filter(std::string_view type, size_t level, size_t bs)
    : m_comp(Compression_Algorithm::create(type)),
      m_buffersize(std::max<size_t>(bs, 256)),
      m_level(level) {
    if (!m_comp) {
        throw Invalid_Argument(fmt("Compression type '{}' not found", type));
    }
}

Compression_Filter::~Compression_Filter() = default;

std::string Compression_Filter::name() const { return m_comp->name(); }

void Compression_Filter::start_msg() { m_comp->start(m_level); }

void Compression_Filter::write(const uint8_t input[], size_t input_length) {
    while (input_length) {
        const size_t take = std::min(m_buffersize, input_length);
        BOTAN_ASSERT(take > 0, "Consumed something");

        m_buffer.assign(input, input + take);
        m_comp->update(m_buffer);

        send(m_buffer);

        input += take;
        input_length -= take;
    }
}

void Compression_Filter::flush() {
    m_buffer.clear();
    m_comp->update(m_buffer, 0, true);
    send(m_buffer);
}

void Compression_Filter::end_msg() {
    m_buffer.clear();
    m_comp->finish(m_buffer);
    send(m_buffer);
}

Decompression_Filter::Decompression_Filter(std::string_view type, size_t bs)
    : m_comp(Decompression_Algorithm::create(type)), m_buffersize(std::max<size_t>(bs, 256)) {
    if (!m_comp) {
        throw Invalid_Argument(fmt("Compression type '{}' not found", type));
    }
}

Decompression_Filter::~Decompression_Filter() = default;

std::string Decompression_Filter::name() const { return m_comp->name(); }

void Decompression_Filter::start_msg() { m_comp->start(); }

void Decompression_Filter::write(const uint8_t input[], size_t input_length) {
    while (input_length) {
        const size_t take = std::min(m_buffersize, input_length);
        BOTAN_ASSERT(take > 0, "Consumed something");

        m_buffer.assign(input, input + take);
        m_comp->update(m_buffer);

        send(m_buffer);

        input += take;
        input_length -= take;
    }
}

void Decompression_Filter::end_msg() {
    m_buffer.clear();
    m_comp->finish(m_buffer);
    send(m_buffer);
}

#endif

}    // namespace Botan
/*
 * DataSink
 * (C) 1999-2007 Jack Lloyd
 *     2005 Matthew Gregan
 *     2017 Philippe Lieser
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include <ostream>

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
#include <fstream>
#endif

namespace Botan {

/*
 * Write to a stream
 */
void DataSink_Stream::write(const uint8_t out[], size_t length) {
    m_sink.write(cast_uint8_ptr_to_char(out), length);
    if (!m_sink.good()) {
        throw Stream_IO_Error("DataSink_Stream: Failure writing to " + m_identifier);
    }
}

/*
 * Flush the stream
 */
void DataSink_Stream::end_msg() { m_sink.flush(); }

/*
 * DataSink_Stream Constructor
 */
DataSink_Stream::DataSink_Stream(std::ostream& out, std::string_view name)
    : m_identifier(name), m_sink(out) {}

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)

/*
 * DataSink_Stream Constructor
 */
DataSink_Stream::DataSink_Stream(std::string_view path, bool use_binary)
    : m_identifier(path),
      m_sink_memory(std::make_unique<std::ofstream>(std::string(path),
                                                    use_binary ? std::ios::binary : std::ios::out)),
      m_sink(*m_sink_memory) {
    if (!m_sink.good()) {
        throw Stream_IO_Error(fmt("DataSink_Stream: Failure opening path '{}'", path));
    }
}
#endif

DataSink_Stream::~DataSink_Stream() = default;

}    // namespace Botan
/*
 * Filter
 * (C) 1999-2007 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/*
 * Filter Constructor
 */
Filter::Filter() {
    m_next.resize(1);
    m_port_num = 0;
    m_filter_owns = 0;
    m_owned = false;
}

void Filter::send(std::span<const uint8_t> in, size_t length) {
    BOTAN_ASSERT_NOMSG(length <= in.size());
    send(in.data(), length);
}

/*
 * Send data to all ports
 */
void Filter::send(const uint8_t input[], size_t length) {
    if (!length) {
        return;
    }

    bool nothing_attached = true;
    for (size_t j = 0; j != total_ports(); ++j) {
        if (m_next[j]) {
            if (!m_write_queue.empty()) {
                m_next[j]->write(m_write_queue.data(), m_write_queue.size());
            }
            m_next[j]->write(input, length);
            nothing_attached = false;
        }
    }

    if (nothing_attached) {
        m_write_queue += std::make_pair(input, length);
    } else {
        m_write_queue.clear();
    }
}

/*
 * Start a new message
 */
void Filter::new_msg() {
    start_msg();
    for (size_t j = 0; j != total_ports(); ++j) {
        if (m_next[j]) {
            m_next[j]->new_msg();
        }
    }
}

/*
 * End the current message
 */
void Filter::finish_msg() {
    end_msg();
    for (size_t j = 0; j != total_ports(); ++j) {
        if (m_next[j]) {
            m_next[j]->finish_msg();
        }
    }
}

/*
 * Attach a filter to the current port
 */
void Filter::attach(Filter* new_filter) {
    if (new_filter) {
        Filter* last = this;
        while (last->get_next()) {
            last = last->get_next();
        }
        last->m_next[last->current_port()] = new_filter;
    }
}

/*
 * Set the active port on a filter
 */
void Filter::set_port(size_t new_port) {
    if (new_port >= total_ports()) {
        throw Invalid_Argument("Filter: Invalid port number");
    }
    m_port_num = new_port;
}

/*
 * Return the next Filter in the logical chain
 */
Filter* Filter::get_next() const {
    if (m_port_num < m_next.size()) {
        return m_next[m_port_num];
    }
    return nullptr;
}

/*
 * Set the next Filters
 */
void Filter::set_next(Filter* filters[], size_t size) {
    m_next.clear();

    m_port_num = 0;
    m_filter_owns = 0;

    while (size && filters && (filters[size - 1] == nullptr)) {
        --size;
    }

    if (filters && size) {
        m_next.assign(filters, filters + size);
    }
}

/*
 * Return the total number of ports
 */
size_t Filter::total_ports() const { return m_next.size(); }

}    // namespace Botan
/*
 * Hex Encoder/Decoder
 * (C) 1999-2007 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/**
 * Size used for internal buffer in hex encoder/decoder
 */
const size_t HEX_CODEC_BUFFER_SIZE = 256;

/*
 * Hex_Encoder Constructor
 */
Hex_Encoder::Hex_Encoder(bool breaks, size_t length, Case c)
    : m_casing(c), m_line_length(breaks ? length : 0) {
    m_in.resize(HEX_CODEC_BUFFER_SIZE);
    m_out.resize(2 * m_in.size());
    m_counter = m_position = 0;
}

/*
 * Hex_Encoder Constructor
 */
Hex_Encoder::Hex_Encoder(Case c) : m_casing(c), m_line_length(0) {
    m_in.resize(HEX_CODEC_BUFFER_SIZE);
    m_out.resize(2 * m_in.size());
    m_counter = m_position = 0;
}

/*
 * Encode and send a block
 */
void Hex_Encoder::encode_and_send(const uint8_t block[], size_t length) {
    hex_encode(cast_uint8_ptr_to_char(m_out.data()), block, length, m_casing == Uppercase);

    if (m_line_length == 0) {
        send(m_out, 2 * length);
    } else {
        size_t remaining = 2 * length, offset = 0;
        while (remaining) {
            size_t sent = std::min(m_line_length - m_counter, remaining);
            send(&m_out[offset], sent);
            m_counter += sent;
            remaining -= sent;
            offset += sent;
            if (m_counter == m_line_length) {
                send('\n');
                m_counter = 0;
            }
        }
    }
}

/*
 * Convert some data into hex format
 */
void Hex_Encoder::write(const uint8_t input[], size_t length) {
    const size_t initial_fill = std::min(m_in.size() - m_position, length);
    copy_mem(&m_in[m_position], input, initial_fill);

    if (m_position + length >= m_in.size()) {
        encode_and_send(m_in.data(), m_in.size());
        input += (m_in.size() - m_position);
        length -= (m_in.size() - m_position);
        while (length >= m_in.size()) {
            encode_and_send(input, m_in.size());
            input += m_in.size();
            length -= m_in.size();
        }
        copy_mem(m_in.data(), input, length);
        m_position = 0;
    }
    m_position += length;
}

/*
 * Flush buffers
 */
void Hex_Encoder::end_msg() {
    encode_and_send(m_in.data(), m_position);
    if (m_counter && m_line_length) {
        send('\n');
    }
    m_counter = m_position = 0;
}

/*
 * Hex_Decoder Constructor
 */
Hex_Decoder::Hex_Decoder(Decoder_Checking c) : m_checking(c) {
    m_in.resize(HEX_CODEC_BUFFER_SIZE);
    m_out.resize(m_in.size() / 2);
    m_position = 0;
}

/*
 * Convert some data from hex format
 */
void Hex_Decoder::write(const uint8_t input[], size_t length) {
    while (length) {
        size_t to_copy = std::min<size_t>(length, m_in.size() - m_position);
        copy_mem(&m_in[m_position], input, to_copy);
        m_position += to_copy;

        size_t consumed = 0;
        size_t written = hex_decode(m_out.data(), cast_uint8_ptr_to_char(m_in.data()), m_position,
                                    consumed, m_checking != FULL_CHECK);

        send(m_out, written);

        if (consumed != m_position) {
            copy_mem(m_in.data(), m_in.data() + consumed, m_position - consumed);
            m_position = m_position - consumed;
        } else {
            m_position = 0;
        }

        length -= to_copy;
        input += to_copy;
    }
}

/*
 * Flush buffers
 */
void Hex_Decoder::end_msg() {
    size_t consumed = 0;
    size_t written = hex_decode(m_out.data(), cast_uint8_ptr_to_char(m_in.data()), m_position,
                                consumed, m_checking != FULL_CHECK);

    send(m_out, written);

    const bool not_full_bytes = consumed != m_position;

    m_position = 0;

    if (not_full_bytes) {
        throw Invalid_Argument("Hex_Decoder: Input not full bytes");
    }
}

}    // namespace Botan
/*
 * Pipe Output Buffer
 * (C) 1999-2007,2011 Jack Lloyd
 *     2012 Markus Wanner
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/*
 * Read data from a message
 */
size_t Output_Buffers::read(uint8_t output[], size_t length, Pipe::message_id msg) {
    SecureQueue* q = get(msg);
    if (q) {
        return q->read(output, length);
    }
    return 0;
}

/*
 * Peek at data in a message
 */
size_t Output_Buffers::peek(uint8_t output[], size_t length, size_t stream_offset,
                            Pipe::message_id msg) const {
    SecureQueue* q = get(msg);
    if (q) {
        return q->peek(output, length, stream_offset);
    }
    return 0;
}

/*
 * Check available bytes in a message
 */
size_t Output_Buffers::remaining(Pipe::message_id msg) const {
    SecureQueue* q = get(msg);
    if (q) {
        return q->size();
    }
    return 0;
}

/*
 * Return the total bytes of a message that have already been read.
 */
size_t Output_Buffers::get_bytes_read(Pipe::message_id msg) const {
    SecureQueue* q = get(msg);
    if (q) {
        return q->get_bytes_read();
    }
    return 0;
}

/*
 * Add a new output queue
 */
void Output_Buffers::add(SecureQueue* queue) {
    BOTAN_ASSERT(queue, "queue was provided");

    BOTAN_ASSERT(m_buffers.size() < m_buffers.max_size(), "Room was available in container");

    m_buffers.push_back(std::unique_ptr<SecureQueue>(queue));
}

/*
 * Retire old output queues
 */
void Output_Buffers::retire() {
    for (auto& buf : m_buffers) {
        if (buf && buf->empty()) {
            buf.reset();
        }
    }

    while (!m_buffers.empty() && !m_buffers[0]) {
        m_buffers.pop_front();
        m_offset = m_offset + Pipe::message_id(1);
    }
}

/*
 * Get a particular output queue
 */
SecureQueue* Output_Buffers::get(Pipe::message_id msg) const {
    if (msg < m_offset) {
        return nullptr;
    }

    BOTAN_ASSERT(msg < message_count(), "Message number is in range");

    return m_buffers[msg - m_offset].get();
}

/*
 * Return the total number of messages
 */
Pipe::message_id Output_Buffers::message_count() const { return (m_offset + m_buffers.size()); }

/*
 * Output_Buffers Constructor
 */
Output_Buffers::Output_Buffers() { m_offset = 0; }

}    // namespace Botan
/*
 * Pipe
 * (C) 1999-2007 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include <memory>

namespace Botan {

namespace {

/*
 * A Filter that does nothing
 */
class Null_Filter final : public Filter {
   public:
    void write(const uint8_t input[], size_t length) override { send(input, length); }

    std::string name() const override { return "Null"; }
};

}    // namespace

Pipe::Invalid_Message_Number::Invalid_Message_Number(std::string_view where, message_id msg)
    : Invalid_Argument(fmt("Pipe::{}: Invalid message number {}", where, msg)) {}

/*
 * Pipe Constructor
 */
Pipe::Pipe(Filter* f1, Filter* f2, Filter* f3, Filter* f4) : Pipe({f1, f2, f3, f4}) {}

/*
 * Pipe Constructor
 */
Pipe::Pipe(std::initializer_list<Filter*> args) {
    m_outputs = std::make_unique<Output_Buffers>();
    m_pipe = nullptr;
    m_default_read = 0;
    m_inside_msg = false;

    for (auto arg : args) {
        do_append(arg);
    }
}

/*
 * Pipe Destructor
 */
Pipe::~Pipe() { destruct(m_pipe); }

/*
 * Reset the Pipe
 */
void Pipe::reset() {
    destruct(m_pipe);
    m_pipe = nullptr;
    m_inside_msg = false;
}

/*
 * Destroy the Pipe
 */
void Pipe::destruct(Filter* to_kill) {
    if (!to_kill || dynamic_cast<SecureQueue*>(to_kill)) {
        return;
    }
    for (size_t j = 0; j != to_kill->total_ports(); ++j) {
        destruct(to_kill->m_next[j]);
    }
    delete to_kill;
}

/*
 * Test if the Pipe has any data in it
 */
bool Pipe::end_of_data() const { return (remaining() == 0); }

/*
 * Set the default read message
 */
void Pipe::set_default_msg(message_id msg) {
    if (msg >= message_count()) {
        throw Invalid_Argument("Pipe::set_default_msg: msg number is too high");
    }
    m_default_read = msg;
}

/*
 * Process a full message at once
 */
void Pipe::process_msg(const uint8_t input[], size_t length) {
    start_msg();
    write(input, length);
    end_msg();
}

/*
 * Process a full message at once
 */
void Pipe::process_msg(const secure_vector<uint8_t>& input) {
    process_msg(input.data(), input.size());
}

void Pipe::process_msg(const std::vector<uint8_t>& input) {
    process_msg(input.data(), input.size());
}

/*
 * Process a full message at once
 */
void Pipe::process_msg(std::string_view input) {
    process_msg(cast_char_ptr_to_uint8(input.data()), input.length());
}

/*
 * Process a full message at once
 */
void Pipe::process_msg(DataSource& input) {
    start_msg();
    write(input);
    end_msg();
}

/*
 * Start a new message
 */
void Pipe::start_msg() {
    if (m_inside_msg) {
        throw Invalid_State("Pipe::start_msg: Message was already started");
    }
    if (m_pipe == nullptr) {
        m_pipe = new Null_Filter;
    }
    find_endpoints(m_pipe);
    m_pipe->new_msg();
    m_inside_msg = true;
}

/*
 * End the current message
 */
void Pipe::end_msg() {
    if (!m_inside_msg) {
        throw Invalid_State("Pipe::end_msg: Message was already ended");
    }
    m_pipe->finish_msg();
    clear_endpoints(m_pipe);
    if (dynamic_cast<Null_Filter*>(m_pipe)) {
        delete m_pipe;
        m_pipe = nullptr;
    }
    m_inside_msg = false;

    m_outputs->retire();
}

/*
 * Find the endpoints of the Pipe
 */
void Pipe::find_endpoints(Filter* f) {
    for (size_t j = 0; j != f->total_ports(); ++j) {
        if (f->m_next[j] && !dynamic_cast<SecureQueue*>(f->m_next[j])) {
            find_endpoints(f->m_next[j]);
        } else {
            SecureQueue* q = new SecureQueue;
            f->m_next[j] = q;
            m_outputs->add(q);
        }
    }
}

/*
 * Remove the SecureQueues attached to the Filter
 */
void Pipe::clear_endpoints(Filter* f) {
    if (!f) {
        return;
    }
    for (size_t j = 0; j != f->total_ports(); ++j) {
        if (f->m_next[j] && dynamic_cast<SecureQueue*>(f->m_next[j])) {
            f->m_next[j] = nullptr;
        }
        clear_endpoints(f->m_next[j]);
    }
}

void Pipe::append(Filter* filter) { do_append(filter); }

void Pipe::append_filter(Filter* filter) {
    if (m_outputs->message_count() != 0) {
        throw Invalid_State("Cannot call Pipe::append_filter after start_msg");
    }

    do_append(filter);
}

void Pipe::prepend(Filter* filter) { do_prepend(filter); }

void Pipe::prepend_filter(Filter* filter) {
    if (m_outputs->message_count() != 0) {
        throw Invalid_State("Cannot call Pipe::prepend_filter after start_msg");
    }

    do_prepend(filter);
}

/*
 * Append a Filter to the Pipe
 */
void Pipe::do_append(Filter* filter) {
    if (!filter) {
        return;
    }
    if (dynamic_cast<SecureQueue*>(filter)) {
        throw Invalid_Argument("Pipe::append: SecureQueue cannot be used");
    }
    if (filter->m_owned) {
        throw Invalid_Argument("Filters cannot be shared among multiple Pipes");
    }

    if (m_inside_msg) {
        throw Invalid_State("Cannot append to a Pipe while it is processing");
    }

    filter->m_owned = true;

    if (!m_pipe) {
        m_pipe = filter;
    } else {
        m_pipe->attach(filter);
    }
}

/*
 * Prepend a Filter to the Pipe
 */
void Pipe::do_prepend(Filter* filter) {
    if (m_inside_msg) {
        throw Invalid_State("Cannot prepend to a Pipe while it is processing");
    }
    if (!filter) {
        return;
    }
    if (dynamic_cast<SecureQueue*>(filter)) {
        throw Invalid_Argument("Pipe::prepend: SecureQueue cannot be used");
    }
    if (filter->m_owned) {
        throw Invalid_Argument("Filters cannot be shared among multiple Pipes");
    }

    filter->m_owned = true;

    if (m_pipe) {
        filter->attach(m_pipe);
    }
    m_pipe = filter;
}

/*
 * Pop a Filter off the Pipe
 */
void Pipe::pop() {
    if (m_inside_msg) {
        throw Invalid_State("Cannot pop off a Pipe while it is processing");
    }

    if (!m_pipe) {
        return;
    }

    if (m_pipe->total_ports() > 1) {
        throw Invalid_State("Cannot pop off a Filter with multiple ports");
    }

    size_t to_remove = m_pipe->owns() + 1;

    while (to_remove--) {
        std::unique_ptr<Filter> to_destroy(m_pipe);
        m_pipe = m_pipe->m_next[0];
    }
}

/*
 * Return the number of messages in this Pipe
 */
Pipe::message_id Pipe::message_count() const { return m_outputs->message_count(); }

/*
 * Static Member Variables
 */
const Pipe::message_id Pipe::LAST_MESSAGE = static_cast<Pipe::message_id>(-2);

const Pipe::message_id Pipe::DEFAULT_MESSAGE = static_cast<Pipe::message_id>(-1);

}    // namespace Botan
/*
 * Pipe I/O
 * (C) 1999-2007 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include <istream>

namespace Botan {

/*
 * Write data from a pipe into an ostream
 */
std::ostream& operator<<(std::ostream& stream, Pipe& pipe) {
    secure_vector<uint8_t> buffer(DefaultBufferSize);
    while (stream.good() && pipe.remaining()) {
        const size_t got = pipe.read(buffer.data(), buffer.size());
        stream.write(cast_uint8_ptr_to_char(buffer.data()), got);
    }
    if (!stream.good()) {
        throw Stream_IO_Error("Pipe output operator (iostream) has failed");
    }
    return stream;
}

/*
 * Read data from an istream into a pipe
 */
std::istream& operator>>(std::istream& stream, Pipe& pipe) {
    secure_vector<uint8_t> buffer(DefaultBufferSize);
    while (stream.good()) {
        stream.read(cast_uint8_ptr_to_char(buffer.data()), buffer.size());
        const size_t got = static_cast<size_t>(stream.gcount());
        pipe.write(buffer.data(), got);
    }
    if (stream.bad() || (stream.fail() && !stream.eof())) {
        throw Stream_IO_Error("Pipe input operator (iostream) has failed");
    }
    return stream;
}

}    // namespace Botan
/*
 * Pipe Reading/Writing
 * (C) 1999-2007 Jack Lloyd
 *     2012 Markus Wanner
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/*
 * Look up the canonical ID for a queue
 */
Pipe::message_id Pipe::get_message_no(std::string_view func_name, message_id msg) const {
    if (msg == DEFAULT_MESSAGE) {
        msg = default_msg();
    } else if (msg == LAST_MESSAGE) {
        msg = message_count() - 1;
    }

    if (msg >= message_count()) {
        throw Invalid_Message_Number(func_name, msg);
    }

    return msg;
}

/*
 * Write into a Pipe
 */
void Pipe::write(const uint8_t input[], size_t length) {
    if (!m_inside_msg) {
        throw Invalid_State("Cannot write to a Pipe while it is not processing");
    }
    m_pipe->write(input, length);
}

/*
 * Write a string into a Pipe
 */
void Pipe::write(std::string_view str) { write(cast_char_ptr_to_uint8(str.data()), str.size()); }

/*
 * Write a single byte into a Pipe
 */
void Pipe::write(uint8_t input) { write(&input, 1); }

/*
 * Write the contents of a DataSource into a Pipe
 */
void Pipe::write(DataSource& source) {
    secure_vector<uint8_t> buffer(DefaultBufferSize);
    while (!source.end_of_data()) {
        size_t got = source.read(buffer.data(), buffer.size());
        write(buffer.data(), got);
    }
}

/*
 * Read some data from the pipe
 */
size_t Pipe::read(uint8_t output[], size_t length, message_id msg) {
    return m_outputs->read(output, length, get_message_no("read", msg));
}

/*
 * Read some data from the pipe
 */
size_t Pipe::read(uint8_t output[], size_t length) { return read(output, length, DEFAULT_MESSAGE); }

/*
 * Read a single byte from the pipe
 */
size_t Pipe::read(uint8_t& out, message_id msg) { return read(&out, 1, msg); }

/*
 * Return all data in the pipe
 */
secure_vector<uint8_t> Pipe::read_all(message_id msg) {
    msg = ((msg != DEFAULT_MESSAGE) ? msg : default_msg());
    secure_vector<uint8_t> buffer(remaining(msg));
    size_t got = read(buffer.data(), buffer.size(), msg);
    buffer.resize(got);
    return buffer;
}

/*
 * Return all data in the pipe as a string
 */
std::string Pipe::read_all_as_string(message_id msg) {
    msg = ((msg != DEFAULT_MESSAGE) ? msg : default_msg());
    secure_vector<uint8_t> buffer(DefaultBufferSize);
    std::string str;
    str.reserve(remaining(msg));

    while (true) {
        size_t got = read(buffer.data(), buffer.size(), msg);
        if (got == 0) {
            break;
        }
        str.append(cast_uint8_ptr_to_char(buffer.data()), got);
    }

    return str;
}

/*
 * Find out how many bytes are ready to read
 */
size_t Pipe::remaining(message_id msg) const {
    return m_outputs->remaining(get_message_no("remaining", msg));
}

/*
 * Peek at some data in the pipe
 */
size_t Pipe::peek(uint8_t output[], size_t length, size_t offset, message_id msg) const {
    return m_outputs->peek(output, length, offset, get_message_no("peek", msg));
}

/*
 * Peek at some data in the pipe
 */
size_t Pipe::peek(uint8_t output[], size_t length, size_t offset) const {
    return peek(output, length, offset, DEFAULT_MESSAGE);
}

/*
 * Peek at a byte in the pipe
 */
size_t Pipe::peek(uint8_t& out, size_t offset, message_id msg) const {
    return peek(&out, 1, offset, msg);
}

size_t Pipe::get_bytes_read() const { return m_outputs->get_bytes_read(default_msg()); }

size_t Pipe::get_bytes_read(message_id msg) const { return m_outputs->get_bytes_read(msg); }

bool Pipe::check_available(size_t n) { return (n <= remaining(default_msg())); }

bool Pipe::check_available_msg(size_t n, message_id msg) const { return (n <= remaining(msg)); }

}    // namespace Botan
/*
 * SecureQueue
 * (C) 1999-2007 Jack Lloyd
 *     2012 Markus Wanner
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/**
 * A node in a SecureQueue
 */
class SecureQueueNode final {
   public:
    SecureQueueNode() : m_buffer(DefaultBufferSize) {
        m_next = nullptr;
        m_start = m_end = 0;
    }

    ~SecureQueueNode() {
        m_next = nullptr;
        m_start = m_end = 0;
    }

    SecureQueueNode(const SecureQueueNode& other) = delete;
    SecureQueueNode(SecureQueueNode&& other) = delete;
    SecureQueueNode& operator=(const SecureQueueNode& other) = delete;
    SecureQueueNode& operator=(SecureQueueNode&& other) = delete;

    size_t write(const uint8_t input[], size_t length) {
        size_t copied = std::min<size_t>(length, m_buffer.size() - m_end);
        copy_mem(m_buffer.data() + m_end, input, copied);
        m_end += copied;
        return copied;
    }

    size_t read(uint8_t output[], size_t length) {
        size_t copied = std::min(length, m_end - m_start);
        copy_mem(output, m_buffer.data() + m_start, copied);
        m_start += copied;
        return copied;
    }

    size_t peek(uint8_t output[], size_t length, size_t offset = 0) {
        const size_t left = m_end - m_start;
        if (offset >= left) {
            return 0;
        }
        size_t copied = std::min(length, left - offset);
        copy_mem(output, m_buffer.data() + m_start + offset, copied);
        return copied;
    }

    size_t size() const { return (m_end - m_start); }

   private:
    friend class SecureQueue;
    SecureQueueNode* m_next;
    secure_vector<uint8_t> m_buffer;
    size_t m_start, m_end;
};

/*
 * Create a SecureQueue
 */
SecureQueue::SecureQueue() {
    m_bytes_read = 0;
    set_next(nullptr, 0);
    m_head = m_tail = new SecureQueueNode;
}

/*
 * Copy a SecureQueue
 */
SecureQueue::SecureQueue(const SecureQueue& input) : Fanout_Filter(), DataSource() {
    m_bytes_read = 0;
    set_next(nullptr, 0);

    m_head = m_tail = new SecureQueueNode;
    SecureQueueNode* temp = input.m_head;
    while (temp) {
        write(&temp->m_buffer[temp->m_start], temp->m_end - temp->m_start);
        temp = temp->m_next;
    }
}

/*
 * Destroy this SecureQueue
 */
void SecureQueue::destroy() {
    SecureQueueNode* temp = m_head;
    while (temp) {
        SecureQueueNode* holder = temp->m_next;
        delete temp;
        temp = holder;
    }
    m_head = m_tail = nullptr;
}

/*
 * Copy a SecureQueue
 */
SecureQueue& SecureQueue::operator=(const SecureQueue& input) {
    if (this == &input) {
        return *this;
    }

    destroy();
    m_bytes_read = input.get_bytes_read();
    m_head = m_tail = new SecureQueueNode;
    SecureQueueNode* temp = input.m_head;
    while (temp) {
        write(&temp->m_buffer[temp->m_start], temp->m_end - temp->m_start);
        temp = temp->m_next;
    }
    return (*this);
}

/*
 * Add some bytes to the queue
 */
void SecureQueue::write(const uint8_t input[], size_t length) {
    if (!m_head) {
        m_head = m_tail = new SecureQueueNode;
    }
    while (length) {
        const size_t n = m_tail->write(input, length);
        input += n;
        length -= n;
        if (length) {
            m_tail->m_next = new SecureQueueNode;
            m_tail = m_tail->m_next;
        }
    }
}

/*
 * Read some bytes from the queue
 */
size_t SecureQueue::read(uint8_t output[], size_t length) {
    size_t got = 0;
    while (length && m_head) {
        const size_t n = m_head->read(output, length);
        output += n;
        got += n;
        length -= n;
        if (m_head->size() == 0) {
            SecureQueueNode* holder = m_head->m_next;
            delete m_head;
            m_head = holder;
        }
    }
    m_bytes_read += got;
    return got;
}

/*
 * Read data, but do not remove it from queue
 */
size_t SecureQueue::peek(uint8_t output[], size_t length, size_t offset) const {
    SecureQueueNode* current = m_head;

    while (offset && current) {
        if (offset >= current->size()) {
            offset -= current->size();
            current = current->m_next;
        } else {
            break;
        }
    }

    size_t got = 0;
    while (length && current) {
        const size_t n = current->peek(output, length, offset);
        offset = 0;
        output += n;
        got += n;
        length -= n;
        current = current->m_next;
    }
    return got;
}

/**
 * Return how many bytes have been read so far.
 */
size_t SecureQueue::get_bytes_read() const { return m_bytes_read; }

/*
 * Return how many bytes the queue holds
 */
size_t SecureQueue::size() const {
    SecureQueueNode* current = m_head;
    size_t count = 0;

    while (current) {
        count += current->size();
        current = current->m_next;
    }
    return count;
}

/*
 * Test if the queue has any data in it
 */
bool SecureQueue::end_of_data() const { return (size() == 0); }

bool SecureQueue::empty() const { return (size() == 0); }

}    // namespace Botan
/*
 * Threaded Fork
 * (C) 2013 Joel Low
 *     2013 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_THREAD_UTILS)

#include <functional>

namespace Botan {

struct Threaded_Fork_Data {
    /*
     * Semaphore for indicating that there is work to be done (or to
     * quit)
     */
    Semaphore m_input_ready_semaphore;

    /*
     * Synchronises all threads to complete processing data in lock-step.
     */
    Barrier m_input_complete_barrier;

    /*
     * The work that needs to be done. This should be only when the threads
     * are NOT running (i.e. before notifying the work condition, after
     * the input_complete_barrier has reset.)
     */
    const uint8_t* m_input = nullptr;

    /*
     * The length of the work that needs to be done.
     */
    size_t m_input_length = 0;
};

/*
 * Threaded_Fork constructor
 */
Threaded_Fork::Threaded_Fork(Filter* f1, Filter* f2, Filter* f3, Filter* f4)
    : Fork(nullptr, static_cast<size_t>(0)), m_thread_data(new Threaded_Fork_Data) {
    Filter* filters[4] = {f1, f2, f3, f4};
    set_next(filters, 4);
}

/*
 * Threaded_Fork constructor
 */
Threaded_Fork::Threaded_Fork(Filter* filters[], size_t count)
    : Fork(nullptr, static_cast<size_t>(0)), m_thread_data(new Threaded_Fork_Data) {
    set_next(filters, count);
}

Threaded_Fork::~Threaded_Fork() {
    m_thread_data->m_input = nullptr;
    m_thread_data->m_input_length = 0;

    m_thread_data->m_input_ready_semaphore.release(m_threads.size());

    for (auto& thread : m_threads) {
        thread->join();
    }
}

std::string Threaded_Fork::name() const { return "Threaded Fork"; }

void Threaded_Fork::set_next(Filter* f[], size_t n) {
    Fork::set_next(f, n);
    n = m_next.size();

    if (n < m_threads.size()) {
        m_threads.resize(n);
    } else {
        m_threads.reserve(n);
        for (size_t i = m_threads.size(); i != n; ++i) {
            m_threads.push_back(std::make_shared<std::thread>(
                std::bind(&Threaded_Fork::thread_entry, this, m_next[i])));
        }
    }
}

void Threaded_Fork::send(const uint8_t input[], size_t length) {
    if (!m_write_queue.empty()) {
        thread_delegate_work(m_write_queue.data(), m_write_queue.size());
    }
    thread_delegate_work(input, length);

    bool nothing_attached = true;
    for (size_t j = 0; j != total_ports(); ++j) {
        if (m_next[j]) {
            nothing_attached = false;
        }
    }

    if (nothing_attached) {
        m_write_queue += std::make_pair(input, length);
    } else {
        m_write_queue.clear();
    }
}

void Threaded_Fork::thread_delegate_work(const uint8_t input[], size_t length) {
    // Set the data to do.
    m_thread_data->m_input = input;
    m_thread_data->m_input_length = length;

    // Let the workers start processing.
    m_thread_data->m_input_complete_barrier.wait(total_ports() + 1);
    m_thread_data->m_input_ready_semaphore.release(total_ports());

    // Wait for all the filters to finish processing.
    m_thread_data->m_input_complete_barrier.sync();

    // Reset the thread data
    m_thread_data->m_input = nullptr;
    m_thread_data->m_input_length = 0;
}

void Threaded_Fork::thread_entry(Filter* filter) {
    while (true) {
        m_thread_data->m_input_ready_semaphore.acquire();

        if (!m_thread_data->m_input) {
            break;
        }

        filter->write(m_thread_data->m_input, m_thread_data->m_input_length);
        m_thread_data->m_input_complete_barrier.sync();
    }
}

}    // namespace Botan

#endif
/*
 * Hash Functions
 * (C) 2015 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_ADLER32)
#endif

#if defined(BOTAN_HAS_CRC24)
#endif

#if defined(BOTAN_HAS_CRC32)
#endif

#if defined(BOTAN_HAS_GOST_34_11)
#endif

#if defined(BOTAN_HAS_KECCAK)
#endif

#if defined(BOTAN_HAS_MD4)
#endif

#if defined(BOTAN_HAS_MD5)
#endif

#if defined(BOTAN_HAS_RIPEMD_160)
#endif

#if defined(BOTAN_HAS_SHA1)
#endif

#if defined(BOTAN_HAS_SHA2_32)
#endif

#if defined(BOTAN_HAS_SHA2_64)
#endif

#if defined(BOTAN_HAS_SHA3)
#endif

#if defined(BOTAN_HAS_SHAKE)
#endif

#if defined(BOTAN_HAS_SKEIN_512)
#endif

#if defined(BOTAN_HAS_STREEBOG)
#endif

#if defined(BOTAN_HAS_SM3)
#endif

#if defined(BOTAN_HAS_WHIRLPOOL)
#endif

#if defined(BOTAN_HAS_PARALLEL_HASH)
#endif

#if defined(BOTAN_HAS_TRUNCATED_HASH)
#endif

#if defined(BOTAN_HAS_COMB4P)
#endif

#if defined(BOTAN_HAS_BLAKE2B)
#endif

#if defined(BOTAN_HAS_BLAKE2S)
#endif

#if defined(BOTAN_HAS_COMMONCRYPTO)
#endif

namespace Botan {

std::unique_ptr<HashFunction> HashFunction::create(std::string_view algo_spec,
                                                   std::string_view provider) {
#if defined(BOTAN_HAS_COMMONCRYPTO)
    if (provider.empty() || provider == "commoncrypto") {
        if (auto hash = make_commoncrypto_hash(algo_spec)) return hash;

        if (!provider.empty()) return nullptr;
    }
#endif

    if (provider.empty() == false && provider != "base") {
        return nullptr;    // unknown provider
    }

#if defined(BOTAN_HAS_SHA1)
    if (algo_spec == "SHA-1") {
        return std::make_unique<SHA_1>();
    }
#endif

#if defined(BOTAN_HAS_SHA2_32)
    if (algo_spec == "SHA-224") {
        return std::make_unique<SHA_224>();
    }

    if (algo_spec == "SHA-256") {
        return std::make_unique<SHA_256>();
    }
#endif

#if defined(BOTAN_HAS_SHA2_64)
    if (algo_spec == "SHA-384") {
        return std::make_unique<SHA_384>();
    }

    if (algo_spec == "SHA-512") {
        return std::make_unique<SHA_512>();
    }

    if (algo_spec == "SHA-512-256") {
        return std::make_unique<SHA_512_256>();
    }
#endif

#if defined(BOTAN_HAS_RIPEMD_160)
    if (algo_spec == "RIPEMD-160") {
        return std::make_unique<RIPEMD_160>();
    }
#endif

#if defined(BOTAN_HAS_WHIRLPOOL)
    if (algo_spec == "Whirlpool") {
        return std::make_unique<Whirlpool>();
    }
#endif

#if defined(BOTAN_HAS_MD5)
    if (algo_spec == "MD5") {
        return std::make_unique<MD5>();
    }
#endif

#if defined(BOTAN_HAS_MD4)
    if (algo_spec == "MD4") {
        return std::make_unique<MD4>();
    }
#endif

#if defined(BOTAN_HAS_GOST_34_11)
    if (algo_spec == "GOST-R-34.11-94" || algo_spec == "GOST-34.11") {
        return std::make_unique<GOST_34_11>();
    }
#endif

#if defined(BOTAN_HAS_ADLER32)
    if (algo_spec == "Adler32") {
        return std::make_unique<Adler32>();
    }
#endif

#if defined(BOTAN_HAS_CRC24)
    if (algo_spec == "CRC24") {
        return std::make_unique<CRC24>();
    }
#endif

#if defined(BOTAN_HAS_CRC32)
    if (algo_spec == "CRC32") {
        return std::make_unique<CRC32>();
    }
#endif

#if defined(BOTAN_HAS_STREEBOG)
    if (algo_spec == "Streebog-256") {
        return std::make_unique<Streebog>(256);
    }
    if (algo_spec == "Streebog-512") {
        return std::make_unique<Streebog>(512);
    }
#endif

#if defined(BOTAN_HAS_SM3)
    if (algo_spec == "SM3") {
        return std::make_unique<SM3>();
    }
#endif

    const SCAN_Name req(algo_spec);

#if defined(BOTAN_HAS_SKEIN_512)
    if (req.algo_name() == "Skein-512") {
        return std::make_unique<Skein_512>(req.arg_as_integer(0, 512), req.arg(1, ""));
    }
#endif

#if defined(BOTAN_HAS_BLAKE2B)
    if (req.algo_name() == "Blake2b" || req.algo_name() == "BLAKE2b") {
        return std::make_unique<BLAKE2b>(req.arg_as_integer(0, 512));
    }
#endif

#if defined(BOTAN_HAS_BLAKE2S)
    if (req.algo_name() == "Blake2s" || req.algo_name() == "BLAKE2s") {
        return std::make_unique<BLAKE2s>(req.arg_as_integer(0, 256));
    }
#endif

#if defined(BOTAN_HAS_KECCAK)
    if (req.algo_name() == "Keccak-1600") {
        return std::make_unique<Keccak_1600>(req.arg_as_integer(0, 512));
    }
#endif

#if defined(BOTAN_HAS_SHA3)
    if (req.algo_name() == "SHA-3") {
        return std::make_unique<SHA_3>(req.arg_as_integer(0, 512));
    }
#endif

#if defined(BOTAN_HAS_SHAKE)
    if (req.algo_name() == "SHAKE-128" && req.arg_count() == 1) {
        return std::make_unique<SHAKE_128>(req.arg_as_integer(0));
    }
    if (req.algo_name() == "SHAKE-256" && req.arg_count() == 1) {
        return std::make_unique<SHAKE_256>(req.arg_as_integer(0));
    }
#endif

#if defined(BOTAN_HAS_PARALLEL_HASH)
    if (req.algo_name() == "Parallel") {
        std::vector<std::unique_ptr<HashFunction>> hashes;

        for (size_t i = 0; i != req.arg_count(); ++i) {
            auto h = HashFunction::create(req.arg(i));
            if (!h) {
                return nullptr;
            }
            hashes.push_back(std::move(h));
        }

        return std::make_unique<Parallel>(hashes);
    }
#endif

#if defined(BOTAN_HAS_TRUNCATED_HASH)
    if (req.algo_name() == "Truncated" && req.arg_count() == 2) {
        auto hash = HashFunction::create(req.arg(0));
        if (!hash) {
            return nullptr;
        }

        return std::make_unique<Truncated_Hash>(std::move(hash), req.arg_as_integer(1));
    }
#endif

#if defined(BOTAN_HAS_COMB4P)
    if (req.algo_name() == "Comb4P" && req.arg_count() == 2) {
        auto h1 = HashFunction::create(req.arg(0));
        auto h2 = HashFunction::create(req.arg(1));

        if (h1 && h2) {
            return std::make_unique<Comb4P>(std::move(h1), std::move(h2));
        }
    }
#endif

    return nullptr;
}

// static
std::unique_ptr<HashFunction> HashFunction::create_or_throw(std::string_view algo,
                                                            std::string_view provider) {
    if (auto hash = HashFunction::create(algo, provider)) {
        return hash;
    }
    throw Lookup_Error("Hash", algo, provider);
}

std::vector<std::string> HashFunction::providers(std::string_view algo_spec) {
    return probe_providers_of<HashFunction>(algo_spec, {"base", "commoncrypto"});
}

}    // namespace Botan
/*
 * Hash Function Identification
 * (C) 1999-2008 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

namespace {

const uint8_t MD5_PKCS_ID[] = {0x30, 0x20, 0x30, 0x0C, 0x06, 0x08, 0x2A, 0x86, 0x48,
                               0x86, 0xF7, 0x0D, 0x02, 0x05, 0x05, 0x00, 0x04, 0x10};

const uint8_t RIPEMD_160_PKCS_ID[] = {0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x24,
                                      0x03, 0x02, 0x01, 0x05, 0x00, 0x04, 0x14};

const uint8_t SHA_1_PKCS_ID[] = {0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E,
                                 0x03, 0x02, 0x1A, 0x05, 0x00, 0x04, 0x14};

const uint8_t SHA_224_PKCS_ID[] = {0x30, 0x2D, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                   0x65, 0x03, 0x04, 0x02, 0x04, 0x05, 0x00, 0x04, 0x1C};

const uint8_t SHA_256_PKCS_ID[] = {0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                   0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20};

const uint8_t SHA_384_PKCS_ID[] = {0x30, 0x41, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                   0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30};

const uint8_t SHA_512_PKCS_ID[] = {0x30, 0x51, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                   0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40};

const uint8_t SHA_512_256_PKCS_ID[] = {0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                       0x65, 0x03, 0x04, 0x02, 0x06, 0x05, 0x00, 0x04, 0x20};

const uint8_t SHA3_224_PKCS_ID[] = {0x30, 0x2D, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                    0x65, 0x03, 0x04, 0x02, 0x07, 0x05, 0x00, 0x04, 0x1C};

const uint8_t SHA3_256_PKCS_ID[] = {0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                    0x65, 0x03, 0x04, 0x02, 0x08, 0x05, 0x00, 0x04, 0x20};

const uint8_t SHA3_384_PKCS_ID[] = {0x30, 0x41, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                    0x65, 0x03, 0x04, 0x02, 0x09, 0x05, 0x00, 0x04, 0x30};

const uint8_t SHA3_512_PKCS_ID[] = {0x30, 0x51, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                    0x65, 0x03, 0x04, 0x02, 0x0A, 0x05, 0x00, 0x04, 0x40};

const uint8_t SM3_PKCS_ID[] = {
    0x30, 0x30, 0x30, 0x0C, 0x06, 0x08, 0x2A, 0x81, 0x1C,
    0xCF, 0x55, 0x01, 0x83, 0x11, 0x05, 0x00, 0x04, 0x20,
};

}    // namespace

/*
 * HashID as specified by PKCS
 */
std::vector<uint8_t> pkcs_hash_id(std::string_view name) {
    // Special case for SSL/TLS RSA signatures
    if (name == "Parallel(MD5,SHA-1)") {
        return std::vector<uint8_t>();
    }

    // If you add a value to this function, also update test_hash_id.cpp

    if (name == "MD5") {
        return std::vector<uint8_t>(MD5_PKCS_ID, MD5_PKCS_ID + sizeof(MD5_PKCS_ID));
    }

    if (name == "RIPEMD-160") {
        return std::vector<uint8_t>(RIPEMD_160_PKCS_ID,
                                    RIPEMD_160_PKCS_ID + sizeof(RIPEMD_160_PKCS_ID));
    }

    if (name == "SHA-1") {
        return std::vector<uint8_t>(SHA_1_PKCS_ID, SHA_1_PKCS_ID + sizeof(SHA_1_PKCS_ID));
    }

    if (name == "SHA-224") {
        return std::vector<uint8_t>(SHA_224_PKCS_ID, SHA_224_PKCS_ID + sizeof(SHA_224_PKCS_ID));
    }

    if (name == "SHA-256") {
        return std::vector<uint8_t>(SHA_256_PKCS_ID, SHA_256_PKCS_ID + sizeof(SHA_256_PKCS_ID));
    }

    if (name == "SHA-384") {
        return std::vector<uint8_t>(SHA_384_PKCS_ID, SHA_384_PKCS_ID + sizeof(SHA_384_PKCS_ID));
    }

    if (name == "SHA-512") {
        return std::vector<uint8_t>(SHA_512_PKCS_ID, SHA_512_PKCS_ID + sizeof(SHA_512_PKCS_ID));
    }

    if (name == "SHA-512-256") {
        return std::vector<uint8_t>(SHA_512_256_PKCS_ID,
                                    SHA_512_256_PKCS_ID + sizeof(SHA_512_256_PKCS_ID));
    }

    if (name == "SHA-3(224)") {
        return std::vector<uint8_t>(SHA3_224_PKCS_ID, SHA3_224_PKCS_ID + sizeof(SHA3_224_PKCS_ID));
    }

    if (name == "SHA-3(256)") {
        return std::vector<uint8_t>(SHA3_256_PKCS_ID, SHA3_256_PKCS_ID + sizeof(SHA3_256_PKCS_ID));
    }

    if (name == "SHA-3(384)") {
        return std::vector<uint8_t>(SHA3_384_PKCS_ID, SHA3_384_PKCS_ID + sizeof(SHA3_384_PKCS_ID));
    }

    if (name == "SHA-3(512)") {
        return std::vector<uint8_t>(SHA3_512_PKCS_ID, SHA3_512_PKCS_ID + sizeof(SHA3_512_PKCS_ID));
    }

    if (name == "SM3") {
        return std::vector<uint8_t>(SM3_PKCS_ID, SM3_PKCS_ID + sizeof(SM3_PKCS_ID));
    }

    throw Invalid_Argument("No PKCS #1 identifier for " + std::string(name));
}

/*
 * HashID as specified by IEEE 1363/X9.31
 */
uint8_t ieee1363_hash_id(std::string_view name) {
    if (name == "SHA-1") {
        return 0x33;
    }

    if (name == "SHA-224") {
        return 0x38;
    }
    if (name == "SHA-256") {
        return 0x34;
    }
    if (name == "SHA-384") {
        return 0x36;
    }
    if (name == "SHA-512") {
        return 0x35;
    }

    if (name == "RIPEMD-160") {
        return 0x31;
    }

    if (name == "Whirlpool") {
        return 0x37;
    }

    return 0;
}

}    // namespace Botan
/*
 * Hex Encoding and Decoding
 * (C) 2010,2020 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

namespace {

uint16_t hex_encode_2nibble(uint8_t n8, bool uppercase) {
    // Offset for upper or lower case 'a' resp
    const uint16_t a_mask = uppercase ? 0x0707 : 0x2727;

    const uint16_t n = (static_cast<uint16_t>(n8 & 0xF0) << 4) | (n8 & 0x0F);
    // n >= 10? If so add offset
    const uint16_t diff = swar_lt<uint16_t>(0x0909, n) & a_mask;
    // Can't overflow between bytes, so don't need explicit SWAR addition:
    return n + 0x3030 + diff;
}

}    // namespace

void hex_encode(char output[], const uint8_t input[], size_t input_length, bool uppercase) {
    for (size_t i = 0; i != input_length; ++i) {
        const uint16_t h = hex_encode_2nibble(input[i], uppercase);
        output[2 * i] = get_byte<0>(h);
        output[2 * i + 1] = get_byte<1>(h);
    }
}

std::string hex_encode(const uint8_t input[], size_t input_length, bool uppercase) {
    std::string output(2 * input_length, 0);

    if (input_length) {
        hex_encode(&output.front(), input, input_length, uppercase);
    }

    return output;
}

namespace {

uint8_t hex_char_to_bin(char input) {
    // Starts of valid value ranges (v_lo) and their lengths (v_range)
    constexpr uint64_t v_lo = make_uint64(0, '0', 'a', 'A', ' ', '\n', '\t', '\r');
    constexpr uint64_t v_range = make_uint64(0, 10, 6, 6, 1, 1, 1, 1);

    const uint8_t x = static_cast<uint8_t>(input);
    const uint64_t x8 = x * 0x0101010101010101;

    const uint64_t v_mask = swar_in_range<uint64_t>(x8, v_lo, v_range) ^ 0x8000000000000000;

    // This is the offset added to x to get the value we need
    const uint64_t val_v = 0xd0a9c960767773 ^ static_cast<uint64_t>(0xFF - x) << 56;

    return x + static_cast<uint8_t>(val_v >> (8 * index_of_first_set_byte(v_mask)));
}

}    // namespace

size_t hex_decode(uint8_t output[], const char input[], size_t input_length, size_t& input_consumed,
                  bool ignore_ws) {
    uint8_t* out_ptr = output;
    bool top_nibble = true;

    clear_mem(output, input_length / 2);

    for (size_t i = 0; i != input_length; ++i) {
        const uint8_t bin = hex_char_to_bin(input[i]);

        if (bin >= 0x10) {
            if (bin == 0x80 && ignore_ws) {
                continue;
            }

            throw Invalid_Argument(
                fmt("hex_decode: invalid character '{}'", format_char_for_display(input[i])));
        }

        if (top_nibble) {
            *out_ptr |= bin << 4;
        } else {
            *out_ptr |= bin;
        }

        top_nibble = !top_nibble;
        if (top_nibble) {
            ++out_ptr;
        }
    }

    input_consumed = input_length;
    size_t written = (out_ptr - output);

    /*
     * We only got half of a uint8_t at the end; zap the half-written
     * output and mark it as unread
     */
    if (!top_nibble) {
        *out_ptr = 0;
        input_consumed -= 1;
    }

    return written;
}

size_t hex_decode(uint8_t output[], const char input[], size_t input_length, bool ignore_ws) {
    size_t consumed = 0;
    size_t written = hex_decode(output, input, input_length, consumed, ignore_ws);

    if (consumed != input_length) {
        throw Invalid_Argument("hex_decode: input did not have full bytes");
    }

    return written;
}

size_t hex_decode(uint8_t output[], std::string_view input, bool ignore_ws) {
    return hex_decode(output, input.data(), input.length(), ignore_ws);
}

size_t hex_decode(std::span<uint8_t> output, std::string_view input, bool ignore_ws) {
    return hex_decode(output.data(), input.data(), input.length(), ignore_ws);
}

secure_vector<uint8_t> hex_decode_locked(const char input[], size_t input_length, bool ignore_ws) {
    secure_vector<uint8_t> bin(1 + input_length / 2);

    size_t written = hex_decode(bin.data(), input, input_length, ignore_ws);

    bin.resize(written);
    return bin;
}

secure_vector<uint8_t> hex_decode_locked(std::string_view input, bool ignore_ws) {
    return hex_decode_locked(input.data(), input.size(), ignore_ws);
}

std::vector<uint8_t> hex_decode(const char input[], size_t input_length, bool ignore_ws) {
    std::vector<uint8_t> bin(1 + input_length / 2);

    size_t written = hex_decode(bin.data(), input, input_length, ignore_ws);

    bin.resize(written);
    return bin;
}

std::vector<uint8_t> hex_decode(std::string_view input, bool ignore_ws) {
    return hex_decode(input.data(), input.size(), ignore_ws);
}

}    // namespace Botan
/*
 * HMAC
 * (C) 1999-2007,2014,2020 Jack Lloyd
 *     2007 Yves Jerschow
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/*
 * Update a HMAC Calculation
 */
void HMAC::add_data(std::span<const uint8_t> input) {
    assert_key_material_set();
    m_hash->update(input);
}

/*
 * Finalize a HMAC Calculation
 */
void HMAC::final_result(std::span<uint8_t> mac) {
    assert_key_material_set();
    m_hash->final(mac);
    m_hash->update(m_okey);
    m_hash->update(mac.first(m_hash_output_length));
    m_hash->final(mac);
    m_hash->update(m_ikey);
}

Key_Length_Specification HMAC::key_spec() const {
    // Support very long lengths for things like PBKDF2 and the TLS PRF
    return Key_Length_Specification(0, 4096);
}

size_t HMAC::output_length() const { return m_hash_output_length; }

bool HMAC::has_keying_material() const { return !m_okey.empty(); }

/*
 * HMAC Key Schedule
 */
void HMAC::key_schedule(std::span<const uint8_t> key) {
    const uint8_t ipad = 0x36;
    const uint8_t opad = 0x5C;

    m_hash->clear();

    m_ikey.resize(m_hash_block_size);
    m_okey.resize(m_hash_block_size);

    clear_mem(m_ikey.data(), m_ikey.size());
    clear_mem(m_okey.data(), m_okey.size());

    /*
     * Sometimes the HMAC key length itself is sensitive, as with PBKDF2 where it
     * reveals the length of the passphrase. Make some attempt to hide this to
     * side channels. Clearly if the secret is longer than the block size then the
     * branch to hash first reveals that. In addition, counting the number of
     * compression functions executed reveals the size at the granularity of the
     * hash function's block size.
     *
     * The greater concern is for smaller keys; being able to detect when a
     * passphrase is say 4 bytes may assist choosing weaker targets. Even though
     * the loop bounds are constant, we can only actually read key[0..length] so
     * it doesn't seem possible to make this computation truly constant time.
     *
     * We don't mind leaking if the length is exactly zero since that's
     * trivial to simply check.
     */

    if (key.size() > m_hash_block_size) {
        m_hash->update(key);
        m_hash->final(m_ikey.data());
    } else if (key.size() >= 20) {
        // For long keys we just leak the length either it is a cryptovariable
        // or a long enough password that just the length is not a useful signal
        copy_mem(std::span{m_ikey}.first(key.size()), key);
    } else if (!key.empty()) {
        for (size_t i = 0, i_mod_length = 0; i != m_hash_block_size; ++i) {
            /*
            access key[i % length] but avoiding division due to variable
            time computation on some processors.
            */
            auto needs_reduction = CT::Mask<size_t>::is_lte(key.size(), i_mod_length);
            i_mod_length = needs_reduction.select(0, i_mod_length);
            const uint8_t kb = key[i_mod_length];

            auto in_range = CT::Mask<size_t>::is_lt(i, key.size());
            m_ikey[i] = static_cast<uint8_t>(in_range.if_set_return(kb));
            i_mod_length += 1;
        }
    }

    for (size_t i = 0; i != m_hash_block_size; ++i) {
        m_ikey[i] ^= ipad;
        m_okey[i] = m_ikey[i] ^ ipad ^ opad;
    }

    m_hash->update(m_ikey);
}

/*
 * Clear memory of sensitive data
 */
void HMAC::clear() {
    m_hash->clear();
    zap(m_ikey);
    zap(m_okey);
}

/*
 * Return the name of this type
 */
std::string HMAC::name() const { return fmt("HMAC({})", m_hash->name()); }

/*
 * Return a new_object of this object
 */
std::unique_ptr<MessageAuthenticationCode> HMAC::new_object() const {
    return std::make_unique<HMAC>(m_hash->new_object());
}

/*
 * HMAC Constructor
 */
HMAC::HMAC(std::unique_ptr<HashFunction> hash)
    : m_hash(std::move(hash)),
      m_hash_output_length(m_hash->output_length()),
      m_hash_block_size(m_hash->hash_block_size()) {
    BOTAN_ARG_CHECK(m_hash_block_size >= m_hash_output_length,
                    "HMAC is not compatible with this hash function");
}

}    // namespace Botan
/*
 * HMAC_DRBG
 * (C) 2014,2015,2016 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

namespace {

size_t hmac_drbg_security_level(size_t mac_output_length) {
    // security strength of the hash function
    // for pre-image resistance (see NIST SP 800-57)
    // SHA-1: 128 bits
    // SHA-224, SHA-512/224: 192 bits,
    // SHA-256, SHA-512/256, SHA-384, SHA-512: >= 256 bits
    // NIST SP 800-90A only supports up to 256 bits though

    if (mac_output_length < 32) {
        return (mac_output_length - 4) * 8;
    } else {
        return 32 * 8;
    }
}

void check_limits(size_t reseed_interval, size_t max_number_of_bytes_per_request) {
    // SP800-90A permits up to 2^48, but it is not usable on 32 bit
    // platforms, so we only allow up to 2^24, which is still reasonably high
    if (reseed_interval == 0 || reseed_interval > static_cast<size_t>(1) << 24) {
        throw Invalid_Argument("Invalid value for reseed_interval");
    }

    if (max_number_of_bytes_per_request == 0 || max_number_of_bytes_per_request > 64 * 1024) {
        throw Invalid_Argument("Invalid value for max_number_of_bytes_per_request");
    }
}

}    // namespace

HMAC_DRBG::HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf,
                     RandomNumberGenerator& underlying_rng, size_t reseed_interval,
                     size_t max_number_of_bytes_per_request)
    : Stateful_RNG(underlying_rng, reseed_interval),
      m_mac(std::move(prf)),
      m_max_number_of_bytes_per_request(max_number_of_bytes_per_request),
      m_security_level(hmac_drbg_security_level(m_mac->output_length())) {
    BOTAN_ASSERT_NONNULL(m_mac);

    check_limits(reseed_interval, max_number_of_bytes_per_request);

    clear();
}

HMAC_DRBG::HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf,
                     RandomNumberGenerator& underlying_rng, Entropy_Sources& entropy_sources,
                     size_t reseed_interval, size_t max_number_of_bytes_per_request)
    : Stateful_RNG(underlying_rng, entropy_sources, reseed_interval),
      m_mac(std::move(prf)),
      m_max_number_of_bytes_per_request(max_number_of_bytes_per_request),
      m_security_level(hmac_drbg_security_level(m_mac->output_length())) {
    BOTAN_ASSERT_NONNULL(m_mac);

    check_limits(reseed_interval, max_number_of_bytes_per_request);

    clear();
}

HMAC_DRBG::HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf,
                     Entropy_Sources& entropy_sources, size_t reseed_interval,
                     size_t max_number_of_bytes_per_request)
    : Stateful_RNG(entropy_sources, reseed_interval),
      m_mac(std::move(prf)),
      m_max_number_of_bytes_per_request(max_number_of_bytes_per_request),
      m_security_level(hmac_drbg_security_level(m_mac->output_length())) {
    BOTAN_ASSERT_NONNULL(m_mac);

    check_limits(reseed_interval, max_number_of_bytes_per_request);

    clear();
}

HMAC_DRBG::HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf)
    : Stateful_RNG(),
      m_mac(std::move(prf)),
      m_max_number_of_bytes_per_request(64 * 1024),
      m_security_level(hmac_drbg_security_level(m_mac->output_length())) {
    BOTAN_ASSERT_NONNULL(m_mac);
    clear();
}

HMAC_DRBG::HMAC_DRBG(std::string_view hmac_hash)
    : Stateful_RNG(),
      m_mac(MessageAuthenticationCode::create_or_throw(fmt("HMAC({})", hmac_hash))),
      m_max_number_of_bytes_per_request(64 * 1024),
      m_security_level(hmac_drbg_security_level(m_mac->output_length())) {
    clear();
}

void HMAC_DRBG::clear_state() {
    if (m_V.empty()) {
        const size_t output_length = m_mac->output_length();
        m_V.resize(output_length);
        m_T.resize(output_length);
    }

    for (size_t i = 0; i != m_V.size(); ++i) {
        m_V[i] = 0x01;
    }
    m_mac->set_key(std::vector<uint8_t>(m_V.size(), 0x00));
}

std::string HMAC_DRBG::name() const { return fmt("HMAC_DRBG({})", m_mac->name()); }

/*
 * HMAC_DRBG generation
 * See NIST SP800-90A section 10.1.2.5
 */
void HMAC_DRBG::generate_output(std::span<uint8_t> output, std::span<const uint8_t> input) {
    BOTAN_ASSERT_NOMSG(!output.empty());

    if (!input.empty()) {
        update(input);
    }

    while (!output.empty()) {
        const size_t to_copy = std::min(output.size(), m_V.size());
        m_mac->update(m_V);
        m_mac->final(m_V);
        copy_mem(output.data(), m_V.data(), to_copy);

        output = output.subspan(to_copy);
    }

    update(input);
}

/*
 * Reset V and the mac key with new values
 * See NIST SP800-90A section 10.1.2.2
 */
void HMAC_DRBG::update(std::span<const uint8_t> input) {
    m_mac->update(m_V);
    m_mac->update(0x00);
    if (!input.empty()) {
        m_mac->update(input);
    }
    m_mac->final(m_T);
    m_mac->set_key(m_T);

    m_mac->update(m_V);
    m_mac->final(m_V);

    if (!input.empty()) {
        m_mac->update(m_V);
        m_mac->update(0x01);
        m_mac->update(input);
        m_mac->final(m_T);
        m_mac->set_key(m_T);

        m_mac->update(m_V);
        m_mac->final(m_V);
    }
}

size_t HMAC_DRBG::security_level() const { return m_security_level; }
}    // namespace Botan
/*
 * KDF Retrieval
 * (C) 1999-2007 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_HKDF)
#endif

#if defined(BOTAN_HAS_KDF1)
#endif

#if defined(BOTAN_HAS_KDF2)
#endif

#if defined(BOTAN_HAS_KDF1_18033)
#endif

#if defined(BOTAN_HAS_TLS_V12_PRF)
#endif

#if defined(BOTAN_HAS_X942_PRF)
#endif

#if defined(BOTAN_HAS_SP800_108)
#endif

#if defined(BOTAN_HAS_SP800_56A)
#endif

#if defined(BOTAN_HAS_SP800_56C)
#endif

namespace Botan {

namespace {

template <typename KDF_Type, typename... ParamTs>
std::unique_ptr<KDF> kdf_create_mac_or_hash(std::string_view nm, ParamTs&&... params) {
    if (auto mac = MessageAuthenticationCode::create(fmt("HMAC({})", nm))) {
        return std::make_unique<KDF_Type>(std::move(mac), std::forward<ParamTs>(params)...);
    }

    if (auto mac = MessageAuthenticationCode::create(nm)) {
        return std::make_unique<KDF_Type>(std::move(mac), std::forward<ParamTs>(params)...);
    }

    return nullptr;
}

}    // namespace

std::unique_ptr<KDF> KDF::create(std::string_view algo_spec, std::string_view provider) {
    const SCAN_Name req(algo_spec);

#if defined(BOTAN_HAS_HKDF)
    if (req.algo_name() == "HKDF" && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            return kdf_create_mac_or_hash<HKDF>(req.arg(0));
        }
    }

    if (req.algo_name() == "HKDF-Extract" && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            return kdf_create_mac_or_hash<HKDF_Extract>(req.arg(0));
        }
    }

    if (req.algo_name() == "HKDF-Expand" && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            return kdf_create_mac_or_hash<HKDF_Expand>(req.arg(0));
        }
    }
#endif

#if defined(BOTAN_HAS_KDF2)
    if (req.algo_name() == "KDF2" && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            if (auto hash = HashFunction::create(req.arg(0))) {
                return std::make_unique<KDF2>(std::move(hash));
            }
        }
    }
#endif

#if defined(BOTAN_HAS_KDF1_18033)
    if (req.algo_name() == "KDF1-18033" && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            if (auto hash = HashFunction::create(req.arg(0))) {
                return std::make_unique<KDF1_18033>(std::move(hash));
            }
        }
    }
#endif

#if defined(BOTAN_HAS_KDF1)
    if (req.algo_name() == "KDF1" && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            if (auto hash = HashFunction::create(req.arg(0))) {
                return std::make_unique<KDF1>(std::move(hash));
            }
        }
    }
#endif

#if defined(BOTAN_HAS_TLS_V12_PRF)
    if (req.algo_name() == "TLS-12-PRF" && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            return kdf_create_mac_or_hash<TLS_12_PRF>(req.arg(0));
        }
    }
#endif

#if defined(BOTAN_HAS_X942_PRF)
    if (req.algo_name() == "X9.42-PRF" && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            return std::make_unique<X942_PRF>(req.arg(0));
        }
    }
#endif

#if defined(BOTAN_HAS_SP800_108)
    if (req.algo_name() == "SP800-108-Counter" && req.arg_count_between(1, 3)) {
        if (provider.empty() || provider == "base") {
            return kdf_create_mac_or_hash<SP800_108_Counter>(req.arg(0), req.arg_as_integer(1, 32),
                                                             req.arg_as_integer(2, 32));
        }
    }

    if (req.algo_name() == "SP800-108-Feedback" && req.arg_count_between(1, 3)) {
        if (provider.empty() || provider == "base") {
            return kdf_create_mac_or_hash<SP800_108_Feedback>(req.arg(0), req.arg_as_integer(1, 32),
                                                              req.arg_as_integer(2, 32));
        }
    }

    if (req.algo_name() == "SP800-108-Pipeline" && req.arg_count_between(1, 3)) {
        if (provider.empty() || provider == "base") {
            return kdf_create_mac_or_hash<SP800_108_Pipeline>(req.arg(0), req.arg_as_integer(1, 32),
                                                              req.arg_as_integer(2, 32));
        }
    }
#endif

#if defined(BOTAN_HAS_SP800_56A)
    if (req.algo_name() == "SP800-56A" && req.arg_count() == 1) {
        if (auto hash = HashFunction::create(req.arg(0))) {
            return std::make_unique<SP800_56C_One_Step_Hash>(std::move(hash));
        }
        if (req.arg(0) == "KMAC-128") {
            return std::make_unique<SP800_56C_One_Step_KMAC128>();
        }
        if (req.arg(0) == "KMAC-256") {
            return std::make_unique<SP800_56C_One_Step_KMAC256>();
        }
        if (auto mac = MessageAuthenticationCode::create(req.arg(0))) {
            return std::make_unique<SP800_56C_One_Step_HMAC>(std::move(mac));
        }
    }
#endif

#if defined(BOTAN_HAS_SP800_56C)
    if (req.algo_name() == "SP800-56C" && req.arg_count() == 1) {
        std::unique_ptr<KDF> exp(kdf_create_mac_or_hash<SP800_108_Feedback>(req.arg(0), 32, 32));
        if (exp) {
            if (auto mac = MessageAuthenticationCode::create(req.arg(0))) {
                return std::make_unique<SP800_56C_Two_Step>(std::move(mac), std::move(exp));
            }

            if (auto mac = MessageAuthenticationCode::create(fmt("HMAC({})", req.arg(0)))) {
                return std::make_unique<SP800_56C_Two_Step>(std::move(mac), std::move(exp));
            }
        }
    }
#endif

    BOTAN_UNUSED(req);
    BOTAN_UNUSED(provider);

    return nullptr;
}

// static
std::unique_ptr<KDF> KDF::create_or_throw(std::string_view algo, std::string_view provider) {
    if (auto kdf = KDF::create(algo, provider)) {
        return kdf;
    }
    throw Lookup_Error("KDF", algo, provider);
}

std::vector<std::string> KDF::providers(std::string_view algo_spec) {
    return probe_providers_of<KDF>(algo_spec);
}

}    // namespace Botan
/*
 * KDF2
 * (C) 1999-2007 Jack Lloyd
 * (C) 2024      René Meusel, Rohde & Schwarz Cybersecurity
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

std::string KDF2::name() const { return fmt("KDF2({})", m_hash->name()); }

std::unique_ptr<KDF> KDF2::new_object() const {
    return std::make_unique<KDF2>(m_hash->new_object());
}

void KDF2::perform_kdf(std::span<uint8_t> key, std::span<const uint8_t> secret,
                       std::span<const uint8_t> salt, std::span<const uint8_t> label) const {
    if (key.empty()) {
        return;
    }

    const size_t hash_output_length = m_hash->output_length();
    const auto blocks_required =
        ceil_division<uint64_t /* for 32bit systems */>(key.size(), hash_output_length);

    // This KDF uses a 32-bit counter for the hash blocks, initialized at 1.
    // It will wrap around after 2^32 - 1 iterations limiting the theoretically
    // possible output to 2^32 - 1 blocks.
    BOTAN_ARG_CHECK(blocks_required <= 0xFFFFFFFE, "KDF2 maximum output length exceeeded");

    BufferStuffer k(key);
    for (uint32_t counter = 1; !k.full(); ++counter) {
        BOTAN_ASSERT_NOMSG(counter != 0);    // no overflow

        m_hash->update(secret);
        m_hash->update_be(counter);
        m_hash->update(label);
        m_hash->update(salt);

        // Write straight into the output buffer, except if the hash output needs
        // a truncation in the final iteration.
        if (k.remaining_capacity() >= hash_output_length) {
            m_hash->final(k.next(hash_output_length));
        } else {
            const auto h = m_hash->final();
            k.append(std::span{h}.first(k.remaining_capacity()));
        }
    }
}

}    // namespace Botan
/*
 * Message Authentication Code base class
 * (C) 1999-2008 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_CMAC)
#endif

#if defined(BOTAN_HAS_GMAC)
#endif

#if defined(BOTAN_HAS_HMAC)
#endif

#if defined(BOTAN_HAS_POLY1305)
#endif

#if defined(BOTAN_HAS_SIPHASH)
#endif

#if defined(BOTAN_HAS_ANSI_X919_MAC)
#endif

#if defined(BOTAN_HAS_BLAKE2BMAC)
#endif

#if defined(BOTAN_HAS_KMAC)
#endif

namespace Botan {

std::unique_ptr<MessageAuthenticationCode> MessageAuthenticationCode::create(
    std::string_view algo_spec, std::string_view provider) {
    const SCAN_Name req(algo_spec);

#if defined(BOTAN_HAS_BLAKE2BMAC)
    if (req.algo_name() == "Blake2b" || req.algo_name() == "BLAKE2b") {
        return std::make_unique<BLAKE2bMAC>(req.arg_as_integer(0, 512));
    }
#endif

#if defined(BOTAN_HAS_GMAC)
    if (req.algo_name() == "GMAC" && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            if (auto bc = BlockCipher::create(req.arg(0))) {
                return std::make_unique<GMAC>(std::move(bc));
            }
        }
    }
#endif

#if defined(BOTAN_HAS_HMAC)
    if (req.algo_name() == "HMAC" && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            if (auto hash = HashFunction::create(req.arg(0))) {
                return std::make_unique<HMAC>(std::move(hash));
            }
        }
    }
#endif

#if defined(BOTAN_HAS_POLY1305)
    if (req.algo_name() == "Poly1305" && req.arg_count() == 0) {
        if (provider.empty() || provider == "base") {
            return std::make_unique<Poly1305>();
        }
    }
#endif

#if defined(BOTAN_HAS_SIPHASH)
    if (req.algo_name() == "SipHash") {
        if (provider.empty() || provider == "base") {
            return std::make_unique<SipHash>(req.arg_as_integer(0, 2), req.arg_as_integer(1, 4));
        }
    }
#endif

#if defined(BOTAN_HAS_CMAC)
    if ((req.algo_name() == "CMAC" || req.algo_name() == "OMAC") && req.arg_count() == 1) {
        if (provider.empty() || provider == "base") {
            if (auto bc = BlockCipher::create(req.arg(0))) {
                return std::make_unique<CMAC>(std::move(bc));
            }
        }
    }
#endif

#if defined(BOTAN_HAS_ANSI_X919_MAC)
    if (req.algo_name() == "X9.19-MAC") {
        if (provider.empty() || provider == "base") {
            return std::make_unique<ANSI_X919_MAC>();
        }
    }
#endif

#if defined(BOTAN_HAS_KMAC)
    if (req.algo_name() == "KMAC-128") {
        if (provider.empty() || provider == "base") {
            if (req.arg_count() != 1) {
                throw Invalid_Argument(
                    "invalid algorithm specification for KMAC-128: need exactly one argument for "
                    "output bit length");
            }
            return std::make_unique<KMAC128>(req.arg_as_integer(0));
        }
    }

    if (req.algo_name() == "KMAC-256") {
        if (provider.empty() || provider == "base") {
            if (req.arg_count() != 1) {
                throw Invalid_Argument(
                    "invalid algorithm specification for KMAC-256: need exactly one argument for "
                    "output bit length");
            }
            return std::make_unique<KMAC256>(req.arg_as_integer(0));
        }
    }
#endif

    BOTAN_UNUSED(req);
    BOTAN_UNUSED(provider);

    return nullptr;
}

std::vector<std::string> MessageAuthenticationCode::providers(std::string_view algo_spec) {
    return probe_providers_of<MessageAuthenticationCode>(algo_spec);
}

// static
std::unique_ptr<MessageAuthenticationCode> MessageAuthenticationCode::create_or_throw(
    std::string_view algo, std::string_view provider) {
    if (auto mac = MessageAuthenticationCode::create(algo, provider)) {
        return mac;
    }
    throw Lookup_Error("MAC", algo, provider);
}

void MessageAuthenticationCode::start_msg(std::span<const uint8_t> nonce) {
    BOTAN_UNUSED(nonce);
    if (!nonce.empty()) {
        throw Invalid_IV_Length(name(), nonce.size());
    }
}

/*
 * Default (deterministic) MAC verification operation
 */
bool MessageAuthenticationCode::verify_mac_result(std::span<const uint8_t> mac) {
    secure_vector<uint8_t> our_mac = final();

    if (our_mac.size() != mac.size()) {
        return false;
    }

    return CT::is_equal(our_mac.data(), mac.data(), mac.size()).as_bool();
}

}    // namespace Botan
/*
 * CBC Padding Methods
 * (C) 1999-2007,2013,2018,2020 Jack Lloyd
 * (C) 2016 René Korthaus, Rohde & Schwarz Cybersecurity
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

/**
 * Get a block cipher padding method by name
 */
std::unique_ptr<BlockCipherModePaddingMethod> BlockCipherModePaddingMethod::create(
    std::string_view algo_spec) {
    if (algo_spec == "NoPadding") {
        return std::make_unique<Null_Padding>();
    }

    if (algo_spec == "PKCS7") {
        return std::make_unique<PKCS7_Padding>();
    }

    if (algo_spec == "OneAndZeros") {
        return std::make_unique<OneAndZeros_Padding>();
    }

    if (algo_spec == "X9.23") {
        return std::make_unique<ANSI_X923_Padding>();
    }

    if (algo_spec == "ESP") {
        return std::make_unique<ESP_Padding>();
    }

    return nullptr;
}

/*
 * Pad with PKCS #7 Method
 */
void PKCS7_Padding::add_padding(secure_vector<uint8_t>& buffer, size_t last_byte_pos,
                                size_t BS) const {
    /*
    Padding format is
    01
    0202
    030303
    ...
    */
    BOTAN_DEBUG_ASSERT(last_byte_pos < BS);

    const uint8_t padding_len = static_cast<uint8_t>(BS - last_byte_pos);

    buffer.resize(buffer.size() + padding_len);

    CT::poison(&last_byte_pos, 1);
    CT::poison(buffer.data(), buffer.size());

    BOTAN_DEBUG_ASSERT(buffer.size() % BS == 0);
    BOTAN_DEBUG_ASSERT(buffer.size() >= BS);

    const size_t start_of_last_block = buffer.size() - BS;
    const size_t end_of_last_block = buffer.size();
    const size_t start_of_padding = buffer.size() - padding_len;

    for (size_t i = start_of_last_block; i != end_of_last_block; ++i) {
        auto needs_padding = CT::Mask<uint8_t>(CT::Mask<size_t>::is_gte(i, start_of_padding));
        buffer[i] = needs_padding.select(padding_len, buffer[i]);
    }

    CT::unpoison(buffer.data(), buffer.size());
    CT::unpoison(last_byte_pos);
}

/*
 * Unpad with PKCS #7 Method
 */
size_t PKCS7_Padding::unpad(const uint8_t input[], size_t input_length) const {
    if (!valid_blocksize(input_length)) {
        return input_length;
    }

    CT::poison(input, input_length);

    const uint8_t last_byte = input[input_length - 1];

    /*
    The input should == the block size so if the last byte exceeds
    that then the padding is certainly invalid
    */
    auto bad_input = CT::Mask<size_t>::is_gt(last_byte, input_length);

    const size_t pad_pos = input_length - last_byte;

    for (size_t i = 0; i != input_length - 1; ++i) {
        // Does this byte equal the expected pad byte?
        const auto pad_eq = CT::Mask<size_t>::is_equal(input[i], last_byte);

        // Ignore values that are not part of the padding
        const auto in_range = CT::Mask<size_t>::is_gte(i, pad_pos);
        bad_input |= in_range & (~pad_eq);
    }

    CT::unpoison(input, input_length);

    return bad_input.select_and_unpoison(input_length, pad_pos);
}

/*
 * Pad with ANSI X9.23 Method
 */
void ANSI_X923_Padding::add_padding(secure_vector<uint8_t>& buffer, size_t last_byte_pos,
                                    size_t BS) const {
    /*
    Padding format is
    01
    0002
    000003
    ...
    */
    BOTAN_DEBUG_ASSERT(last_byte_pos < BS);

    const uint8_t padding_len = static_cast<uint8_t>(BS - last_byte_pos);

    buffer.resize(buffer.size() + padding_len);

    CT::poison(&last_byte_pos, 1);
    CT::poison(buffer.data(), buffer.size());

    BOTAN_DEBUG_ASSERT(buffer.size() % BS == 0);
    BOTAN_DEBUG_ASSERT(buffer.size() >= BS);

    const size_t start_of_last_block = buffer.size() - BS;
    const size_t end_of_zero_padding = buffer.size() - 1;
    const size_t start_of_padding = buffer.size() - padding_len;

    for (size_t i = start_of_last_block; i != end_of_zero_padding; ++i) {
        auto needs_padding = CT::Mask<uint8_t>(CT::Mask<size_t>::is_gte(i, start_of_padding));
        buffer[i] = needs_padding.select(0, buffer[i]);
    }

    buffer[buffer.size() - 1] = padding_len;
    CT::unpoison(buffer.data(), buffer.size());
    CT::unpoison(last_byte_pos);
}

/*
 * Unpad with ANSI X9.23 Method
 */
size_t ANSI_X923_Padding::unpad(const uint8_t input[], size_t input_length) const {
    if (!valid_blocksize(input_length)) {
        return input_length;
    }

    CT::poison(input, input_length);

    const size_t last_byte = input[input_length - 1];

    auto bad_input = CT::Mask<size_t>::is_gt(last_byte, input_length);

    const size_t pad_pos = input_length - last_byte;

    for (size_t i = 0; i != input_length - 1; ++i) {
        // Ignore values that are not part of the padding
        const auto in_range = CT::Mask<size_t>::is_gte(i, pad_pos);
        const auto pad_is_nonzero = CT::Mask<size_t>::expand(input[i]);
        bad_input |= pad_is_nonzero & in_range;
    }

    CT::unpoison(input, input_length);

    return bad_input.select_and_unpoison(input_length, pad_pos);
}

/*
 * Pad with One and Zeros Method
 */
void OneAndZeros_Padding::add_padding(secure_vector<uint8_t>& buffer, size_t last_byte_pos,
                                      size_t BS) const {
    /*
    Padding format is
    80
    8000
    800000
    ...
    */

    BOTAN_DEBUG_ASSERT(last_byte_pos < BS);

    const uint8_t padding_len = static_cast<uint8_t>(BS - last_byte_pos);

    buffer.resize(buffer.size() + padding_len);

    CT::poison(&last_byte_pos, 1);
    CT::poison(buffer.data(), buffer.size());

    BOTAN_DEBUG_ASSERT(buffer.size() % BS == 0);
    BOTAN_DEBUG_ASSERT(buffer.size() >= BS);

    const size_t start_of_last_block = buffer.size() - BS;
    const size_t end_of_last_block = buffer.size();
    const size_t start_of_padding = buffer.size() - padding_len;

    for (size_t i = start_of_last_block; i != end_of_last_block; ++i) {
        auto needs_80 = CT::Mask<uint8_t>(CT::Mask<size_t>::is_equal(i, start_of_padding));
        auto needs_00 = CT::Mask<uint8_t>(CT::Mask<size_t>::is_gt(i, start_of_padding));
        buffer[i] = needs_00.select(0x00, needs_80.select(0x80, buffer[i]));
    }

    CT::unpoison(buffer.data(), buffer.size());
    CT::unpoison(last_byte_pos);
}

/*
 * Unpad with One and Zeros Method
 */
size_t OneAndZeros_Padding::unpad(const uint8_t input[], size_t input_length) const {
    if (!valid_blocksize(input_length)) {
        return input_length;
    }

    CT::poison(input, input_length);

    auto bad_input = CT::Mask<uint8_t>::cleared();
    auto seen_0x80 = CT::Mask<uint8_t>::cleared();

    size_t pad_pos = input_length - 1;
    size_t i = input_length;

    while (i) {
        const auto is_0x80 = CT::Mask<uint8_t>::is_equal(input[i - 1], 0x80);
        const auto is_zero = CT::Mask<uint8_t>::is_zero(input[i - 1]);

        seen_0x80 |= is_0x80;
        pad_pos -= seen_0x80.if_not_set_return(1);
        bad_input |= ~seen_0x80 & ~is_zero;
        i--;
    }
    bad_input |= ~seen_0x80;

    CT::unpoison(input, input_length);

    return CT::Mask<size_t>::expand(bad_input).select_and_unpoison(input_length, pad_pos);
}

/*
 * Pad with ESP Padding Method
 */
void ESP_Padding::add_padding(secure_vector<uint8_t>& buffer, size_t last_byte_pos,
                              size_t BS) const {
    /*
    Padding format is
    01
    0102
    010203
    ...
    */
    BOTAN_DEBUG_ASSERT(last_byte_pos < BS);

    const uint8_t padding_len = static_cast<uint8_t>(BS - last_byte_pos);

    buffer.resize(buffer.size() + padding_len);

    CT::poison(&last_byte_pos, 1);
    CT::poison(buffer.data(), buffer.size());

    BOTAN_DEBUG_ASSERT(buffer.size() % BS == 0);
    BOTAN_DEBUG_ASSERT(buffer.size() >= BS);

    const size_t start_of_last_block = buffer.size() - BS;
    const size_t end_of_last_block = buffer.size();
    const size_t start_of_padding = buffer.size() - padding_len;

    uint8_t pad_ctr = 0x01;

    for (size_t i = start_of_last_block; i != end_of_last_block; ++i) {
        auto needs_padding = CT::Mask<uint8_t>(CT::Mask<size_t>::is_gte(i, start_of_padding));
        buffer[i] = needs_padding.select(pad_ctr, buffer[i]);
        pad_ctr = needs_padding.select(pad_ctr + 1, pad_ctr);
    }

    CT::unpoison(buffer.data(), buffer.size());
    CT::unpoison(last_byte_pos);
}

/*
 * Unpad with ESP Padding Method
 */
size_t ESP_Padding::unpad(const uint8_t input[], size_t input_length) const {
    if (!valid_blocksize(input_length)) {
        return input_length;
    }

    CT::poison(input, input_length);

    const uint8_t input_length_8 = static_cast<uint8_t>(input_length);
    const uint8_t last_byte = input[input_length - 1];

    auto bad_input =
        CT::Mask<uint8_t>::is_zero(last_byte) | CT::Mask<uint8_t>::is_gt(last_byte, input_length_8);

    const uint8_t pad_pos = input_length_8 - last_byte;
    size_t i = input_length_8 - 1;
    while (i) {
        const auto in_range = CT::Mask<size_t>::is_gt(i, pad_pos);
        const auto incrementing = CT::Mask<uint8_t>::is_equal(input[i - 1], input[i] - 1);

        bad_input |= CT::Mask<uint8_t>(in_range) & ~incrementing;
        --i;
    }

    CT::unpoison(input, input_length);
    return bad_input.select_and_unpoison(input_length_8, pad_pos);
}

}    // namespace Botan
/*
 * Cipher Modes
 * (C) 2015 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include <sstream>

#if defined(BOTAN_HAS_BLOCK_CIPHER)
#endif

#if defined(BOTAN_HAS_AEAD_MODES)
#endif

#if defined(BOTAN_HAS_MODE_CBC)
#endif

#if defined(BOTAN_HAS_MODE_CFB)
#endif

#if defined(BOTAN_HAS_MODE_XTS)
#endif

#if defined(BOTAN_HAS_COMMONCRYPTO)
#endif

namespace Botan {

std::unique_ptr<Cipher_Mode> Cipher_Mode::create_or_throw(std::string_view algo,
                                                          Cipher_Dir direction,
                                                          std::string_view provider) {
    if (auto mode = Cipher_Mode::create(algo, direction, provider)) {
        return mode;
    }

    throw Lookup_Error("Cipher mode", algo, provider);
}

std::unique_ptr<Cipher_Mode> Cipher_Mode::create(std::string_view algo, Cipher_Dir direction,
                                                 std::string_view provider) {
#if defined(BOTAN_HAS_COMMONCRYPTO)
    if (provider.empty() || provider == "commoncrypto") {
        if (auto cm = make_commoncrypto_cipher_mode(algo, direction)) return cm;

        if (!provider.empty()) return nullptr;
    }
#endif

    if (provider != "base" && !provider.empty()) {
        return nullptr;
    }

#if defined(BOTAN_HAS_STREAM_CIPHER)
    if (auto sc = StreamCipher::create(algo)) {
        return std::make_unique<Stream_Cipher_Mode>(std::move(sc));
    }
#endif

#if defined(BOTAN_HAS_AEAD_MODES)
    if (auto aead = AEAD_Mode::create(algo, direction)) {
        return aead;
    }
#endif

    if (algo.find('/') != std::string::npos) {
        const std::vector<std::string> algo_parts = split_on(algo, '/');
        std::string_view cipher_name = algo_parts[0];
        const std::vector<std::string> mode_info = parse_algorithm_name(algo_parts[1]);

        if (mode_info.empty()) {
            return std::unique_ptr<Cipher_Mode>();
        }

        std::ostringstream mode_name;

        mode_name << mode_info[0] << '(' << cipher_name;
        for (size_t i = 1; i < mode_info.size(); ++i) {
            mode_name << ',' << mode_info[i];
        }
        for (size_t i = 2; i < algo_parts.size(); ++i) {
            mode_name << ',' << algo_parts[i];
        }
        mode_name << ')';

        return Cipher_Mode::create(mode_name.str(), direction, provider);
    }

#if defined(BOTAN_HAS_BLOCK_CIPHER)

    SCAN_Name spec(algo);

    if (spec.arg_count() == 0) {
        return std::unique_ptr<Cipher_Mode>();
    }

    auto bc = BlockCipher::create(spec.arg(0), provider);

    if (!bc) {
        return std::unique_ptr<Cipher_Mode>();
    }

#if defined(BOTAN_HAS_MODE_CBC)
    if (spec.algo_name() == "CBC") {
        const std::string padding = spec.arg(1, "PKCS7");

        if (padding == "CTS") {
            if (direction == Cipher_Dir::Encryption) {
                return std::make_unique<CTS_Encryption>(std::move(bc));
            } else {
                return std::make_unique<CTS_Decryption>(std::move(bc));
            }
        } else {
            auto pad = BlockCipherModePaddingMethod::create(padding);

            if (pad) {
                if (direction == Cipher_Dir::Encryption) {
                    return std::make_unique<CBC_Encryption>(std::move(bc), std::move(pad));
                } else {
                    return std::make_unique<CBC_Decryption>(std::move(bc), std::move(pad));
                }
            }
        }
    }
#endif

#if defined(BOTAN_HAS_MODE_XTS)
    if (spec.algo_name() == "XTS") {
        if (direction == Cipher_Dir::Encryption) {
            return std::make_unique<XTS_Encryption>(std::move(bc));
        } else {
            return std::make_unique<XTS_Decryption>(std::move(bc));
        }
    }
#endif

#if defined(BOTAN_HAS_MODE_CFB)
    if (spec.algo_name() == "CFB") {
        const size_t feedback_bits = spec.arg_as_integer(1, 8 * bc->block_size());
        if (direction == Cipher_Dir::Encryption) {
            return std::make_unique<CFB_Encryption>(std::move(bc), feedback_bits);
        } else {
            return std::make_unique<CFB_Decryption>(std::move(bc), feedback_bits);
        }
    }
#endif

#endif

    return std::unique_ptr<Cipher_Mode>();
}

// static
std::vector<std::string> Cipher_Mode::providers(std::string_view algo_spec) {
    const std::vector<std::string>& possible = {"base", "commoncrypto"};
    std::vector<std::string> providers;
    for (auto&& prov : possible) {
        auto mode = Cipher_Mode::create(algo_spec, Cipher_Dir::Encryption, prov);
        if (mode) {
            providers.push_back(prov);    // available
        }
    }
    return providers;
}

}    // namespace Botan
/*
 * OS and machine specific utility functions
 * (C) 2015,2016,2017,2018 Jack Lloyd
 * (C) 2016 Daniel Neus
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_CPUID)
#endif

#include <chrono>
#include <cstdlib>
#include <iomanip>

#if defined(BOTAN_TARGET_OS_HAS_POSIX1)
#include <errno.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#undef B0
#endif

#if defined(BOTAN_TARGET_OS_IS_EMSCRIPTEN)
#include <emscripten/emscripten.h>
#endif

#if defined(BOTAN_TARGET_OS_HAS_GETAUXVAL) || defined(BOTAN_TARGET_OS_HAS_ELF_AUX_INFO)
#include <sys/auxv.h>
#endif

#if defined(BOTAN_TARGET_OS_HAS_WIN32)
#define NOMINMAX 1
#define _WINSOCKAPI_    // stop windows.h including winsock.h
#include <windows.h>
#if defined(BOTAN_BUILD_COMPILER_IS_MSVC)
#include <libloaderapi.h>
#include <stringapiset.h>
#endif
#endif

#if defined(BOTAN_TARGET_OS_IS_IOS) || defined(BOTAN_TARGET_OS_IS_MACOS)
#include <mach/vm_statistics.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#endif

#if defined(BOTAN_TARGET_OS_HAS_PRCTL)
#include <sys/prctl.h>
#endif

#if defined(BOTAN_TARGET_OS_IS_FREEBSD) || defined(BOTAN_TARGET_OS_IS_OPENBSD) || \
    defined(BOTAN_TARGET_OS_IS_DRAGONFLY)
#include <pthread_np.h>
#endif

#if defined(BOTAN_TARGET_OS_IS_HAIKU)
#include <kernel/OS.h>
#endif

namespace Botan {

uint32_t OS::get_process_id() {
#if defined(BOTAN_TARGET_OS_HAS_POSIX1)
    return ::getpid();
#elif defined(BOTAN_TARGET_OS_HAS_WIN32)
    return ::GetCurrentProcessId();
#elif defined(BOTAN_TARGET_OS_IS_LLVM) || defined(BOTAN_TARGET_OS_IS_NONE)
    return 0;    // truly no meaningful value
#else
#error "Missing get_process_id"
#endif
}

namespace {

#if defined(BOTAN_TARGET_OS_HAS_GETAUXVAL) || defined(BOTAN_TARGET_OS_HAS_ELF_AUX_INFO)
#define BOTAN_TARGET_HAS_AUXVAL_INTERFACE
#endif

std::optional<unsigned long> auxval_hwcap() {
#if defined(AT_HWCAP)
    return AT_HWCAP;
#elif defined(BOTAN_TARGET_HAS_AUXVAL_INTERFACE)
    // If the value is not defined in a header we can see,
    // but auxval is supported, return the Linux/Android value
    return 16;
#else
    return {};
#endif
}

std::optional<unsigned long> auxval_hwcap2() {
#if defined(AT_HWCAP2)
    return AT_HWCAP2;
#elif defined(BOTAN_TARGET_HAS_AUXVAL_INTERFACE)
    // If the value is not defined in a header we can see,
    // but auxval is supported, return the Linux/Android value
    return 26;
#else
    return {};
#endif
}

std::optional<unsigned long> get_auxval(std::optional<unsigned long> id) {
    if (id) {
#if defined(BOTAN_TARGET_OS_HAS_GETAUXVAL)
        return ::getauxval(*id);
#elif defined(BOTAN_TARGET_OS_HAS_ELF_AUX_INFO)
        unsigned long auxinfo = 0;
        if (::elf_aux_info(static_cast<int>(*id), &auxinfo, sizeof(auxinfo)) == 0) {
            return auxinfo;
        }
#endif
    }

    return {};
}

}    // namespace

std::optional<std::pair<unsigned long, unsigned long>> OS::get_auxval_hwcap() {
    if (const auto hwcap = get_auxval(auxval_hwcap())) {
        // If hwcap worked/was valid, we don't require hwcap2 to also
        // succeed but instead will return zeros if it failed.
        auto hwcap2 = get_auxval(auxval_hwcap2()).value_or(0);
        return std::make_pair(*hwcap, hwcap2);
    } else {
        return {};
    }
}

namespace {

/**
 * Test if we are currently running with elevated permissions
 * eg setuid, setgid, or with POSIX caps set.
 */
bool running_in_privileged_state() {
#if defined(AT_SECURE)
    if (auto at_secure = get_auxval(AT_SECURE)) {
        return at_secure != 0;
    }
#endif

#if defined(BOTAN_TARGET_OS_HAS_POSIX1)
    return (::getuid() != ::geteuid()) || (::getgid() != ::getegid());
#else
    return false;
#endif
}

}    // namespace

uint64_t OS::get_cpu_cycle_counter() {
    uint64_t rtc = 0;

#if defined(BOTAN_TARGET_OS_HAS_WIN32)
    LARGE_INTEGER tv;
    ::QueryPerformanceCounter(&tv);
    rtc = tv.QuadPart;

#elif defined(BOTAN_USE_GCC_INLINE_ASM)

#if defined(BOTAN_TARGET_ARCH_IS_X86_64)

    uint32_t rtc_low = 0, rtc_high = 0;
    asm volatile("rdtsc" : "=d"(rtc_high), "=a"(rtc_low));
    rtc = (static_cast<uint64_t>(rtc_high) << 32) | rtc_low;

#elif defined(BOTAN_TARGET_CPU_IS_X86_FAMILY) && defined(BOTAN_HAS_CPUID)

    if (CPUID::has(CPUID::Feature::RDTSC)) {
        uint32_t rtc_low = 0, rtc_high = 0;
        asm volatile("rdtsc" : "=d"(rtc_high), "=a"(rtc_low));
        rtc = (static_cast<uint64_t>(rtc_high) << 32) | rtc_low;
    }

#elif defined(BOTAN_TARGET_ARCH_IS_PPC64)

    for (;;) {
        uint32_t rtc_low = 0, rtc_high = 0, rtc_high2 = 0;
        asm volatile("mftbu %0" : "=r"(rtc_high));
        asm volatile("mftb %0" : "=r"(rtc_low));
        asm volatile("mftbu %0" : "=r"(rtc_high2));

        if (rtc_high == rtc_high2) {
            rtc = (static_cast<uint64_t>(rtc_high) << 32) | rtc_low;
            break;
        }
    }

#elif defined(BOTAN_TARGET_ARCH_IS_ALPHA)
    asm volatile("rpcc %0" : "=r"(rtc));

    // OpenBSD does not trap access to the %tick register
#elif defined(BOTAN_TARGET_ARCH_IS_SPARC64) && !defined(BOTAN_TARGET_OS_IS_OPENBSD)
    asm volatile("rd %%tick, %0" : "=r"(rtc));

#elif defined(BOTAN_TARGET_ARCH_IS_IA64)
    asm volatile("mov %0=ar.itc" : "=r"(rtc));

#elif defined(BOTAN_TARGET_ARCH_IS_S390X)
    asm volatile("stck 0(%0)" : : "a"(&rtc) : "memory", "cc");

#elif defined(BOTAN_TARGET_ARCH_IS_HPPA)
    asm volatile("mfctl 16,%0" : "=r"(rtc));    // 64-bit only?

#else
    // #warning "OS::get_cpu_cycle_counter not implemented"
#endif

#endif

    return rtc;
}

size_t OS::get_cpu_available() {
#if defined(BOTAN_TARGET_OS_HAS_POSIX1)

#if defined(_SC_NPROCESSORS_ONLN)
    const long cpu_online = ::sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu_online > 0) {
        return static_cast<size_t>(cpu_online);
    }
#endif

#if defined(_SC_NPROCESSORS_CONF)
    const long cpu_conf = ::sysconf(_SC_NPROCESSORS_CONF);
    if (cpu_conf > 0) {
        return static_cast<size_t>(cpu_conf);
    }
#endif

#endif

#if defined(BOTAN_TARGET_OS_HAS_THREADS)
    // hardware_concurrency is allowed to return 0 if the value is not
    // well defined or not computable.
    const size_t hw_concur = std::thread::hardware_concurrency();

    if (hw_concur > 0) {
        return hw_concur;
    }
#endif

    return 1;
}

uint64_t OS::get_high_resolution_clock() {
    if (uint64_t cpu_clock = OS::get_cpu_cycle_counter()) {
        return cpu_clock;
    }

#if defined(BOTAN_TARGET_OS_IS_EMSCRIPTEN)
    return emscripten_get_now();
#endif

    /*
    If we got here either we either don't have an asm instruction
    above, or (for x86) RDTSC is not available at runtime. Try some
    clock_gettimes and return the first one that works, or otherwise
    fall back to std::chrono.
    */

#if defined(BOTAN_TARGET_OS_HAS_CLOCK_GETTIME)

    // The ordering here is somewhat arbitrary...
    const clockid_t clock_types[] = {
#if defined(CLOCK_MONOTONIC_HR)
        CLOCK_MONOTONIC_HR,
#endif
#if defined(CLOCK_MONOTONIC_RAW)
        CLOCK_MONOTONIC_RAW,
#endif
#if defined(CLOCK_MONOTONIC)
        CLOCK_MONOTONIC,
#endif
#if defined(CLOCK_PROCESS_CPUTIME_ID)
        CLOCK_PROCESS_CPUTIME_ID,
#endif
#if defined(CLOCK_THREAD_CPUTIME_ID)
        CLOCK_THREAD_CPUTIME_ID,
#endif
    };

    for (clockid_t clock : clock_types) {
        struct timespec ts;
        if (::clock_gettime(clock, &ts) == 0) {
            return (static_cast<uint64_t>(ts.tv_sec) * 1000000000) +
                   static_cast<uint64_t>(ts.tv_nsec);
        }
    }
#endif

#if defined(BOTAN_TARGET_OS_HAS_SYSTEM_CLOCK)
    // Plain C++11 fallback
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
#else
    return 0;
#endif
}

uint64_t OS::get_system_timestamp_ns() {
#if defined(BOTAN_TARGET_OS_HAS_CLOCK_GETTIME)
    struct timespec ts;
    if (::clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        return (static_cast<uint64_t>(ts.tv_sec) * 1000000000) + static_cast<uint64_t>(ts.tv_nsec);
    }
#endif

#if defined(BOTAN_TARGET_OS_HAS_SYSTEM_CLOCK)
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
#else
    throw Not_Implemented("OS::get_system_timestamp_ns this system does not support a clock");
#endif
}

std::string OS::format_time(time_t time, const std::string& format) {
    std::tm tm;

#if defined(BOTAN_TARGET_OS_HAS_WIN32)
    localtime_s(&tm, &time);
#elif defined(BOTAN_TARGET_OS_HAS_POSIX1)
    localtime_r(&time, &tm);
#else
    if (auto tmp = std::localtime(&time)) {
        tm = *tmp;
    } else {
        throw Encoding_Error("Could not convert time_t to localtime");
    }
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, format.c_str());
    return oss.str();
}

size_t OS::system_page_size() {
    const size_t default_page_size = 4096;

#if defined(BOTAN_TARGET_OS_HAS_POSIX1)
    long p = ::sysconf(_SC_PAGESIZE);
    if (p > 1) {
        return static_cast<size_t>(p);
    } else {
        return default_page_size;
    }
#elif defined(BOTAN_TARGET_OS_HAS_VIRTUAL_LOCK)
    BOTAN_UNUSED(default_page_size);
    SYSTEM_INFO sys_info;
    ::GetSystemInfo(&sys_info);
    return sys_info.dwPageSize;
#else
    return default_page_size;
#endif
}

size_t OS::get_memory_locking_limit() {
    /*
     * Linux defaults to only 64 KiB of mlockable memory per process (too small)
     * but BSDs offer a small fraction of total RAM (more than we need). Bound the
     * total mlock size to 512 KiB which is enough to run the entire test suite
     * without spilling to non-mlock memory (and thus presumably also enough for
     * many useful programs), but small enough that we should not cause problems
     * even if many processes are mlocking on the same machine.
     */
    const size_t max_locked_kb = 512;

    /*
     * If RLIMIT_MEMLOCK is not defined, likely the OS does not support
     * unprivileged mlock calls.
     */
#if defined(RLIMIT_MEMLOCK) && defined(BOTAN_TARGET_OS_HAS_POSIX1) && \
    defined(BOTAN_TARGET_OS_HAS_POSIX_MLOCK)
    const size_t mlock_requested = std::min<size_t>(
        read_env_variable_sz("BOTAN_MLOCK_POOL_SIZE", max_locked_kb), max_locked_kb);

    if (mlock_requested > 0) {
        struct ::rlimit limits;

        ::getrlimit(RLIMIT_MEMLOCK, &limits);

        if (limits.rlim_cur < limits.rlim_max) {
            limits.rlim_cur = limits.rlim_max;
            ::setrlimit(RLIMIT_MEMLOCK, &limits);
            ::getrlimit(RLIMIT_MEMLOCK, &limits);
        }

        return std::min<size_t>(limits.rlim_cur, mlock_requested * 1024);
    }

#elif defined(BOTAN_TARGET_OS_HAS_VIRTUAL_LOCK)
    const size_t mlock_requested = std::min<size_t>(
        read_env_variable_sz("BOTAN_MLOCK_POOL_SIZE", max_locked_kb), max_locked_kb);

    SIZE_T working_min = 0, working_max = 0;
    if (!::GetProcessWorkingSetSize(::GetCurrentProcess(), &working_min, &working_max)) {
        return 0;
    }

    // According to Microsoft MSDN:
    // The maximum number of pages that a process can lock is equal to the number of pages in its
    // minimum working set minus a small overhead In the book "Windows Internals Part 2": the
    // maximum lockable pages are minimum working set size - 8 pages But the information in the book
    // seems to be inaccurate/outdated I've tested this on Windows 8.1 x64, Windows 10 x64 and
    // Windows 7 x86 On all three OS the value is 11 instead of 8
    const size_t overhead = OS::system_page_size() * 11;
    if (working_min > overhead) {
        const size_t lockable_bytes = working_min - overhead;
        return std::min<size_t>(lockable_bytes, mlock_requested * 1024);
    }
#else
    // Not supported on this platform
    BOTAN_UNUSED(max_locked_kb);
#endif

    return 0;
}

bool OS::read_env_variable(std::string& value_out, std::string_view name_view) {
    value_out = "";

    if (running_in_privileged_state()) {
        return false;
    }

#if defined(BOTAN_TARGET_OS_HAS_WIN32) && defined(BOTAN_BUILD_COMPILER_IS_MSVC)
    const std::string name(name_view);
    char val[128] = {0};
    size_t req_size = 0;
    if (getenv_s(&req_size, val, sizeof(val), name.c_str()) == 0) {
        // Microsoft's implementation always writes a terminating \0,
        // and includes it in the reported length of the environment variable
        // if a value exists.
        if (req_size > 0 && val[req_size - 1] == '\0') {
            value_out = std::string(val);
        } else {
            value_out = std::string(val, req_size);
        }
        return true;
    }
#else
    const std::string name(name_view);
    if (const char* val = std::getenv(name.c_str())) {
        value_out = val;
        return true;
    }
#endif

    return false;
}

size_t OS::read_env_variable_sz(std::string_view name, size_t def) {
    std::string value;
    if (read_env_variable(value, name) && !value.empty()) {
        try {
            const size_t val = std::stoul(value, nullptr);
            return val;
        } catch (std::exception&) { /* ignore it */
        }
    }

    return def;
}

#if defined(BOTAN_TARGET_OS_HAS_POSIX1) && defined(BOTAN_TARGET_OS_HAS_POSIX_MLOCK)

namespace {

int get_locked_fd() {
#if defined(BOTAN_TARGET_OS_IS_IOS) || defined(BOTAN_TARGET_OS_IS_MACOS)
    // On Darwin, tagging anonymous pages allows vmmap to track these.
    // Allowed from 240 to 255 for userland applications
    static constexpr int default_locked_fd = 255;
    int locked_fd = default_locked_fd;

    if (size_t locked_fdl = OS::read_env_variable_sz("BOTAN_LOCKED_FD", default_locked_fd)) {
        if (locked_fdl < 240 || locked_fdl > 255) {
            locked_fdl = default_locked_fd;
        }
        locked_fd = static_cast<int>(locked_fdl);
    }
    return VM_MAKE_TAG(locked_fd);
#else
    return -1;
#endif
}

}    // namespace

#endif

std::vector<void*> OS::allocate_locked_pages(size_t count) {
    std::vector<void*> result;

#if (defined(BOTAN_TARGET_OS_HAS_POSIX1) && defined(BOTAN_TARGET_OS_HAS_POSIX_MLOCK)) || \
    defined(BOTAN_TARGET_OS_HAS_VIRTUAL_LOCK)

    result.reserve(count);

    const size_t page_size = OS::system_page_size();

#if defined(BOTAN_TARGET_OS_HAS_POSIX1) && defined(BOTAN_TARGET_OS_HAS_POSIX_MLOCK)
    static const int locked_fd = get_locked_fd();
#endif

    for (size_t i = 0; i != count; ++i) {
        void* ptr = nullptr;

#if defined(BOTAN_TARGET_OS_HAS_POSIX1) && defined(BOTAN_TARGET_OS_HAS_POSIX_MLOCK)

        int mmap_flags = MAP_PRIVATE;

#if defined(MAP_ANONYMOUS)
        mmap_flags |= MAP_ANONYMOUS;
#elif defined(MAP_ANON)
        mmap_flags |= MAP_ANON;
#endif

#if defined(MAP_CONCEAL)
        mmap_flags |= MAP_CONCEAL;
#elif defined(MAP_NOCORE)
        mmap_flags |= MAP_NOCORE;
#endif

        int mmap_prot = PROT_READ | PROT_WRITE;

#if defined(PROT_MAX)
        mmap_prot |= PROT_MAX(mmap_prot);
#endif

        ptr = ::mmap(nullptr, 3 * page_size, mmap_prot, mmap_flags,
                     /*fd=*/locked_fd,
                     /*offset=*/0);

        if (ptr == MAP_FAILED) {
            continue;
        }

        // lock the data page
        if (::mlock(static_cast<uint8_t*>(ptr) + page_size, page_size) != 0) {
            ::munmap(ptr, 3 * page_size);
            continue;
        }

#if defined(MADV_DONTDUMP)
        // we ignore errors here, as DONTDUMP is just a bonus
        ::madvise(static_cast<uint8_t*>(ptr) + page_size, page_size, MADV_DONTDUMP);
#endif

#elif defined(BOTAN_TARGET_OS_HAS_VIRTUAL_LOCK)
        ptr = ::VirtualAlloc(nullptr, 3 * page_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        if (ptr == nullptr) continue;

        if (::VirtualLock(static_cast<uint8_t*>(ptr) + page_size, page_size) == 0) {
            ::VirtualFree(ptr, 0, MEM_RELEASE);
            continue;
        }
#endif

        std::memset(ptr, 0, 3 * page_size);    // zero data page and both guard pages

        // Attempts to name the data page
        page_named(ptr, 3 * page_size);
        // Make guard page preceeding the data page
        page_prohibit_access(static_cast<uint8_t*>(ptr));
        // Make guard page following the data page
        page_prohibit_access(static_cast<uint8_t*>(ptr) + 2 * page_size);

        result.push_back(static_cast<uint8_t*>(ptr) + page_size);
    }
#else
    BOTAN_UNUSED(count);
#endif

    return result;
}

void OS::page_allow_access(void* page) {
#if defined(BOTAN_TARGET_OS_HAS_POSIX1)
    const size_t page_size = OS::system_page_size();
    ::mprotect(page, page_size, PROT_READ | PROT_WRITE);
#elif defined(BOTAN_TARGET_OS_HAS_VIRTUAL_LOCK)
    const size_t page_size = OS::system_page_size();
    DWORD old_perms = 0;
    ::VirtualProtect(page, page_size, PAGE_READWRITE, &old_perms);
    BOTAN_UNUSED(old_perms);
#else
    BOTAN_UNUSED(page);
#endif
}

void OS::page_prohibit_access(void* page) {
#if defined(BOTAN_TARGET_OS_HAS_POSIX1)
    const size_t page_size = OS::system_page_size();
    ::mprotect(page, page_size, PROT_NONE);
#elif defined(BOTAN_TARGET_OS_HAS_VIRTUAL_LOCK)
    const size_t page_size = OS::system_page_size();
    DWORD old_perms = 0;
    ::VirtualProtect(page, page_size, PAGE_NOACCESS, &old_perms);
    BOTAN_UNUSED(old_perms);
#else
    BOTAN_UNUSED(page);
#endif
}

void OS::free_locked_pages(const std::vector<void*>& pages) {
    const size_t page_size = OS::system_page_size();

    for (size_t i = 0; i != pages.size(); ++i) {
        void* ptr = pages[i];

        secure_scrub_memory(ptr, page_size);

        // ptr points to the data page, guard pages are before and after
        page_allow_access(static_cast<uint8_t*>(ptr) - page_size);
        page_allow_access(static_cast<uint8_t*>(ptr) + page_size);

#if defined(BOTAN_TARGET_OS_HAS_POSIX1) && defined(BOTAN_TARGET_OS_HAS_POSIX_MLOCK)
        ::munlock(ptr, page_size);
        ::munmap(static_cast<uint8_t*>(ptr) - page_size, 3 * page_size);
#elif defined(BOTAN_TARGET_OS_HAS_VIRTUAL_LOCK)
        ::VirtualUnlock(ptr, page_size);
        ::VirtualFree(static_cast<uint8_t*>(ptr) - page_size, 0, MEM_RELEASE);
#endif
    }
}

void OS::page_named(void* page, size_t size) {
#if defined(BOTAN_TARGET_OS_HAS_PRCTL) && defined(PR_SET_VMA) && defined(PR_SET_VMA_ANON_NAME)
    static constexpr char name[] = "Botan mlock pool";
    int r = prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, reinterpret_cast<uintptr_t>(page), size, name);
    BOTAN_UNUSED(r);
#else
    BOTAN_UNUSED(page, size);
#endif
}

#if defined(BOTAN_TARGET_OS_HAS_THREADS)
void OS::set_thread_name(std::thread& thread, const std::string& name) {
#if defined(BOTAN_TARGET_OS_IS_LINUX) || defined(BOTAN_TARGET_OS_IS_FREEBSD) || \
    defined(BOTAN_TARGET_OS_IS_DRAGONFLY)
    static_cast<void>(pthread_setname_np(thread.native_handle(), name.c_str()));
#elif defined(BOTAN_TARGET_OS_IS_OPENBSD)
    static_cast<void>(pthread_set_name_np(thread.native_handle(), name.c_str()));
#elif defined(BOTAN_TARGET_OS_IS_NETBSD)
    static_cast<void>(
        pthread_setname_np(thread.native_handle(), "%s", const_cast<char*>(name.c_str())));
#elif defined(BOTAN_TARGET_OS_HAS_WIN32) && defined(_LIBCPP_HAS_THREAD_API_PTHREAD)
    static_cast<void>(pthread_setname_np(thread.native_handle(), name.c_str()));
#elif defined(BOTAN_TARGET_OS_HAS_WIN32) && defined(BOTAN_BUILD_COMPILER_IS_MSVC)
    typedef HRESULT(WINAPI * std_proc)(HANDLE, PCWSTR);
    HMODULE kern = GetModuleHandleA("KernelBase.dll");
    std_proc set_thread_name =
        reinterpret_cast<std_proc>(GetProcAddress(kern, "SetThreadDescription"));
    if (set_thread_name) {
        std::wstring w;
        auto sz = MultiByteToWideChar(CP_UTF8, 0, name.data(), -1, nullptr, 0);
        if (sz > 0) {
            w.resize(sz);
            if (MultiByteToWideChar(CP_UTF8, 0, name.data(), -1, &w[0], sz) > 0) {
                (void)set_thread_name(thread.native_handle(), w.c_str());
            }
        }
    }
#elif defined(BOTAN_TARGET_OS_IF_HAIKU)
    auto thread_id = get_pthread_thread_id(thread.native_handle());
    static_cast<void>(rename_thread(thread_id, name.c_str()));
#else
    // TODO other possible oses ?
    // macOs does not seem to allow to name threads other than the current one.
    BOTAN_UNUSED(thread, name);
#endif
}
#endif

#if defined(BOTAN_TARGET_OS_HAS_POSIX1) && !defined(BOTAN_TARGET_OS_IS_EMSCRIPTEN)

namespace {

// NOLINTNEXTLINE(*-avoid-non-const-global-variables)
::sigjmp_buf g_sigill_jmp_buf;

void botan_sigill_handler(int /*unused*/) {
    siglongjmp(g_sigill_jmp_buf, /*non-zero return value*/ 1);
}

}    // namespace

#endif

int OS::run_cpu_instruction_probe(const std::function<int()>& probe_fn) {
    volatile int probe_result = -3;

#if defined(BOTAN_TARGET_OS_HAS_POSIX1) && !defined(BOTAN_TARGET_OS_IS_EMSCRIPTEN)
    struct sigaction old_sigaction;
    struct sigaction sigaction;

    sigaction.sa_handler = botan_sigill_handler;
    sigemptyset(&sigaction.sa_mask);
    sigaction.sa_flags = 0;

    int rc = ::sigaction(SIGILL, &sigaction, &old_sigaction);

    if (rc != 0) {
        throw System_Error("run_cpu_instruction_probe sigaction failed", errno);
    }

    rc = sigsetjmp(g_sigill_jmp_buf, /*save sigs*/ 1);

    if (rc == 0) {
        // first call to sigsetjmp
        probe_result = probe_fn();
    } else if (rc == 1) {
        // non-local return from siglongjmp in signal handler: return error
        probe_result = -1;
    }

    // Restore old SIGILL handler, if any
    rc = ::sigaction(SIGILL, &old_sigaction, nullptr);
    if (rc != 0) {
        throw System_Error("run_cpu_instruction_probe sigaction restore failed", errno);
    }

#else
    BOTAN_UNUSED(probe_fn);
#endif

    return probe_result;
}

std::unique_ptr<OS::Echo_Suppression> OS::suppress_echo_on_terminal() {
#if defined(BOTAN_TARGET_OS_HAS_POSIX1)
    class POSIX_Echo_Suppression : public Echo_Suppression {
       public:
        POSIX_Echo_Suppression() {
            m_stdin_fd = fileno(stdin);
            if (::tcgetattr(m_stdin_fd, &m_old_termios) != 0) {
                throw System_Error("Getting terminal status failed", errno);
            }

            struct termios noecho_flags = m_old_termios;
            noecho_flags.c_lflag &= ~ECHO;
            noecho_flags.c_lflag |= ECHONL;

            if (::tcsetattr(m_stdin_fd, TCSANOW, &noecho_flags) != 0) {
                throw System_Error("Clearing terminal echo bit failed", errno);
            }
        }

        void reenable_echo() override {
            if (m_stdin_fd > 0) {
                if (::tcsetattr(m_stdin_fd, TCSANOW, &m_old_termios) != 0) {
                    throw System_Error("Restoring terminal echo bit failed", errno);
                }
                m_stdin_fd = -1;
            }
        }

        ~POSIX_Echo_Suppression() override {
            try {
                reenable_echo();
            } catch (...) {
            }
        }

        POSIX_Echo_Suppression(const POSIX_Echo_Suppression& other) = delete;
        POSIX_Echo_Suppression(POSIX_Echo_Suppression&& other) = delete;
        POSIX_Echo_Suppression& operator=(const POSIX_Echo_Suppression& other) = delete;
        POSIX_Echo_Suppression& operator=(POSIX_Echo_Suppression&& other) = delete;

       private:
        int m_stdin_fd;
        struct termios m_old_termios;
    };

    return std::make_unique<POSIX_Echo_Suppression>();

#elif defined(BOTAN_TARGET_OS_HAS_WIN32)

    class Win32_Echo_Suppression : public Echo_Suppression {
       public:
        Win32_Echo_Suppression() {
            m_input_handle = ::GetStdHandle(STD_INPUT_HANDLE);
            if (::GetConsoleMode(m_input_handle, &m_console_state) == 0)
                throw System_Error("Getting console mode failed", ::GetLastError());

            DWORD new_mode = ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT;
            if (::SetConsoleMode(m_input_handle, new_mode) == 0)
                throw System_Error("Setting console mode failed", ::GetLastError());
        }

        void reenable_echo() override {
            if (m_input_handle != INVALID_HANDLE_VALUE) {
                if (::SetConsoleMode(m_input_handle, m_console_state) == 0)
                    throw System_Error("Setting console mode failed", ::GetLastError());
                m_input_handle = INVALID_HANDLE_VALUE;
            }
        }

        ~Win32_Echo_Suppression() override {
            try {
                reenable_echo();
            } catch (...) {
            }
        }

        Win32_Echo_Suppression(const Win32_Echo_Suppression& other) = delete;
        Win32_Echo_Suppression(Win32_Echo_Suppression&& other) = delete;
        Win32_Echo_Suppression& operator=(const Win32_Echo_Suppression& other) = delete;
        Win32_Echo_Suppression& operator=(Win32_Echo_Suppression&& other) = delete;

       private:
        HANDLE m_input_handle;
        DWORD m_console_state;
    };

    return std::make_unique<Win32_Echo_Suppression>();

#else

    // Not supported on this platform, return null
    return nullptr;
#endif
}

}    // namespace Botan
/*
 * PBKDF
 * (C) 2012 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_PBKDF2)
#endif

#if defined(BOTAN_HAS_PGP_S2K)
#endif

namespace Botan {

std::unique_ptr<PBKDF> PBKDF::create(std::string_view algo_spec, std::string_view provider) {
    const SCAN_Name req(algo_spec);

#if defined(BOTAN_HAS_PBKDF2)
    if (req.algo_name() == "PBKDF2") {
        if (provider.empty() || provider == "base") {
            if (auto mac = MessageAuthenticationCode::create("HMAC(" + req.arg(0) + ")")) {
                return std::make_unique<PKCS5_PBKDF2>(std::move(mac));
            }

            if (auto mac = MessageAuthenticationCode::create(req.arg(0))) {
                return std::make_unique<PKCS5_PBKDF2>(std::move(mac));
            }
        }

        return nullptr;
    }
#endif

#if defined(BOTAN_HAS_PGP_S2K)
    if (req.algo_name() == "OpenPGP-S2K" && req.arg_count() == 1) {
        if (auto hash = HashFunction::create(req.arg(0))) {
            return std::make_unique<OpenPGP_S2K>(std::move(hash));
        }
    }
#endif

    BOTAN_UNUSED(req, provider);

    return nullptr;
}

// static
std::unique_ptr<PBKDF> PBKDF::create_or_throw(std::string_view algo, std::string_view provider) {
    if (auto pbkdf = PBKDF::create(algo, provider)) {
        return pbkdf;
    }
    throw Lookup_Error("PBKDF", algo, provider);
}

std::vector<std::string> PBKDF::providers(std::string_view algo_spec) {
    return probe_providers_of<PBKDF>(algo_spec);
}

void PBKDF::pbkdf_timed(uint8_t out[], size_t out_len, std::string_view passphrase,
                        const uint8_t salt[], size_t salt_len, std::chrono::milliseconds msec,
                        size_t& iterations) const {
    iterations = pbkdf(out, out_len, passphrase, salt, salt_len, 0, msec);
}

void PBKDF::pbkdf_iterations(uint8_t out[], size_t out_len, std::string_view passphrase,
                             const uint8_t salt[], size_t salt_len, size_t iterations) const {
    if (iterations == 0) {
        throw Invalid_Argument(name() + ": Invalid iteration count");
    }

    const size_t iterations_run =
        pbkdf(out, out_len, passphrase, salt, salt_len, iterations, std::chrono::milliseconds(0));
    BOTAN_ASSERT_EQUAL(iterations, iterations_run, "Expected PBKDF iterations");
}

secure_vector<uint8_t> PBKDF::pbkdf_iterations(size_t out_len, std::string_view passphrase,
                                               const uint8_t salt[], size_t salt_len,
                                               size_t iterations) const {
    secure_vector<uint8_t> out(out_len);
    pbkdf_iterations(out.data(), out_len, passphrase, salt, salt_len, iterations);
    return out;
}

secure_vector<uint8_t> PBKDF::pbkdf_timed(size_t out_len, std::string_view passphrase,
                                          const uint8_t salt[], size_t salt_len,
                                          std::chrono::milliseconds msec,
                                          size_t& iterations) const {
    secure_vector<uint8_t> out(out_len);
    pbkdf_timed(out.data(), out_len, passphrase, salt, salt_len, msec, iterations);
    return out;
}

}    // namespace Botan
/*
 * (C) 2018 Ribose Inc
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_PBKDF2)
#endif

#if defined(BOTAN_HAS_PGP_S2K)
#endif

#if defined(BOTAN_HAS_SCRYPT)
#endif

#if defined(BOTAN_HAS_ARGON2)
#endif

#if defined(BOTAN_HAS_PBKDF_BCRYPT)
#endif

namespace Botan {

void PasswordHash::derive_key(uint8_t out[], size_t out_len, const char* password,
                              size_t password_len, const uint8_t salt[], size_t salt_len,
                              const uint8_t ad[], size_t ad_len, const uint8_t key[],
                              size_t key_len) const {
    BOTAN_UNUSED(ad, key);

    if (ad_len == 0 && key_len == 0) {
        return this->derive_key(out, out_len, password, password_len, salt, salt_len);
    } else {
        throw Not_Implemented("PasswordHash " + this->to_string() + " does not support AD or key");
    }
}

std::unique_ptr<PasswordHashFamily> PasswordHashFamily::create(std::string_view algo_spec,
                                                               std::string_view provider) {
    const SCAN_Name req(algo_spec);

#if defined(BOTAN_HAS_PBKDF2)
    if (req.algo_name() == "PBKDF2") {
        if (provider.empty() || provider == "base") {
            if (auto mac = MessageAuthenticationCode::create("HMAC(" + req.arg(0) + ")")) {
                return std::make_unique<PBKDF2_Family>(std::move(mac));
            }

            if (auto mac = MessageAuthenticationCode::create(req.arg(0))) {
                return std::make_unique<PBKDF2_Family>(std::move(mac));
            }
        }

        return nullptr;
    }
#endif

#if defined(BOTAN_HAS_SCRYPT)
    if (req.algo_name() == "Scrypt") {
        return std::make_unique<Scrypt_Family>();
    }
#endif

#if defined(BOTAN_HAS_ARGON2)
    if (req.algo_name() == "Argon2d") {
        return std::make_unique<Argon2_Family>(static_cast<uint8_t>(0));
    } else if (req.algo_name() == "Argon2i") {
        return std::make_unique<Argon2_Family>(static_cast<uint8_t>(1));
    } else if (req.algo_name() == "Argon2id") {
        return std::make_unique<Argon2_Family>(static_cast<uint8_t>(2));
    }
#endif

#if defined(BOTAN_HAS_PBKDF_BCRYPT)
    if (req.algo_name() == "Bcrypt-PBKDF") {
        return std::make_unique<Bcrypt_PBKDF_Family>();
    }
#endif

#if defined(BOTAN_HAS_PGP_S2K)
    if (req.algo_name() == "OpenPGP-S2K" && req.arg_count() == 1) {
        if (auto hash = HashFunction::create(req.arg(0))) {
            return std::make_unique<RFC4880_S2K_Family>(std::move(hash));
        }
    }
#endif

    BOTAN_UNUSED(req);
    BOTAN_UNUSED(provider);

    return nullptr;
}

// static
std::unique_ptr<PasswordHashFamily> PasswordHashFamily::create_or_throw(std::string_view algo,
                                                                        std::string_view provider) {
    if (auto pbkdf = PasswordHashFamily::create(algo, provider)) {
        return pbkdf;
    }
    throw Lookup_Error("PasswordHashFamily", algo, provider);
}

std::vector<std::string> PasswordHashFamily::providers(std::string_view algo_spec) {
    return probe_providers_of<PasswordHashFamily>(algo_spec);
}

}    // namespace Botan
/*
 * PBKDF2
 * (C) 1999-2007 Jack Lloyd
 * (C) 2018 Ribose Inc
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

namespace {

void pbkdf2_set_key(MessageAuthenticationCode& prf, const char* password, size_t password_len) {
    try {
        prf.set_key(cast_char_ptr_to_uint8(password), password_len);
    } catch (Invalid_Key_Length&) {
        throw Invalid_Argument("PBKDF2 cannot accept passphrase of the given size");
    }
}

size_t tune_pbkdf2(MessageAuthenticationCode& prf, size_t output_length,
                   std::chrono::milliseconds msec,
                   std::chrono::milliseconds tune_time = std::chrono::milliseconds(10)) {
    if (output_length == 0) {
        output_length = 1;
    }

    const size_t prf_sz = prf.output_length();
    BOTAN_ASSERT_NOMSG(prf_sz > 0);
    secure_vector<uint8_t> U(prf_sz);

    const size_t trial_iterations = 2000;

    // Short output ensures we only need a single PBKDF2 block

    prf.set_key(nullptr, 0);

    const uint64_t duration_nsec = measure_cost(tune_time, [&]() {
        uint8_t out[12] = {0};
        uint8_t salt[12] = {0};
        pbkdf2(prf, out, sizeof(out), salt, sizeof(salt), trial_iterations);
    });

    const uint64_t desired_nsec = static_cast<uint64_t>(msec.count()) * 1000000;

    if (duration_nsec > desired_nsec) {
        return trial_iterations;
    }

    const size_t blocks_needed = (output_length + prf_sz - 1) / prf_sz;

    const size_t multiplier = static_cast<size_t>(desired_nsec / duration_nsec / blocks_needed);

    if (multiplier == 0) {
        return trial_iterations;
    } else {
        return trial_iterations * multiplier;
    }
}

}    // namespace

size_t pbkdf2(MessageAuthenticationCode& prf, uint8_t out[], size_t out_len,
              std::string_view password, const uint8_t salt[], size_t salt_len, size_t iterations,
              std::chrono::milliseconds msec) {
    if (iterations == 0) {
        iterations = tune_pbkdf2(prf, out_len, msec);
    }

    PBKDF2 pbkdf2(prf, iterations);

    pbkdf2.derive_key(out, out_len, password.data(), password.size(), salt, salt_len);

    return iterations;
}

void pbkdf2(MessageAuthenticationCode& prf, uint8_t out[], size_t out_len, const uint8_t salt[],
            size_t salt_len, size_t iterations) {
    if (iterations == 0) {
        throw Invalid_Argument("PBKDF2: Invalid iteration count");
    }

    clear_mem(out, out_len);

    if (out_len == 0) {
        return;
    }

    const size_t prf_sz = prf.output_length();
    BOTAN_ASSERT_NOMSG(prf_sz > 0);

    secure_vector<uint8_t> U(prf_sz);

    uint32_t counter = 1;
    while (out_len) {
        const size_t prf_output = std::min<size_t>(prf_sz, out_len);

        prf.update(salt, salt_len);
        prf.update_be(counter++);
        prf.final(U.data());

        xor_buf(out, U.data(), prf_output);

        for (size_t i = 1; i != iterations; ++i) {
            prf.update(U);
            prf.final(U.data());
            xor_buf(out, U.data(), prf_output);
        }

        out_len -= prf_output;
        out += prf_output;
    }
}

// PBKDF interface
size_t PKCS5_PBKDF2::pbkdf(uint8_t key[], size_t key_len, std::string_view password,
                           const uint8_t salt[], size_t salt_len, size_t iterations,
                           std::chrono::milliseconds msec) const {
    if (iterations == 0) {
        iterations = tune_pbkdf2(*m_mac, key_len, msec);
    }

    PBKDF2 pbkdf2(*m_mac, iterations);

    pbkdf2.derive_key(key, key_len, password.data(), password.size(), salt, salt_len);

    return iterations;
}

std::string PKCS5_PBKDF2::name() const { return fmt("PBKDF2({})", m_mac->name()); }

std::unique_ptr<PBKDF> PKCS5_PBKDF2::new_object() const {
    return std::make_unique<PKCS5_PBKDF2>(m_mac->new_object());
}

// PasswordHash interface

PBKDF2::PBKDF2(const MessageAuthenticationCode& prf, size_t olen, std::chrono::milliseconds msec)
    : m_prf(prf.new_object()), m_iterations(tune_pbkdf2(*m_prf, olen, msec)) {}

std::string PBKDF2::to_string() const { return fmt("PBKDF2({},{})", m_prf->name(), m_iterations); }

void PBKDF2::derive_key(uint8_t out[], size_t out_len, const char* password,
                        const size_t password_len, const uint8_t salt[], size_t salt_len) const {
    pbkdf2_set_key(*m_prf, password, password_len);
    pbkdf2(*m_prf, out, out_len, salt, salt_len, m_iterations);
}

std::string PBKDF2_Family::name() const { return fmt("PBKDF2({})", m_prf->name()); }

std::unique_ptr<PasswordHash> PBKDF2_Family::tune(size_t output_len, std::chrono::milliseconds msec,
                                                  size_t /*max_memory_usage_mb*/,
                                                  std::chrono::milliseconds tune_time) const {
    auto iterations = tune_pbkdf2(*m_prf, output_len, msec, tune_time);
    return std::make_unique<PBKDF2>(*m_prf, iterations);
}

std::unique_ptr<PasswordHash> PBKDF2_Family::default_params() const {
    return std::make_unique<PBKDF2>(*m_prf, 150000);
}

std::unique_ptr<PasswordHash> PBKDF2_Family::from_params(size_t iter, size_t /*i2*/,
                                                         size_t /*i3*/) const {
    return std::make_unique<PBKDF2>(*m_prf, iter);
}

std::unique_ptr<PasswordHash> PBKDF2_Family::from_iterations(size_t iter) const {
    return std::make_unique<PBKDF2>(*m_prf, iter);
}

}    // namespace Botan
/*
 * EME Base Class
 * (C) 1999-2008 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_EME_OAEP)
#endif

#if defined(BOTAN_HAS_EME_PKCS1)
#endif

#if defined(BOTAN_HAS_EME_RAW)
#endif

namespace Botan {

std::unique_ptr<EME> EME::create(std::string_view algo_spec) {
#if defined(BOTAN_HAS_EME_RAW)
    if (algo_spec == "Raw") {
        return std::make_unique<EME_Raw>();
    }
#endif

#if defined(BOTAN_HAS_EME_PKCS1)
    // TODO(Botan4) Remove all but "PKCS1v15"
    if (algo_spec == "PKCS1v15" || algo_spec == "EME-PKCS1-v1_5") {
        return std::make_unique<EME_PKCS1v15>();
    }
#endif

#if defined(BOTAN_HAS_EME_OAEP)
    SCAN_Name req(algo_spec);

    // TODO(Botan4) Remove all but "OAEP"
    if (req.algo_name() == "OAEP" || req.algo_name() == "EME-OAEP" || req.algo_name() == "EME1") {
        if (req.arg_count() == 1 ||
            ((req.arg_count() == 2 || req.arg_count() == 3) && req.arg(1) == "MGF1")) {
            if (auto hash = HashFunction::create(req.arg(0))) {
                return std::make_unique<OAEP>(std::move(hash), req.arg(2, ""));
            }
        } else if (req.arg_count() == 2 || req.arg_count() == 3) {
            auto mgf_params = parse_algorithm_name(req.arg(1));

            if (mgf_params.size() == 2 && mgf_params[0] == "MGF1") {
                auto hash = HashFunction::create(req.arg(0));
                auto mgf1_hash = HashFunction::create(mgf_params[1]);

                if (hash && mgf1_hash) {
                    return std::make_unique<OAEP>(std::move(hash), std::move(mgf1_hash),
                                                  req.arg(2, ""));
                }
            }
        }
    }
#endif

    throw Algorithm_Not_Found(algo_spec);
}

EME::~EME() = default;

}    // namespace Botan
/*
 * (C) 2015 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_EMSA_X931)
#endif

#if defined(BOTAN_HAS_EMSA_PKCS1)
#endif

#if defined(BOTAN_HAS_EMSA_PSSR)
#endif

#if defined(BOTAN_HAS_EMSA_RAW)
#endif

#if defined(BOTAN_HAS_ISO_9796)
#endif

namespace Botan {

std::unique_ptr<EMSA> EMSA::create(std::string_view algo_spec) {
    SCAN_Name req(algo_spec);

#if defined(BOTAN_HAS_EMSA_PKCS1)
    // TODO(Botan4) Remove all but "PKCS1v15"
    if (req.algo_name() == "EMSA_PKCS1" || req.algo_name() == "PKCS1v15" ||
        req.algo_name() == "EMSA-PKCS1-v1_5" || req.algo_name() == "EMSA3") {
        if (req.arg_count() == 2 && req.arg(0) == "Raw") {
            return std::make_unique<EMSA_PKCS1v15_Raw>(req.arg(1));
        } else if (req.arg_count() == 1) {
            if (req.arg(0) == "Raw") {
                return std::make_unique<EMSA_PKCS1v15_Raw>();
            } else {
                if (auto hash = HashFunction::create(req.arg(0))) {
                    return std::make_unique<EMSA_PKCS1v15>(std::move(hash));
                }
            }
        }
    }
#endif

#if defined(BOTAN_HAS_EMSA_PSSR)
    // TODO(Botan4) Remove all but "PSS_Raw"
    if (req.algo_name() == "PSS_Raw" || req.algo_name() == "PSSR_Raw") {
        if (req.arg_count_between(1, 3) && req.arg(1, "MGF1") == "MGF1") {
            if (auto hash = HashFunction::create(req.arg(0))) {
                if (req.arg_count() == 3) {
                    const size_t salt_size = req.arg_as_integer(2, 0);
                    return std::make_unique<PSSR_Raw>(std::move(hash), salt_size);
                } else {
                    return std::make_unique<PSSR_Raw>(std::move(hash));
                }
            }
        }
    }

    // TODO(Botan4) Remove all but "PSS"
    if (req.algo_name() == "PSS" || req.algo_name() == "PSSR" || req.algo_name() == "EMSA-PSS" ||
        req.algo_name() == "PSS-MGF1" || req.algo_name() == "EMSA4") {
        if (req.arg_count_between(1, 3) && req.arg(1, "MGF1") == "MGF1") {
            if (auto hash = HashFunction::create(req.arg(0))) {
                if (req.arg_count() == 3) {
                    const size_t salt_size = req.arg_as_integer(2, 0);
                    return std::make_unique<PSSR>(std::move(hash), salt_size);
                } else {
                    return std::make_unique<PSSR>(std::move(hash));
                }
            }
        }
    }
#endif

#if defined(BOTAN_HAS_ISO_9796)
    if (req.algo_name() == "ISO_9796_DS2") {
        if (req.arg_count_between(1, 3)) {
            if (auto hash = HashFunction::create(req.arg(0))) {
                const size_t salt_size = req.arg_as_integer(2, hash->output_length());
                const bool implicit = req.arg(1, "exp") == "imp";
                return std::make_unique<ISO_9796_DS2>(std::move(hash), implicit, salt_size);
            }
        }
    }
    // ISO-9796-2 DS 3 is deterministic and DS2 without a salt
    if (req.algo_name() == "ISO_9796_DS3") {
        if (req.arg_count_between(1, 2)) {
            if (auto hash = HashFunction::create(req.arg(0))) {
                const bool implicit = req.arg(1, "exp") == "imp";
                return std::make_unique<ISO_9796_DS3>(std::move(hash), implicit);
            }
        }
    }
#endif

#if defined(BOTAN_HAS_EMSA_X931)
    // TODO(Botan4) Remove all but "X9.31"
    if (req.algo_name() == "EMSA_X931" || req.algo_name() == "EMSA2" ||
        req.algo_name() == "X9.31") {
        if (req.arg_count() == 1) {
            if (auto hash = HashFunction::create(req.arg(0))) {
                return std::make_unique<EMSA_X931>(std::move(hash));
            }
        }
    }
#endif

#if defined(BOTAN_HAS_EMSA_RAW)
    if (req.algo_name() == "Raw") {
        if (req.arg_count() == 0) {
            return std::make_unique<EMSA_Raw>();
        } else {
            auto hash = HashFunction::create(req.arg(0));
            if (hash) {
                return std::make_unique<EMSA_Raw>(hash->output_length());
            }
        }
    }
#endif

    return nullptr;
}

std::unique_ptr<EMSA> EMSA::create_or_throw(std::string_view algo_spec) {
    auto emsa = EMSA::create(algo_spec);
    if (emsa) {
        return emsa;
    }
    throw Algorithm_Not_Found(algo_spec);
}

}    // namespace Botan
/*
 * (C) 2017,2018 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

namespace {

/*
 * The minimum weight irreducible binary polynomial of size n
 *
 * See "Table of Low-Weight Binary Irreducible Polynomials"
 * by Gadiel Seroussi, HP Labs Tech Report HPL-98-135
 * http://www.hpl.hp.com/techreports/98/HPL-98-135.pdf
 */
enum class MinWeightPolynomial : uint32_t {
    P64 = 0x1B,
    P128 = 0x87,
    P192 = 0x87,
    P256 = 0x425,
    P512 = 0x125,
    P1024 = 0x80043,
};

/**
 * If the top bit of c is set, returns the carry (the polynomial)
 *
 * Otherwise returns zero.
 */
template <MinWeightPolynomial P>
inline uint64_t return_carry(uint64_t c) {
    return CT::Mask<uint64_t>::expand_top_bit(c).if_set_return(static_cast<uint64_t>(P));
}

template <size_t LIMBS, MinWeightPolynomial P>
void poly_double(uint8_t out[], const uint8_t in[]) {
    uint64_t W[LIMBS];
    load_be(W, in, LIMBS);

    const uint64_t carry = return_carry<P>(W[0]);

    if constexpr (LIMBS > 0) {
        for (size_t i = 0; i != LIMBS - 1; ++i) {
            W[i] = (W[i] << 1) ^ (W[i + 1] >> 63);
        }
    }

    W[LIMBS - 1] = (W[LIMBS - 1] << 1) ^ carry;

    copy_out_be(std::span(out, LIMBS * 8), W);
}

template <size_t LIMBS, MinWeightPolynomial P>
void poly_double_le(uint8_t out[], const uint8_t in[]) {
    uint64_t W[LIMBS];
    load_le(W, in, LIMBS);

    const uint64_t carry = return_carry<P>(W[LIMBS - 1]);

    if constexpr (LIMBS > 0) {
        for (size_t i = 0; i != LIMBS - 1; ++i) {
            W[LIMBS - 1 - i] = (W[LIMBS - 1 - i] << 1) ^ (W[LIMBS - 2 - i] >> 63);
        }
    }

    W[0] = (W[0] << 1) ^ carry;

    copy_out_le(std::span(out, LIMBS * 8), W);
}

}    // namespace

void poly_double_n(uint8_t out[], const uint8_t in[], size_t n) {
    switch (n) {
        case 8:
            return poly_double<1, MinWeightPolynomial::P64>(out, in);
        case 16:
            return poly_double<2, MinWeightPolynomial::P128>(out, in);
        case 24:
            return poly_double<3, MinWeightPolynomial::P192>(out, in);
        case 32:
            return poly_double<4, MinWeightPolynomial::P256>(out, in);
        case 64:
            return poly_double<8, MinWeightPolynomial::P512>(out, in);
        case 128:
            return poly_double<16, MinWeightPolynomial::P1024>(out, in);
        default:
            throw Invalid_Argument("Unsupported size for poly_double_n");
    }
}

void poly_double_n_le(uint8_t out[], const uint8_t in[], size_t n) {
    switch (n) {
        case 8:
            return poly_double_le<1, MinWeightPolynomial::P64>(out, in);
        case 16:
            return poly_double_le<2, MinWeightPolynomial::P128>(out, in);
        case 24:
            return poly_double_le<3, MinWeightPolynomial::P192>(out, in);
        case 32:
            return poly_double_le<4, MinWeightPolynomial::P256>(out, in);
        case 64:
            return poly_double_le<8, MinWeightPolynomial::P512>(out, in);
        case 128:
            return poly_double_le<16, MinWeightPolynomial::P1024>(out, in);
        default:
            throw Invalid_Argument("Unsupported size for poly_double_n_le");
    }
}

void xts_update_tweak_block(uint8_t tweak[], size_t BS, size_t blocks_in_tweak) {
    if (BS == 16) {
        constexpr size_t LIMBS = 2;

        uint64_t W[LIMBS];
        load_le(W, &tweak[0], LIMBS);

        for (size_t i = 1; i < blocks_in_tweak; ++i) {
            const uint64_t carry = return_carry<MinWeightPolynomial::P128>(W[1]);
            W[1] = (W[1] << 1) ^ (W[0] >> 63);
            W[0] = (W[0] << 1) ^ carry;
            copy_out_le(std::span(&tweak[i * BS], 2 * 8), W);
        }
    } else {
        for (size_t i = 1; i < blocks_in_tweak; ++i) {
            const uint8_t* prev = &tweak[(i - 1) * BS];
            uint8_t* cur = &tweak[i * BS];
            poly_double_n_le(cur, prev, BS);
        }
    }
}

}    // namespace Botan
/*
 * (C) 2016 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_ENTROPY_SOURCE)
#endif

#if defined(BOTAN_HAS_SYSTEM_RNG)
#endif

#if defined(BOTAN_HAS_OS_UTILS)
#endif

#include <array>

namespace Botan {

void RandomNumberGenerator::randomize_with_ts_input(std::span<uint8_t> output) {
    if (this->accepts_input()) {
        std::array<uint8_t, 32> additional_input = {0};

#if defined(BOTAN_HAS_OS_UTILS)
        store_le(std::span{additional_input}.subspan<0, 8>(), OS::get_high_resolution_clock());
        store_le(std::span{additional_input}.subspan<8, 4>(), OS::get_process_id());
        constexpr size_t offset = 12;
#else
        constexpr size_t offset = 0;
#endif

#if defined(BOTAN_HAS_SYSTEM_RNG)
        system_rng().randomize(std::span{additional_input}.subspan<offset>());
#else
        BOTAN_UNUSED(offset);
#endif

        this->fill_bytes_with_input(output, additional_input);
    } else {
        this->fill_bytes_with_input(output, {});
    }
}

size_t RandomNumberGenerator::reseed(Entropy_Sources& srcs, size_t poll_bits,
                                     std::chrono::milliseconds poll_timeout) {
    if (this->accepts_input()) {
#if defined(BOTAN_HAS_ENTROPY_SOURCE)
        return srcs.poll(*this, poll_bits, poll_timeout);
#else
        BOTAN_UNUSED(srcs, poll_bits, poll_timeout);
#endif
    }

    return 0;
}

void RandomNumberGenerator::reseed_from_rng(RandomNumberGenerator& rng, size_t poll_bits) {
    if (this->accepts_input()) {
        this->add_entropy(rng.random_vec(poll_bits / 8));
    }
}

void Null_RNG::fill_bytes_with_input(std::span<uint8_t> output,
                                     std::span<const uint8_t> /* ignored */) {
    // throw if caller tries to obtain random bytes
    if (!output.empty()) {
        throw PRNG_Unseeded("Null_RNG called");
    }
}

}    // namespace Botan
/*
 * SHA-1
 * (C) 1999-2008,2011 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_HAS_CPUID)
#endif

namespace Botan {

namespace SHA1_F {

namespace {

/*
 * SHA-1 F1 Function
 */
inline void F1(uint32_t A, uint32_t& B, uint32_t C, uint32_t D, uint32_t& E, uint32_t msg) {
    E += choose(B, C, D) + msg + 0x5A827999 + rotl<5>(A);
    B = rotl<30>(B);
}

/*
 * SHA-1 F2 Function
 */
inline void F2(uint32_t A, uint32_t& B, uint32_t C, uint32_t D, uint32_t& E, uint32_t msg) {
    E += (B ^ C ^ D) + msg + 0x6ED9EBA1 + rotl<5>(A);
    B = rotl<30>(B);
}

/*
 * SHA-1 F3 Function
 */
inline void F3(uint32_t A, uint32_t& B, uint32_t C, uint32_t D, uint32_t& E, uint32_t msg) {
    E += majority(B, C, D) + msg + 0x8F1BBCDC + rotl<5>(A);
    B = rotl<30>(B);
}

/*
 * SHA-1 F4 Function
 */
inline void F4(uint32_t A, uint32_t& B, uint32_t C, uint32_t D, uint32_t& E, uint32_t msg) {
    E += (B ^ C ^ D) + msg + 0xCA62C1D6 + rotl<5>(A);
    B = rotl<30>(B);
}

}    // namespace

}    // namespace SHA1_F

/*
 * SHA-1 Compression Function
 */
void SHA_1::compress_n(digest_type& digest, std::span<const uint8_t> input, size_t blocks) {
    using namespace SHA1_F;

#if defined(BOTAN_HAS_SHA1_X86_SHA_NI)
    if (CPUID::has(CPUID::Feature::SHA)) {
        return sha1_compress_x86(digest, input, blocks);
    }
#endif

#if defined(BOTAN_HAS_SHA1_ARMV8)
    if (CPUID::has(CPUID::Feature::SHA1)) {
        return sha1_armv8_compress_n(digest, input, blocks);
    }
#endif

#if defined(BOTAN_HAS_SHA1_SIMD_4X32)
    if (CPUID::has(CPUID::Feature::SIMD_4X32)) {
        return simd_compress_n(digest, input, blocks);
    }

#endif

    uint32_t A = digest[0], B = digest[1], C = digest[2], D = digest[3], E = digest[4];
    std::array<uint32_t, 80> W;
    auto W_in = std::span{W}.first<block_bytes / sizeof(uint32_t)>();

    BufferSlicer in(input);

    for (size_t i = 0; i != blocks; ++i) {
        load_be(W_in, in.take<block_bytes>());

        // clang-format off

      for(size_t j = 16; j != 80; j += 8) {
         W[j + 0] = rotl<1>(W[j - 3] ^ W[j - 8] ^ W[j - 14] ^ W[j - 16]);
         W[j + 1] = rotl<1>(W[j - 2] ^ W[j - 7] ^ W[j - 13] ^ W[j - 15]);
         W[j + 2] = rotl<1>(W[j - 1] ^ W[j - 6] ^ W[j - 12] ^ W[j - 14]);
         W[j + 3] = rotl<1>(W[j    ] ^ W[j - 5] ^ W[j - 11] ^ W[j - 13]);
         W[j + 4] = rotl<1>(W[j + 1] ^ W[j - 4] ^ W[j - 10] ^ W[j - 12]);
         W[j + 5] = rotl<1>(W[j + 2] ^ W[j - 3] ^ W[j -  9] ^ W[j - 11]);
         W[j + 6] = rotl<1>(W[j + 3] ^ W[j - 2] ^ W[j -  8] ^ W[j - 10]);
         W[j + 7] = rotl<1>(W[j + 4] ^ W[j - 1] ^ W[j -  7] ^ W[j -  9]);
      }

        // clang-format on

        F1(A, B, C, D, E, W[0]);
        F1(E, A, B, C, D, W[1]);
        F1(D, E, A, B, C, W[2]);
        F1(C, D, E, A, B, W[3]);
        F1(B, C, D, E, A, W[4]);
        F1(A, B, C, D, E, W[5]);
        F1(E, A, B, C, D, W[6]);
        F1(D, E, A, B, C, W[7]);
        F1(C, D, E, A, B, W[8]);
        F1(B, C, D, E, A, W[9]);
        F1(A, B, C, D, E, W[10]);
        F1(E, A, B, C, D, W[11]);
        F1(D, E, A, B, C, W[12]);
        F1(C, D, E, A, B, W[13]);
        F1(B, C, D, E, A, W[14]);
        F1(A, B, C, D, E, W[15]);
        F1(E, A, B, C, D, W[16]);
        F1(D, E, A, B, C, W[17]);
        F1(C, D, E, A, B, W[18]);
        F1(B, C, D, E, A, W[19]);

        F2(A, B, C, D, E, W[20]);
        F2(E, A, B, C, D, W[21]);
        F2(D, E, A, B, C, W[22]);
        F2(C, D, E, A, B, W[23]);
        F2(B, C, D, E, A, W[24]);
        F2(A, B, C, D, E, W[25]);
        F2(E, A, B, C, D, W[26]);
        F2(D, E, A, B, C, W[27]);
        F2(C, D, E, A, B, W[28]);
        F2(B, C, D, E, A, W[29]);
        F2(A, B, C, D, E, W[30]);
        F2(E, A, B, C, D, W[31]);
        F2(D, E, A, B, C, W[32]);
        F2(C, D, E, A, B, W[33]);
        F2(B, C, D, E, A, W[34]);
        F2(A, B, C, D, E, W[35]);
        F2(E, A, B, C, D, W[36]);
        F2(D, E, A, B, C, W[37]);
        F2(C, D, E, A, B, W[38]);
        F2(B, C, D, E, A, W[39]);

        F3(A, B, C, D, E, W[40]);
        F3(E, A, B, C, D, W[41]);
        F3(D, E, A, B, C, W[42]);
        F3(C, D, E, A, B, W[43]);
        F3(B, C, D, E, A, W[44]);
        F3(A, B, C, D, E, W[45]);
        F3(E, A, B, C, D, W[46]);
        F3(D, E, A, B, C, W[47]);
        F3(C, D, E, A, B, W[48]);
        F3(B, C, D, E, A, W[49]);
        F3(A, B, C, D, E, W[50]);
        F3(E, A, B, C, D, W[51]);
        F3(D, E, A, B, C, W[52]);
        F3(C, D, E, A, B, W[53]);
        F3(B, C, D, E, A, W[54]);
        F3(A, B, C, D, E, W[55]);
        F3(E, A, B, C, D, W[56]);
        F3(D, E, A, B, C, W[57]);
        F3(C, D, E, A, B, W[58]);
        F3(B, C, D, E, A, W[59]);

        F4(A, B, C, D, E, W[60]);
        F4(E, A, B, C, D, W[61]);
        F4(D, E, A, B, C, W[62]);
        F4(C, D, E, A, B, W[63]);
        F4(B, C, D, E, A, W[64]);
        F4(A, B, C, D, E, W[65]);
        F4(E, A, B, C, D, W[66]);
        F4(D, E, A, B, C, W[67]);
        F4(C, D, E, A, B, W[68]);
        F4(B, C, D, E, A, W[69]);
        F4(A, B, C, D, E, W[70]);
        F4(E, A, B, C, D, W[71]);
        F4(D, E, A, B, C, W[72]);
        F4(C, D, E, A, B, W[73]);
        F4(B, C, D, E, A, W[74]);
        F4(A, B, C, D, E, W[75]);
        F4(E, A, B, C, D, W[76]);
        F4(D, E, A, B, C, W[77]);
        F4(C, D, E, A, B, W[78]);
        F4(B, C, D, E, A, W[79]);

        A = (digest[0] += A);
        B = (digest[1] += B);
        C = (digest[2] += C);
        D = (digest[3] += D);
        E = (digest[4] += E);
    }
}

/*
 * Clear memory of sensitive data
 */
void SHA_1::init(digest_type& digest) {
    digest.assign({0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0});
}

std::string SHA_1::provider() const {
#if defined(BOTAN_HAS_SHA1_X86_SHA_NI)
    if (auto feat = CPUID::check(CPUID::Feature::SHA)) {
        return *feat;
    }
#endif

#if defined(BOTAN_HAS_SHA1_ARMV8)
    if (auto feat = CPUID::check(CPUID::Feature::SHA1)) {
        return *feat;
    }
#endif

#if defined(BOTAN_HAS_SHA1_SIMD_4X32)
    if (auto feat = CPUID::check(CPUID::Feature::SIMD_4X32)) {
        return *feat;
    }
#endif

    return "base";
}

std::unique_ptr<HashFunction> SHA_1::new_object() const { return std::make_unique<SHA_1>(); }

std::unique_ptr<HashFunction> SHA_1::copy_state() const { return std::make_unique<SHA_1>(*this); }

void SHA_1::add_data(std::span<const uint8_t> input) { m_md.update(input); }

void SHA_1::final_result(std::span<uint8_t> output) { m_md.final(output); }

}    // namespace Botan
/*
 * (C) 2016,2020 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

void Stateful_RNG::clear() {
    lock_guard_type<recursive_mutex_type> lock(m_mutex);
    m_reseed_counter = 0;
    m_last_pid = 0;
    clear_state();
}

void Stateful_RNG::force_reseed() {
    lock_guard_type<recursive_mutex_type> lock(m_mutex);
    m_reseed_counter = 0;
}

bool Stateful_RNG::is_seeded() const {
    lock_guard_type<recursive_mutex_type> lock(m_mutex);
    return m_reseed_counter > 0;
}

void Stateful_RNG::initialize_with(std::span<const uint8_t> input) {
    lock_guard_type<recursive_mutex_type> lock(m_mutex);

    clear();
    add_entropy(input);
}

void Stateful_RNG::generate_batched_output(std::span<uint8_t> output,
                                           std::span<const uint8_t> input) {
    BOTAN_ASSERT_NOMSG(!output.empty());

    const size_t max_per_request = max_number_of_bytes_per_request();

    if (max_per_request == 0) {
        // no limit
        reseed_check();
        this->generate_output(output, input);
    } else {
        while (!output.empty()) {
            const size_t this_req = std::min(max_per_request, output.size());

            reseed_check();
            this->generate_output(output.subspan(0, this_req), input);

            // only include the input for the first iteration
            input = {};

            output = output.subspan(this_req);
        }
    }
}

void Stateful_RNG::fill_bytes_with_input(std::span<uint8_t> output,
                                         std::span<const uint8_t> input) {
    lock_guard_type<recursive_mutex_type> lock(m_mutex);

    if (output.empty()) {
        // Special case for exclusively adding entropy to the stateful RNG.
        this->update(input);

        if (8 * input.size() >= security_level()) {
            reset_reseed_counter();
        }
    } else {
        generate_batched_output(output, input);
    }
}

size_t Stateful_RNG::reseed(Entropy_Sources& srcs, size_t poll_bits,
                            std::chrono::milliseconds poll_timeout) {
    lock_guard_type<recursive_mutex_type> lock(m_mutex);

    const size_t bits_collected = RandomNumberGenerator::reseed(srcs, poll_bits, poll_timeout);

    if (bits_collected >= security_level()) {
        reset_reseed_counter();
    }

    return bits_collected;
}

void Stateful_RNG::reseed_from_rng(RandomNumberGenerator& rng, size_t poll_bits) {
    lock_guard_type<recursive_mutex_type> lock(m_mutex);

    RandomNumberGenerator::reseed_from_rng(rng, poll_bits);

    if (poll_bits >= security_level()) {
        reset_reseed_counter();
    }
}

void Stateful_RNG::reset_reseed_counter() {
    // Lock is held whenever this function is called
    m_reseed_counter = 1;
}

void Stateful_RNG::reseed_check() {
    // Lock is held whenever this function is called

    const uint32_t cur_pid = OS::get_process_id();

    const bool fork_detected = (m_last_pid > 0) && (cur_pid != m_last_pid);

    if (is_seeded() == false || fork_detected ||
        (m_reseed_interval > 0 && m_reseed_counter >= m_reseed_interval)) {
        m_reseed_counter = 0;
        m_last_pid = cur_pid;

        if (m_underlying_rng) {
            reseed_from_rng(*m_underlying_rng, security_level());
        }

        if (m_entropy_sources) {
            reseed(*m_entropy_sources, security_level());
        }

        if (!is_seeded()) {
            if (fork_detected) {
                throw Invalid_State("Detected use of fork but cannot reseed DRBG");
            } else {
                throw PRNG_Unseeded(name());
            }
        }
    } else {
        BOTAN_ASSERT(m_reseed_counter != 0, "RNG is seeded");
        m_reseed_counter += 1;
    }
}

}    // namespace Botan
/*
 * (C) 2017,2023 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include <new>

#if defined(BOTAN_HAS_LOCKING_ALLOCATOR)
#endif

namespace Botan {

BOTAN_MALLOC_FN void* allocate_memory(size_t elems, size_t elem_size) {
    if (elems == 0 || elem_size == 0) {
        return nullptr;
    }

    // Some calloc implementations do not check for overflow (?!?)
    if (!checked_mul(elems, elem_size).has_value()) {
        throw std::bad_alloc();
    }

#if defined(BOTAN_HAS_LOCKING_ALLOCATOR)
    if (void* p = mlock_allocator::instance().allocate(elems, elem_size)) {
        return p;
    }
#endif

#if defined(BOTAN_TARGET_OS_HAS_ALLOC_CONCEAL)
    void* ptr = ::calloc_conceal(elems, elem_size);
#else
    void* ptr = std::calloc(elems, elem_size);    // NOLINT(*-no-malloc)
#endif
    if (!ptr) {
        [[unlikely]] throw std::bad_alloc();
    }
    return ptr;
}

void deallocate_memory(void* p, size_t elems, size_t elem_size) {
    if (p == nullptr) {
        [[unlikely]] return;
    }

    secure_scrub_memory(p, elems * elem_size);

#if defined(BOTAN_HAS_LOCKING_ALLOCATOR)
    if (mlock_allocator::instance().deallocate(p, elems, elem_size)) {
        return;
    }
#endif

    std::free(p);    // NOLINT(*-no-malloc)
}

void initialize_allocator() {
#if defined(BOTAN_HAS_LOCKING_ALLOCATOR)
    mlock_allocator::instance();
#endif
}

}    // namespace Botan
/*
 * Runtime assertion checking
 * (C) 2010,2012,2018 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_TERMINATE_ON_ASSERTS)
#include <iostream>
#endif

namespace Botan {

void throw_invalid_argument(const char* message, const char* func, const char* file) {
    throw Invalid_Argument(fmt("{} in {}:{}", message, func, file));
}

void throw_invalid_state(const char* expr, const char* func, const char* file) {
    throw Invalid_State(fmt("Invalid state: expr {} was false in {}:{}", expr, func, file));
}

void assertion_failure(const char* expr_str, const char* assertion_made, const char* func,
                       const char* file, int line) {
    std::ostringstream format;

    format << "False assertion ";

    if (assertion_made && assertion_made[0] != 0) {
        format << "'" << assertion_made << "' (expression " << expr_str << ") ";
    } else {
        format << expr_str << " ";
    }

    if (func) {
        format << "in " << func << " ";
    }

    format << "@" << file << ":" << line;

#if defined(BOTAN_TERMINATE_ON_ASSERTS)
    std::cerr << format.str() << '\n';
    std::abort();
#else
    throw Internal_Error(format.str());
#endif
}

void assert_unreachable(const char* file, int line) {
    const std::string msg =
        fmt("Codepath that was marked unreachable was reached @{}:{}", file, line);

#if defined(BOTAN_TERMINATE_ON_ASSERTS)
    std::cerr << msg << '\n';
    std::abort();
#else
    throw Internal_Error(msg);
#endif
}

}    // namespace Botan
/*
 * Calendar Functions
 * (C) 1999-2010,2017 Jack Lloyd
 * (C) 2015 Simon Warta (Kullo GmbH)
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include <ctime>

namespace Botan {

namespace {

std::tm do_gmtime(std::time_t time_val) {
    std::tm tm;

#if defined(BOTAN_TARGET_OS_HAS_WIN32)
    ::gmtime_s(&tm, &time_val);    // Windows
#elif defined(BOTAN_TARGET_OS_HAS_POSIX1)
    ::gmtime_r(&time_val, &tm);    // Unix/SUSv2
#else
    std::tm* tm_p = std::gmtime(&time_val);
    if (tm_p == nullptr) throw Encoding_Error("time_t_to_tm could not convert");
    tm = *tm_p;
#endif

    return tm;
}

/*
Portable replacement for timegm, _mkgmtime, etc

Algorithm due to Howard Hinnant

See https://howardhinnant.github.io/date_algorithms.html#days_from_civil
for details and explaination. The code is slightly simplified by our assumption
that the date is at least 1970, which is sufficient for our purposes.
*/
uint64_t days_since_epoch(uint32_t year, uint32_t month, uint32_t day) {
    BOTAN_ARG_CHECK(year >= 1970, "Years before 1970 not supported");

    if (month <= 2) {
        year -= 1;
    }
    const uint32_t era = year / 400;
    const uint32_t yoe = year - era * 400;                                            // [0, 399]
    const uint32_t doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;    // [0, 365]
    const uint32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;                       // [0, 146096]
    return era * 146097 + doe - 719468;
}

}    // namespace

uint64_t calendar_point::seconds_since_epoch() const {
    return (days_since_epoch(year(), month(), day()) * 86400) + (hour() * 60 * 60) +
           (minutes() * 60) + seconds();
}

std::chrono::system_clock::time_point calendar_point::to_std_timepoint() const {
    const uint64_t seconds_64 = this->seconds_since_epoch();
    const time_t seconds_time_t = static_cast<time_t>(seconds_64);

    if (seconds_64 - seconds_time_t != 0) {
        throw Invalid_Argument("calendar_point::to_std_timepoint time_t overflow");
    }

    return std::chrono::system_clock::from_time_t(seconds_time_t);
}

std::string calendar_point::to_string() const {
    // desired format: <YYYY>-<MM>-<dd>T<HH>:<mm>:<ss>
    std::stringstream output;
    output << std::setfill('0') << std::setw(4) << year() << "-" << std::setw(2) << month() << "-"
           << std::setw(2) << day() << "T" << std::setw(2) << hour() << ":" << std::setw(2)
           << minutes() << ":" << std::setw(2) << seconds();
    return output.str();
}

calendar_point::calendar_point(const std::chrono::system_clock::time_point& time_point) {
    std::tm tm = do_gmtime(std::chrono::system_clock::to_time_t(time_point));

    m_year = tm.tm_year + 1900;
    m_month = tm.tm_mon + 1;
    m_day = tm.tm_mday;
    m_hour = tm.tm_hour;
    m_minutes = tm.tm_min;
    m_seconds = tm.tm_sec;
}

}    // namespace Botan
/*
 * Character Set Handling
 * (C) 1999-2007,2021 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

namespace {

void append_utf8_for(std::string& s, uint32_t c) {
    if (c >= 0xD800 && c < 0xE000) {
        throw Decoding_Error("Invalid Unicode character");
    }

    if (c <= 0x7F) {
        const uint8_t b0 = static_cast<uint8_t>(c);
        s.push_back(static_cast<char>(b0));
    } else if (c <= 0x7FF) {
        const uint8_t b0 = 0xC0 | static_cast<uint8_t>(c >> 6);
        const uint8_t b1 = 0x80 | static_cast<uint8_t>(c & 0x3F);
        s.push_back(static_cast<char>(b0));
        s.push_back(static_cast<char>(b1));
    } else if (c <= 0xFFFF) {
        const uint8_t b0 = 0xE0 | static_cast<uint8_t>(c >> 12);
        const uint8_t b1 = 0x80 | static_cast<uint8_t>((c >> 6) & 0x3F);
        const uint8_t b2 = 0x80 | static_cast<uint8_t>(c & 0x3F);
        s.push_back(static_cast<char>(b0));
        s.push_back(static_cast<char>(b1));
        s.push_back(static_cast<char>(b2));
    } else if (c <= 0x10FFFF) {
        const uint8_t b0 = 0xF0 | static_cast<uint8_t>(c >> 18);
        const uint8_t b1 = 0x80 | static_cast<uint8_t>((c >> 12) & 0x3F);
        const uint8_t b2 = 0x80 | static_cast<uint8_t>((c >> 6) & 0x3F);
        const uint8_t b3 = 0x80 | static_cast<uint8_t>(c & 0x3F);
        s.push_back(static_cast<char>(b0));
        s.push_back(static_cast<char>(b1));
        s.push_back(static_cast<char>(b2));
        s.push_back(static_cast<char>(b3));
    } else {
        throw Decoding_Error("Invalid Unicode character");
    }
}

}    // namespace

std::string ucs2_to_utf8(const uint8_t ucs2[], size_t len) {
    if (len % 2 != 0) {
        throw Decoding_Error("Invalid length for UCS-2 string");
    }

    const size_t chars = len / 2;

    std::string s;
    for (size_t i = 0; i != chars; ++i) {
        const uint32_t c = load_be<uint16_t>(ucs2, i);
        append_utf8_for(s, c);
    }

    return s;
}

std::string ucs4_to_utf8(const uint8_t ucs4[], size_t len) {
    if (len % 4 != 0) {
        throw Decoding_Error("Invalid length for UCS-4 string");
    }

    const size_t chars = len / 4;

    std::string s;
    for (size_t i = 0; i != chars; ++i) {
        const uint32_t c = load_be<uint32_t>(ucs4, i);
        append_utf8_for(s, c);
    }

    return s;
}

/*
 * Convert from ISO 8859-1 to UTF-8
 */
std::string latin1_to_utf8(const uint8_t chars[], size_t len) {
    std::string s;
    for (size_t i = 0; i != len; ++i) {
        const uint32_t c = static_cast<uint8_t>(chars[i]);
        append_utf8_for(s, c);
    }
    return s;
}

std::string format_char_for_display(char c) {
    std::ostringstream oss;

    oss << "'";

    if (c == '\t') {
        oss << "\\t";
    } else if (c == '\n') {
        oss << "\\n";
    } else if (c == '\r') {
        oss << "\\r";
    } else if (static_cast<unsigned char>(c) >= 128) {
        unsigned char z = static_cast<unsigned char>(c);
        oss << "\\x" << std::hex << std::uppercase << static_cast<int>(z);
    } else {
        oss << c;
    }

    oss << "'";

    return oss.str();
}

}    // namespace Botan
/*
 * (C) 2018,2021 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

CT::Option<size_t> CT::copy_output(CT::Choice accept, std::span<uint8_t> output,
                                   std::span<const uint8_t> input, size_t offset) {
    // This leaks information about the input length, but this happens
    // unavoidably since we are unable to ready any bytes besides those
    // in input[0..n]
    BOTAN_ARG_CHECK(output.size() >= input.size(), "Invalid span lengths");

    /*
     * We do not poison the input here because if we did we would have
     * to unpoison it at exit. We assume instead that callers have
     * already poisoned the input and will unpoison it at their own
     * time.
     */
    CT::poison(offset);

    /**
     * Zeroize the entire output buffer to get started
     */
    clear_mem(output);

    /*
     * If the offset is greater than input length, then the arguments are
     * invalid. Ideally we would throw an exception, but that leaks
     * information about the offset. Instead treat it as if the input
     * was invalid.
     */
    accept = accept && CT::Mask<size_t>::is_lte(offset, input.size()).as_choice();

    /*
     * If the input is invalid, then set offset == input_length
     */
    offset = CT::Mask<size_t>::from_choice(accept).select(offset, input.size());

    /*
     * Move the desired output bytes to the front using a slow (O^n)
     * but constant time loop that does not leak the value of the offset
     */
    for (size_t i = 0; i != input.size(); ++i) {
        /*
         * If bad_input was set then we modified offset to equal the input_length.
         * In that case, this_loop will be greater than input_length, and so is_eq
         * mask will always be false. As a result none of the input values will be
         * written to output.
         *
         * This is ignoring the possibility of integer overflow of offset + i. But
         * for this to happen the input would have to consume nearly the entire
         * address space.
         */
        const size_t this_loop = offset + i;

        /*
        start index from i rather than 0 since we know j must be >= i + offset
        to have any effect, and starting from i does not reveal information
        */
        for (size_t j = i; j != input.size(); ++j) {
            const uint8_t b = input[j];
            const auto is_eq = CT::Mask<size_t>::is_equal(j, this_loop);
            output[i] |= is_eq.if_set_return(b);
        }
    }

    // This will always be zero if the input was invalid
    const size_t output_bytes = input.size() - offset;

    CT::unpoison_all(output, output_bytes);

    return CT::Option<size_t>(output_bytes, accept);
}

size_t CT::count_leading_zero_bytes(std::span<const uint8_t> input) {
    size_t leading_zeros = 0;
    auto only_zeros = Mask<uint8_t>::set();
    for (size_t i = 0; i != input.size(); ++i) {
        only_zeros &= CT::Mask<uint8_t>::is_zero(input[i]);
        leading_zeros += only_zeros.if_set_return(1);
    }
    return leading_zeros;
}

secure_vector<uint8_t> CT::strip_leading_zeros(std::span<const uint8_t> input) {
    const size_t leading_zeros = CT::count_leading_zero_bytes(input);

    secure_vector<uint8_t> output(input.size());

    const auto written = CT::copy_output(CT::Choice::yes(), output, input, leading_zeros);

    /*
    This is potentially not const time, depending on how std::vector is
    implemented. But since we are always reducing length, it should
    just amount to setting the member var holding the length.
    */
    output.resize(written.value_or(0));

    return output;
}

}    // namespace Botan
/*
 * DataSource
 * (C) 1999-2007 Jack Lloyd
 *     2005 Matthew Gregan
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
#include <fstream>
#endif

namespace Botan {

/*
 * Read a single byte from the DataSource
 */
size_t DataSource::read_byte(uint8_t& out) { return read(&out, 1); }

/*
 * Peek a single byte from the DataSource
 */
size_t DataSource::peek_byte(uint8_t& out) const { return peek(&out, 1, 0); }

/*
 * Discard the next N bytes of the data
 */
size_t DataSource::discard_next(size_t n) {
    uint8_t buf[64] = {0};
    size_t discarded = 0;

    while (n) {
        const size_t got = this->read(buf, std::min(n, sizeof(buf)));
        discarded += got;
        n -= got;

        if (got == 0) {
            break;
        }
    }

    return discarded;
}

/*
 * Read from a memory buffer
 */
size_t DataSource_Memory::read(uint8_t out[], size_t length) {
    const size_t got = std::min<size_t>(m_source.size() - m_offset, length);
    copy_mem(out, m_source.data() + m_offset, got);
    m_offset += got;
    return got;
}

bool DataSource_Memory::check_available(size_t n) { return (n <= (m_source.size() - m_offset)); }

/*
 * Peek into a memory buffer
 */
size_t DataSource_Memory::peek(uint8_t out[], size_t length, size_t peek_offset) const {
    const size_t bytes_left = m_source.size() - m_offset;
    if (peek_offset >= bytes_left) {
        return 0;
    }

    const size_t got = std::min(bytes_left - peek_offset, length);
    copy_mem(out, &m_source[m_offset + peek_offset], got);
    return got;
}

/*
 * Check if the memory buffer is empty
 */
bool DataSource_Memory::end_of_data() const { return (m_offset == m_source.size()); }

/*
 * DataSource_Memory Constructor
 */
DataSource_Memory::DataSource_Memory(std::string_view in)
    : m_source(cast_char_ptr_to_uint8(in.data()), cast_char_ptr_to_uint8(in.data()) + in.length()),
      m_offset(0) {}

/*
 * Read from a stream
 */
size_t DataSource_Stream::read(uint8_t out[], size_t length) {
    m_source.read(cast_uint8_ptr_to_char(out), length);
    if (m_source.bad()) {
        throw Stream_IO_Error("DataSource_Stream::read: Source failure");
    }

    const size_t got = static_cast<size_t>(m_source.gcount());
    m_total_read += got;
    return got;
}

bool DataSource_Stream::check_available(size_t n) {
    const std::streampos orig_pos = m_source.tellg();
    m_source.seekg(0, std::ios::end);
    const size_t avail = static_cast<size_t>(m_source.tellg() - orig_pos);
    m_source.seekg(orig_pos);
    return (avail >= n);
}

/*
 * Peek into a stream
 */
size_t DataSource_Stream::peek(uint8_t out[], size_t length, size_t offset) const {
    if (end_of_data()) {
        throw Invalid_State("DataSource_Stream: Cannot peek when out of data");
    }

    size_t got = 0;

    if (offset) {
        secure_vector<uint8_t> buf(offset);
        m_source.read(cast_uint8_ptr_to_char(buf.data()), buf.size());
        if (m_source.bad()) {
            throw Stream_IO_Error("DataSource_Stream::peek: Source failure");
        }
        got = static_cast<size_t>(m_source.gcount());
    }

    if (got == offset) {
        m_source.read(cast_uint8_ptr_to_char(out), length);
        if (m_source.bad()) {
            throw Stream_IO_Error("DataSource_Stream::peek: Source failure");
        }
        got = static_cast<size_t>(m_source.gcount());
    }

    if (m_source.eof()) {
        m_source.clear();
    }
    m_source.seekg(m_total_read, std::ios::beg);

    return got;
}

/*
 * Check if the stream is empty or in error
 */
bool DataSource_Stream::end_of_data() const { return (!m_source.good()); }

/*
 * Return a human-readable ID for this stream
 */
std::string DataSource_Stream::id() const { return m_identifier; }

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)

/*
 * DataSource_Stream Constructor
 */
DataSource_Stream::DataSource_Stream(std::string_view path, bool use_binary)
    : m_identifier(path),
      m_source_memory(std::make_unique<std::ifstream>(
          std::string(path), use_binary ? std::ios::binary : std::ios::in)),
      m_source(*m_source_memory),
      m_total_read(0) {
    if (!m_source.good()) {
        throw Stream_IO_Error(fmt("DataSource: Failure opening file '{}'", path));
    }
}

#endif

/*
 * DataSource_Stream Constructor
 */
DataSource_Stream::DataSource_Stream(std::istream& in, std::string_view name)
    : m_identifier(name), m_source(in), m_total_read(0) {}

DataSource_Stream::~DataSource_Stream() = default;

}    // namespace Botan
/*
 * (C) 2017 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

std::string to_string(ErrorType type) {
    switch (type) {
        case ErrorType::Unknown:
            return "Unknown";
        case ErrorType::SystemError:
            return "SystemError";
        case ErrorType::NotImplemented:
            return "NotImplemented";
        case ErrorType::OutOfMemory:
            return "OutOfMemory";
        case ErrorType::InternalError:
            return "InternalError";
        case ErrorType::IoError:
            return "IoError";
        case ErrorType::InvalidObjectState:
            return "InvalidObjectState";
        case ErrorType::KeyNotSet:
            return "KeyNotSet";
        case ErrorType::InvalidArgument:
            return "InvalidArgument";
        case ErrorType::InvalidKeyLength:
            return "InvalidKeyLength";
        case ErrorType::InvalidNonceLength:
            return "InvalidNonceLength";
        case ErrorType::LookupError:
            return "LookupError";
        case ErrorType::EncodingFailure:
            return "EncodingFailure";
        case ErrorType::DecodingFailure:
            return "DecodingFailure";
        case ErrorType::TLSError:
            return "TLSError";
        case ErrorType::HttpError:
            return "HttpError";
        case ErrorType::InvalidTag:
            return "InvalidTag";
        case ErrorType::RoughtimeError:
            return "RoughtimeError";
        case ErrorType::CommonCryptoError:
            return "CommonCryptoError";
        case ErrorType::Pkcs11Error:
            return "Pkcs11Error";
        case ErrorType::TPMError:
            return "TPMError";
        case ErrorType::DatabaseError:
            return "DatabaseError";
        case ErrorType::ZlibError:
            return "ZlibError";
        case ErrorType::Bzip2Error:
            return "Bzip2Error";
        case ErrorType::LzmaError:
            return "LzmaError";
    }

    // No default case in above switch so compiler warns
    return "Unrecognized Botan error";
}

Exception::Exception(std::string_view msg) : m_msg(msg) {}

Exception::Exception(std::string_view msg, const std::exception& e)
    : m_msg(fmt("{} failed with {}", msg, e.what())) {}

Exception::Exception(const char* prefix, std::string_view msg) : m_msg(fmt("{} {}", prefix, msg)) {}

Invalid_Argument::Invalid_Argument(std::string_view msg) : Exception(msg) {}

Invalid_Argument::Invalid_Argument(std::string_view msg, std::string_view where)
    : Exception(fmt("{} in {}", msg, where)) {}

Invalid_Argument::Invalid_Argument(std::string_view msg, const std::exception& e)
    : Exception(msg, e) {}

namespace {

std::string format_lookup_error(std::string_view type, std::string_view algo,
                                std::string_view provider) {
    if (provider.empty()) {
        return fmt("Unavailable {} {}", type, algo);
    } else {
        return fmt("Unavailable {} {} for provider {}", type, algo, provider);
    }
}

}    // namespace

Lookup_Error::Lookup_Error(std::string_view type, std::string_view algo, std::string_view provider)
    : Exception(format_lookup_error(type, algo, provider)) {}

Internal_Error::Internal_Error(std::string_view err) : Exception("Internal error:", err) {}

Unknown_PK_Field_Name::Unknown_PK_Field_Name(std::string_view algo_name,
                                             std::string_view field_name)
    : Invalid_Argument(fmt("Unknown field '{}' for algorithm {}", field_name, algo_name)) {}

Invalid_Key_Length::Invalid_Key_Length(std::string_view name, size_t length)
    : Invalid_Argument(fmt("{} cannot accept a key of length {}", name, length)) {}

Invalid_IV_Length::Invalid_IV_Length(std::string_view mode, size_t bad_len)
    : Invalid_Argument(fmt("IV length {} is invalid for {}", bad_len, mode)) {}

Key_Not_Set::Key_Not_Set(std::string_view algo) : Invalid_State(fmt("Key not set in {}", algo)) {}

PRNG_Unseeded::PRNG_Unseeded(std::string_view algo)
    : Invalid_State(fmt("PRNG {} not seeded", algo)) {}

Algorithm_Not_Found::Algorithm_Not_Found(std::string_view name)
    : Lookup_Error(fmt("Could not find any algorithm named '{}'", name)) {}

Provider_Not_Found::Provider_Not_Found(std::string_view algo, std::string_view provider)
    : Lookup_Error(fmt("Could not find provider '{}' for algorithm '{}'", provider, algo)) {}

Invalid_Algorithm_Name::Invalid_Algorithm_Name(std::string_view name)
    : Invalid_Argument(fmt("Invalid algorithm name: '{}'", name)) {}

Encoding_Error::Encoding_Error(std::string_view name) : Exception("Encoding error:", name) {}

Decoding_Error::Decoding_Error(std::string_view name) : Exception(name) {}

Decoding_Error::Decoding_Error(std::string_view category, std::string_view err)
    : Exception(fmt("{}: {}", category, err)) {}

Decoding_Error::Decoding_Error(std::string_view msg, const std::exception& e) : Exception(msg, e) {}

Invalid_Authentication_Tag::Invalid_Authentication_Tag(std::string_view msg)
    : Exception("Invalid authentication tag:", msg) {}

Stream_IO_Error::Stream_IO_Error(std::string_view err) : Exception("I/O error:", err) {}

System_Error::System_Error(std::string_view msg, int err_code)
    : Exception(fmt("{} error code {}", msg, err_code)), m_error_code(err_code) {}

Not_Implemented::Not_Implemented(std::string_view err) : Exception("Not implemented", err) {}

}    // namespace Botan
/*
 * (C) 2015,2017,2019 Jack Lloyd
 * (C) 2015 Simon Warta (Kullo GmbH)
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include <deque>

#if defined(BOTAN_TARGET_OS_HAS_POSIX1)
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <functional>
#elif defined(BOTAN_TARGET_OS_HAS_WIN32)
#define NOMINMAX 1
#define _WINSOCKAPI_    // stop windows.h including winsock.h
#include <windows.h>
#endif

namespace Botan {

namespace {

#if defined(BOTAN_TARGET_OS_HAS_POSIX1)

std::vector<std::string> impl_readdir(std::string_view dir_path) {
    std::vector<std::string> out;
    std::deque<std::string> dir_list;
    dir_list.push_back(std::string(dir_path));

    while (!dir_list.empty()) {
        const std::string cur_path = dir_list[0];
        dir_list.pop_front();

        std::unique_ptr<DIR, std::function<int(DIR*)>> dir(::opendir(cur_path.c_str()), ::closedir);

        if (dir) {
            while (struct dirent* dirent = ::readdir(dir.get())) {
                const std::string filename = dirent->d_name;
                if (filename == "." || filename == "..") {
                    continue;
                }

                std::ostringstream full_path_sstr;
                full_path_sstr << cur_path << "/" << filename;
                const std::string full_path = full_path_sstr.str();

                struct stat stat_buf;

                if (::stat(full_path.c_str(), &stat_buf) == -1) {
                    continue;
                }

                if (S_ISDIR(stat_buf.st_mode)) {
                    dir_list.push_back(full_path);
                } else if (S_ISREG(stat_buf.st_mode)) {
                    out.push_back(full_path);
                }
            }
        }
    }

    return out;
}

#elif defined(BOTAN_TARGET_OS_HAS_WIN32)

std::vector<std::string> impl_win32(std::string_view dir_path) {
    std::vector<std::string> out;
    std::deque<std::string> dir_list;
    dir_list.push_back(std::string(dir_path));

    while (!dir_list.empty()) {
        const std::string cur_path = dir_list[0];
        dir_list.pop_front();

        WIN32_FIND_DATAA find_data;
        HANDLE dir = ::FindFirstFileA((cur_path + "/*").c_str(), &find_data);

        if (dir != INVALID_HANDLE_VALUE) {
            do {
                const std::string filename = find_data.cFileName;
                if (filename == "." || filename == "..") continue;
                const std::string full_path = cur_path + "/" + filename;

                if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    dir_list.push_back(full_path);
                } else {
                    out.push_back(full_path);
                }
            } while (::FindNextFileA(dir, &find_data));
        }

        ::FindClose(dir);
    }

    return out;
}
#endif

}    // namespace

bool has_filesystem_impl() {
#if defined(BOTAN_TARGET_OS_HAS_POSIX1)
    return true;
#elif defined(BOTAN_TARGET_OS_HAS_WIN32)
    return true;
#else
    return false;
#endif
}

std::vector<std::string> get_files_recursive(std::string_view dir) {
    std::vector<std::string> files;

#if defined(BOTAN_TARGET_OS_HAS_POSIX1)
    files = impl_readdir(dir);
#elif defined(BOTAN_TARGET_OS_HAS_WIN32)
    files = impl_win32(dir);
#else
    BOTAN_UNUSED(dir);
    throw No_Filesystem_Access();
#endif

    std::sort(files.begin(), files.end());

    return files;
}

}    // namespace Botan
/*
 * (C) 2017 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

uint8_t ct_compare_u8(const uint8_t x[], const uint8_t y[], size_t len) {
    return CT::is_equal(x, y, len).value();
}

bool constant_time_compare(std::span<const uint8_t> x, std::span<const uint8_t> y) {
    const auto min_size = CT::Mask<size_t>::is_lte(x.size(), y.size()).select(x.size(), y.size());
    const auto equal_size = CT::Mask<size_t>::is_equal(x.size(), y.size());
    const auto equal_content = CT::Mask<size_t>::expand(CT::is_equal(x.data(), y.data(), min_size));
    return (equal_content & equal_size).as_bool();
}

}    // namespace Botan
/*
 * (C) 2025 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include <cstring>

#if defined(BOTAN_TARGET_OS_HAS_EXPLICIT_BZERO)
#include <string.h>
#endif

#if defined(BOTAN_TARGET_OS_HAS_RTLSECUREZEROMEMORY)
#define NOMINMAX 1
#define _WINSOCKAPI_    // stop windows.h including winsock.h
#include <windows.h>
#endif

namespace Botan {

void secure_scrub_memory(void* ptr, size_t n) {
#if defined(BOTAN_TARGET_OS_HAS_RTLSECUREZEROMEMORY)
    ::RtlSecureZeroMemory(ptr, n);

#elif defined(BOTAN_TARGET_OS_HAS_EXPLICIT_BZERO)
    ::explicit_bzero(ptr, n);

#elif defined(BOTAN_TARGET_OS_HAS_EXPLICIT_MEMSET)
    (void)::explicit_memset(ptr, 0, n);

#else
    /*
     * Call memset through a static volatile pointer, which the compiler should
     * not elide. This construct should be safe in conforming compilers, but who
     * knows. This has been checked to generate the expected code, which saves the
     * memset address in the data segment and unconditionally loads and jumps to
     * that address, with the following targets:
     *
     * x86-64: Clang 19, GCC 6, 11, 13, 14
     * riscv64: GCC 14
     * aarch64: GCC 14
     * armv7: GCC 14
     *
     * Actually all of them generated the expected jump even without marking the
     * function pointer as volatile. However this seems worth including as an
     * additional precaution.
     */
    static void* (*const volatile memset_ptr)(void*, int, size_t) = std::memset;
    (memset_ptr)(ptr, 0, n);
#endif
}

}    // namespace Botan
/*
 * Various string utils and parsing functions
 * (C) 1999-2007,2013,2014,2015,2018 Jack Lloyd
 * (C) 2015 Simon Warta (Kullo GmbH)
 * (C) 2017 René Korthaus, Rohde & Schwarz Cybersecurity
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

#include <cctype>
#include <limits>

namespace Botan {

uint16_t to_uint16(std::string_view str) {
    const uint32_t x = to_u32bit(str);

    if (x >> 16) {
        throw Invalid_Argument("Integer value exceeds 16 bit range");
    }

    return static_cast<uint16_t>(x);
}

uint32_t to_u32bit(std::string_view str_view) {
    const std::string str(str_view);

    // std::stoul is not strict enough. Ensure that str is digit only [0-9]*
    for (const char chr : str) {
        if (chr < '0' || chr > '9') {
            throw Invalid_Argument("to_u32bit invalid decimal string '" + str + "'");
        }
    }

    const unsigned long int x = std::stoul(str);

    if constexpr (sizeof(unsigned long int) > 4) {
        // x might be uint64
        if (x > std::numeric_limits<uint32_t>::max()) {
            throw Invalid_Argument("Integer value of " + str + " exceeds 32 bit range");
        }
    }

    return static_cast<uint32_t>(x);
}

/*
 * Parse a SCAN-style algorithm name
 */
std::vector<std::string> parse_algorithm_name(std::string_view namex) {
    if (namex.find('(') == std::string::npos && namex.find(')') == std::string::npos) {
        return {std::string(namex)};
    }

    std::string name(namex);
    std::string substring;
    std::vector<std::string> elems;
    size_t level = 0;

    elems.push_back(name.substr(0, name.find('(')));
    name = name.substr(name.find('('));

    for (auto i = name.begin(); i != name.end(); ++i) {
        char c = *i;

        if (c == '(') {
            ++level;
        }
        if (c == ')') {
            if (level == 1 && i == name.end() - 1) {
                if (elems.size() == 1) {
                    elems.push_back(substring.substr(1));
                } else {
                    elems.push_back(substring);
                }
                return elems;
            }

            if (level == 0 || (level == 1 && i != name.end() - 1)) {
                throw Invalid_Algorithm_Name(namex);
            }
            --level;
        }

        if (c == ',' && level == 1) {
            if (elems.size() == 1) {
                elems.push_back(substring.substr(1));
            } else {
                elems.push_back(substring);
            }
            substring.clear();
        } else {
            substring += c;
        }
    }

    if (!substring.empty()) {
        throw Invalid_Algorithm_Name(namex);
    }

    return elems;
}

std::vector<std::string> split_on(std::string_view str, char delim) {
    std::vector<std::string> elems;
    if (str.empty()) {
        return elems;
    }

    std::string substr;
    for (auto i = str.begin(); i != str.end(); ++i) {
        if (*i == delim) {
            if (!substr.empty()) {
                elems.push_back(substr);
            }
            substr.clear();
        } else {
            substr += *i;
        }
    }

    if (substr.empty()) {
        throw Invalid_Argument(fmt("Unable to split string '{}", str));
    }
    elems.push_back(substr);

    return elems;
}

/*
 * Join a string
 */
std::string string_join(const std::vector<std::string>& strs, char delim) {
    std::ostringstream out;

    for (size_t i = 0; i != strs.size(); ++i) {
        if (i != 0) {
            out << delim;
        }
        out << strs[i];
    }

    return out.str();
}

/*
 * Convert a decimal-dotted string to binary IP
 */
std::optional<uint32_t> string_to_ipv4(std::string_view str) {
    // At least 3 dots + 4 1-digit integers
    // At most 3 dots + 4 3-digit integers
    if (str.size() < 3 + 4 * 1 || str.size() > 3 + 4 * 3) {
        return {};
    }

    // the final result
    uint32_t ip = 0;
    // the number of '.' seen so far
    size_t dots = 0;
    // accumulates one quad (range 0-255)
    uint32_t accum = 0;
    // # of digits pushed to accum since last dot
    size_t cur_digits = 0;

    for (char c : str) {
        if (c == '.') {
            // . without preceding digit is invalid
            if (cur_digits == 0) {
                return {};
            }
            dots += 1;
            // too many dots
            if (dots > 3) {
                return {};
            }

            cur_digits = 0;
            ip = (ip << 8) | accum;
            accum = 0;
        } else if (c >= '0' && c <= '9') {
            const auto d = static_cast<uint8_t>(c - '0');

            // prohibit leading zero in quad (used for octal)
            if (cur_digits > 0 && accum == 0) {
                return {};
            }
            accum = (accum * 10) + d;

            if (accum > 255) {
                return {};
            }

            cur_digits++;
            BOTAN_ASSERT_NOMSG(cur_digits <= 3);
        } else {
            return {};
        }
    }

    // no trailing digits?
    if (cur_digits == 0) {
        return {};
    }

    // insufficient # of dots
    if (dots != 3) {
        return {};
    }

    ip = (ip << 8) | accum;

    return ip;
}

/*
 * Convert an IP address to decimal-dotted string
 */
std::string ipv4_to_string(uint32_t ip) {
    uint8_t bits[4];
    store_be(ip, bits);

    std::string str;

    for (size_t i = 0; i != 4; ++i) {
        if (i > 0) {
            str += ".";
        }
        str += std::to_string(bits[i]);
    }

    return str;
}

std::string tolower_string(std::string_view in) {
    std::string s(in);
    for (size_t i = 0; i != s.size(); ++i) {
        const int cu = static_cast<unsigned char>(s[i]);
        if (std::isalpha(cu)) {
            s[i] = static_cast<char>(std::tolower(cu));
        }
    }
    return s;
}

bool host_wildcard_match(std::string_view issued_, std::string_view host_) {
    const std::string issued = tolower_string(issued_);
    const std::string host = tolower_string(host_);

    if (host.empty() || issued.empty()) {
        return false;
    }

    /*
    If there are embedded nulls in your issued name
    Well I feel bad for you son
    */
    if (std::count(issued.begin(), issued.end(), char(0)) > 0) {
        return false;
    }

    // If more than one wildcard, then issued name is invalid
    const size_t stars = std::count(issued.begin(), issued.end(), '*');
    if (stars > 1) {
        return false;
    }

    // '*' is not a valid character in DNS names so should not appear on the host side
    if (std::count(host.begin(), host.end(), '*') != 0) {
        return false;
    }

    // Similarly a DNS name can't end in .
    if (host[host.size() - 1] == '.') {
        return false;
    }

    // And a host can't have an empty name component, so reject that
    if (host.find("..") != std::string::npos) {
        return false;
    }

    // Exact match: accept
    if (issued == host) {
        return true;
    }

    /*
    Otherwise it might be a wildcard

    If the issued size is strictly longer than the hostname size it
    couldn't possibly be a match, even if the issued value is a
    wildcard. The only exception is when the wildcard ends up empty
    (eg www.example.com matches www*.example.com)
    */
    if (issued.size() > host.size() + 1) {
        return false;
    }

    // If no * at all then not a wildcard, and so not a match
    if (stars != 1) {
        return false;
    }

    /*
    Now walk through the issued string, making sure every character
    matches. When we come to the (singular) '*', jump forward in the
    hostname by the corresponding amount. We know exactly how much
    space the wildcard takes because it must be exactly `len(host) -
    len(issued) + 1 chars`.

    We also verify that the '*' comes in the leftmost component, and
    doesn't skip over any '.' in the hostname.
    */
    size_t dots_seen = 0;
    size_t host_idx = 0;

    for (size_t i = 0; i != issued.size(); ++i) {
        dots_seen += (issued[i] == '.');

        if (issued[i] == '*') {
            // Fail: wildcard can only come in leftmost component
            if (dots_seen > 0) {
                return false;
            }

            /*
            Since there is only one * we know the tail of the issued and
            hostname must be an exact match. In this case advance host_idx
            to match.
            */
            const size_t advance = (host.size() - issued.size() + 1);

            if (host_idx + advance > host.size()) {    // shouldn't happen
                return false;
            }

            // Can't be any intervening .s that we would have skipped
            if (std::count(host.begin() + host_idx, host.begin() + host_idx + advance, '.') != 0) {
                return false;
            }

            host_idx += advance;
        } else {
            if (issued[i] != host[host_idx]) {
                return false;
            }

            host_idx += 1;
        }
    }

    // Wildcard issued name must have at least 3 components
    if (dots_seen < 2) {
        return false;
    }

    return true;
}

std::string check_and_canonicalize_dns_name(std::string_view name) {
    if (name.size() > 255) {
        throw Decoding_Error("DNS name exceeds maximum allowed length");
    }

    if (name.empty()) {
        throw Decoding_Error("DNS name cannot be empty");
    }

    if (name.starts_with(".")) {
        throw Decoding_Error("DNS name cannot start with a dot");
    }

    /*
     * Table mapping uppercase to lowercase and only including values for valid DNS names
     * namely A-Z, a-z, 0-9, hypen, and dot, plus '*' for wildcarding.
     */
    // clang-format off
   constexpr uint8_t DNS_CHAR_MAPPING[128] = {
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
      '\0', '\0', '\0', '\0',  '*', '\0', '\0',  '-',  '.', '\0',  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',
       '9', '\0', '\0', '\0', '\0', '\0', '\0', '\0',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',
       'l',  'm',  'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z', '\0', '\0', '\0', '\0',
      '\0', '\0',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',  'p',  'q',
       'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z', '\0', '\0', '\0', '\0', '\0',
   };
    // clang-format on

    std::string canon;
    canon.reserve(name.size());

    for (size_t i = 0; i != name.size(); ++i) {
        char c = name[i];

        if (c == '.') {
            if (name[i - 1] == '.') {
                throw Decoding_Error("DNS name contains sequential period chars");
            }
            if (i == name.size() - 1) {
                throw Decoding_Error("DNS name cannot end in a period");
            }
        }

        const uint8_t cu = static_cast<uint8_t>(c);
        if (cu >= 128) {
            throw Decoding_Error("DNS name must not contain any extended ASCII code points");
        }
        const uint8_t mapped = DNS_CHAR_MAPPING[cu];
        if (mapped == 0) {
            throw Decoding_Error("DNS name includes invalid character");
        }
        // TODO check label lengths
        canon.push_back(static_cast<char>(mapped));
    }

    return canon;
}

}    // namespace Botan
/*
 * (C) 2023 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

uint64_t prefetch_array_raw(size_t bytes, const void* arrayv) noexcept {
#if defined(__cpp_lib_hardware_interference_size)
    const size_t cache_line_size = std::hardware_destructive_interference_size;
#else
    // We arbitrarily use a 64 byte cache line, which is by far the most
    // common size.
    //
    // Runtime detection adds too much overhead to this function.
    const size_t cache_line_size = 64;
#endif

    const uint8_t* array = static_cast<const uint8_t*>(arrayv);

    volatile uint64_t combiner = 1;

    for (size_t idx = 0; idx < bytes; idx += cache_line_size) {
#if BOTAN_COMPILER_HAS_BUILTIN(__builtin_prefetch)
        // we have no way of knowing if the compiler will emit anything here
        __builtin_prefetch(&array[idx]);
#endif

        combiner = combiner | array[idx];
    }

    /*
     * The combiner variable is initialized with 1, and we accumulate using OR, so
     * now combiner must be a value other than zero. This being the case we will
     * always return zero here. Hopefully the compiler will not figure this out.
     */
    return ct_is_zero(combiner);
}

}    // namespace Botan
/*
 * Simple config/test file reader
 * (C) 2013,2014,2015 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

namespace {

std::string clean_ws(std::string_view s) {
    const char* ws = " \t\n";
    auto start = s.find_first_not_of(ws);
    auto end = s.find_last_not_of(ws);

    if (start == std::string::npos) {
        return "";
    }

    if (end == std::string::npos) {
        return std::string(s.substr(start, end));
    } else {
        return std::string(s.substr(start, start + end + 1));
    }
}

}    // namespace

std::map<std::string, std::string> read_cfg(std::istream& is) {
    std::map<std::string, std::string> kv;
    size_t line = 0;

    while (is.good()) {
        std::string s;

        std::getline(is, s);

        ++line;

        if (s.empty() || s[0] == '#') {
            continue;
        }

        s = clean_ws(s.substr(0, s.find('#')));

        if (s.empty()) {
            continue;
        }

        auto eq = s.find('=');

        if (eq == std::string::npos || eq == 0 || eq == s.size() - 1) {
            throw Decoding_Error("Bad read_cfg input '" + s + "' on line " + std::to_string(line));
        }

        const std::string key = clean_ws(s.substr(0, eq));
        const std::string val = clean_ws(s.substr(eq + 1, std::string::npos));

        kv[key] = val;
    }

    return kv;
}

}    // namespace Botan
/*
 * (C) 2018 Ribose Inc
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

std::map<std::string, std::string> read_kv(std::string_view kv) {
    std::map<std::string, std::string> m;
    if (kv.empty()) {
        return m;
    }

    std::vector<std::string> parts;

    try {
        parts = split_on(kv, ',');
    } catch (std::exception&) {
        throw Invalid_Argument("Bad KV spec");
    }

    bool escaped = false;
    bool reading_key = true;
    std::string cur_key;
    std::string cur_val;

    for (char c : kv) {
        if (c == '\\' && !escaped) {
            escaped = true;
        } else if (c == ',' && !escaped) {
            if (cur_key.empty()) {
                throw Invalid_Argument("Bad KV spec empty key");
            }

            if (m.find(cur_key) != m.end()) {
                throw Invalid_Argument("Bad KV spec duplicated key");
            }
            m[cur_key] = cur_val;
            cur_key = "";
            cur_val = "";
            reading_key = true;
        } else if (c == '=' && !escaped) {
            if (reading_key == false) {
                throw Invalid_Argument("Bad KV spec unexpected equals sign");
            }
            reading_key = false;
        } else {
            if (reading_key) {
                cur_key += c;
            } else {
                cur_val += c;
            }

            if (escaped) {
                escaped = false;
            }
        }
    }

    if (!cur_key.empty()) {
        if (reading_key == false) {
            if (m.find(cur_key) != m.end()) {
                throw Invalid_Argument("Bad KV spec duplicated key");
            }
            m[cur_key] = cur_val;
        } else {
            throw Invalid_Argument("Bad KV spec incomplete string");
        }
    }

    return m;
}

}    // namespace Botan
/*
 * SCAN Name Abstraction
 * (C) 2008-2009,2015 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

namespace {

std::string make_arg(const std::vector<std::pair<size_t, std::string>>& name, size_t start) {
    std::string output = name[start].second;
    size_t level = name[start].first;

    size_t paren_depth = 0;

    for (size_t i = start + 1; i != name.size(); ++i) {
        if (name[i].first <= name[start].first) {
            break;
        }

        if (name[i].first > level) {
            output += "(" + name[i].second;
            ++paren_depth;
        } else if (name[i].first < level) {
            for (size_t j = name[i].first; j < level; j++) {
                output += ")";
                --paren_depth;
            }
            output += "," + name[i].second;
        } else {
            if (output[output.size() - 1] != '(') {
                output += ",";
            }
            output += name[i].second;
        }

        level = name[i].first;
    }

    for (size_t i = 0; i != paren_depth; ++i) {
        output += ")";
    }

    return output;
}

}    // namespace

SCAN_Name::SCAN_Name(const char* algo_spec) : SCAN_Name(std::string(algo_spec)) {}

SCAN_Name::SCAN_Name(std::string_view algo_spec)
    : m_orig_algo_spec(algo_spec), m_alg_name(), m_args(), m_mode_info() {
    if (algo_spec.empty()) {
        throw Invalid_Argument("Expected algorithm name, got empty string");
    }

    std::vector<std::pair<size_t, std::string>> name;
    size_t level = 0;
    std::pair<size_t, std::string> accum = std::make_pair(level, "");

    const std::string decoding_error = "Bad SCAN name '" + m_orig_algo_spec + "': ";

    for (char c : algo_spec) {
        if (c == '/' || c == ',' || c == '(' || c == ')') {
            if (c == '(') {
                ++level;
            } else if (c == ')') {
                if (level == 0) {
                    throw Decoding_Error(decoding_error + "Mismatched parens");
                }
                --level;
            }

            if (c == '/' && level > 0) {
                accum.second.push_back(c);
            } else {
                if (!accum.second.empty()) {
                    name.push_back(accum);
                }
                accum = std::make_pair(level, "");
            }
        } else {
            accum.second.push_back(c);
        }
    }

    if (!accum.second.empty()) {
        name.push_back(accum);
    }

    if (level != 0) {
        throw Decoding_Error(decoding_error + "Missing close paren");
    }

    if (name.empty()) {
        throw Decoding_Error(decoding_error + "Empty name");
    }

    m_alg_name = name[0].second;

    bool in_modes = false;

    for (size_t i = 1; i != name.size(); ++i) {
        if (name[i].first == 0) {
            m_mode_info.push_back(make_arg(name, i));
            in_modes = true;
        } else if (name[i].first == 1 && !in_modes) {
            m_args.push_back(make_arg(name, i));
        }
    }
}

std::string SCAN_Name::arg(size_t i) const {
    if (i >= arg_count()) {
        throw Invalid_Argument("SCAN_Name::arg " + std::to_string(i) + " out of range for '" +
                               to_string() + "'");
    }
    return m_args[i];
}

std::string SCAN_Name::arg(size_t i, std::string_view def_value) const {
    if (i >= arg_count()) {
        return std::string(def_value);
    }
    return m_args[i];
}

size_t SCAN_Name::arg_as_integer(size_t i, size_t def_value) const {
    if (i >= arg_count()) {
        return def_value;
    }
    return to_u32bit(m_args[i]);
}

size_t SCAN_Name::arg_as_integer(size_t i) const { return to_u32bit(arg(i)); }

}    // namespace Botan
/*
 * Version Information
 * (C) 1999-2013,2015 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 */

namespace Botan {

const char* short_version_cstr() { return BOTAN_SHORT_VERSION_STRING; }

const char* version_cstr() { return BOTAN_FULL_VERSION_STRING; }

std::string version_string() { return std::string(version_cstr()); }

std::string short_version_string() { return std::string(short_version_cstr()); }

uint32_t version_datestamp() { return BOTAN_VERSION_DATESTAMP; }

std::optional<std::string> version_vc_revision() {
#if defined(BOTAN_VC_REVISION)
    return std::string(BOTAN_VC_REVISION);
#else
    return std::nullopt;
#endif
}

std::optional<std::string> version_distribution_info() {
#if defined(BOTAN_DISTRIBUTION_INFO_STRING)
    return std::string(BOTAN_DISTRIBUTION_INFO_STRING);
#else
    return std::nullopt;
#endif
}

/*
 * Return parts of the version as integers
 */
uint32_t version_major() { return BOTAN_VERSION_MAJOR; }

uint32_t version_minor() { return BOTAN_VERSION_MINOR; }

uint32_t version_patch() { return BOTAN_VERSION_PATCH; }

bool unsafe_for_production_build() {
#if defined(BOTAN_UNSAFE_FUZZER_MODE) || defined(BOTAN_TERMINATE_ON_ASSERTS)
    return true;
#else
    return false;
#endif
}

std::string runtime_version_check(uint32_t major, uint32_t minor, uint32_t patch) {
    if (major != version_major() || minor != version_minor() || patch != version_patch()) {
        return fmt("Warning: linked version ({}) does not match version built against ({}.{}.{})\n",
                   short_version_cstr(), major, minor, patch);
    }

    return "";
}

}    // namespace Botan
