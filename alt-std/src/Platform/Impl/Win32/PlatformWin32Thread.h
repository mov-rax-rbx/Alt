#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32_THREAD__
#define __ALT_STD_PLATFORM_WIN32_THREAD__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32.h"

namespace Alt {
namespace _PlatformDetails {

class Thread;

} // namespace _PlatformDetails

namespace TypeTraits {

template <>
struct __is_trivially_reallocatable__<_PlatformDetails::Thread>
    : std::true_type
{};

} // namespace TypeTraits

/*
 * Win32 function for thread should return u32 value
 */
#define ALT_THREAD_RETURN_SUCCESS return __Win32__ERROR_SUCCESS

namespace _PlatformDetails {

/**
 * @brief Represents a native OS thread and provides controls over thread.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::Thread thread = Alt::Thread::Create([]() noexcept {
 * 
 *     ALT_PRINTLN("Hello from another thread");
 * }) iferr_return;
 * 
 * thread.WaitCompletion() iferr_return;
 */
class Thread {
private:

    ALT_MAKE_CLASS_NON_COPYABLE(Thread);

public:

    /**
     * The return value of function for thread.
     */
    using NativeReturnType = u32;

    /**
     * The declaration for a native function pointer which used by OS.
     */
    using NativeFuncPtr = FuncPtr<NativeReturnType, void*>;

    /**
     * @brief Configuration description for thread creation.
     */
    struct CreateDesc {

        /**
         * @brief The stack size to allocate for the new thread.
         * @details If Nullopt, the system default stack size is used.
         */
        Optional<usize> stackSize = Nullopt;
    };

    /**
     * @brief Enumeration of supported platform-specific thread priorities.
     */
    enum class Priority : s32 {

        Idle = static_cast<s32>(Platform::ThreadPriority::Idle),

        Low = static_cast<s32>(Platform::ThreadPriority::Low),
        Med = static_cast<s32>(Platform::ThreadPriority::Med),
        Norm = static_cast<s32>(Platform::ThreadPriority::Norm),
        High = static_cast<s32>(Platform::ThreadPriority::High),

        Highest = static_cast<s32>(Platform::ThreadPriority::Highest),

        Critical = static_cast<s32>(Platform::ThreadPriority::Critical),
    };

    /**
     * @brief Description of thread priority and boost settings.
     */
    struct PriorityDesc {

        /** @brief The target priority level. */
        Priority priority = Priority::Norm;

        /** @brief Whether to enable the priority boost for the thread. */
        bool isBoostEnabled = false;
    };

    /**
     * @brief Yields the current thread's execution to the OS scheduler.
     * @return True if the thread was rescheduled
     * @example
     * 
     * Alt::Thread::Reschedule();
     */
    ALT_FORCE_INLINE static bool Reschedule() noexcept {

        return Platform::ThreadSwitchToAnother();
    }

    /**
     * @brief Get thread unique ID.
     * @return The system thread ID.
     * @example
     * 
     * const u32 threadID = Alt::Thread::GetCurrentID();
     */
    ALT_NODISCARD ALT_FORCE_INLINE static u32 GetCurrentID() noexcept {

        return Platform::ThreadGetCurrentID();
    }

    /**
     * @brief Updates the execution priority and boost status of the current thread.
     * @param desc A struct containing the target Priority and boolean for boost enabled.
     * @example
     * 
     * PriorityDesc desc{};
     * desc.priority = Thread::Priority::Highest;
     * 
     * Thread::SetCurrentPriority(desc);
     */
    ALT_FORCE_INLINE static void SetCurrentPriority(const PriorityDesc& desc, const Optional<u64> logicalProcessorsMask = Nullopt) noexcept {

        const Platform::Handle handle = Platform::ThreadGetCurrentPseudoHandle();

        ALT_MAYBE_UNUSED Result<void, Platform::Error> err;

        err = Platform::ThreadSetPriority(handle, static_cast<Platform::ThreadPriority>(desc.priority));
        err = Platform::ThreadSetPriorityBoost(handle, desc.isBoostEnabled);

        if (logicalProcessorsMask) {
            err = Platform::ThreadSetAffinityMask(handle, *logicalProcessorsMask);
        }
    }

