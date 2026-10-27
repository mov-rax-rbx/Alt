#pragma once

#ifndef __ALT_STD_ARENA_ALLOCATOR_VIRTUAL__
#define __ALT_STD_ARENA_ALLOCATOR_VIRTUAL__

#include "../../TypeTraits.h"

#include "../../Memory/Impl/MemoryPreInclude.h"

#include "../../Memory/UniquePtr.h"

#include "../../Utils/Num.h"
#include "../../Utils/Optional.h"

namespace Alt {

/**
 * @brief Templated version of ArenaAllocatorVirtual.
 * 
 * @param MIN_SIZE_IN_BYTES Minimum initial size in bytes (rounded to the page size).
 * @param MAX_SIZE_IN_BYTES_TO_RESERVE Maximum reserves size in bytes for commits (rounded to the page size).
 * 
 * @details Before using this arena you should reserve memory using 'CreateCapacity()'
 * method.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * using Arena = Alt::ArenaAllocatorVirtual<>;
 * 
 * // Important to call 'CreateCapacity(...)'
 * Arena arena = Arena::CreateCapacity() iferr_return;
 * 
 * Arena::Checkpoint checkpoint = arena.GetCheckpoint();
 * usize* pVal = arena.Allocate<usize>(4);
 * assert(pVal);
 * 
 * *pVal = 44;
 * 
 * arena.ReleaseToCheckpoint(checkpoint);
 * arena.ReleaseBytesAll();
 */
template <
    usize MIN_SIZE_IN_BYTES = Memory::KB(4),
    usize MAX_SIZE_IN_BYTES_TO_RESERVE = ALT_PLATFORM_32BIT_BOOL
        ? Memory::GB(2)
        : Memory::TB(1)
>
class ArenaAllocatorVirtual;

/**
 * @brief Version of "dynamic" ArenaAllocatorVirtual - store object memory layout
 * description with arena.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * using Arena = Alt::ArenaAllocatorVirtualDynamic;
 * 
 * // It is virtual memory we need to reserve some "virtual" memory - this memory will be used further
 * Arena::MemoryDesc desc{};
 * desc.maxSizeInBytesReserved = Alt::Memory::TB(4);
 * 
 * // Important to call 'CreateCapacity(...)'
 * Arena arena = Arena::CreateCapacity(desc) iferr_return;
 * 
 * {
 *     Arena::UniquePtr<Entity> entity = arena.Allocate<Entity>("Text Label"_salloc);
 *     assert(entity);
 * 
 *     entity->SetText("Arena allocated Entity"_salloc);
 * }
 * 
 * arena.ReleaseBytesAll();
 */
class ArenaAllocatorVirtualDynamic;

namespace TypeTraits {

template <usize MIN_SIZE_IN_BYTES, usize MAX_SIZE_IN_BYTES_TO_RESERVE>
struct __is_trivially_reallocatable__<ArenaAllocatorVirtual<MIN_SIZE_IN_BYTES, MAX_SIZE_IN_BYTES_TO_RESERVE>>
    : std::true_type
{};

template <>
struct __is_trivially_reallocatable__<ArenaAllocatorVirtualDynamic>
    : std::true_type
{};

} // namespace TypeTraits

namespace _ArenaAllocatorVirtualDetails {

template <typename SelfT>
class ArenaAllocatorVirtualImpl {
private:

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

protected:

