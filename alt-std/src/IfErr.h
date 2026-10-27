#pragma once

#ifndef __ALT_STD_IFERR_SCOPE_HANDLER__
#define __ALT_STD_IFERR_SCOPE_HANDLER__

#include "TypeTraits.h"

#include "Memory/Ref.h"

/**
 * @brief Manages the error state within a local scope.
 * 
 * @details Acts as the central state container for error tracking. When used with `iferr_scope`
 * macros, it tracks whether a failure has occurred within the current execution block, allowing
 * for automated control flow changes (break, continue, return) or the execution of error-triggered
 * callbacks via RAII.
 * 
 * @example
 * 
 * bool ParseExample() {
 *     iferr_scope;
 * 
 *     for (;;) {
 * 
 *         const usize val0 = Parse() iferr_break;
 *         const usize val1 = Parse() iferr_continue;
 * 
 *         const bool isEnd = ParseEnd() iferr_return false
 *         if (isEnd) break;
 *     }
 * 
 *     const usize val2 = Parse() iferr_return false;
 * 
 *     return true;
 * }
 *
 * bool ParseExample() {
 * 
 *     iferr_scope_on_err {
 * 
 *         ALT_EPRINTLN("Failed to parse");
 *     };
 * 
 *     for (;;) {
 * 
 *         ifloop_scope_on_defer {
 * 
 *             if (isErr) {
 *                 ALT_EPRINTLN("Skiped");
 *             }
 *         };
 * 
 *         const usize val0 = Parse() ifloop_break;
 *         const usize val1 = Parse() ifloop_continue;
 * 
 *         bool isEnd = ParseEnd() iferr_return false
 *         if (isEnd) break;
 *     }
 * 
 *     Alt::Optional<usize> val2 = Parse();
 *     if (!val2) {
 * 
 *         // use 'iferr_return_return' to trigger lambda in 'iferr_scope_on_err'
 *         iferr_return_return false;
 *     }
 * 
 *     return true;
 * }
 * 
 * bool ParseExample() {
 *     iferr_scope;
 * 
 *     const usize val0 = Parse() iferr_break;
 *     return true;
 * }
 * 
 * bool ParseWithLog() {
 * 
 *     iferr_scope_on_err {
 * 
 *         ALT_EPRINTLN("Failed to parse");
 *         ALT_BREAKPOINT_DEV;
 *     };
 * 
 *     const usize val = Parse() iferr_return; 
 *     return true;
 * }
 * 
 * For clasess which haven't default constructor or this constructor should be private are possible
 * to use "iferr injection" - `ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(...)`, which allows to construct
 * object if error occurs. Injected default constructor used only when we assume that return from execution
 * flow on error (e.g., iferr_return, iferr_panic, ...), for like iferr, ifok, iferr_with the usage
 * of injected default constructor forbitten because we can't garanty that programmer won't to use
 * this default value in further code flow, in this case Optiont<T> will be returned. Also need to be
 * careful when uses 'iferr' with global or member variable, even if we return from control flow of
 * execution we retrun default value and in case of "iferr injection" possible to use state of variable
 * which was forbitten due to default constructor deletion.
 * 
 * class SomeClassWhichCantBeDefaultConstructed {
 * public:
 * 
 *     static SomeClassWhichCantBeDefaultConstructed Create() noexcept { return SomeClassWhichCantBeDefaultConstructed{}; }
 * 
 * private:
 * 
 *     SomeClassWhichCantBeDefaultConstructed() noexcept = default;
 *     ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(SomeClassWhichCantBeDefaultConstructed);
 * };
 * 
 * iferr_scope;
 * 
 * SomeClassWhichCantBeDefaultConstructed result = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed() iferr_return;
 * Alt::Optional<SomeClassWhichCantBeDefaultConstructed> result2 = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed() iferr {
 * 
 *     ALT_PRINTLN("We just failed and contionue execution");
 * }
 * 
 * class SomeClassWhichCantBeDefaultConstructed2 {
 * public:
 * 
 *     static SomeClassWhichCantBeDefaultConstructed2 Create() noexcept { return SomeClassWhichCantBeDefaultConstructed2{}; }
 * 
 * private:
 * 
 *     SomeClassWhichCantBeDefaultConstructed2() noexcept = default;
 * };
 * 
 * iferr_scope;
 * 
 * // Not implemented default construction injection for iferr, so deal with Optional<>
 * Alt::Optional<SomeClassWhichCantBeDefaultConstructed2> result = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed2() iferr_return;
 * Alt::Optional<SomeClassWhichCantBeDefaultConstructed2> result2 = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed2() iferr {
 * 
 *     ALT_PRINTLN("We just failed and contionue execution");
 * }
 */