    /**
     * @brief Retrieves the current priority and boost status of the current thread.
     * @return A PriorityDesc struct containing both values.
     */
    ALT_NODISCARD ALT_FORCE_INLINE static PriorityDesc GetCurrentPriority() noexcept {
        iferr_scope;

        const Platform::Handle handle = Platform::ThreadGetCurrentPseudoHandle();

        PriorityDesc desc{};
        desc.priority = static_cast<Priority>(Platform::ThreadGetPriority(handle));
        desc.isBoostEnabled = Platform::ThreadGetPriorityBoost(handle) iferr_panic_only_dev("just ignore - it is ok to set default 'false'");

        return desc;
    }

    /**
     * @brief Suspends the execution of the current thread until the time-out interval elapses.
     * @param milliseconds The number of milliseconds to sleep.
     * @example
     * 
     * Alt::Thread::Sleep(16);
     */
    ALT_FORCE_INLINE static void Sleep(const usize milliseconds) noexcept {

        Platform::SleepMs(milliseconds);
    }

    /**
     * @brief Determines the number of logical processors available on the system.
     * @return The count of logical cores.
     * @example
     * 
     * const usize totalNumThreads = Alt::Thread::GetNumOfLogicalProcessors();
     */
    ALT_NODISCARD ALT_FORCE_INLINE static usize GetNumOfLogicalProcessors() noexcept {

        return Platform::ThreadGetNumOfLogicalProcessors();
    }

    /**
     * @brief Allocates and spawn a new thread using a provided callable object.
     * 
     * @details The provided `FuncT` is allocated via `Memory::AllocateObject`. 
     * The memory for the object is automatically deallocated once the thread's 
     * entry point finishes execution.
     * 
     * @tparam FuncT A callable type (lambda, functor, ...) that owns its state.
     * @param func The callable object to be executed by the new thread.
     * @return An Optional containing the new Thread object on success, or Nullopt if creation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Thread thread = Alt::Thread::Allocate([]() {
     *     Alt::Thread::Sleep(100);
     * }) iferr_return;
     * 
     */
    template <typename FuncT>
    ALT_NODISCARD static Optional<Thread> Allocate(FuncT&& func) noexcept {

        FuncT* pFuncObj = nullptr;
        iferr_scope_on_err {

            Memory::DeallocateObject(pFuncObj);
        };

        pFuncObj = Memory::AllocateObject<FuncT>(std::forward<FuncT>(func)) iferr_return Nullopt;

        Thread thread{};

        thread.m_Handle = Platform::ThreadCreate([](void* pArgs) noexcept -> NativeReturnType {

            FuncT* pFuncObj = reinterpret_cast<FuncT*>(pArgs);
            defer{ Memory::DeallocateObject(pFuncObj); };

            (*pFuncObj)();

            ALT_THREAD_RETURN_SUCCESS;
        }, reinterpret_cast<void*>(pFuncObj)) iferr_return Nullopt;

        return thread;
    }

    /**
     * @brief Creates a new thread using a function pointer.
     * @param pFunc A pointer to the function to be executed by the new thread.
     * @return An Optional containing the new Thread object on success, or Nullopt if creation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Thread thread = Alt::Thread::Create([]() {
     *     Alt::Thread::Sleep(100);
     * }) iferr_return;
     * 
     */
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<Thread> Create(FuncPtr<void> pFunc) noexcept {
        iferr_scope;

        Thread thread{};

        thread.m_Handle = Platform::ThreadCreate([](void* pArgs) noexcept -> NativeReturnType {

            FuncPtr<void> pFunc = reinterpret_cast<FuncPtr<void>>(pArgs);
            (*pFunc)();

            ALT_THREAD_RETURN_SUCCESS;
        }, reinterpret_cast<void*>(pFunc)) iferr_return Nullopt;

        return thread;
    }

