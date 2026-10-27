#pragma once

#ifndef __ALT_STD_FUNC__
#define __ALT_STD_FUNC__

#include "../TypeTraits.h"

#include "Num.h"
#include "Optional.h"

namespace Alt {

/**
 * @brief A type alias for a non-exception function pointer.
 * 
 * @tparam ReturnT The return type of the function.
 * @tparam ...ArgsT The arguments to function.
 */
template <typename ReturnT, typename ...ArgsT>
using FuncPtr = ReturnT(*)(ArgsT...) noexcept;

/**
 * @brief A type alias for a non-exception member function pointer.
 * 
 * @tparam ObjectT The class or struct type containing the method.
 * @tparam ReturnT The return type of the method.
 * @tparam ...ArgsT The arguments to method.
 */
template <typename ObjectT, typename ReturnT, typename ...ArgsT>
using MethPtr = ReturnT(ObjectT::*)(ArgsT...) noexcept;

template <typename Return, typename ...Args>
class FuncRef;

template <usize SIZE_IN_BYTES, usize ALIGN_IN_BYTES, typename Return, typename ...Args>
class StaticFunc;

namespace TypeTraits {

template <typename Return, typename ...Args>
struct __is_trivially_reallocatable__<FuncRef<Return, Args...>>
    : std::true_type
{};

template <usize SIZE_IN_BYTES, usize ALIGN_IN_BYTES, typename Return, typename ...Args>
struct __is_trivially_reallocatable__<StaticFunc<SIZE_IN_BYTES, ALIGN_IN_BYTES, Return, Args...>>
    : std::false_type
{};

} // namespace TypeTraits

/**
 * @brief A wrapper for callable functors including functions and lambdas.
 * 
 * @details Provides a unified interface to invoke different types of callables (function pointers, 
 * lambda expressions, or captured lambdas) by wrapping them in a type-erased internal function pointer.
 * This allows for uniform storage and invocation logic across different callable types (function with payload).
 * 
 * @tparam Return The return type of the wrapped callable.
 * @tparam ...Args The variadic pack of argument types expected by the callable.
 */
template <typename Return, typename ...Args>
class FuncRef {
public:

    FuncRef() noexcept = delete;

    /**
     * @brief Constructs a FuncRef from a functor or function pointer.
     * @tparam Func The type of the callable entity.
     * @example
     * 
     * static usize FuncPtr(const f32) noexcept {
     *     return 0;
     * }
     * 
     * Alt::FuncPtr<usize, f32> func0 = FuncPtr;
     * Alt::FuncPtr<usize, f32> func1 = [](const f32) noexcept -> usize { return 0 };
     */
    template <typename FuncT,
        std::enable_if_t<
            std::is_convertible_v<FuncT, Return(*)(Args...)> &&
            std::is_same_v<TypeTraits::clean_t<Return>, TypeTraits::clean_t<std::invoke_result_t<FuncT&&, Args...>>>
        >* = nullptr>
    ALT_FORCE_INLINE FuncRef(FuncT&& func) noexcept {

        Assign(func);
    }

    /**
     * @brief Constructs a FuncRef from a functor which own data.
     * @tparam Func The type of the callable entity.
     * @example
     * 
     * struct EmulateLambda {
     * 
     *     usize acc = 0;
     * 
     *     usize operator()(const f32) noexcept {
     *         return acc++;
     *     }
     * };
     * 
     * auto lambda0 = [&acc](const f32) -> usize {
     *     return acc++;
     * };
     * 
     * Alt::FuncPtr<usize, f32> func0 = lambda0;
     * 
     * EmulateLambda emuLambda{};
     * Alt::FuncPtr<usize, f32> func1 = emuLambda;
     */
    template <typename FuncT,
        std::enable_if_t<
            !std::is_convertible_v<FuncT, Return(*)(Args...)> &&
            std::is_same_v<TypeTraits::clean_t<Return>, TypeTraits::clean_t<std::invoke_result_t<FuncT&, Args...>>>
        >* = nullptr>
    ALT_FORCE_INLINE FuncRef(FuncT& func) noexcept {

        Assign(func);
    }

    /**
     * @brief Constructs a FuncRef from a functor which own data.
     * @tparam Func The type of the callable entity.
     * @example
     * 
     * struct EmulateLambda {
     * 
     *     mutable usize acc = 0;
     * 
     *     usize operator()(const f32) const noexcept {
     *         return acc++;
     *     }
     * };
     * 
     * EmulateLambda emuLambda{};
     * Alt::FuncPtr<usize, f32> func = emuLambda;
     */
    template <typename FuncT,
        std::enable_if_t<
            !std::is_convertible_v<FuncT, Return(*)(Args...)> &&
            std::is_same_v<TypeTraits::clean_t<Return>, TypeTraits::clean_t<std::invoke_result_t<const FuncT&, Args...>>>
        >* = nullptr>
    ALT_FORCE_INLINE FuncRef(const FuncT& func) noexcept {

        AssignConst(func);
    }