    ALT_FORCE_INLINE static Optional<ArenaAllocatorVirtualImpl> CreateCapacity(const usize maxSizeInBytesReserved) noexcept {

        ArenaAllocatorVirtualImpl arena{};

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
     * @brief A smart pointer for objects managed by the Arena.
     * @tparam T The type of the object.
     * @details we can't deallocate memory but must destroy object
     */
    template <typename T>
    struct UniquePtrDeleter {

        void operator()(T* ptr) const noexcept(std::is_nothrow_destructible_v<T>) {

            Memory::DestroyObject(*ptr);
        }
    };

    template <typename T>
    using UniquePtr = UniquePtr<T, UniquePtrDeleter<T>>;

    /**
     * @brief Represents usage statistics for the allocator.
     */
    struct Stats {

        usize consumedBytes = 0;  /**< Effectively used number of bytes. */
        usize availableBytes = 0; /**< Remaining bytes available in the most recently allocated block. */

        /**
         * @brief Calculates the sum of all memory managed by the allocator.
         * @return The total capacity (consumed + available).
         */
        ALT_NODISCARD ALT_FORCE_INLINE usize GetTotal() const noexcept {
            return consumedBytes + availableBytes;
        }
    };

    /**
     * @brief A snapshot of the allocator's state - can be used for scoped memory management.
     */
    struct Checkpoint {
    private:

        friend ArenaAllocatorVirtualImpl;

        usize offset = 0;
    };

    ALT_FORCE_INLINE static usize GetGrowFactor() noexcept {
        return 2;
    }

    ALT_FORCE_INLINE usize GetMinSizeInBytes() const noexcept {
        return static_cast<const SelfT*>(this)->GetMinSizeInBytes();
    }

    ALT_FORCE_INLINE usize GetMaxSizeInBytesReserved() const noexcept {
        return static_cast<const SelfT*>(this)->GetMaxSizeInBytesReserved();
    }

    ALT_FORCE_INLINE ArenaAllocatorVirtualImpl() noexcept = default;

    ArenaAllocatorVirtualImpl(const ArenaAllocatorVirtualImpl&) noexcept = delete;
    ArenaAllocatorVirtualImpl& operator=(const ArenaAllocatorVirtualImpl&) noexcept = delete;

    ArenaAllocatorVirtualImpl(ArenaAllocatorVirtualImpl&& rhs) noexcept
        : ArenaAllocatorVirtualImpl(Rule5Move, rhs)
    {}

    ArenaAllocatorVirtualImpl& operator=(ArenaAllocatorVirtualImpl&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    ~ArenaAllocatorVirtualImpl() noexcept {

        Destroy();
    }

    /**
     * @brief Allocates and constructs an object of type T.
     * 
     * @tparam T The type of the object to create.
     * @tparam Args Constructor arguments for T. Must be compatible with T's constructor.
     * @return A pointer to the constructed object, or `nullptr` if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * using Arena = Alt::ArenaAllocatorVirtual<>;
     * 
     * Arena arena = Arena::CreateCapacity() iferr_return;
     * 
     * Alt::String* pStr = arena.Allocate<Alt::String>("Arena"_salloc);
     * assert(pStr && *pStr == "Arena");
     */
    template <typename T, typename ...Args>
    ALT_NODISCARD T* Allocate(Args&&... args) noexcept(std::is_nothrow_constructible_v<T>) {

        u8* pMemory = AllocateBytes(sizeof(T), alignof(T));
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
     * @example
     * 
     * iferr_scope;
     * 
     * using Arena = Alt::ArenaAllocatorVirtual<>;
     * 
     * Arena arena = Arena::CreateCapacity() iferr_return;
     * 
     * Alt::String str = "Pool"_salloc;
     * Alt::String* pVal = arena.Allocate<Alt::String>(str);
     * 
     * assert(pStr && *pStr == "Arena");
     */
    template <typename T>
    ALT_NODISCARD T* Allocate(T&& data) noexcept(std::is_nothrow_move_constructible_v<T>) {

        u8* pMemory = AllocateBytes(sizeof(T), alignof(T));
        if (!pMemory) {
            return nullptr;
        }

        return Memory::ConstructObject<T>(pMemory, std::forward<T>(data));
    }

    /**
     * @brief Allocates memory from the arena.
     * @return A pointer to the allocated memory, or `nullptr` if allocation fails.
     */
    ALT_NODISCARD u8* AllocateBytes(const usize sizeInBytes, const usize alignment) noexcept {

        // Reserve memory with 'CreateCapacity(...)'
        ALT_ASSERT(m_pMemory);

        ALT_ASSERT(Num::IsPowerOf2(alignment));

        const usize alignmentOffset = Memory::Align(m_pMemory, alignment);
        ALT_ASSERT(!Num::IsOverflowCheckAdd(alignment, sizeInBytes));

        if (alignmentOffset + sizeInBytes <= m_AvailableSizeInBytes) {

            u8* pMemory = m_pMemory + alignmentOffset;

            m_pMemory += alignmentOffset + sizeInBytes;
            m_AvailableSizeInBytes -= alignmentOffset + sizeInBytes;

            return pMemory;
        }

        const usize minSizeInBytes = GetMinSizeInBytes();
        const usize maxSizeInBytesReserved = GetMaxSizeInBytesReserved();

        const usize sizeInBytesPerfectForNextGrow = Num::MulWrap(m_TotalSizeInBytes, GetGrowFactor());
        const usize sizeInBytesPerfectGrow = Num::Clamp(sizeInBytesPerfectForNextGrow, minSizeInBytes, maxSizeInBytesReserved);

        const usize sizeInBytesOnDemand = alignmentOffset + sizeInBytes - m_AvailableSizeInBytes;

        usize totalSizeInBytes;
        if (Num::IsOverflowCheckAdd(m_TotalSizeInBytes, sizeInBytesOnDemand, &totalSizeInBytes)) {
            return nullptr;
        }

        const usize totalSizeInBytesPerfectGrow = Num::AddWrap(m_TotalSizeInBytes, sizeInBytesPerfectGrow, maxSizeInBytesReserved);

        totalSizeInBytes = totalSizeInBytesPerfectGrow >= totalSizeInBytes
            ? totalSizeInBytesPerfectGrow
            : Num::Max(Num::Min(Memory::RoundUpToPageSizeWrap(totalSizeInBytes), maxSizeInBytesReserved), totalSizeInBytes);

        return AllocateVirtualMemory(sizeInBytes, alignmentOffset, totalSizeInBytes);
    }

    /**
     * @brief Decommits all memory in the arena.
     * @note Must be called after object destruction
     */
    ALT_FORCE_INLINE void ReleaseBytesAll() noexcept {

        const usize offset = m_TotalSizeInBytes - m_AvailableSizeInBytes;
        Platform::VirtualMemoryDeallocateBytes(m_pMemory - offset, 0, Platform::VirtualMemoryDeallocateBytesBits::Decommit);

        m_AvailableSizeInBytes = 0;
        m_TotalSizeInBytes = 0;
    }

    /**
     * @brief Decommits all memory after specified range
     * block state for the future reusing.
     * @note Must be called after object destruction
     */
    void ReleaseBytesAfter(const usize totalSizeInBytesNew) noexcept {

        DecommitVirtualMemoryBytesAfter(totalSizeInBytesNew);
    }

    /**
     * @brief Creates `Checkpoint` of the current state of the arena.
     * @return A `Checkpoint` object storing current state.
     */
    ALT_NODISCARD ALT_FORCE_INLINE Checkpoint GetCheckpoint() const noexcept {

        Checkpoint result{};
        result.offset = m_TotalSizeInBytes - m_AvailableSizeInBytes;

        return result;
    }

    /**
     * @brief Resets the allocator to a previously captured checkpoint.
     * @param checkpoint The `Checkpoint` to revert to.
     * @details If the current block is the checkpointed block, it simply resets the pointer. Otherwise,
     * it resest whole memory block.
     */
    ALT_FORCE_INLINE void ResetToCheckpoint(const Checkpoint& checkpoint) noexcept {

        const usize availableSizeInBytes = m_TotalSizeInBytes - checkpoint.offset;

        ALT_ASSERT(availableSizeInBytes >= m_AvailableSizeInBytes);

        m_pMemory += m_AvailableSizeInBytes - availableSizeInBytes;
        m_AvailableSizeInBytes = availableSizeInBytes;
    }

    /**
     * @brief Decommits memory to a previously captured checkpoint.
     * @param checkpoint The `Checkpoint` to revert to.
     * @details If the current block is the checkpointed block, it simply resets the pointer. Otherwise,
     * it resest whole memory block.
     */
    void ReleaseToCheckpoint(const Checkpoint& checkpoint) noexcept {

        ResetToCheckpoint(checkpoint);
        DecommitVirtualMemoryBytesAfter(checkpoint.offset);
    }

    /**
     * @brief Retrieves current usage statistics.
     * @return A `Stats` struct with statistics.
     */
    ALT_NODISCARD Stats GetStats() const noexcept {

        Stats stats{};
        stats.consumedBytes = m_TotalSizeInBytes - m_AvailableSizeInBytes;
        stats.availableBytes = m_AvailableSizeInBytes;

        return stats;
    }

private:

    ALT_FORCE_INLINE void ResetBlock() noexcept {

        const usize offset = m_TotalSizeInBytes - m_AvailableSizeInBytes;

        m_pMemory -= offset;
        m_AvailableSizeInBytes = m_TotalSizeInBytes;
    }

    ALT_FORCE_INLINE void DecommitVirtualMemoryBytesAfter(const usize totalSizeInBytes_) noexcept {

        const usize totalSizeInBytesNew = Memory::RoundUpToPageSizeWrapPreserveZero(totalSizeInBytes_);
        const usize sizeInBytesDiff = Num::SubWrap(m_TotalSizeInBytes, totalSizeInBytesNew);

        m_TotalSizeInBytes -= sizeInBytesDiff;
        m_pMemory -= Num::SubWrap(sizeInBytesDiff, m_AvailableSizeInBytes);
        m_AvailableSizeInBytes = Num::SubWrap(m_AvailableSizeInBytes, sizeInBytesDiff);

        u8* pMemory = m_pMemory + m_AvailableSizeInBytes;

        Platform::VirtualMemoryDeallocateBytes(pMemory, sizeInBytesDiff, Platform::VirtualMemoryDeallocateBytesBits::Decommit);
    }

    ALT_FORCE_INLINE u8* AllocateVirtualMemory(const usize sizeInBytes, const usize alignmentOffset, const usize totalSizeInBytesOnDemand) noexcept {

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

        ALT_ASSERT(totalSizeInBytesOnDemand <= GetMaxSizeInBytesReserved());

        pMemory = m_pMemory;
        usize availableSizeInBytes = m_AvailableSizeInBytes + size;

        u8* pMemoryElement = pMemory + alignmentOffset;
        pMemory += alignmentOffset + sizeInBytes;
        availableSizeInBytes -= alignmentOffset + sizeInBytes;

        m_pMemory = pMemory;
        m_AvailableSizeInBytes = availableSizeInBytes;
        m_TotalSizeInBytes = totalSizeInBytesOnDemand;

        return pMemoryElement;
    }

    ALT_FORCE_INLINE ArenaAllocatorVirtualImpl(Rule5Move_t, ArenaAllocatorVirtualImpl& rhs) noexcept
        : m_pMemory(Exchange(rhs.m_pMemory, nullptr))
        , m_TotalSizeInBytes(Exchange(rhs.m_TotalSizeInBytes, 0))
        , m_AvailableSizeInBytes(Exchange(rhs.m_AvailableSizeInBytes, 0))
    {}

    ALT_FORCE_INLINE void Assign(Rule5Move_t, ArenaAllocatorVirtualImpl& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        if (m_pMemory) {

            const usize offset = m_TotalSizeInBytes - m_AvailableSizeInBytes;
            Platform::VirtualMemoryDeallocateBytes(m_pMemory - offset, 0, Platform::VirtualMemoryDeallocateBytesBits::Release);
        }

        m_pMemory = Exchange(rhs.m_pMemory, nullptr);
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

    // approx ~ 2^0 + 2^1 + 2^2 + ... = 2^(n + 1) - 1
    usize m_TotalSizeInBytes = 0;
    usize m_AvailableSizeInBytes = 0;
};

} // namespace _ArenaAllocatorVirtualDetails

/**
 * @brief Templated version of ArenaAllocatorVirtual.
 * 
 * @param MIN_SIZE_IN_BYTES Minimum initial size in bytes (rounded to the page size).
 * @param MAX_SIZE_IN_BYTES_TO_RESERVE Maximum reserves size in bytes for commits (rounded to the page size).
 * 
 * @details Before using this arena you should reserve memory using 'CreateCapacity()' method.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * using Arena = Alt::ArenaAllocatorVirtual<>;
 * 
 * // Important to call 'CreateCapacity(...)'
 * Arena arena = Arena::CreateCapacity() iferr_return;
 * 
 * Arena::Checkpoint checkpoint = arena.GetCheckpoint();
 * usize* pVal = arena.Allocate<usize>(4);
 * assert(pVal);
 * 
 * *pVal = 44;
 * 
 * arena.ReleaseToCheckpoint(checkpoint);
 * arena.ReleaseBytesAll();
 */
template <usize MIN_SIZE_IN_BYTES, usize MAX_SIZE_IN_BYTES_TO_RESERVE>
class ArenaAllocatorVirtual
    : public _ArenaAllocatorVirtualDetails::ArenaAllocatorVirtualImpl<ArenaAllocatorVirtual<MIN_SIZE_IN_BYTES, MAX_SIZE_IN_BYTES_TO_RESERVE>>
{
private:

    static_assert(MIN_SIZE_IN_BYTES <= MAX_SIZE_IN_BYTES_TO_RESERVE, "");

    using Super = _ArenaAllocatorVirtualDetails::ArenaAllocatorVirtualImpl<
        ArenaAllocatorVirtual<MIN_SIZE_IN_BYTES, MAX_SIZE_IN_BYTES_TO_RESERVE>
    >;

public:

    ALT_NODISCARD static Optional<ArenaAllocatorVirtual> CreateCapacity() noexcept {

        ArenaAllocatorVirtual self{};

        Optional<Super> arena = Super::CreateCapacity(self.m_MaxSizeInBytesReserved);
        if (!arena) {
            return Nullopt;
        }

        static_cast<Super&>(self) = std::move(*arena);

        return self;
    }

    ALT_FORCE_INLINE ArenaAllocatorVirtual() noexcept {

        m_MinSizeInBytes = Memory::RoundUpToPageSizeWrapPreserveZero(MIN_SIZE_IN_BYTES);
        m_MaxSizeInBytesReserved = Memory::RoundUpToPageSizeWrap(MAX_SIZE_IN_BYTES_TO_RESERVE);
    }

    ArenaAllocatorVirtual(ArenaAllocatorVirtual&& rhs) noexcept
        : Super(std::move(rhs))
        , m_MinSizeInBytes(rhs.m_MinSizeInBytes)
        , m_MaxSizeInBytesReserved(rhs.m_MaxSizeInBytesReserved)
    {}

    ArenaAllocatorVirtual& operator=(ArenaAllocatorVirtual&& rhs) noexcept {

        if (this == &rhs) {
            return *this;
        }

        Super::operator=(std::move(rhs));
        m_MinSizeInBytes = rhs.m_MinSizeInBytes;
        m_MaxSizeInBytesReserved = rhs.m_MaxSizeInBytesReserved;

        return *this;
    }

    ALT_FORCE_INLINE usize GetMinSizeInBytes() const noexcept {
        return m_MinSizeInBytes;
    }

    ALT_FORCE_INLINE usize GetMaxSizeInBytesReserved() const noexcept {
        return m_MaxSizeInBytesReserved;
    }

private:

    usize m_MinSizeInBytes;
    usize m_MaxSizeInBytesReserved;
};

/**
 * @brief Version of "dynamic" ArenaAllocatorVirtual - store object memory layout
 * description with arena.
 * 
 * @details Before using this arena you should reserve memory using 'CreateCapacity()' method.
 * 
 * @example
 * 
 * using Arena = Alt::ArenaAllocatorVirtualDynamic;
 * 
 * // It is virtual memory we need to reserve some "virtual" memory - this memory will be used further
 * Arena::MemoryDesc desc{};
 * desc.maxSizeInBytesReserved = Alt::Memory::TB(4);
 * 
 * // Important to call 'CreateCapacity(...)'
 * Arena arena = Arena::CreateCapacity(desc) iferr_return;
 * 
 * {
 *     Arena::UniquePtr<Entity> entity = arena.Allocate<Entity>("Text Label"_salloc);
 *     assert(entity);
 * 
 *     entity->SetText("Arena allocated Entity"_salloc);
 * }
 * 
 * arena.ReleaseBytesAll();
 */
class ArenaAllocatorVirtualDynamic
    : public _ArenaAllocatorVirtualDetails::ArenaAllocatorVirtualImpl<ArenaAllocatorVirtualDynamic>
{
private:

    using Super = _ArenaAllocatorVirtualDetails::ArenaAllocatorVirtualImpl<ArenaAllocatorVirtualDynamic>;

public:

    struct MemoryDesc {

        usize minSizeInBytes = Memory::KB(4);

        usize maxSizeInBytesReserved = ALT_PLATFORM_32BIT_BOOL
            ? Memory::GB(2)
            : Memory::TB(1);
    };

    ALT_NODISCARD static Optional<ArenaAllocatorVirtualDynamic> CreateCapacity(const MemoryDesc& desc_) noexcept {

        const usize minSizeInBytes_ = Num::Min(desc_.minSizeInBytes, desc_.maxSizeInBytesReserved);
        const usize maxSizeInBytes_ = Num::Max(desc_.minSizeInBytes, desc_.maxSizeInBytesReserved);

        const usize minSizeInBytes = Memory::RoundUpToPageSizeWrapPreserveZero(minSizeInBytes_);
        const usize maxSizeInBytesReserved = Memory::RoundUpToPageSizeWrap(maxSizeInBytes_);

        MemoryDesc desc{};
        desc.minSizeInBytes = minSizeInBytes;
        desc.maxSizeInBytesReserved = maxSizeInBytesReserved;

        Optional<Super> arena = Super::CreateCapacity(desc.maxSizeInBytesReserved);
        if (!arena) {
            return Nullopt;
        }

        ArenaAllocatorVirtualDynamic self{};
        static_cast<Super&>(self) = std::move(*arena);

        self.m_Desc = desc;

        return self;
    }

    ALT_FORCE_INLINE ArenaAllocatorVirtualDynamic() noexcept = default;

    ArenaAllocatorVirtualDynamic(ArenaAllocatorVirtualDynamic&& rhs) noexcept
        : Super(std::move(rhs))
        , m_Desc(std::move(rhs.m_Desc))
    {}

    ArenaAllocatorVirtualDynamic& operator=(ArenaAllocatorVirtualDynamic&& rhs) noexcept {

        if (this == &rhs) {
            return *this;
        }

        Super::operator=(std::move(rhs));
        m_Desc = std::move(rhs.m_Desc);

        return *this;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetMinSizeInBytes() const noexcept {
        return m_Desc.minSizeInBytes;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetMaxSizeInBytesReserved() const noexcept {
        return m_Desc.maxSizeInBytesReserved;
    }

private:

    MemoryDesc m_Desc;
};

} // namespace Alt

#endif // __ALT_STD_ARENA_ALLOCATOR_VIRTUAL__