    /**
     * @brief Creates a new thread from allocated or pinned callable object.
     * @details The provided `FuncT` must has lifetime >= then thread itself.
     * @tparam FuncT A callable type (lambda, functor, ...) that owns its state.
     * @param pFuncobj The callable object to be executed by the new thread.
     * @return An Optional containing the new Thread object on success, or Nullopt if creation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * struct JustSomeFunctor {
     * 
     *     void operator()() noexcept {
     *     }
     * 
     *     bool state = false;
     * };
     * 
     * static JustSomeFunctor g_Functor = JustSomeFunctor{};
     * 
     * Alt::Thread thread = Alt::Thread::CreateFromPinned(&g_Functor) iferr_return;
     * 
     */
    template <typename FuncT>
    ALT_NODISCARD static Optional<Thread> CreateFromPinned(FuncT* pFuncObj) noexcept {
        iferr_scope;

        Thread thread{};

        thread.m_Handle = Platform::ThreadCreate([](void* pArgs) noexcept -> NativeReturnType {

            FuncT* pFuncObj = reinterpret_cast<FuncT*>(pArgs);
            (*pFuncObj)();

            ALT_THREAD_RETURN_SUCCESS;
        }, reinterpret_cast<void*>(pFuncObj)) iferr_return Nullopt;

        return thread;
    }

    template <typename FuncT>
    ALT_NODISCARD static Optional<Thread> CreateFromPinned(const FuncT* pFuncObj) noexcept {
        iferr_scope;

        Thread thread{};

        thread.m_Handle = Platform::ThreadCreate([](void* pArgs) noexcept -> NativeReturnType {

            const FuncT* pFuncObj = reinterpret_cast<const FuncT*>(pArgs);
            (*pFuncObj)();

            ALT_THREAD_RETURN_SUCCESS;
        }, reinterpret_cast<const void*>(pFuncObj)) iferr_return Nullopt;

        return thread;
    }

    /**
     * @brief Creates a new thread using a native function pointer.
     * @param pFunc A pointer to the naative function to be executed by the new thread.
     * @param pArgsg A argument whic passed to function as payload.
     * @return An Optional containing the new Thread object on success, or Nullopt if creation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Thread thread = Alt::Thread::CreateFromNativeFuncPtr([](void*) noexcept -> Alt::Thread::NativeReturnType {
     *     Alt::Thread::Sleep(100);
     * 
     *     ALT_THREAD_RETURN_SUCCESS;
     * }, nullptr) iferr_return;
     * 
     */
    ALT_NODISCARD static Optional<Thread> CreateFromNativeFuncPtr(NativeFuncPtr pFunc, void* pArgs) noexcept {
        iferr_scope;

        Thread thread{};
        thread.m_Handle = Platform::ThreadCreate(pFunc, pArgs) iferr_return Nullopt;

        return thread;
    }

    ALT_FORCE_INLINE Thread(Thread&& rhs) noexcept
        : m_Handle(Exchange(rhs.m_Handle, Platform::Handle{}))
    {}

    ALT_FORCE_INLINE Thread& operator=(Thread&& rhs) noexcept {

        m_Handle = Exchange(rhs.m_Handle, Platform::Handle{});
        return *this;
    }

    /**
     * @brief Marks the thread as "forgotten".
     * @details Modifies the handle's internal tag to indicate that the 
     * object should not wait for completion upon destruction.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Thread thread = Alt::Thread::Create([]() {
     *     Alt::Thread::Sleep(100);
     * }) iferr_return;
     * 
     * thread.Forget();
     * 
     * // On destructor we don't wait for completion
     * 
     */
    ALT_FORCE_INLINE void Forget() noexcept {

        /*
         * https://devblogs.microsoft.com/oldnewthing/20050121-00/?p=36633/
         */
        static_assert(__Win32__OBJ_HANDLE_TAGBITS != 0, "");

        m_Handle.handle = reinterpret_cast<HANDLE>(reinterpret_cast<uptr>(m_Handle.handle) | 0b1);
    }

    /**
     * @brief Checks if the thread has been marked as "forgotten".
     * @return True if the handle tag indicates a "forgotten" state, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsForgotten() const noexcept {

        static_assert(__Win32__OBJ_HANDLE_TAGBITS != 0, "");

        return (reinterpret_cast<uptr>(m_Handle.handle) & 0b1) != 0;
    }

    /**
     * @brief Blocks the current thread until the represented thread completes execution.
     * @return True if completion was signaled, false otherwise.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Thread thread = Alt::Thread::Create([]() {
     *     Alt::Thread::Sleep(100);
     * }) iferr_return;
     * 
     * const bool isCompleted = thread.WaitCompletion();
     * assert(true);
     */
    ALT_FORCE_INLINE bool WaitCompletion() const noexcept {
        iferr_scope;

        const Platform::WaitForSingleObjectResult result = Platform::WaitForSingleObject(m_Handle, __Win32__INFINITE)
            iferr_return false;

        return result == Platform::WaitForSingleObjectResult::Signaled;
    }

