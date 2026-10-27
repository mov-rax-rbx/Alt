#pragma once

#ifndef __ALT_STD_ARENA_ALLOCATOR_VIRTUAL__
#define __ALT_STD_ARENA_ALLOCATOR_VIRTUAL__

#include "../../TypeTraits.h"

#include "../../Memory/Impl/MemoryPreInclude.h"

#include "../../Utils/Num.h"
#include "../../Utils/Optional.h"

namespace Alt {

/**
 * @brief Templated version of PoolAllocatorVirtual.
 * 
 * @tparam T The type of the object.
 * @param MIN_NUM_ELEMENTS Minimum initial number of elements (rounded to the page size).
 * @param MAX_NUM_ELEMENTS_TO_RESERVE Maximum number of elements (rounded to the page size).
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * using Pool = Alt::PoolAllocatorVirtual<usize, 64>;
 * 
 * Pool pool = Pool::CreateCapacity() iferr_return;
 * 
 * usize* pVal = pool.Allocate<usize>(4);
 * assert(pVal);
 * 
 * *pVal = 44;
 * 
 * pool.Deallocate(pVal);
 * 
 * ...
 * 
 * pool.ReleaseBytesAll(); // useful to shrink unused memory in pool (but after all pool object was destructed)
 */
template <
    typename T,
    usize MIN_NUM_ELEMENTS = 16,
    usize MAX_NUM_ELEMENTS_TO_RESERVE = ALT_PLATFORM_32BIT_BOOL
        ? Memory::GB(2) / Num::Max(sizeof(T), sizeof(uptr))
        : Memory::TB(1) / Num::Max(sizeof(T), sizeof(uptr))
>
class PoolAllocatorVirtual;

/**
 * @brief Version of "dynamic" PoolAllocatorVirtualDynamic - store object memory layout
 * description with pool.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * using Pool = Alt::PoolAllocatorDynamicDynamic;
 * 
 * // It is virtual memory we need to reserve some "virtual" memory - this memory will be used further
 * Pool::MemoryDesc desc = Pool::MemoryDesc::Create<Entity>();
 * desc.maxNumElementsReserved = Memory::TB(1) / sizeof(Entity);
 * 
 * // Important to call 'CreateCapacity(...)'
 * Pool pool = Pool::CreateCapacity(desc) iferr_return;
 * 
 * {
 *     Entity* pEntity = pool.Allocate<Entity>("Text Label"_salloc);
 *     assert(pEntity);
 * 
 *     pEntity->SetText("Pool allocated Entity"_salloc);
 * 
 *     pool.Deallocate(pEntity); // or pEntity->~Entity(); pool.DeallocateBytes(pEntity);
 * }
 * 
 * pool.DeallocateBytesExceptFirstBlockAndReset(); // only deallocates pool memory
 */
class PoolAllocatorVirtualDynamic;

namespace TypeTraits {

template <typename T, usize MIN_NUM_ELEMENTS, usize MAX_NUM_ELEMENTS_TO_RESERVE>
struct __is_trivially_reallocatable__<PoolAllocatorVirtual<T, MIN_NUM_ELEMENTS, MAX_NUM_ELEMENTS_TO_RESERVE>>
    : std::true_type
{};

template <>
struct __is_trivially_reallocatable__<PoolAllocatorVirtualDynamic>
    : std::true_type
{};

} // namespace TypeTraits

namespace _PoolAllocatorVirtualDetails {

template <typename SelfT>
class PoolAllocatorVirtualImpl {
private:

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

protected:

    ALT_FORCE_INLINE static Optional<PoolAllocatorVirtualImpl> CreateCapacity(const usize maxSizeInBytesReserved) noexcept {

        PoolAllocatorVirtualImpl arena{};

        u8* pMemory = Platform::VirtualMemoryAllocateBytes(
            nullptr,
            maxSizeInBytesReserved,
            Platform::VirtualMemoryAllocateBytesBits::Reserve,
            Platform::VirtualMemoryAccessBits::ReadWrite
        );

        if (!pMemory) {
            return Nullopt;
        }

        arena.m_pMemory = pMemory;

        return arena;
    }

public:

    /**
     * @brief Represents usage statistics for the allocator.
     */
    struct Stats {

        usize consumedElements = 0;   /**< Total number of elements currently in use. */
        usize availableElements = 0;  /**< Total number of elements available for allocation. */