    template <usize SIZE_IN_BYTES, usize ALIGN_IN_BYTES>
    ALT_FORCE_INLINE FuncRef(StaticFunc<SIZE_IN_BYTES, ALIGN_IN_BYTES, Return, Args...>&& rhs) noexcept {

        m_pFuncOrObj = rhs.m_Memory;
        m_pCall = rhs.m_pOperatorBracketBracket;
    }

    template <usize SIZE_IN_BYTES, usize ALIGN_IN_BYTES>
    ALT_FORCE_INLINE FuncRef(StaticFunc<SIZE_IN_BYTES, ALIGN_IN_BYTES, Return, Args...>& rhs) noexcept {

        m_pFuncOrObj = rhs.m_Memory;
        m_pCall = rhs.m_pOperatorBracketBracket;
    }

    template <usize SIZE_IN_BYTES, usize ALIGN_IN_BYTES>
    ALT_FORCE_INLINE FuncRef(const StaticFunc<SIZE_IN_BYTES, ALIGN_IN_BYTES, Return, Args...>& rhs) noexcept {

        m_pFuncOrObj = rhs.m_Memory;
        m_pCall = rhs.m_pOperatorBracketBracket;
    }

    /**
     * @brief Assings a FuncRef from a functor which own data.
     * @tparam Func The type of the callable entity.
     */
    template <typename FuncT,
        std::enable_if_t<
            std::is_convertible_v<FuncT, Return(*)(Args...)> &&
            std::is_same_v<TypeTraits::clean_t<Return>, TypeTraits::clean_t<std::invoke_result_t<FuncT&&, Args...>>>
        >* = nullptr>
    ALT_FORCE_INLINE FuncRef& operator=(FuncT&& func) noexcept {

        Assign(func);
        return *this;
    }

    template <typename FuncT,
        std::enable_if_t<
            !std::is_convertible_v<FuncT, Return(*)(Args...)> &&
            std::is_same_v<TypeTraits::clean_t<Return>, TypeTraits::clean_t<std::invoke_result_t<FuncT&, Args...>>>
        >* = nullptr>
    ALT_FORCE_INLINE FuncRef& operator=(FuncT& func) noexcept {

        Assign(func);
        return *this;
    }

    template <typename FuncT,
        std::enable_if_t<
            !std::is_convertible_v<FuncT, Return(*)(Args...)> &&
            std::is_same_v<TypeTraits::clean_t<Return>, TypeTraits::clean_t<std::invoke_result_t<const FuncT&, Args...>>>
        >* = nullptr>
    ALT_FORCE_INLINE FuncRef& operator=(const FuncT& func) noexcept {

        AssignConst(func);
        return *this;
    }

    template <usize SIZE_IN_BYTES, usize ALIGN_IN_BYTES>
    ALT_FORCE_INLINE FuncRef& operator=(StaticFunc<SIZE_IN_BYTES, ALIGN_IN_BYTES, Return, Args...>&& rhs) noexcept {

        m_pFuncOrObj = rhs.m_Memory;
        m_pCall = rhs.m_pCall;

        return *this;
    }

    template <usize SIZE_IN_BYTES, usize ALIGN_IN_BYTES>
    ALT_FORCE_INLINE FuncRef& operator=(StaticFunc<SIZE_IN_BYTES, ALIGN_IN_BYTES, Return, Args...>& rhs) noexcept {

        m_pFuncOrObj = rhs.m_Memory;
        m_pCall = rhs.m_pCall;

        return *this;
    }

    template <usize SIZE_IN_BYTES, usize ALIGN_IN_BYTES>
    ALT_FORCE_INLINE FuncRef& operator=(const StaticFunc<SIZE_IN_BYTES, ALIGN_IN_BYTES, Return, Args...>& rhs) noexcept {

        m_pFuncOrObj = rhs.m_Memory;
        m_pCall = rhs.m_pCall;

        return *this;
    }

    ALT_FORCE_INLINE FuncRef(const FuncRef&) noexcept = default;
    ALT_FORCE_INLINE FuncRef& operator=(const FuncRef&) noexcept = default;

    ALT_FORCE_INLINE FuncRef(FuncRef&&) noexcept = default;
    ALT_FORCE_INLINE FuncRef& operator=(FuncRef&&) noexcept = default;