namespace Alt {

#define ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(self__) \
private: \
    explicit self__(Alt::_IfErrDetails::IfErrInjection_t) noexcept {} \
    friend class Alt::_IfErrDetails::IfErrScopeHandlerControl

#define ALT_IFERR_INJECT_MOVE_CONSTRUCTOR(self__) \
private: \
\
    template <typename IfErrT> \
    class IfErrInjectionMove { \
    public: \
\
        ALT_FORCE_INLINE IfErrInjectionMove(IfErrInjectionMove&&) noexcept = default; \
\
    private: \
\
        ALT_FORCE_INLINE IfErrInjectionMove(IfErrT&& obj) noexcept \
            : m_Obj(std::move(obj)) \
        {} \
\
        ALT_FORCE_INLINE IfErrInjectionMove() noexcept = default; \
        ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(IfErrInjectionMove); \
\
        friend IfErrT; \
\
        IfErrT m_Obj; \
    }; \
\
public: \
\
    ALT_FORCE_INLINE self__(IfErrInjectionMove<self__>&& rhs) noexcept : self__(std::move(rhs.m_Obj)) {} \
private:

template <typename T>
class Optional;

template <typename T, typename E>
class Result;

namespace _IfErrDetails {

struct IfErrInjection_t {};
constexpr static IfErrInjection_t IfErrInjection = IfErrInjection_t{};

class IfErrScopeHandlerControl {
public:

    template <typename T, typename = void>
    struct is_default_constructible_injection
        : std::false_type
    {};

    template <typename T>
    struct is_default_constructible_injection<T, std::void_t<decltype(T(IfErrInjection))>>
        : std::true_type
    {};
    
    template <typename T>
    constexpr static bool is_default_constructible_injection_v = is_default_constructible_injection<T>::value;

    class DefaultCheck;
    class DefaultCheckCanUseInjectedDefaultConstructor;

    template <typename T>
    class DefaultCheckWithErr;

    template <typename T>
    class DefaultCheckWithErrCanUseInjectedDefaultConstructor;

    template <typename T>
    class IteratorCheck;

    IfErrScopeHandlerControl() noexcept = default;

    ALT_FORCE_INLINE DefaultCheck GetDefaultCheck() noexcept {
        return DefaultCheck(*this);
    }

    ALT_FORCE_INLINE DefaultCheckCanUseInjectedDefaultConstructor GetDefaultCheckCanUseInjectedDefaultConstructor() noexcept {
        return DefaultCheckCanUseInjectedDefaultConstructor(*this);
    }

    template <typename T>
    ALT_FORCE_INLINE DefaultCheckWithErr<T> GetDefaultCheckWithErr(T& err) noexcept {
        return DefaultCheckWithErr<T>(*this, err);
    }

    template <typename T>
    ALT_FORCE_INLINE DefaultCheckWithErrCanUseInjectedDefaultConstructor<T> GetDefaultCheckWithErrCanUseInjectedDefaultConstructor(T& err) noexcept {
        return DefaultCheckWithErrCanUseInjectedDefaultConstructor<T>(*this, err);
    }