        /**
         * @brief Calculates the total capacity of the allocator.
         * @return The sum of consumed and available elements.
         */
        ALT_NODISCARD ALT_FORCE_INLINE usize GetTotalElements() const noexcept {
            return consumedElements + availableElements;
        }
    };

    ALT_FORCE_INLINE static usize GetGrowFactor() noexcept {
        return 2;
    }

    ALT_FORCE_INLINE usize GetElementSizeOf() const noexcept {
        return static_cast<const SelfT*>(this)->GetElementSizeOf();
    }

    ALT_FORCE_INLINE usize GetElementAlignOf() const noexcept {
        return static_cast<const SelfT*>(this)->GetElementAlignOf();
    }

    ALT_FORCE_INLINE usize GetMinElementsSizeInBytes() const noexcept {
        return static_cast<const SelfT*>(this)->GetMinElementsSizeInBytes();
    }

    ALT_FORCE_INLINE usize GetMaxElementsSizeInBytesReserved() const noexcept {
        return static_cast<const SelfT*>(this)->GetMaxElementsSizeInBytesReserved();
    }

    ALT_FORCE_INLINE PoolAllocatorVirtualImpl() noexcept = default;

    PoolAllocatorVirtualImpl(const PoolAllocatorVirtualImpl&) noexcept = delete;
    PoolAllocatorVirtualImpl& operator=(const PoolAllocatorVirtualImpl&) noexcept = delete;

    PoolAllocatorVirtualImpl(PoolAllocatorVirtualImpl&& rhs) noexcept
        : PoolAllocatorVirtualImpl(Rule5Move, rhs)
    {}