    /**
     * @brief Invokes the wrapped callable with provided arguments.
     * @tparam ...ArgsT The types of the arguments to pass. Must be constructible from the input values.
     * @return The result of the function call.
     * @example
     * 
     * Alt::FuncPtr<usize, f32> func = [](const f32) noexcept -> usize {
     *     return 0
     * };
     * 
     * assert((func)(), 0)
     */
    template <typename ...ArgsT,
        std::enable_if_t<std::conjunction_v<std::is_constructible<Args, ArgsT&&>...>>* = nullptr>
    ALT_FORCE_INLINE Return operator()(ArgsT&&... args) noexcept {

        return (*m_pCall)(m_pFuncOrObj, std::forward<ArgsT>(args)...);
    }

    template <typename ...ArgsT,
        std::enable_if_t<std::conjunction_v<std::is_constructible<Args, ArgsT&&>...>>* = nullptr>
    ALT_FORCE_INLINE Return operator()(ArgsT&&... args) const noexcept {

        return (*m_pCall)(m_pFuncOrObj, std::forward<ArgsT>(args)...);
    }

private:

    template <typename FuncT>
    ALT_FORCE_INLINE void Assign(const FuncT& func) noexcept {

        m_pFuncOrObj = const_cast<void*>(reinterpret_cast<const void*>(&func));
        m_pCall = [](void* pObj, Args... args) noexcept -> Return {
            return (*reinterpret_cast<std::remove_reference_t<FuncT>*>(pObj))(std::forward<Args>(args)...);
        };
    }

    template <typename FuncT>
    ALT_FORCE_INLINE void AssignConst(const FuncT& func) noexcept {

        m_pFuncOrObj = const_cast<void*>(reinterpret_cast<const void*>(&func));
        m_pCall = [](void* pObj, Args... args) noexcept -> Return {
            return (*reinterpret_cast<const std::remove_reference_t<FuncT>*>(pObj))(std::forward<Args>(args)...);
        };
    }

private:

    void* m_pFuncOrObj;
    FuncPtr<Return, void*, Args...> m_pCall;
};

/**\cond */
namespace _FuncDetails {

template <typename Return, typename ...Args>
struct FTable {

    //FuncPtr<Return, void*, Args...> pOperatorBracketBracket = nullptr;
    FuncPtr<void, void*, void*> pMoveConstruct = nullptr;
    FuncPtr<void, void*> pDestruct = nullptr;
};

template <typename FuncT, typename Return, typename ...Args>
ALT_FORCE_INLINE FuncPtr<Return, void*, Args...> OperatorBracketBracketFunc() noexcept {

    using T = std::remove_reference_t<FuncT>;

    return [](void* pObj, Args... args) noexcept -> Return {

        return (*reinterpret_cast<T*>(pObj))(std::forward<Args>(args)...);
    };
}

template <typename FuncT>
ALT_FORCE_INLINE FuncPtr<void, void*, void*> MoveConstructFunc() noexcept {

    using T = TypeTraits::clean_t<std::remove_reference_t<FuncT>>;

    return [](void* pObjTo, void* pObjFrom) noexcept {

        Memory::ConstructObject(reinterpret_cast<T*>(pObjTo), std::move(*reinterpret_cast<T*>(pObjFrom)));
    };
}

template <typename FuncT>
ALT_FORCE_INLINE FuncPtr<void, void*> DestructFunc() noexcept {

    using T = TypeTraits::clean_t<std::remove_reference_t<FuncT>>;

    return [](void* pObjTo) noexcept {

        Memory::DestroyObject(*reinterpret_cast<T*>(pObjTo));
    };
}

template <typename FuncT, typename Return, typename ...Args>
ALT_FORCE_INLINE FTable<Return, Args...> CreateFTable() noexcept {

    FTable<Return, Args...> fTable;

    //fTable.pOperatorBracketBracket = OperatorBracketBracketFunc<FuncT, Return, Args...>();
    fTable.pMoveConstruct = MoveConstructFunc<FuncT>();
    fTable.pDestruct = DestructFunc<FuncT>();

    return fTable;
}

template <typename FuncT, typename Return, typename ...Args>
ALT_NODISCARD static FTable<Return, Args...>* GetOrCreateFTable() noexcept {

    static FTable<Return, Args...> fTable = CreateFTable<FuncT, Return, Args...>();
    return &fTable;
}

} // namespace _FuncDetails
/**\endcond */

/**
 * @brief A fixed-size memory buffer wrapper for callable objects.
 * 
 * @details Provides a way to store callables (lambdas/functors) within a static memory block,
 *          avoiding heap allocations while maintaining the ability to use type erasure via `FuncRef`.
 * 
 * @tparam SIZE_IN_BYTES The amount of bytes to allocate for the callable's state.
 * @tparam ALIGN_IN_BYTES The alignment requirement for the buffer (must be a power of 2).
 * @tparam Return The return type of the wrapped callable.
 * @tparam ...Args The variadic pack of argument types expected by the callable.
 * 
 * @example
 * 
 * struct Lambda {
 * 
 *     usize x = 10;
 *     usize operator()(f32) const noexcept { return x; }
 * };
 * 
 * Alt::StaticFunc<64, 16, int, f32> func = ComplexLambda{};
 * assert((sFunc)(1.0f) == 10);
 */
template <usize SIZE_IN_BYTES, usize ALIGN_IN_BYTES, typename Return, typename ...Args>
class StaticFunc {
private:

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