    template <typename IteratorEndT>
    ALT_FORCE_INLINE IteratorCheck<IteratorEndT> GetIteratorCheck(IteratorEndT iteratorEnd) noexcept {
        return IteratorCheck(*this, iteratorEnd);
    }

    ALT_FORCE_INLINE void CallErrFuncOnceIfDev() const noexcept {}
    ALT_NODISCARD ALT_FORCE_INLINE bool IsOk() const noexcept { return m_IsOk; }

private:

    template <typename T,
        std::enable_if_t<is_default_constructible_injection<T>::value>* = nullptr>
    ALT_FORCE_INLINE T CreateDefaultConstruct() const noexcept { return T(IfErrInjection); }

    template <typename T,
        std::enable_if_t<!is_default_constructible_injection<T>::value>* = nullptr>
    ALT_FORCE_INLINE T CreateDefaultConstruct() const noexcept { return T{}; }

public:

    class DefaultCheck {
    public:

        ALT_FORCE_INLINE DefaultCheck(IfErrScopeHandlerControl& ifErrHandlerState) noexcept
            : m_pIfErrControl(&ifErrHandlerState)
        {}

    public:

        IfErrScopeHandlerControl* m_pIfErrControl = nullptr;
    };

    friend class DefaultCheckCanUseInjectedDefaultConstructor;

    class DefaultCheckCanUseInjectedDefaultConstructor : public DefaultCheck {
    public:

        ALT_FORCE_INLINE DefaultCheckCanUseInjectedDefaultConstructor(IfErrScopeHandlerControl& ifErrHandlerState) noexcept
            : DefaultCheck(ifErrHandlerState)
        {}

        template <typename T>
        ALT_FORCE_INLINE T CreateDefaultConstruct() const noexcept { return this->m_pIfErrControl->template CreateDefaultConstruct<T>(); }
    };

    template <typename T>
    friend class DefaultCheckWithErrCanUseInjectedDefaultConstructor;

    template <typename T>
    class DefaultCheckWithErr {
    public:

        ALT_FORCE_INLINE DefaultCheckWithErr(IfErrScopeHandlerControl& ifErrHandlerState, T& err) noexcept
            : m_pIfErrControl(&ifErrHandlerState)
            , m_pErr(&err)
        {}

    public:

        IfErrScopeHandlerControl* m_pIfErrControl = nullptr;
        T* m_pErr = nullptr;
    };

    template <typename T>
    class DefaultCheckWithErrCanUseInjectedDefaultConstructor : public DefaultCheckWithErr<T> {
    public:

        ALT_FORCE_INLINE DefaultCheckWithErrCanUseInjectedDefaultConstructor(IfErrScopeHandlerControl& ifErrHandlerState, T& err) noexcept
            : DefaultCheckWithErr<T>(ifErrHandlerState, err)
        {}

        template <typename U>
        ALT_FORCE_INLINE U CreateDefaultConstruct() const noexcept { return this->m_pIfErrControl->template CreateDefaultConstruct<U>(); }
    };

    template <typename IteratorEndT>
    class IteratorCheck {
    public:

        ALT_FORCE_INLINE IteratorCheck(IfErrScopeHandlerControl& ifErrHandlerState, IteratorEndT iteratorEnd) noexcept
            : m_pIfErrControl(&ifErrHandlerState)
            , m_IteratorEnd(iteratorEnd)
        {}

    public:

        IfErrScopeHandlerControl* m_pIfErrControl = nullptr;
        IteratorEndT m_IteratorEnd;
    };

public:

