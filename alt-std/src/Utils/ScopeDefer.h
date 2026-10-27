#pragma once

#ifndef __ALT_STD_SCOPE_DEFER__
#define __ALT_STD_SCOPE_DEFER__

#include "../TypeTraits.h"

/**
 * @brief Defer a function/lambda until the end of the scope.
 * 
 * @tparam FuncT The type of the callable. Must be invokable without arguments.
 * @example
 * 
 * // Start scope
 * {
 *     Object* object = new Object();
 *     defer{ delete object };
 * 
 *     int* array_ = new int[10];
 *     defer{ delete[] array_ };
 * 
 *     ...
 * 
 *     // By the end of scope all defer{ ... } will be called: (delete object; delete[] array_;)
 * }
 * 
 * @brief Defer a function/lambda until the end of the scope. Can be disabled.
 * 
 * @tparam FuncT The type of the callable. Must be invokable without arguments.
 * @example
 * 
 * // Start scope
 * {
 *     Object* object = new Object();
 *     auto cleanup0 = defer_disable{ delete object };
 * 
 *     int* array_ = new int[10];
 *     auto cleanup1 = defer_disable{ delete[] array_ };
 * 
 *     ...
 * 
 *     delete object;
 * 
 *     cleanup0.Disable();
 * 
 *     // By the end of scope only second defer_disable{ ... } will be called: (delete[] array_;)
 * }
 */
namespace Alt {
namespace _ScopeDeferDetails {

template <typename FuncT>
struct ScopeDefer {
    ScopeDefer(FuncT func_) noexcept : func(func_) {}
    ~ScopeDefer() { (func)(); }
    FuncT func;
};

struct ScopeDeferUnit
{};

template <typename FuncT>
ALT_FORCE_INLINE ScopeDefer<FuncT> operator+(ScopeDeferUnit, FuncT&& func) noexcept {
    return ScopeDefer<FuncT>(std::forward<FuncT>(func));
}

template <typename FuncT>
struct ScopeDeferDisable {
    ScopeDeferDisable(const ScopeDeferDisable&) = delete;
    ScopeDeferDisable(ScopeDeferDisable&&) = delete;
    ScopeDeferDisable& operator=(const ScopeDeferDisable&) = delete;
    ScopeDeferDisable& operator=(ScopeDeferDisable&&) = delete;
    ScopeDeferDisable(FuncT func_) noexcept : m_Func(func_) {}
    ~ScopeDeferDisable() { if (!m_IsDisabled) (m_Func)(); }
    ALT_FORCE_INLINE void Disable() { m_IsDisabled = true; }
private:
    FuncT m_Func;
    bool m_IsDisabled = false;
};

struct ScopeDeferDisableUnit
{};

template <typename FuncT>
ALT_NODISCARD ALT_FORCE_INLINE ScopeDeferDisable<FuncT> operator+(ScopeDeferDisableUnit, FuncT&& func) noexcept {
    return ScopeDeferDisable<FuncT>(std::forward<FuncT>(func));
}

} // namespace _ScopeDeferDetails
} // namespace Alt

#define defer auto __ALT_STD_CONCAT_MACROS__(_scope_defer_, __COUNTER__) = Alt::_ScopeDeferDetails::ScopeDeferUnit{} + [&]() mutable
#define defer_disable Alt::_ScopeDeferDetails::ScopeDeferDisableUnit{} + [&]() mutable

#endif // __ALT_STD_SCOPE_DEFER__