    template <typename FuncT>
    constexpr static bool is_check_size_ok_v = sizeof(FuncT) <= SIZE_IN_BYTES && alignof(FuncT) <= ALIGN_IN_BYTES;

    static_assert(Num::IsPowerOf2(ALIGN_IN_BYTES), "");

public:

    /**
     * @brief Constructs a StaticFunc from a functor or function pointer.
     * @tparam Func The type of the callable entity.
     * @example
     * 
     * static usize FuncPtr(const f32) noexcept {
     *     return 0;
     * }
     * 
     * Alt::StaticFunc<64, 16, usize, f32> func0 = &FuncPtr;
     * Alt::StaticFunc<64, 16, usize, f32> func1 = [](const f32) noexcept -> usize { return 0 };
     */
    ALT_FORCE_INLINE StaticFunc(StaticFunc&& rhs) noexcept {

        Construct(Rule5Move, std::move(rhs));
    }

    template <usize SIZE_IN_BYTES_, usize ALIGN_IN_BYTES_,
        std::enable_if_t<(SIZE_IN_BYTES_ + ALIGN_IN_BYTES_) <= SIZE_IN_BYTES + ALIGN_IN_BYTES>* = nullptr>
    ALT_FORCE_INLINE StaticFunc(StaticFunc<SIZE_IN_BYTES_, ALIGN_IN_BYTES_, Return, Args...>&& rhs) noexcept {

        Construct(Rule5Move, std::move(rhs));
    }

    /**
     * @brief Constructs a StaticFunc from a functor which own data.
     * @tparam Func The type of the callable entity.
     * @example
     * 
     * struct EmulateLambda {
     * 
     *     usize acc = 0;
     * 
     *     usize operator()(const f32) noexcept {
     *         return acc++;
     *     }
     * };
     * 
     * auto lambda0 = [&acc](const f32) -> usize {
     *     return acc++;
     * };
     * 
     * Alt::StaticFunc<64, 16, usize, f32> func0 = lambda0;
     * 
     * EmulateLambda emuLambda{};
     * Alt::StaticFunc<64, 16, usize, f32> func1 = emuLambda;
     */
    template <typename FuncT,
        std::enable_if_t<
            is_check_size_ok_v<FuncT> &&
            std::is_same_v<TypeTraits::clean_t<Return>, TypeTraits::clean_t<std::invoke_result_t<FuncT&&, Args...>>>
        >* = nullptr>
    ALT_FORCE_INLINE StaticFunc(FuncT&& rhs) noexcept {

        Construct(std::forward<FuncT>(rhs));
    }

    ALT_FORCE_INLINE StaticFunc& operator=(StaticFunc&& rhs) noexcept {

        Assign(Rule5Move, std::move(rhs));
        return *this;
    }

    template <usize SIZE_IN_BYTES_, usize ALIGN_IN_BYTES_,
        std::enable_if_t<(SIZE_IN_BYTES_ + ALIGN_IN_BYTES_) <= SIZE_IN_BYTES + ALIGN_IN_BYTES>* = nullptr>
    ALT_FORCE_INLINE StaticFunc& operator=(StaticFunc<SIZE_IN_BYTES_, ALIGN_IN_BYTES_, Return, Args...>&& rhs) noexcept {

        Assign(Rule5Move, std::move(rhs));
        return *this;
    }

    template <typename FuncT,
        std::enable_if_t<
            is_check_size_ok_v<FuncT> &&
            std::is_same_v<TypeTraits::clean_t<Return>, TypeTraits::clean_t<std::invoke_result_t<FuncT&&, Args...>>>
        >* = nullptr>
    ALT_FORCE_INLINE StaticFunc& operator=(FuncT&& rhs) noexcept {

        Assign(std::forward<FuncT>(rhs));
        return *this;
    }