    bool m_IsOk = true;
};

template <typename T>
constexpr static bool is_default_constructible_injection_v = IfErrScopeHandlerControl::is_default_constructible_injection_v<T>;

template <typename T>
constexpr static bool is_use_injected_default_constructible =
    !std::is_default_constructible_v<T> &&
    !std::is_void_v<T> &&
    !(std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>);

template <typename T,
    std::enable_if_t<TypeTraits::is_explicitly_convertible_v<T, bool>>* = nullptr>
ALT_FORCE_INLINE T operator%(T&& value, IfErrScopeHandlerControl::DefaultCheck errHandler) noexcept {

    errHandler.m_pIfErrControl->m_IsOk = static_cast<bool>(value);

    T result = std::move(value);
    return result;
}

template <typename T,
 std::enable_if_t<TypeTraits::is_explicitly_convertible_v<T, bool>>* = nullptr>
ALT_FORCE_INLINE T operator%(T& value, IfErrScopeHandlerControl::DefaultCheck errHandler) noexcept {

    errHandler.m_pIfErrControl->m_IsOk = static_cast<bool>(value);

    T result = std::move(value);
    return result;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_explicitly_convertible_v<T, bool>>* = nullptr>
ALT_FORCE_INLINE T operator%(const T& value, IfErrScopeHandlerControl::DefaultCheck errHandler) noexcept {

    errHandler.m_pIfErrControl->m_IsOk = static_cast<bool>(value);

    T result = std::move(value);
    return result;
}

template <typename T>
ALT_FORCE_INLINE T* operator%(T* ptr, IfErrScopeHandlerControl::DefaultCheck errHandler) noexcept {

    errHandler.m_pIfErrControl->m_IsOk = ptr != nullptr;
    return ptr;
}

template <typename T>
ALT_FORCE_INLINE const T* operator%(const T* ptr, IfErrScopeHandlerControl::DefaultCheck errHandler) noexcept {

    errHandler.m_pIfErrControl->m_IsOk = ptr != nullptr;
    return ptr;
}

template <typename T,
    std::enable_if_t<std::is_same_v<T, bool>>* = nullptr>
ALT_FORCE_INLINE bool operator%(T value, IfErrScopeHandlerControl::DefaultCheck errHandler) noexcept {

    errHandler.m_pIfErrControl->m_IsOk = value;
    return value;
}

template <typename T,
    std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Optional<T>&& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetValue());
        return result;
    }

    return T{};
}

template <typename T,
    std::enable_if_t<std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>>* = nullptr>
ALT_FORCE_INLINE TypeTraits::remove_reference_or_alt_ref_t<T>* operator%(
    Optional<T>&& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    using RefT = TypeTraits::remove_reference_or_alt_ref_t<T>;

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        RefT& result = value.GetValue();
        return &result;
    }

    return nullptr;
}

template <typename T,
    std::enable_if_t<is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Optional<T>&& value, IfErrScopeHandlerControl::DefaultCheckCanUseInjectedDefaultConstructor errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetValue());
        return result;
    }

    return errHandler.template CreateDefaultConstruct<T>();
}

template <typename T,
    std::enable_if_t<!is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE Optional<T> operator%(
    Optional<T>&& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetValue());
        return result;
    }

    return Optional<T>::Null();
}

template <typename T,
    std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Optional<T>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetValue());
        return result;
    }

    return T{};
}

template <typename T,
    std::enable_if_t<std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>>* = nullptr>
ALT_FORCE_INLINE TypeTraits::remove_reference_or_alt_ref_t<T>* operator%(
    Optional<T>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    using RefT = TypeTraits::remove_reference_or_alt_ref_t<T>;

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        RefT& result = value.GetValue();
        return &result;
    }

    return nullptr;
}

template <typename T,
    std::enable_if_t<is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Optional<T>& value, IfErrScopeHandlerControl::DefaultCheckCanUseInjectedDefaultConstructor errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetValue());
        return result;
    }

    return errHandler.template CreateDefaultConstruct<T>();
}

template <typename T,
    std::enable_if_t<!is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE Optional<T> operator%(
    Optional<T>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetValue());
        return result;
    }

    return Optional<T>::Null();
}

template <typename T,
    std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    const Optional<T>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetValue());
        return result;
    }

    return T{};
}