    PoolAllocatorVirtualImpl& operator=(PoolAllocatorVirtualImpl&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    ~PoolAllocatorVirtualImpl() noexcept {

        Destroy();
    }

    /**
     * @brief Allocates and constructs an object of type T.
     * 
     * @tparam T The type to allocate.
     * @tparam Args Constructor arguments for T.
     * @param args Arguments to pass to the constructor.
     * @return A pointer to the constructed object, or `nullptr` if allocation fails.
     * 
     * @note Constraints: `sizeof(T)` must be $\le$ `GetElementSizeOf()` and `alignof(T)` must be $\le$ `GetElementAlignOf()`.
     * @example
     * 
     * iferr_scope;
     * 
     * using Pool = Alt::PoolAllocatorVirtual<usize>;
     * 
     * Pool pool = Pool::CreateCapacity() iferr_return;
     * usize* pVal = pool.Allocate<usize>(42);
     * assert(pVal && *pVal == 42);
     * 
     * Alt::PoolAllocatorVirtual<Alt::String> pool = Alt::PoolAllocatorVirtual<Alt::String>() iferr_return;
     * Alt::String* pVal = pool.Allocate<Alt::String>("Pool"_salloc);
     * assert(pVal && *pVal == "Pool");
     */
    template <typename T, typename ...Args>
    ALT_NODISCARD T* Allocate(Args&&... args) noexcept(std::is_nothrow_constructible_v<T>) {

        ALT_ASSERT(sizeof(T) <= GetElementSizeOf());
        ALT_ASSERT(alignof(T) <= GetElementAlignOf());

        u8* pMemory = AllocateBytes();
        if (!pMemory) {
            return nullptr;
        }

        return Memory::ConstructObject<T>(pMemory, std::forward<Args>(args)...);
    }

    /**
     * @brief Allocates an object of type T.
     * 
     * @tparam T The type to allocate.
     * @param data The object to move-construct into the allocated memory.
     * @return A pointer to the constructed object, or `nullptr` if allocation fails.
     * 
     * @note Constraints: `sizeof(T)` must be $\le$ `GetElementSizeOf()` and `alignof(T)` must be $\le$ `GetElementAlignOf()`.
     * @example
     * 
     * iferr_scope;
     * 
     * using Pool = Alt::PoolAllocatorVirtual<usize>;
     * 
     * Pool pool = Pool::CreateCapacity() iferr_return;
     * 
     * Alt::String str = "Pool"_salloc;
     * Alt::String* pVal = pool.Allocate<Alt::String>(std::move(str));
     * 
     * assert(pVal && *pVal == "Pool");
     */
    template <typename T>
    ALT_NODISCARD T* Allocate(T&& data) noexcept(std::is_nothrow_move_constructible_v<T>) {

        ALT_ASSERT(sizeof(T) <= GetElementSizeOf());
        ALT_ASSERT(alignof(T) <= GetElementAlignOf());

        u8* pMemory = AllocateBytes();
        if (!pMemory) {
            return nullptr;
        }

        return Memory::ConstructObject<T>(pMemory, std::forward<T>(data));
    }

    /**
     * @brief Destructs and deallocates an object.
     * 
     * @tparam T The type of the object.
     * @param pMemory Pointer to the object to deallocate.
     * @example
     * 
     * iferr_scope;
     * 
     * using Pool = Alt::PoolAllocatorVirtual<usize>;
     * 
     * Pool pool = Pool::CreateCapacity() iferr_return;
     * Alt::String* pVal = pool.Allocate<Alt::String>("Pool"_salloc);
     * 
     * pool.Deallocate(pVal);
     */
    template <typename T>
    ALT_FORCE_INLINE void Deallocate(T* pMemory) noexcept(std::is_nothrow_destructible_v<T>) {

        if (!pMemory) {
            return;
        }

        Memory::DestroyObject(*pMemory);
        DeallocateBytes(pMemory);
    }

    /**
     * @brief Allocates memory from the pool.
     * @return A pointer to the allocated memory, or `nullptr` if allocation fails.
     */
    ALT_NODISCARD u8* AllocateBytes() noexcept {

        // Reserve memory with 'CreateCapacity(...)'
        ALT_ASSERT(m_pMemory);

        if (m_FreePtr != 0) {

            u8* pMemory = reinterpret_cast<u8*>(m_FreePtr);
            m_FreePtr = *reinterpret_cast<uptr*>(m_FreePtr);

            return pMemory;
        }

        const usize elementSizeOf = GetElementSizeOf();
        const usize elementAlignOf = GetElementAlignOf();

        if (elementSizeOf <= m_AvailableSizeInBytes) {

            u8* pMemory = m_pMemory;

            m_pMemory += elementSizeOf;
            m_AvailableSizeInBytes -= elementSizeOf;

            return pMemory;
        }

        const usize minSizeInBytes = GetMinElementsSizeInBytes();
        const usize maxSizeInBytesReserved = GetMaxElementsSizeInBytesReserved();

        const usize sizeInBytesPerfectForNextGrow = Num::MulWrap(m_TotalSizeInBytes, GetGrowFactor());
        const usize sizeInBytesPerfectGrow = Num::Clamp(sizeInBytesPerfectForNextGrow, minSizeInBytes, maxSizeInBytesReserved);

        const bool isFirstAllocation = m_TotalSizeInBytes == 0;
        const usize sizeInBytesOnDemand = elementSizeOf + (isFirstAllocation ? elementAlignOf - 1 : 0) - m_AvailableSizeInBytes;

        usize totalSizeInBytes;
        if (Num::IsOverflowCheckAdd(m_TotalSizeInBytes, sizeInBytesOnDemand, &totalSizeInBytes)) {
            return nullptr;
        }

        const usize totalSizeInBytesPerfectGrow = Num::AddWrap(m_TotalSizeInBytes, sizeInBytesPerfectGrow, maxSizeInBytesReserved);

        totalSizeInBytes = totalSizeInBytesPerfectGrow >= totalSizeInBytes
            ? totalSizeInBytesPerfectGrow
            : Num::Max(Num::Min(Memory::RoundUpToPageSizeWrap(totalSizeInBytes), maxSizeInBytesReserved), totalSizeInBytes);

        return AllocateVirtualMemory(totalSizeInBytes);
    }

    /**
     * @brief Returns memory to the pool free list.
     * @param pMemory Pointer to the memory to free.
     * @note Must be called after object destruction
     */
    ALT_FORCE_INLINE void DeallocateBytes(void* pMemory) noexcept {

        *reinterpret_cast<uptr*>(pMemory) = m_FreePtr;
        m_FreePtr = reinterpret_cast<uptr>(pMemory);
    }

    /**
     * @brief Decommits all memory in the pool.
     * @note Must be called after object destruction
     */
    ALT_FORCE_INLINE void ReleaseBytesAll() noexcept {

        const usize offset = m_TotalSizeInBytes - m_AvailableSizeInBytes;
        Platform::VirtualMemoryDeallocateBytes(m_pMemory - offset, 0, Platform::VirtualMemoryDeallocateBytesBits::Decommit);

        m_FreePtr = 0;

        m_AvailableSizeInBytes = 0;
        m_TotalSizeInBytes = 0;
    }

    /**
     * @brief Retrieves current usage statistics.
     * @return A `Stats` struct with statistics.
     */
    ALT_NODISCARD Stats GetStats() const noexcept {

        const usize elementSizeOf = GetElementSizeOf();

        Stats stats{};

        uptr freePtr = m_FreePtr;
        while (freePtr != 0) {

            stats.availableElements += 1;
            freePtr = *reinterpret_cast<uptr*>(freePtr);
        }

        stats.availableElements += m_AvailableSizeInBytes / elementSizeOf;
        stats.consumedElements = m_TotalSizeInBytes / elementSizeOf - stats.availableElements;

        return stats;
    }

private:

    ALT_FORCE_INLINE u8* AllocateVirtualMemory(const usize totalSizeInBytesOnDemand) noexcept {

        const usize elementSizeOf = GetElementSizeOf();
        const usize elementAlignOf = GetElementAlignOf();

        const usize size = totalSizeInBytesOnDemand - m_TotalSizeInBytes;

        u8* pMemory = Platform::VirtualMemoryAllocateBytes(
            m_pMemory + m_AvailableSizeInBytes,
            size,
            Platform::VirtualMemoryAllocateBytesBits::Commit,
            Platform::VirtualMemoryAccessBits::ReadWrite
        );

        if (!pMemory) {
            return nullptr;
        }

        ALT_ASSERT(totalSizeInBytesOnDemand <= GetMaxElementsSizeInBytesReserved());

        const usize alignmentOffset = Memory::Align(m_pMemory, elementAlignOf);
        ALT_ASSERT(m_TotalSizeInBytes == 0 || m_TotalSizeInBytes != 0 && alignmentOffset == 0);

        pMemory = m_pMemory;
        usize availableSizeInBytes = m_AvailableSizeInBytes + size;

        u8* pMemoryElement = pMemory + alignmentOffset;
        pMemory += alignmentOffset + elementSizeOf;
        availableSizeInBytes -= alignmentOffset + elementSizeOf;

        m_pMemory = pMemory;
        m_AvailableSizeInBytes = availableSizeInBytes;
        m_TotalSizeInBytes = totalSizeInBytesOnDemand;

        return pMemoryElement;
    }

    ALT_FORCE_INLINE PoolAllocatorVirtualImpl(Rule5Move_t, PoolAllocatorVirtualImpl& rhs) noexcept
        : m_pMemory(Exchange(rhs.m_pMemory, nullptr))
        , m_FreePtr(Exchange(rhs.m_FreePtr, 0))
        , m_TotalSizeInBytes(Exchange(rhs.m_TotalSizeInBytes, 0))
        , m_AvailableSizeInBytes(Exchange(rhs.m_AvailableSizeInBytes, 0))
    {}

    ALT_FORCE_INLINE void Assign(Rule5Move_t, PoolAllocatorVirtualImpl& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        if (m_pMemory) {

            const usize offset = m_TotalSizeInBytes - m_AvailableSizeInBytes;
            Platform::VirtualMemoryDeallocateBytes(m_pMemory - offset, 0, Platform::VirtualMemoryDeallocateBytesBits::Release);
        }

        m_pMemory = Exchange(rhs.m_pMemory, nullptr);
        m_FreePtr = Exchange(rhs.m_FreePtr, 0);
        m_TotalSizeInBytes = Exchange(rhs.m_TotalSizeInBytes, 0);
        m_AvailableSizeInBytes = Exchange(rhs.m_AvailableSizeInBytes, 0);
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        if (!m_pMemory) {
            return;
        }

        const usize offset = m_TotalSizeInBytes - m_AvailableSizeInBytes;
        Platform::VirtualMemoryDeallocateBytes(m_pMemory - offset, 0, Platform::VirtualMemoryDeallocateBytesBits::Release);

        m_pMemory = nullptr;
    }

private:

    u8* m_pMemory = nullptr;
    uptr m_FreePtr = 0;

    // approx ~ 2^0 + 2^1 + 2^2 + ... = 2^(n + 1) - 1
    usize m_TotalSizeInBytes = 0;
    usize m_AvailableSizeInBytes = 0;
};

} // namespace _PoolAllocatorVirtualDetails

/**
 * @brief Templated version of PoolAllocatorVirtual.
 * 
 * @tparam T The type of the object.
 * @param MIN_NUM_ELEMENTS Minimum initial number of elements (rounded to the page size).
 * @param MAX_NUM_ELEMENTS_TO_RESERVE Maximum number of elements (rounded to the page size).
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * using Pool = Alt::PoolAllocatorVirtual<usize, 64>;
 * 
 * Pool pool = Pool::CreateCapacity() iferr_return;
 * 
 * usize* pVal = pool.Allocate<usize>(4);
 * assert(pVal);
 * 
 * *pVal = 44;
 * 
 * pool.Deallocate(pVal);
 * 
 * ...
 * 
 * pool.ReleaseBytesAll(); // useful to shrink unused memory in pool (but after all pool object was destructed)
 */
template <typename T, usize MIN_NUM_ELEMENTS, usize MAX_NUM_ELEMENTS_TO_RESERVE>
class PoolAllocatorVirtual
    : public _PoolAllocatorVirtualDetails::PoolAllocatorVirtualImpl<PoolAllocatorVirtual<T, MIN_NUM_ELEMENTS, MAX_NUM_ELEMENTS_TO_RESERVE>>
{
private:

    static_assert(MIN_NUM_ELEMENTS <= MAX_NUM_ELEMENTS_TO_RESERVE, "");

    using Super = _PoolAllocatorVirtualDetails::PoolAllocatorVirtualImpl<
        PoolAllocatorVirtual<T, MIN_NUM_ELEMENTS, MAX_NUM_ELEMENTS_TO_RESERVE>
    >;

public:

    ALT_NODISCARD static Optional<PoolAllocatorVirtual> CreateCapacity() noexcept {

        PoolAllocatorVirtual self{};

        Optional<Super> arena = Super::CreateCapacity(self.m_MaxElementsSizeInBytesReserved);
        if (!arena) {
            return Nullopt;
        }

        static_cast<Super&>(self) = std::move(*arena);

        return self;
    }

    ALT_FORCE_INLINE PoolAllocatorVirtual() noexcept {

        const usize minElementsSizeInBytes = Num::MulWrap(MIN_NUM_ELEMENTS, GetElementSizeOf());
        const usize maxElementsSizeInBytes = Num::MulWrap(MAX_NUM_ELEMENTS_TO_RESERVE, GetElementSizeOf());

        m_MinElementsSizeInBytes = Memory::RoundUpToPageSizeWrapPreserveZero(minElementsSizeInBytes);
        m_MaxElementsSizeInBytesReserved = Memory::RoundUpToPageSizeWrap(maxElementsSizeInBytes);
    }

    PoolAllocatorVirtual(PoolAllocatorVirtual&& rhs) noexcept
        : Super(std::move(rhs))
        , m_MinElementsSizeInBytes(rhs.m_MinElementsSizeInBytes)
        , m_MaxElementsSizeInBytesReserved(rhs.m_MaxElementsSizeInBytesReserved)
    {}

    PoolAllocatorVirtual& operator=(PoolAllocatorVirtual&& rhs) noexcept {

        if (this == &rhs) {
            return *this;
        }

        Super::operator=(std::move(rhs));
        m_MinElementsSizeInBytes = rhs.m_MinElementsSizeInBytes;
        m_MaxElementsSizeInBytesReserved = rhs.m_MaxElementsSizeInBytesReserved;

        return *this;
    }

    ALT_FORCE_INLINE usize GetElementSizeOf() const noexcept {
        return Num::Max(sizeof(T), alignof(T), sizeof(uptr));
    }

    ALT_FORCE_INLINE usize GetElementAlignOf() const noexcept {
        return Num::Max(alignof(T), alignof(uptr));
    }

    ALT_FORCE_INLINE usize GetMinElementsSizeInBytes() const noexcept {
        return m_MinElementsSizeInBytes;
    }

    ALT_FORCE_INLINE usize GetMaxElementsSizeInBytesReserved() const noexcept {
        return m_MaxElementsSizeInBytesReserved;
    }

private:

    usize m_MinElementsSizeInBytes;
    usize m_MaxElementsSizeInBytesReserved;
};

/**
 * @brief Version of "dynamic" PoolAllocatorVirtualDynamic - store object memory layout
 * description with pool.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * using Pool = Alt::PoolAllocatorDynamicDynamic;
 * 
 * // It is virtual memory we need to reserve some "virtual" memory - this memory will be used further
 * Pool::MemoryDesc desc = Pool::MemoryDesc::Create<Entity>();
 * desc.maxNumElementsReserved = Memory::TB(1) / sizeof(Entity);
 * 
 * // Important to call 'CreateCapacity(...)'
 * Pool pool = Pool::CreateCapacity(desc) iferr_return;
 * 
 * {
 *     Entity* pEntity = pool.Allocate<Entity>("Text Label"_salloc);
 *     assert(pEntity);
 * 
 *     pEntity->SetText("Pool allocated Entity"_salloc);
 * 
 *     pool.Deallocate(pEntity); // or pEntity->~Entity(); pool.DeallocateBytes(pEntity);
 * }
 * 
 * pool.DeallocateBytesExceptFirstBlockAndReset(); // only deallocates pool memory
 */
class PoolAllocatorVirtualDynamic
    : public _PoolAllocatorVirtualDetails::PoolAllocatorVirtualImpl<PoolAllocatorVirtualDynamic>
{
private:

    using Super = _PoolAllocatorVirtualDetails::PoolAllocatorVirtualImpl<PoolAllocatorVirtualDynamic>;

public:

    struct MemoryDesc {

        usize elementSizeOf = sizeof(uptr);
        usize elementAlignOf = alignof(uptr);

        usize minNumElements = 16;
        usize maxNumElementsReserved = ALT_PLATFORM_32BIT_BOOL
            ? Memory::GB(2) / sizeof(uptr)
            : Memory::TB(1) / sizeof(uptr);

        template <typename T>
        ALT_FORCE_INLINE static MemoryDesc Create() noexcept {

            MemoryDesc desc{};
            desc.elementSizeOf = sizeof(T);
            desc.elementAlignOf = alignof(T);

            desc.maxNumElementsReserved = ALT_PLATFORM_32BIT_BOOL
                ? Memory::GB(2) / Num::Max(sizeof(T), sizeof(uptr))
                : Memory::TB(1) / Num::Max(sizeof(T), sizeof(uptr));

            return desc;
        }
    };

    ALT_NODISCARD static Optional<PoolAllocatorVirtualDynamic> CreateCapacity(const MemoryDesc& desc_) noexcept {
        ALT_ASSERT(Num::IsPowerOf2(desc_.elementAlignOf));

        MemoryDesc_ desc{};
        desc.elementAlignOf = Num::Max(desc_.elementAlignOf, alignof(uptr));
        desc.elementSizeOf = Num::Max(desc_.elementSizeOf, desc_.elementAlignOf, sizeof(uptr));

        const usize minNumElements = Num::Min(desc_.minNumElements, desc_.maxNumElementsReserved);
        const usize maxNumElements = Num::Max(desc_.minNumElements, desc_.maxNumElementsReserved);

        const usize minElementsSizeInBytes = Num::MulWrap(minNumElements, desc.elementSizeOf);
        const usize maxElementsSizeInBytes = Num::MulWrap(maxNumElements, desc.elementSizeOf);

        desc.minElementsSizeInBytes = Memory::RoundUpToPageSizeWrapPreserveZero(minElementsSizeInBytes);
        desc.maxElementsSizeInBytesReserved = Memory::RoundUpToPageSizeWrap(maxElementsSizeInBytes);

        Optional<Super> arena = Super::CreateCapacity(desc.maxElementsSizeInBytesReserved);
        if (!arena) {
            return Nullopt;
        }

        PoolAllocatorVirtualDynamic self{};
        static_cast<Super&>(self) = std::move(*arena);

        self.m_Desc = desc;

        return self;
    }

    ALT_FORCE_INLINE PoolAllocatorVirtualDynamic() noexcept = default;

    PoolAllocatorVirtualDynamic(PoolAllocatorVirtualDynamic&& rhs) noexcept
        : Super(std::move(rhs))
        , m_Desc(std::move(rhs.m_Desc))
    {}

    PoolAllocatorVirtualDynamic& operator=(PoolAllocatorVirtualDynamic&& rhs) noexcept {

        if (this == &rhs) {
            return *this;
        }

        Super::operator=(std::move(rhs));
        m_Desc = std::move(rhs.m_Desc);

        return *this;
    }

    ALT_FORCE_INLINE usize GetElementSizeOf() const noexcept {
        return m_Desc.elementSizeOf;
    }

    ALT_FORCE_INLINE usize GetElementAlignOf() const noexcept {
        return m_Desc.elementAlignOf;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetMinElementsSizeInBytes() const noexcept {
        return m_Desc.minElementsSizeInBytes;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetMaxElementsSizeInBytesReserved() const noexcept {
        return m_Desc.maxElementsSizeInBytesReserved;
    }

private:

    struct MemoryDesc_ {

        usize elementSizeOf;
        usize elementAlignOf;

        usize minElementsSizeInBytes;
        usize maxElementsSizeInBytesReserved;
    };

    MemoryDesc_ m_Desc;
};

} // namespace Alt

#endif // __ALT_STD_ARENA_ALLOCATOR_VIRTUAL__