    /**
     * @brief Invokes the wrapped callable with provided arguments.
     * @tparam ...ArgsT The types of the arguments to pass. Must be constructible from the input values.
     * @return The result of the function call.
     * @example
     * 
     * Alt::StaticFunc<64, 16, usize, f32> func = [](const f32) noexcept -> usize {
     *     return 0
     * };
     * 
     * assert((func)(), 0)
     */
    template <typename ...ArgsT,
        std::enable_if_t<std::conjunction_v<std::is_constructible<Args, ArgsT&&>...>>* = nullptr>
    ALT_FORCE_INLINE Return operator()(ArgsT&&... args) noexcept {
        ALT_ASSERT(m_pFTable);

        return (*m_pOperatorBracketBracket)(m_Memory, std::forward<ArgsT>(args)...);
    }

    template <typename ...ArgsT,
        std::enable_if_t<std::conjunction_v<std::is_constructible<Args, ArgsT&&>...>>* = nullptr>
    ALT_FORCE_INLINE Return operator()(ArgsT&&... args) const noexcept {
        ALT_ASSERT(m_pFTable);

        return (*m_pOperatorBracketBracket)(m_Memory, std::forward<ArgsT>(args)...);
    }

    ~StaticFunc() {

        if (!m_pFTable) {
            return;
        }

        (*m_pFTable->pDestruct)(m_Memory);
        m_pFTable = nullptr;
    }

private:

    ALT_FORCE_INLINE StaticFunc() noexcept = default;

    template <typename FuncT>
    ALT_FORCE_INLINE void Construct(FuncT&& func) noexcept {

        using T = TypeTraits::clean_t<std::remove_reference_t<FuncT>>;

        m_pOperatorBracketBracket = _FuncDetails::OperatorBracketBracketFunc<FuncT, Return, Args...>();
        m_pFTable = _FuncDetails::GetOrCreateFTable<FuncT, Return, Args...>();

        void* pMemory = Memory::AlignPtr(m_Memory, alignof(T));
        Memory::ConstructObject<T>(reinterpret_cast<T*>(pMemory), std::forward<FuncT>(func));
    }

    template <typename FuncT>
    ALT_FORCE_INLINE void Assign(FuncT&& func) noexcept {
        ALT_ASSERT(m_pFTable);

        (*m_pFTable->pDestruct)(m_Memory);

        Construct(std::forward<FuncT>(func));
    }

    template <usize SIZE_IN_BYTES_, usize ALIGN_IN_BYTES_,
        std::enable_if_t<(SIZE_IN_BYTES_ + ALIGN_IN_BYTES_) <= SIZE_IN_BYTES + ALIGN_IN_BYTES>* = nullptr>
    ALT_FORCE_INLINE void Construct(Rule5Move_t, StaticFunc<SIZE_IN_BYTES_, ALIGN_IN_BYTES_, Return, Args...>&& rhs) noexcept {

        m_pOperatorBracketBracket = Exchange(rhs.m_pOperatorBracketBracket, nullptr);
        m_pFTable = Exchange(rhs.m_pFTable, nullptr);

        (*m_pFTable->pMoveConstruct)(m_Memory, rhs.m_Memory);
    }

    template <usize SIZE_IN_BYTES_, usize ALIGN_IN_BYTES_,
        std::enable_if_t<(SIZE_IN_BYTES_ + ALIGN_IN_BYTES_) <= SIZE_IN_BYTES + ALIGN_IN_BYTES>* = nullptr>
    ALT_FORCE_INLINE void Assign(Rule5Move_t, StaticFunc<SIZE_IN_BYTES_, ALIGN_IN_BYTES_, Return, Args...>&& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return;
        }

        (*m_pFTable->pDestruct)(m_Memory);

        m_pOperatorBracketBracket = Exchange(rhs.m_pOperatorBracketBracket, nullptr);
        m_pFTable = Exchange(rhs.m_pFTable, nullptr);

        (*m_pFTable->pMoveConstruct)(m_Memory, rhs.m_Memory);
    }

private:

    template <usize, usize, typename, typename...>
    friend class StaticFunc;

    template <typename , typename...>
    friend class FuncRef;

    FuncPtr<Return, void*, Args...> m_pOperatorBracketBracket;
    _FuncDetails::FTable<Return, Args...>* m_pFTable;

    alignas(ALIGN_IN_BYTES) u8 m_Memory[SIZE_IN_BYTES];
};

} // namespace Alt

#endif // __ALT_STD_FUNC__