template <typename T,
    std::enable_if_t<std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>>* = nullptr>
ALT_FORCE_INLINE const TypeTraits::remove_reference_or_alt_ref_t<T>* operator%(
    const Optional<T>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    using RefT = TypeTraits::remove_reference_or_alt_ref_t<T>;

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        const RefT& result = value.GetValue();
        return &result;
    }

    return nullptr;
}

template <typename T,
    std::enable_if_t<is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    const Optional<T>& value, IfErrScopeHandlerControl::DefaultCheckCanUseInjectedDefaultConstructor errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetValue());
        return result;
    }

    return errHandler.template CreateDefaultConstruct<T>();
}

template <typename T,
    std::enable_if_t<!is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE Optional<T> operator%(
    const Optional<T>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetValue());
        return result;
    }

    return Optional<T>::Null();
}

template <typename T, typename E,
    std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Result<T, E>&& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    return T{};
}

template <typename T, typename E,
    std::enable_if_t<std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>>* = nullptr>
ALT_FORCE_INLINE TypeTraits::remove_reference_or_alt_ref_t<T>* operator%(
    Result<T, E>&& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    using RefT = TypeTraits::remove_reference_or_alt_ref_t<T>;

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        RefT& result = value.GetOk();
        return &result;
    }

    return nullptr;
}

template <typename T, typename E,
    std::enable_if_t<is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Result<T, E>&& value, IfErrScopeHandlerControl::DefaultCheckCanUseInjectedDefaultConstructor errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    return errHandler.template CreateDefaultConstruct<T>();
}

template <typename T, typename E,
    std::enable_if_t<!is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE Optional<T> operator%(
    Result<T, E>&& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    return Optional<T>::Null();
}

template <typename T, typename E,
    std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    return T{};
}

template <typename T, typename E,
    std::enable_if_t<is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheckCanUseInjectedDefaultConstructor errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    return errHandler.template CreateDefaultConstruct<T>();
}

template <typename T, typename E,
    std::enable_if_t<std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>>* = nullptr>
ALT_FORCE_INLINE TypeTraits::remove_reference_or_alt_ref_t<T>* operator%(
    Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    using RefT = TypeTraits::remove_reference_or_alt_ref_t<T>;

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        RefT& result = value.GetOk();
        return &result;
    }

    return nullptr;
}

template <typename T, typename E,
    std::enable_if_t<!is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE Optional<T> operator%(
    Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    return Optional<T>::Null();
}

template <typename T, typename E,
    std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    const Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    return T{};
}

template <typename T, typename E,
    std::enable_if_t<std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>>* = nullptr>
ALT_FORCE_INLINE const TypeTraits::remove_reference_or_alt_ref_t<T>* operator%(
    const Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    using RefT = TypeTraits::remove_reference_or_alt_ref_t<T>;

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        const RefT& result = value.GetOk();
        return &result;
    }

    return nullptr;
}

template <typename T, typename E,
    std::enable_if_t<is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    const Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheckCanUseInjectedDefaultConstructor errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    return errHandler.template CreateDefaultConstruct<T>();
}

template <typename T, typename E,
    std::enable_if_t<!is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T>>* = nullptr>
ALT_FORCE_INLINE Optional<T> operator%(
    const Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    return Optional<T>::Null();
}