    /**
     * @brief Get thread unique ID.
     * @return The system thread ID.
     */
    ALT_FORCE_INLINE u32 GetThreadID() const noexcept {

        return Platform::ThreadGetIDByHandle(m_Handle);
    }

    /**
     * @brief Updates the execution priority and boost status of the thread.
     * @param desc A struct containing the target Priority and boolean for boost enabled.
     */
    ALT_FORCE_INLINE void SetPriority(const PriorityDesc& desc, const Optional<u64> logicalProcessorsMask = Nullopt) const noexcept {

        ALT_MAYBE_UNUSED Result<void, Platform::Error> err;

        err = Platform::ThreadSetPriority(m_Handle, static_cast<Platform::ThreadPriority>(desc.priority));
        err = Platform::ThreadSetPriorityBoost(m_Handle, desc.isBoostEnabled);

        if (logicalProcessorsMask) {
            err = Platform::ThreadSetAffinityMask(m_Handle, *logicalProcessorsMask);
        }
    }

    /**
     * @brief Retrieves the current priority and boost status of the thread.
     * @return A PriorityDesc struct containing both values.
     */
    ALT_NODISCARD ALT_FORCE_INLINE PriorityDesc GetPriority() const noexcept {
        iferr_scope;

        PriorityDesc desc{};
        desc.priority = static_cast<Priority>(Platform::ThreadGetPriority(m_Handle));
        desc.isBoostEnabled = Platform::ThreadGetPriorityBoost(m_Handle) iferr_panic_only_dev("just ignore - it is ok to set default 'false'");

        return desc;
    }

    /**
     * @brief Sets the name/description for the thread.
     * @param name A slice of characters representing the new name.
     * @return True if successfully set.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Thread thread = Alt::Thread::Create([]() {
     *     Alt::Thread::Sleep(100);
     * }) iferr_return;
     * 
     * thread.SetName("Alt Thread");
     * 
     */
    ALT_FORCE_INLINE bool SetName(const Slice<const char> name) const noexcept {
        iferr_scope;

        Platform::ThreadSetName(m_Handle, name) iferr_return false;

        return true;
    }

    ALT_FORCE_INLINE bool SetName(const char* pName) const noexcept {
        iferr_scope;

        Platform::ThreadSetName(m_Handle, pName) iferr_return false;

        return true;
    }

    /**
     * @brief Retrieves the current thread name.
     * @param buffer A String object where the appended result will be stored.
     * @return True if successfully appended.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Thread thread = Alt::Thread::Create([]() {
     *     Alt::Thread::Sleep(100);
     * }) iferr_return;
     * 
     * thread.SetName("Alt Thread");
     * 
     * Alt::String name = thread.GetName() iferr_return;
     * assert(name == "Alt Thread"_s);
     */
    ALT_FORCE_INLINE bool GetNameAppend(String& buffer) const noexcept {
        iferr_scope;

        Platform::ThreadGetNameAppend(m_Handle, buffer) iferr_return false;

        return true;
    }

    ALT_FORCE_INLINE bool GetName(String& buffer) const noexcept {
        iferr_scope;

        Platform::ThreadGetName(m_Handle, buffer) iferr_return false;

        return true;
    }

    ALT_FORCE_INLINE Optional<String> GetName() const noexcept {
        iferr_scope;

        String name = Platform::ThreadGetName(m_Handle) iferr_return Nullopt;

        return name;
    }

    ~Thread() noexcept {
        iferr_scope;

        if (m_Handle.handle == __Win32__INVALID_HANDLE_VALUE) {
            return;
        }

        if (!IsForgotten()) {

            WaitCompletion();
        }

        Platform::ThreadClose(m_Handle) iferr_panic_only_dev("Just allow to fail. The system will close all handles on process exit.");
        m_Handle = Platform::Handle{};
    }

private:

    ALT_FORCE_INLINE Thread() noexcept = default;
    ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(Thread);

private:

    Platform::Handle m_Handle;
};

} // namespace _PlatformDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32_THREAD__