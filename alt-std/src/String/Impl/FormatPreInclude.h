#pragma once

#ifndef __ALT_STD_FORMAT_PRE_INCLUDE__
#define __ALT_STD_FORMAT_PRE_INCLUDE__

#include "../../TypeTraits.h"

#include "../../Utils/Optional.h"
#include "../../Utils/Slice.h"

namespace Alt {

template <typename T>
class Format;

class FormatEstimator;

template <typename SelfT>
class FormatWriter;

/**
 * @brief Holds the pre-calculated size of a formatted output.
 * @details This structure is used during the estimation phase to determine how many bytes 
 * must be reserved in the buffer for pre-allocation.
 */
struct EstimateExactSizeResult {

    usize size = 0;
};

/**\cond */
namespace _FormatDetails {

template <typename T, typename FormatWriterT, typename = void>
struct __has_format_write_append__
    : std::false_type
{};

template <typename T, typename FormatWriterT>
struct __has_format_write_append__<
    T,
    FormatWriterT,
    std::void_t<
        std::is_same<
            decltype(Format<T>::WriteAppend(std::declval<FormatWriter<FormatWriterT>&>(), std::declval<const Slice<const char>&>(), std::declval<const T&>())),
            bool
        >
    >
>
    : std::true_type
{};

template <typename T, typename FormatWriterT>
struct __is_formattable__
    : __has_format_write_append__<T, FormatWriterT>
{};

template <typename T, typename = void>
struct __has_format_estimated_exact_size__
    : std::false_type
{};

template <typename T>
struct __has_format_estimated_exact_size__<
    T,
    std::void_t<
        std::is_same<
            decltype(Format<T>::EstimateExactSize(std::declval<FormatEstimator&>(), std::declval<Slice<const char>>(), std::declval<const T&>())),
            Optional<EstimateExactSizeResult>
        >
    >
>
    : std::true_type
{};

template <typename T>
struct __is_format_estimatable__
    : __has_format_estimated_exact_size__<T>
{};

template <typename T>
using clean_t = TypeTraits::clean_t<std::remove_reference_t<T>>;

} // namespace _FormatDetails
/**\endcond */

namespace TypeTraits {

/**
 * @brief Check if type T is formattable - implements `Format<SomeType>::WriteAppend(...)`.
 */
template <typename T, typename FormatWriterT>
using is_formattable_t = _FormatDetails::__is_formattable__<_FormatDetails::clean_t<T>, FormatWriterT>;

template <typename T, typename FormatWriterT>
constexpr static bool is_formattable_v = is_formattable_t<T, FormatWriterT>::value;

/**
 * @brief Check if type T's size can be estimated - implements `Format<SomeType>::EstimateExactSize(...)`.
 */
template <typename T>
using is_format_estimatable_t = _FormatDetails::__is_format_estimatable__<_FormatDetails::clean_t<T>>;

template <typename T>
constexpr static bool is_format_estimatable_v = is_format_estimatable_t<T>::value;

} // namespace TypeTraits

/**
 * @brief A helper which maintains indentation - used in 'WriteAppend(...)' and 'EstimateExactSize(...)'.
 */
class FormatEstimator {
public:

    ALT_FORCE_INLINE void PushIndent() noexcept {
        m_IndentScope += 1;
    }

    ALT_FORCE_INLINE void PopIndent() noexcept {
        m_IndentScope -= 1;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetIndentEstimatedSize(const ssize additionalIndent = 0) const noexcept {

        const usize indentScope = static_cast<usize>(Num::Max(m_IndentScope + additionalIndent, 0));
        return indentScope * 4;
    }

protected:

    ssize m_IndentScope = 0;
};

} // namespace Alt

#endif // __ALT_STD_FORMAT_PRE_INCLUDE__