template <typename T, typename E>
ALT_FORCE_INLINE Optional<T> operator%(
    const Result<void, E>& value, IfErrScopeHandlerControl::DefaultCheck errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        return;
    }

    return Optional<T>::Null();
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<std::is_default_constructible_v<T> && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Result<T, E>&& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return T{};
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<(std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>) && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE TypeTraits::remove_reference_or_alt_ref_t<T>* operator%(
    Result<T, E>&& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    using RefT = TypeTraits::remove_reference_or_alt_ref_t<T>;

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        RefT& result = value.GetOk();
        return &result;
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return nullptr;
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T> && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Result<T, E>&& value, IfErrScopeHandlerControl::DefaultCheckWithErrCanUseInjectedDefaultConstructor<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        return std::move(value.GetOk());
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return errHandler.template CreateDefaultConstruct<T>();
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<!is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T> && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE Optional<T> operator%(
    Result<T, E>&& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        return std::move(value.GetOk());
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return errHandler.template CreateDefaultConstruct<T>();
}

template <typename E, typename ErrT,
    std::enable_if_t<std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE void operator%(
    Result<void, E>&& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        return;
    }

    *errHandler.m_pErr = std::move(value.GetErr());
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<std::is_default_constructible_v<T> && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return T{};
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<(std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>) && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE TypeTraits::remove_reference_or_alt_ref_t<T>* operator%(
    Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    using RefT = TypeTraits::remove_reference_or_alt_ref_t<T>;

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        RefT& result = value.GetOk();
        return &result;
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return nullptr;
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T> && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheckWithErrCanUseInjectedDefaultConstructor<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        return std::move(value.GetOk());
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return errHandler.template CreateDefaultConstruct<T>();
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<!is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T> && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE Optional<T> operator%(
    Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        return std::move(value.GetOk());
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return Optional<T>::Null();
}

template <typename E, typename ErrT,
    std::enable_if_t<std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE void operator%(
    Result<void, E>& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        return;
    }

    *errHandler.m_pErr = std::move(value.GetErr());
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<std::is_default_constructible_v<T> && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    const Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        T result = std::move(value.GetOk());
        return result;
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return T{};
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<(std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>) && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE const TypeTraits::remove_reference_or_alt_ref_t<T>* operator%(
    const Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    using RefT = TypeTraits::remove_reference_or_alt_ref_t<T>;

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        const RefT& result = value.GetOk();
        return &result;
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return nullptr;
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T> && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE T operator%(
    const Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheckWithErrCanUseInjectedDefaultConstructor<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        return std::move(value.GetOk());
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return errHandler.template CreateDefaultConstruct<T>();
}

template <typename T, typename E, typename ErrT,
    std::enable_if_t<!is_default_constructible_injection_v<T> && is_use_injected_default_constructible<T> && std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE Optional<T> operator%(
    const Result<T, E>& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        return std::move(value.GetOk());
    }

    *errHandler.m_pErr = std::move(value.GetErr());
    return Optional<T>::Null();
}

template <typename E, typename ErrT,
    std::enable_if_t<std::is_assignable_v<ErrT&, E&&>>* = nullptr>
ALT_FORCE_INLINE void operator%(
    const Result<void, E>& value, IfErrScopeHandlerControl::DefaultCheckWithErr<ErrT> errHandler
) noexcept {

    const bool isOk = value.IsOk();
    errHandler.m_pIfErrControl->m_IsOk = isOk;

    if (isOk) {

        return;
    }

    *errHandler.m_pErr = std::move(value.GetErr());
}

template <typename IteratorT>
ALT_FORCE_INLINE IteratorT operator%(IteratorT value, IfErrScopeHandlerControl::IteratorCheck<IteratorT> errHandler) noexcept {

    errHandler.m_pIfErrControl->m_IsOk = value != errHandler.m_IteratorEnd;
    return value;
}

class IfErrScopeHandler : public IfErrScopeHandlerControl
{};

template <typename FuncT>
class IfErrScopeHandlerOnErr : public IfErrScopeHandler {
public:
    IfErrScopeHandlerOnErr(FuncT func) noexcept : m_Func(func) {}
    ~IfErrScopeHandlerOnErr() noexcept { CallErrFuncOnceImpl(); }
    ALT_FORCE_INLINE void CallErrFuncOnceIfDev() noexcept {
        /*
         * To simplify debugging process in dev/debug we are calling error lambda dirrectly when error occures,
         * but in release we do not do this to help compiler in code generation.
         */
#ifdef ALT_DEV
        CallErrFuncOnceImpl();
#endif // ALT_DEV
    }
private:
    ALT_FORCE_INLINE void CallErrFuncOnceImpl() noexcept {
#ifdef ALT_DEV
        if (!m_IsOk && !m_IsDissabled) { m_IsDissabled = true; (m_Func)(); }
#else
        if (!m_IsOk) { (m_Func)(); }
#endif // ALT_DEV
    }
private:
    FuncT m_Func;
#ifdef ALT_DEV
    bool m_IsDissabled = false;
#endif // ALT_DEV
};

struct IfErrScopeHandlerOnErrUnit
{};

template <typename FuncT>
ALT_FORCE_INLINE IfErrScopeHandlerOnErr<FuncT> operator+(IfErrScopeHandlerOnErrUnit, FuncT&& func) noexcept {
    return IfErrScopeHandlerOnErr<FuncT>(std::forward<FuncT>(func));
}

class IfLoopScopeHandler : public IfErrScopeHandlerControl
{};

template <typename FuncT>
class IfLoopScopeHandlerOnDefer : public IfLoopScopeHandler {
public:
    IfLoopScopeHandlerOnDefer(FuncT func) noexcept : m_Func(func) {}
    ~IfLoopScopeHandlerOnDefer() noexcept { (m_Func)(!m_IsOk); }
private:
    FuncT m_Func;
};

struct IfLoopScopeHandlerOnDeferUnit
{};

template <typename FuncT>
ALT_FORCE_INLINE IfLoopScopeHandlerOnDefer<FuncT> operator+(IfLoopScopeHandlerOnDeferUnit, FuncT&& func) noexcept {
    return IfLoopScopeHandlerOnDefer<FuncT>(std::forward<FuncT>(func));
}

} // namespace _IfErrDetails
} // namespace Alt

// --------------------------------------------------------------------------------------------------------------
#define iferr_scope Alt::_IfErrDetails::IfErrScopeHandler __iferr_handler_state__{}
#define iferr_scope_on_err auto __iferr_handler_state__ = Alt::_IfErrDetails::IfErrScopeHandlerOnErrUnit{} + [&]() mutable
// --------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------
#define ifok % __iferr_handler_state__.GetDefaultCheck(); if (__iferr_handler_state__.m_IsOk)
#define ifok_with(err) % __iferr_handler_state__.GetDefaultCheckWithErr(err); if (__iferr_handler_state__.m_IsOk)
#define iferr % __iferr_handler_state__.GetDefaultCheck(); if (!__iferr_handler_state__.m_IsOk)
#define iferr_with(err) % __iferr_handler_state__.GetDefaultCheckWithErr(err); if (!__iferr_handler_state__.m_IsOk)

// yes, we are doing 'if' check twice to simplify debugging for `iferr_scope_on_err` case
#define iferr_return % __iferr_handler_state__.GetDefaultCheckCanUseInjectedDefaultConstructor(); if (!__iferr_handler_state__.m_IsOk) __iferr_handler_state__.CallErrFuncOnceIfDev(); if (!__iferr_handler_state__.m_IsOk) return
#define iferr_return_with(err) % __iferr_handler_state__.GetDefaultCheckWithErrCanUseInjectedDefaultConstructor(err); if (!__iferr_handler_state__.m_IsOk) __iferr_handler_state__.CallErrFuncOnceIfDev(); if (!__iferr_handler_state__.m_IsOk) return

#define iferr_panic(...) % __iferr_handler_state__.GetDefaultCheckCanUseInjectedDefaultConstructor(); if (!__iferr_handler_state__.m_IsOk) do { ALT_PANIC(__VA_ARGS__); } while(0)
#define iferr_panic_with(err, ...) % __iferr_handler_state__.GetDefaultCheckWithErrCanUseInjectedDefaultConstructor(err); if (!__iferr_handler_state__.m_IsOk) do { ALT_PANIC(__VA_ARGS__); } while(0)
#define iferr_panic_silent % __iferr_handler_state__.GetDefaultCheckCanUseInjectedDefaultConstructor(); if (!__iferr_handler_state__.m_IsOk) do { ALT_PANIC_SILENT(); } while(0)

#ifdef ALT_DEV

#define iferr_panic_only_dev(...) % __iferr_handler_state__.GetDefaultCheckCanUseInjectedDefaultConstructor(); if (!__iferr_handler_state__.m_IsOk) do { ALT_PANIC(__VA_ARGS__); } while(0)
#else

#define iferr_panic_only_dev(...) % Alt::_IfErrDetails::IfErrScopeHandler{}.GetDefaultCheckCanUseInjectedDefaultConstructor(); if (!__iferr_handler_state__.m_IsOk) do {} while(0)
#endif // ALT_DEV

#define iferr_return_return __iferr_handler_state__.m_IsOk = false; __iferr_handler_state__.CallErrFuncOnceIfDev(); return
// --------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------
#define iferr_return_with_iter(somethingWhichHasEnd_) % __iferr_handler_state__.GetIteratorCheck(somethingWhichHasEnd_.end()); if (!__iferr_handler_state__.m_IsOk) __iferr_handler_state__.CallErrFuncOnceIfDev(); if (!__iferr_handler_state__.m_IsOk) return
// --------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------
#define ifloop_scope Alt::_IfErrDetails::IfLoopScopeHandler __ifloop_handler_state__{}
#define ifloop_scope_on_defer auto __ifloop_handler_state__ = Alt::_IfErrDetails::IfLoopScopeHandlerOnDeferUnit{} + [&](ALT_MAYBE_UNUSED const bool isErr) mutable
// --------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------
#define ifloop_break % __ifloop_handler_state__.GetDefaultCheckCanUseInjectedDefaultConstructor(); if (!__ifloop_handler_state__.m_IsOk) break
#define ifloop_break_breakpoint % __ifloop_handler_state__.GetDefaultCheckCanUseInjectedDefaultConstructor(); if (!__ifloop_handler_state__.m_IsOk) ALT_BREAKPOINT; if (!__ifloop_handler_state__.m_IsOk) break

#define ifloop_continue % __ifloop_handler_state__.GetDefaultCheckCanUseInjectedDefaultConstructor(); if (!__ifloop_handler_state__.m_IsOk) continue
#define ifloop_continue_breakpoint % __ifloop_handler_state__.GetDefaultCheckCanUseInjectedDefaultConstructor(); if (!__ifloop_handler_state__.m_IsOk) ALT_BREAKPOINT; if (!__ifloop_handler_state__.m_IsOk) continue
// --------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------
#define ifloop_break_with_iter(somethingWhichHasEnd_) % __ifloop_handler_state__.GetIteratorCheck(somethingWhichHasEnd_.end()); if (!__ifloop_handler_state__.m_IsOk) break
#define ifloop_break_with_iter_breakpoint(somethingWhichHasEnd_) % __ifloop_handler_state__.GetIteratorCheck(somethingWhichHasEnd_.end()); if (!__ifloop_handler_state__.m_IsOk) ALT_BREAKPOINT; if (!__ifloop_handler_state__.m_IsOk) break

#define ifloop_continue_with_iter(somethingWhichHasEnd_) % __ifloop_handler_state__.GetIteratorCheck(somethingWhichHasEnd_.end()); if (!__ifloop_handler_state__.m_IsOk) continue
#define ifloop_continue_with_iter_breakpoint(somethingWhichHasEnd_) % __ifloop_handler_state__.GetIteratorCheck(somethingWhichHasEnd_.end()); if (!__ifloop_handler_state__.m_IsOk) ALT_BREAKPOINT; if (!__ifloop_handler_state__.m_IsOk) continue
// --------------------------------------------------------------------------------------------------------------

#endif // __ALT_STD_IFERR_SCOPE_HANDLER__