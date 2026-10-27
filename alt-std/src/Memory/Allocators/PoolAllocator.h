#pragma once

#ifndef __ALT_STD_POOL_ALLOCATOR__
#define __ALT_STD_POOL_ALLOCATOR__

#include "../../TypeTraits.h"

#include "../../Memory/Impl/MemoryPreInclude.h"

#include "../../Utils/Num.h"

namespace Alt {

/**
 * @brief Defines the allocation strategies for the Pool Allocator.
 */
enum class PoolAllocatorTrait {

    /** @brief Blocks grow exponentially by a factor (2), see. 'GetGrowFactor()'. */
    ExponentialBlockSize,

    /** @brief Every block is allocated with a fixed number of elements. */
    FixedBlockSize,

    /** @brief Each block contains exactly one element. */
    OneElement
};

/**
 * @brief Templated version of PoolAllocator.
 * 
 * @tparam T The type of the object.
 * @param MIN_NUM_ELEMENTS_PER_BLOCK Minimum number of elements in blocks.
 * @param MAX_NUM_ELEMENTS_PER_BLOCK Maximum number of elements in blocks.
 * 
 * @details By default if MIN_NUM_ELEMENTS_PER_BLOCK != MAX_NUM_ELEMENTS_PER_BLOCK the
 * 'ExponentialBlockSize' strategy used for new allocated blocks.
 * If MIN_NUM_ELEMENTS_PER_BLOCK == MAX_NUM_ELEMENTS_PER_BLOCK the 'FixedBlockSize' strategy
 * used and if MIN_NUM_ELEMENTS_PER_BLOCK in [0, 1] and MAX_NUM_ELEMENTS_PER_BLOCK in [0, 1]
 * the 'OneElement' strategy used. For more info see. `PoolAllocatorTrait`.
 * 
 * @example
 * 
 * Alt::PoolAllocator<usize, 64> pool;
 * usize* pVal = pool.Allocate<usize>(4);
 * assert(pVal);
 * 
 * *pVal = 44;
 * 
 * pool.Deallocate(pVal);
 * 
 * ...
 * 
 * pool.DeallocateBytesExceptFirstBlockAndReset(); // useful to shrink unused memory in pool (but after all pool object was destructed)
 */
template <
    typename T,
    usize MIN_NUM_ELEMENTS_PER_BLOCK = 16,
    usize MAX_NUM_ELEMENTS_PER_BLOCK = ALT_PLATFORM_32BIT_BOOL
        ? Memory::GB(2) / Num::Max(sizeof(T), sizeof(uptr))
        : Memory::TB(1) / Num::Max(sizeof(T), sizeof(uptr))
>
class PoolAllocator;

/**
 * @brief Templated version of "dynamic" PoolAllocator - store object memory layout
 * description with pool.
 * 
 * @param TRAIT Trait of the pool - grow strategy.
 * @example
 * 
 * auto desc = Alt::PoolAllocatorDynamic<Alt::PoolAllocatorTrait::ExponentialBlockSize>::MemoryDesc::Create<Entity>();
 * 
 * Alt::PoolAllocatorDynamic<Alt::PoolAllocatorTrait::ExponentialBlockSize> pool{ desc };
 * Entity* pEntity = pool.Allocate<Entity>("Text Label"_salloc);
 * assert(pEntity);
 * 
 * pEntity->SetText("Pool allocated Entity"_salloc);
 * 
 * pool.Deallocate(pEntity); // or pEntity->~Entity(); pool.DeallocateBytes(pEntity);
 * pool.DeallocateBytesExceptFirstBlockAndReset(); // only deallocates pool memory
 */
template <PoolAllocatorTrait TRAIT = PoolAllocatorTrait::ExponentialBlockSize>
class PoolAllocatorDynamic;

namespace TypeTraits {

template <typename T, usize MIN_NUM_ELEMENTS_PER_BLOCK, usize MAX_NUM_ELEMENTS_PER_BLOCK>
struct __is_trivially_reallocatable__<PoolAllocator<T, MIN_NUM_ELEMENTS_PER_BLOCK, MAX_NUM_ELEMENTS_PER_BLOCK>>
    : std::true_type
{};

template <PoolAllocatorTrait TRAIT>
struct __is_trivially_reallocatable__<PoolAllocatorDynamic<TRAIT>>
    : std::true_type
{};

} // namespace TypeTraits

namespace _PoolAllocatorDetails {

template <PoolAllocatorTrait TRAIT>
using PoolTrait = std::integral_constant<PoolAllocatorTrait, TRAIT>;

template <usize MIN_NUM_ELEMENTS_PER_BLOCK, usize MAX_NUM_ELEMENTS_PER_BLOCK>
constexpr static PoolAllocatorTrait pool_trait_v =

    std::conditional_t<

        (MIN_NUM_ELEMENTS_PER_BLOCK == 0 || MIN_NUM_ELEMENTS_PER_BLOCK == 1) &&
            (MAX_NUM_ELEMENTS_PER_BLOCK == 0 || MAX_NUM_ELEMENTS_PER_BLOCK == 1),

        PoolTrait<PoolAllocatorTrait::OneElement>,

        std::conditional_t<

            MIN_NUM_ELEMENTS_PER_BLOCK == MAX_NUM_ELEMENTS_PER_BLOCK,

            PoolTrait<PoolAllocatorTrait::FixedBlockSize>,
            PoolTrait<PoolAllocatorTrait::ExponentialBlockSize>
        >
    >::value;

template <PoolAllocatorTrait TRAIT>
class PoolAllocatorBase_;

template <>
class PoolAllocatorBase_<PoolAllocatorTrait::ExponentialBlockSize> {
protected:

    struct Block {

        u8* pMemory = nullptr;
        Block* pPrev = nullptr;

        usize totalSizeInBytes = 0;
        usize availableSizeInBytes = 0;

        ALT_FORCE_INLINE Block(
            u8* pMemory,
            Block* pPrev,

            usize totalSizeInBytes,
            usize availableSizeInBytes
        ) noexcept
            : pMemory(pMemory)
            , pPrev(pPrev)

            , totalSizeInBytes(totalSizeInBytes)
            , availableSizeInBytes(availableSizeInBytes)
        {}

        ALT_FORCE_INLINE constexpr static usize GetGrowFactor() noexcept {
            return 2;
        }

        ALT_FORCE_INLINE usize GetSizeInBytesPerfectForNextGrow(const usize alignment) const noexcept {

            /*
             * synced with 'AllocateBlock(...)' - we can always calculate perfect block size from alignment and
             * current total block size in bytes, because Pool on each block allocation uses "true" number of
             * elements multiplied by factor
             */

            // malloc aligns memory suitable for any object type with fundamental alignment
            constexpr static bool isMallocAlreadyAlignedToBlock = alignof(Block) <= Memory::MallocDefaultAlignment();

            const usize defaultBlockAlignmentMalloc = isMallocAlreadyAlignedToBlock
                ? 1
                : alignof(Block);

            const usize extraBlockSizeInBytes = alignment <= alignof(Block)
                ? (defaultBlockAlignmentMalloc - 1) + sizeof(Block)
                : (defaultBlockAlignmentMalloc - 1) + sizeof(Block) + (alignment - 1);

            return Num::SubWrap(totalSizeInBytes, extraBlockSizeInBytes);
        }

        ALT_FORCE_INLINE void CalculateNextBlockSize(const usize alignment, usize& sizeInBytesBlock, const usize maxBlockSizeInBytes) const noexcept {

            sizeInBytesBlock = Num::MulWrap(GetSizeInBytesPerfectForNextGrow(alignment), GetGrowFactor(), maxBlockSizeInBytes);
        }
    };
};

template <>
class PoolAllocatorBase_<PoolAllocatorTrait::OneElement> {
protected:

    struct Block {

        u8* pMemory = nullptr;
        Block* pPrev = nullptr;
    };
};

template <>
class PoolAllocatorBase_<PoolAllocatorTrait::FixedBlockSize> {
protected:

    struct Block {

        u8* pMemory = nullptr;
        Block* pPrev = nullptr;

        usize totalSizeInBytes = 0;
        usize availableSizeInBytes = 0;

        ALT_FORCE_INLINE Block(
            u8* pMemory,
            Block* pPrev,

            usize totalSizeInBytes,
            usize availableSizeInBytes
        ) noexcept
            : pMemory(pMemory)
            , pPrev(pPrev)

            , totalSizeInBytes(totalSizeInBytes)
            , availableSizeInBytes(availableSizeInBytes)
        {}

        ALT_FORCE_INLINE constexpr static usize GetGrowFactor() noexcept {
            return 1;
        }

        ALT_FORCE_INLINE usize GetSizeInBytesPerfectForNextGrow() const noexcept {
            return 0;
        }

        ALT_FORCE_INLINE void CalculateNextBlockSize(const usize, usize&, const usize) const noexcept {
        }
    };
};

template <PoolAllocatorTrait TRAIT>
class PoolAllocatorBase : PoolAllocatorBase_<TRAIT> {
protected:

    using Block = typename PoolAllocatorBase_<TRAIT>::Block;

public:

    /**
     * @brief Represents usage statistics for the allocator.
     */
    struct Stats {

        usize numBlocks = 0;          /**< Total number of blocks currently allocated. */

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

protected:

    ALT_FORCE_INLINE constexpr static usize GetControlBlockSizeInBytesForOveralBlockSizeEstimation() noexcept {

        // malloc aligns memory suitable for any object type with fundamental alignment
        constexpr bool isMallocAlreadyAlignedToBlock = alignof(Block) <= Memory::MallocDefaultAlignment();

        const usize defaultBlockAlignmentMalloc = isMallocAlreadyAlignedToBlock
            ? 1
            : alignof(Block);

        return (defaultBlockAlignmentMalloc - 1) + sizeof(Block);
    }
};

template <typename SelfT, PoolAllocatorTrait TRAIT>
class PoolAllocatorImpl : public PoolAllocatorBase<TRAIT> {
private:

    using Super = PoolAllocatorBase<TRAIT>;

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

protected:

    using Block = typename Super::Block;

public:

    using Stats = typename Super::Stats;

    ALT_FORCE_INLINE usize GetElementSizeOf() const noexcept {
        return static_cast<const SelfT*>(this)->GetElementSizeOf();
    }

    ALT_FORCE_INLINE usize GetElementAlignOf() const noexcept {
        return static_cast<const SelfT*>(this)->GetElementAlignOf();
    }

    ALT_FORCE_INLINE usize GetMinElementsSizeInBytesPerBlock() const noexcept {
        return static_cast<const SelfT*>(this)->GetMinElementsSizeInBytesPerBlock();
    }

    ALT_FORCE_INLINE usize GetMaxElementsSizeInBytesPerBlock() const noexcept {
        return static_cast<const SelfT*>(this)->GetMaxElementsSizeInBytesPerBlock();
    }

    ALT_FORCE_INLINE PoolAllocatorImpl() noexcept = default;

    PoolAllocatorImpl(const PoolAllocatorImpl&) noexcept = delete;
    PoolAllocatorImpl& operator=(const PoolAllocatorImpl&) noexcept = delete;

    PoolAllocatorImpl(PoolAllocatorImpl&& rhs) noexcept
        : PoolAllocatorImpl(Rule5Move, rhs)
    {}

    PoolAllocatorImpl& operator=(PoolAllocatorImpl&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    ~PoolAllocatorImpl() noexcept {

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
     * Alt::PoolAllocator<usize> pool;
     * usize* pVal = pool.Allocate<usize>(42);
     * assert(pVal && *pVal == 42);
     * 
     * Alt::PoolAllocator<Alt::String> pool;
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
     * Alt::PoolAllocator<Alt::String> pool;
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
     * Alt::PoolAllocator<Alt::String> pool;
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

        if (m_FreePtr != 0) {

            u8* pMemory = reinterpret_cast<u8*>(m_FreePtr);
            m_FreePtr = *reinterpret_cast<uptr*>(m_FreePtr);

            return pMemory;
        }

        const usize elementSizeOf = GetElementSizeOf();
        usize sizeInBytesBlock = GetMinElementsSizeInBytesPerBlock();

        if (!m_pLastBlock) {
            goto block_alloc_label;
        }

        if (elementSizeOf <= m_pLastBlock->availableSizeInBytes) {

            u8* pMemory = m_pLastBlock->pMemory;

            m_pLastBlock->pMemory += elementSizeOf;
            m_pLastBlock->availableSizeInBytes -= elementSizeOf;

            return pMemory;
        }

        m_pLastBlock->CalculateNextBlockSize(GetElementAlignOf(), sizeInBytesBlock, GetMaxElementsSizeInBytesPerBlock());

block_alloc_label:
        return AllocateBlock(sizeInBytesBlock);
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
     * @brief Deallocates all blocks in the pool.
     * @note Must be called after object destruction
     */
    void DeallocateBytesAllBlocks() noexcept {

        while (m_pLastBlock) {

            Block* pBlock = m_pLastBlock;
            m_pLastBlock = pBlock->pPrev;

            DeallocateBlock(*pBlock);
        }

        m_FreePtr = 0;
    }

    /**
     * @brief Clears all blocks except the first one and resets the first block.
     * 
     * @details This is useful for reusing the base block without reallocating everything.
     * @note Must be called after object destruction
     */
    void DeallocateBytesExceptFirstBlockAndReset() noexcept {

        if (!m_pLastBlock) {
            return;
        }

        while (m_pLastBlock->pPrev) {

            Block* pBlock = m_pLastBlock;
            m_pLastBlock = pBlock->pPrev;

            DeallocateBlock(*pBlock);
        }

        ResetBlock(*m_pLastBlock);
        m_FreePtr = 0;
    }

    /**
     * @brief Retrieves current usage statistics.
     * @return A `Stats` struct with statistics.
     */
    ALT_NODISCARD Stats GetStats() const noexcept {

        Stats stats{};

        uptr freePtr = m_FreePtr;
        while (freePtr != 0) {

            stats.availableElements += 1;
            freePtr = *reinterpret_cast<uptr*>(freePtr);
        }

        Block* pBlock = m_pLastBlock;

        const usize elementSizeOf = GetElementSizeOf();
        const usize extraBlockSizeInBytes = GetExtraBlockSizeInBytes(GetElementAlignOf());

        while (pBlock) {

            stats.numBlocks += 1;
            stats.consumedElements += (pBlock->totalSizeInBytes - extraBlockSizeInBytes) / elementSizeOf;

            const usize availableBytes = pBlock->availableSizeInBytes;
            stats.availableElements += availableBytes / elementSizeOf;

            pBlock = pBlock->pPrev;
        }

        stats.consumedElements -= stats.availableElements;

        return stats;
    }

private:

    ALT_FORCE_INLINE static void DeallocateBlock(const Block& block) noexcept {

        const usize offset = block.totalSizeInBytes - block.availableSizeInBytes;

        // itself block is aligned, so we need to offset the pointer to base address
        Memory::Free(block.pMemory - offset);
    }

    ALT_FORCE_INLINE static usize GetExtraBlockSizeInBytes(const usize alignment) noexcept {

        // malloc aligns memory suitable for any object type with fundamental alignment
        constexpr static bool isMallocAlreadyAlignedToBlock = alignof(Block) <= Memory::MallocDefaultAlignment();

        const usize defaultBlockAlignmentMalloc = isMallocAlreadyAlignedToBlock
            ? 1
            : alignof(Block);

        return alignment <= alignof(Block)
            ? (defaultBlockAlignmentMalloc - 1) + sizeof(Block)
            : (defaultBlockAlignmentMalloc - 1) + sizeof(Block) + (alignment - 1);
    }

    ALT_FORCE_INLINE u8* AllocateBlock(const usize sizeInBytesBlock) noexcept {
        ALT_ASSERT(m_FreePtr == 0);

        const usize sizeInBytesElement = GetElementSizeOf();
        const usize alignment = GetElementAlignOf();

        // malloc aligns memory suitable for any object type with fundamental alignment
        constexpr static bool isMallocAlreadyAlignedToBlock = alignof(Block) <= Memory::MallocDefaultAlignment();

        const usize defaultBlockAlignmentMalloc = isMallocAlreadyAlignedToBlock
            ? 1
            : alignof(Block);

        const usize extraBlockSizeInBytes = alignment <= alignof(Block)
            ? (defaultBlockAlignmentMalloc - 1) + sizeof(Block)
            : (defaultBlockAlignmentMalloc - 1) + sizeof(Block) + (alignment - 1);

        usize size;
        if (Num::IsOverflowCheckAdd(extraBlockSizeInBytes, sizeInBytesBlock, &size)) {
            return nullptr;
        }

        u8* pMemory = Memory::Malloc<u8>(size);
        if (!pMemory) {
            return nullptr;
        }

        usize availableSizeInBytes = size;

        const usize blockAlignmentOffset = isMallocAlreadyAlignedToBlock
            ? 0
            : Memory::Align(pMemory, defaultBlockAlignmentMalloc);

        u8* pMemoryBlock = pMemory + blockAlignmentOffset;
        pMemory += blockAlignmentOffset + sizeof(Block);
        availableSizeInBytes -= blockAlignmentOffset + sizeof(Block);

        const usize elementsAlignmentOffset = Memory::Align(pMemory, alignment);

        u8* pMemoryElement = pMemory + elementsAlignmentOffset;
        pMemory += elementsAlignmentOffset + sizeInBytesElement;
        availableSizeInBytes -= elementsAlignmentOffset + sizeInBytesElement;

        Block block{
             pMemory,
             m_pLastBlock,
             size,
             availableSizeInBytes,
        };

        m_pLastBlock = Memory::ConstructObject<Block>(pMemoryBlock, block);

        return pMemoryElement;
    }

    ALT_FORCE_INLINE void ResetBlock(Block& block) const noexcept {

        // malloc aligns memory suitable for any object type with fundamental alignment
        constexpr static bool isMallocAlreadyAlignedToBlock = alignof(Block) <= Memory::MallocDefaultAlignment();

        const usize offset = block.totalSizeInBytes - block.availableSizeInBytes;

        block.pMemory -= offset;
        block.availableSizeInBytes = block.totalSizeInBytes;

        const usize alignmentOffset = isMallocAlreadyAlignedToBlock
            ? 0
            : Memory::Align(block.pMemory, alignof(Block));

        block.pMemory += alignmentOffset + sizeof(Block);
        block.availableSizeInBytes -= alignmentOffset + sizeof(Block);

        const usize elementsAlignmentOffset = Memory::Align(block.pMemory, GetElementAlignOf());

        block.pMemory += elementsAlignmentOffset;
        block.availableSizeInBytes -= elementsAlignmentOffset;
    }

    ALT_FORCE_INLINE PoolAllocatorImpl(Rule5Move_t, PoolAllocatorImpl& rhs) noexcept
        : m_pLastBlock(Exchange(rhs.m_pLastBlock, nullptr))
        , m_FreePtr(Exchange(rhs.m_FreePtr, 0))
    {}

    ALT_FORCE_INLINE void Assign(Rule5Move_t, PoolAllocatorImpl& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        DeallocateBytesAllBlocks();

        m_pLastBlock = Exchange(rhs.m_pLastBlock, nullptr);
        m_FreePtr = Exchange(rhs.m_FreePtr, 0);
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        DeallocateBytesAllBlocks();
    }

private:

    Block* m_pLastBlock = nullptr;
    uptr m_FreePtr = 0;
};

template <typename SelfT>
class PoolAllocatorImpl<SelfT, PoolAllocatorTrait::OneElement>
    : public PoolAllocatorBase<PoolAllocatorTrait::OneElement>
{
private:

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

public:

    ALT_FORCE_INLINE usize GetElementSizeOf() const noexcept {
        return static_cast<const SelfT*>(this)->GetElementSizeOf();
    }

    ALT_FORCE_INLINE usize GetElementAlignOf() const noexcept {
        return static_cast<const SelfT*>(this)->GetElementAlignOf();
    }

    ALT_FORCE_INLINE usize GetMinElementsSizeInBytesPerBlock() const noexcept {
        return static_cast<const SelfT*>(this)->GetMinElementsSizeInBytesPerBlock();
    }

    ALT_FORCE_INLINE usize GetMaxElementsSizeInBytesPerBlock() const noexcept {
        return static_cast<const SelfT*>(this)->GetMaxElementsSizeInBytesPerBlock();
    }

    ALT_FORCE_INLINE PoolAllocatorImpl() noexcept = default;

    PoolAllocatorImpl(const PoolAllocatorImpl&) noexcept = delete;
    PoolAllocatorImpl& operator=(const PoolAllocatorImpl&) noexcept = delete;

    PoolAllocatorImpl(PoolAllocatorImpl&& rhs) noexcept
        : PoolAllocatorImpl(Rule5Move, rhs)
    {}

    PoolAllocatorImpl& operator=(PoolAllocatorImpl&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    ~PoolAllocatorImpl() noexcept {

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
     * Alt::PoolAllocator<usize> pool;
     * usize* pVal = pool.Allocate<usize>(42);
     * assert(pVal && *pVal == 42);
     * 
     * Alt::PoolAllocator<Alt::String> pool;
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
     * Alt::PoolAllocator<Alt::String> pool;
     * 
     * Alt::String str = "Pool"_salloc;
     * Alt::String* pVal = pool.Allocate<Alt::String>(str);
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
     * Alt::PoolAllocator<Alt::String> pool;
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

        if (m_FreePtr != 0) {

            u8* pMemory = reinterpret_cast<u8*>(m_FreePtr);
            m_FreePtr = *reinterpret_cast<uptr*>(m_FreePtr);

            return pMemory;
        }

        const usize sizeInBytesBlock = GetMinElementsSizeInBytesPerBlock();
        return AllocateBlock(sizeInBytesBlock);
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
     * @brief Deallocates all blocks in the pool.
     * @note Must be called after object destruction
     */
    void DeallocateBytesAllBlocks() noexcept {

        while (m_pLastBlock) {

            Block* pBlock = m_pLastBlock;
            m_pLastBlock = pBlock->pPrev;

            DeallocateBlock(*pBlock);
        }

        m_FreePtr = 0;
    }

    /**
     * @brief Clears all blocks except the first one and resets the first block.
     * 
     * @details This is useful for reusing the base block without reallocating everything.
     * @note Must be called after object destruction
     */
    void DeallocateBytesExceptFirstBlockAndReset() noexcept {

        if (!m_pLastBlock) {
            return;
        }

        while (m_pLastBlock->pPrev) {

            Block* pBlock = m_pLastBlock;
            m_pLastBlock = pBlock->pPrev;

            DeallocateBlock(*pBlock);
        }

        u8* pMemoryElement = GetMemoryElement(*m_pLastBlock);

        *reinterpret_cast<uptr*>(pMemoryElement) = 0;
        m_FreePtr = reinterpret_cast<uptr>(pMemoryElement);
    }

    /**
     * @brief Retrieves current usage statistics.
     * @return A `Stats` struct with statistics.
     */
    ALT_NODISCARD Stats GetStats() const noexcept {

        Stats stats{};

        uptr freePtr = m_FreePtr;
        while (freePtr != 0) {

            stats.availableElements += 1;
            freePtr = *reinterpret_cast<uptr*>(freePtr);
        }

        Block* pBlock = m_pLastBlock;

        while (pBlock) {

            stats.numBlocks += 1;
            pBlock = pBlock->pPrev;
        }

        stats.consumedElements = stats.numBlocks - stats.availableElements;

        return stats;
    }

private:

    ALT_FORCE_INLINE static void DeallocateBlock(const Block& block) noexcept {

        Memory::Free(block.pMemory);
    }

    ALT_FORCE_INLINE u8* GetMemoryElement(const Block& block) const noexcept {

        u8* pMemoryElement = block.pMemory;

        // malloc aligns memory suitable for any object type with fundamental alignment
        constexpr static bool isMallocAlreadyAlignedToBlock = alignof(Block) <= Memory::MallocDefaultAlignment();

        const usize blockAlignmentOffset = isMallocAlreadyAlignedToBlock
            ? 0
            : Memory::Align(pMemoryElement, alignof(Block));

        pMemoryElement += blockAlignmentOffset + sizeof(Block);

        const usize elementAlignOfOffset = Memory::Align(pMemoryElement, GetElementAlignOf());
        pMemoryElement += elementAlignOfOffset;

        return pMemoryElement;
    }

    ALT_FORCE_INLINE u8* AllocateBlock(const usize sizeInBytesBlock) noexcept {
        ALT_ASSERT(m_FreePtr == 0);

        const usize alignment = GetElementAlignOf();

        // malloc aligns memory suitable for any object type with fundamental alignment
        constexpr static bool isMallocAlreadyAlignedToBlock = alignof(Block) <= Memory::MallocDefaultAlignment();

        const usize defaultBlockAlignmentMalloc = isMallocAlreadyAlignedToBlock
            ? 1
            : alignof(Block);

        const usize extraBlockSizeInBytes = alignment <= alignof(Block)
            ? (defaultBlockAlignmentMalloc - 1) + sizeof(Block)
            : (defaultBlockAlignmentMalloc - 1) + sizeof(Block) + (alignment - 1);

        usize size;
        if (Num::IsOverflowCheckAdd(extraBlockSizeInBytes, sizeInBytesBlock, &size)) {
            return nullptr;
        }

        u8* pMemory = Memory::Malloc<u8>(size);
        if (!pMemory) {
            return nullptr;
        }

        const usize blockAlignmentOffset = isMallocAlreadyAlignedToBlock
            ? 0
            : Memory::Align(pMemory, defaultBlockAlignmentMalloc);

        u8* pMemoryBlock = pMemory + blockAlignmentOffset;
        u8* pMemoryElement = pMemory + blockAlignmentOffset + sizeof(Block);

        const usize elementAlignOfOffset = Memory::Align(pMemoryElement, alignment);
        pMemoryElement += elementAlignOfOffset;

        Block block{
            pMemory,
            m_pLastBlock,
        };

        m_pLastBlock = Memory::ConstructObject<Block>(pMemoryBlock, block);

        return pMemoryElement;
    }

    ALT_FORCE_INLINE PoolAllocatorImpl(Rule5Move_t, PoolAllocatorImpl& rhs) noexcept
        : m_pLastBlock(Exchange(rhs.m_pLastBlock, nullptr))
        , m_FreePtr(Exchange(rhs.m_FreePtr, 0))
    {}

    ALT_FORCE_INLINE void Assign(Rule5Move_t, PoolAllocatorImpl& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        DeallocateBytesAllBlocks();

        m_pLastBlock = Exchange(rhs.m_pLastBlock, nullptr);
        m_FreePtr = Exchange(rhs.m_FreePtr, 0);
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        DeallocateBytesAllBlocks();
    }

private:

    Block* m_pLastBlock = nullptr;
    uptr m_FreePtr = 0;
};

} // namespace _PoolAllocatorDetails

namespace TypeTraits {

template <typename SelfT, PoolAllocatorTrait TRAIT>
struct __is_trivially_reallocatable__<_PoolAllocatorDetails::PoolAllocatorImpl<SelfT, TRAIT>>
    : std::true_type
{};

} // namespace TypeTraits

/**
 * @brief Templated version of PoolAllocator.
 * 
 * @tparam T The type of the object.
 * @param MIN_NUM_ELEMENTS_PER_BLOCK Minimum number of elements in blocks.
 * @param MAX_NUM_ELEMENTS_PER_BLOCK Maximum number of elements in blocks.
 * 
 * @details By default if MIN_NUM_ELEMENTS_PER_BLOCK != MAX_NUM_ELEMENTS_PER_BLOCK the
 * 'ExponentialBlockSize' strategy used for new allocated blocks.
 * If MIN_NUM_ELEMENTS_PER_BLOCK == MAX_NUM_ELEMENTS_PER_BLOCK the 'FixedBlockSize' strategy
 * used and if MIN_NUM_ELEMENTS_PER_BLOCK in [0, 1] and MAX_NUM_ELEMENTS_PER_BLOCK in [0, 1]
 * the 'OneElement' strategy used. For more info see. `PoolAllocatorTrait`.
 * 
 * @example
 * 
 * Alt::PoolAllocator<usize, 64> pool;
 * usize* pVal = pool.Allocate<usize>(4);
 * assert(pVal);
 * 
 * *pVal = 44;
 * 
 * pool.Deallocate(pVal);
 * 
 * ...
 * 
 * pool.DeallocateBytesExceptFirstBlockAndReset(); // useful to shrink unused memory in pool (but after all pool object was destructed)
 */
template <
    typename T,
    usize MIN_NUM_ELEMENTS_PER_BLOCK,
    usize MAX_NUM_ELEMENTS_PER_BLOCK
>
class PoolAllocator
    : public _PoolAllocatorDetails::PoolAllocatorImpl<
        PoolAllocator<T, MIN_NUM_ELEMENTS_PER_BLOCK, MAX_NUM_ELEMENTS_PER_BLOCK>,
        _PoolAllocatorDetails::pool_trait_v<MIN_NUM_ELEMENTS_PER_BLOCK, MAX_NUM_ELEMENTS_PER_BLOCK>
    >
{
private:

    static_assert(MIN_NUM_ELEMENTS_PER_BLOCK <= MAX_NUM_ELEMENTS_PER_BLOCK, "");

    using Super = _PoolAllocatorDetails::PoolAllocatorImpl<
        PoolAllocator<T, MIN_NUM_ELEMENTS_PER_BLOCK, MAX_NUM_ELEMENTS_PER_BLOCK>,
        _PoolAllocatorDetails::pool_trait_v<MIN_NUM_ELEMENTS_PER_BLOCK, MAX_NUM_ELEMENTS_PER_BLOCK>
    >;

public:

    constexpr static PoolAllocatorTrait TRAIT = _PoolAllocatorDetails::pool_trait_v<
        MIN_NUM_ELEMENTS_PER_BLOCK, MAX_NUM_ELEMENTS_PER_BLOCK
    >;

    ALT_FORCE_INLINE constexpr static usize GetElementSizeOf() noexcept {
        return Num::Max(sizeof(T), alignof(T), sizeof(uptr));
    }

    ALT_FORCE_INLINE constexpr static usize GetElementAlignOf() noexcept {
        return Num::Max(alignof(T), alignof(uptr));
    }

    ALT_FORCE_INLINE constexpr static usize GetMinElementsSizeInBytesPerBlock() noexcept {

        constexpr usize extraSizeInBytes =
            Super::GetControlBlockSizeInBytesForOveralBlockSizeEstimation() +
            GetElementAlignOf() <= Memory::MallocDefaultAlignment() ? 0 : GetElementAlignOf() - 1;

        constexpr usize maxPossibleBlockSizeInBytes = Num::Desc<usize>::Max() - extraSizeInBytes;

        constexpr usize minNumElementsPerBlock = MIN_NUM_ELEMENTS_PER_BLOCK > maxPossibleBlockSizeInBytes / GetElementSizeOf()
            ? maxPossibleBlockSizeInBytes / GetElementSizeOf()
            : Num::Max(MIN_NUM_ELEMENTS_PER_BLOCK, 1);

        return minNumElementsPerBlock * GetElementSizeOf();
    }

    ALT_FORCE_INLINE constexpr static usize GetMaxElementsSizeInBytesPerBlock() noexcept {

        constexpr usize extraSizeInBytes =
            Super::GetControlBlockSizeInBytesForOveralBlockSizeEstimation() +
            GetElementAlignOf() <= Memory::MallocDefaultAlignment() ? 0 : GetElementAlignOf() - 1;

        constexpr usize maxPossibleBlockSizeInBytes = Num::Desc<usize>::Max() - extraSizeInBytes;

        constexpr usize maxNumElementsPerBlock = MAX_NUM_ELEMENTS_PER_BLOCK > maxPossibleBlockSizeInBytes / GetElementSizeOf()
            ? maxPossibleBlockSizeInBytes / GetElementSizeOf()
            : Num::Max(MAX_NUM_ELEMENTS_PER_BLOCK, 1);

        return maxNumElementsPerBlock * GetElementSizeOf();
    }
};

/**
 * @brief Templated version of "dynamic" PoolAllocator - store object memory layout
 * description with pool.
 * 
 * @param TRAIT Trait of the pool - grow strategy.
 * @example
 * 
 * auto desc = Alt::PoolAllocatorDynamic<Alt::PoolAllocatorTrait::ExponentialBlockSize>::MemoryDesc::Create<Entity>();
 * 
 * Alt::PoolAllocatorDynamic<Alt::PoolAllocatorTrait::ExponentialBlockSize> pool{ desc };
 * Entity* pEntity = pool.Allocate<Entity>("Text Label"_salloc);
 * assert(pEntity);
 * 
 * pEntity->SetText("Pool allocated Entity"_salloc);
 * 
 * pool.Deallocate(pEntity); // of pEntity->~Entity(); pool.DeallocateBytes(pEntity);
 * pool.DeallocateBytesExceptFirstBlockAndReset(); // only deallocates pool memory
 */
template <>
class PoolAllocatorDynamic<PoolAllocatorTrait::ExponentialBlockSize>
    : public _PoolAllocatorDetails::PoolAllocatorImpl<
        PoolAllocatorDynamic<PoolAllocatorTrait::ExponentialBlockSize>,
        PoolAllocatorTrait::ExponentialBlockSize
    >
{
private:

    using Super = _PoolAllocatorDetails::PoolAllocatorImpl<
        PoolAllocatorDynamic<PoolAllocatorTrait::ExponentialBlockSize>,
        PoolAllocatorTrait::ExponentialBlockSize
    >;

public:

    constexpr static PoolAllocatorTrait TRAIT = PoolAllocatorTrait::ExponentialBlockSize;

    struct MemoryDesc {

        usize elementSizeOf = sizeof(uptr);
        usize elementAlignOf = alignof(uptr);

        usize minNumElementsPerBlock = 16;
        usize maxNumElementsPerBlock = ALT_PLATFORM_32BIT_BOOL
            ? Memory::GB(2) / sizeof(uptr)
            : Memory::TB(1) / sizeof(uptr);

        template <typename T>
        ALT_FORCE_INLINE static MemoryDesc Create() noexcept {

            MemoryDesc desc{};
            desc.elementSizeOf = sizeof(T);
            desc.elementAlignOf = alignof(T);

            desc.maxNumElementsPerBlock = ALT_PLATFORM_32BIT_BOOL
                ? Memory::GB(2) / Num::Max(sizeof(T), sizeof(uptr))
                : Memory::TB(1) / Num::Max(sizeof(T), sizeof(uptr));

            return desc;
        }
    };

    ALT_FORCE_INLINE PoolAllocatorDynamic(const MemoryDesc& desc) noexcept {
        ALT_ASSERT(Num::IsPowerOf2(desc.elementAlignOf));

        m_Desc.elementAlignOf = Num::Max(desc.elementAlignOf, alignof(uptr));
        m_Desc.elementSizeOf = Num::Max(desc.elementSizeOf, desc.elementAlignOf, sizeof(uptr));

        const usize minNumElementsPerBlock = Num::Min(desc.minNumElementsPerBlock, desc.maxNumElementsPerBlock);
        const usize maxNumElementsPerBlock = Num::Max(desc.minNumElementsPerBlock, desc.maxNumElementsPerBlock);

        usize extraSizeInBytes = Super::GetControlBlockSizeInBytesForOveralBlockSizeEstimation();
        extraSizeInBytes += m_Desc.elementAlignOf <= Memory::MallocDefaultAlignment() ? 0 : m_Desc.elementAlignOf - 1;

        const usize minElementsSizeInBytesPerBlock = Num::Min(
            Num::MulWrap(Num::Max(minNumElementsPerBlock, 1), m_Desc.elementSizeOf),
            Num::Desc<usize>::Max() - extraSizeInBytes
        );

        const usize maxElementsSizeInBytesPerBlock = Num::Min(
            Num::MulWrap(Num::Max(maxNumElementsPerBlock, 1), m_Desc.elementSizeOf),
            Num::Desc<usize>::Max() - extraSizeInBytes
        );

        m_Desc.minElementsSizeInBytesPerBlock = Num::RoundDown(minElementsSizeInBytesPerBlock, m_Desc.elementSizeOf);
        m_Desc.maxElementsSizeInBytesPerBlock = Num::RoundDown(maxElementsSizeInBytesPerBlock, m_Desc.elementSizeOf);
    }

    PoolAllocatorDynamic(PoolAllocatorDynamic&& rhs) noexcept
        : Super(std::move(rhs))
        , m_Desc(std::move(rhs.m_Desc))
    {}

    PoolAllocatorDynamic& operator=(PoolAllocatorDynamic&& rhs) noexcept {

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

    ALT_FORCE_INLINE usize GetMinElementsSizeInBytesPerBlock() const noexcept {
        return m_Desc.minElementsSizeInBytesPerBlock;
    }

    ALT_FORCE_INLINE usize GetMaxElementsSizeInBytesPerBlock() const noexcept {
        return m_Desc.maxElementsSizeInBytesPerBlock;
    }

private:

    struct MemoryDesc_ {

        usize elementSizeOf;
        usize elementAlignOf;

        usize minElementsSizeInBytesPerBlock;
        usize maxElementsSizeInBytesPerBlock;
    };

    MemoryDesc_ m_Desc;
};

template <>
class PoolAllocatorDynamic<PoolAllocatorTrait::FixedBlockSize>
    : public _PoolAllocatorDetails::PoolAllocatorImpl<
        PoolAllocatorDynamic<PoolAllocatorTrait::FixedBlockSize>,
        PoolAllocatorTrait::FixedBlockSize
    >
{
private:

    using Super = _PoolAllocatorDetails::PoolAllocatorImpl<
        PoolAllocatorDynamic<PoolAllocatorTrait::FixedBlockSize>,
        PoolAllocatorTrait::FixedBlockSize
    >;

public:

    constexpr static PoolAllocatorTrait TRAIT = PoolAllocatorTrait::FixedBlockSize;

    struct MemoryDesc {

        usize elementSizeOf = sizeof(uptr);
        usize elementAlignOf = alignof(uptr);

        usize numElementsPerBlock = 64;

        template <typename T>
        ALT_FORCE_INLINE static MemoryDesc Create() noexcept {

            MemoryDesc desc{};
            desc.elementSizeOf = sizeof(T);
            desc.elementAlignOf = alignof(T);

            return desc;
        }
    };

    ALT_FORCE_INLINE PoolAllocatorDynamic(const MemoryDesc& desc) noexcept {
        ALT_ASSERT(Num::IsPowerOf2(desc.elementAlignOf));

        m_Desc.elementAlignOf = Num::Max(desc.elementAlignOf, alignof(uptr));
        m_Desc.elementSizeOf = Num::Max(desc.elementSizeOf, desc.elementAlignOf, sizeof(uptr));

        usize extraSizeInBytes = Super::GetControlBlockSizeInBytesForOveralBlockSizeEstimation();
        extraSizeInBytes += GetElementAlignOf() <= Memory::MallocDefaultAlignment() ? 0 : GetElementAlignOf() - 1;

        const usize numElementsSizeInBytesPerBlock = Num::Min(
            Num::MulWrap(Num::Max(desc.numElementsPerBlock, 1), m_Desc.elementSizeOf),
            Num::Desc<usize>::Max() - extraSizeInBytes
        );

        m_Desc.elementsSizeInBytesPerBlock = Num::RoundDown(numElementsSizeInBytesPerBlock, m_Desc.elementSizeOf);
    }

    PoolAllocatorDynamic(PoolAllocatorDynamic&& rhs) noexcept
        : Super(std::move(rhs))
        , m_Desc(std::move(rhs.m_Desc))
    {}

    PoolAllocatorDynamic& operator=(PoolAllocatorDynamic&& rhs) noexcept {

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

    ALT_FORCE_INLINE usize GetMinElementsSizeInBytesPerBlock() const noexcept {
        return m_Desc.elementsSizeInBytesPerBlock;
    }

    ALT_FORCE_INLINE usize GetMaxElementsSizeInBytesPerBlock() const noexcept {
        return m_Desc.elementsSizeInBytesPerBlock;
    }

private:

    struct MemoryDesc_ {

        usize elementSizeOf;
        usize elementAlignOf;

        usize elementsSizeInBytesPerBlock;
    };

    MemoryDesc_ m_Desc;
};

template <>
class PoolAllocatorDynamic<PoolAllocatorTrait::OneElement>
    : public _PoolAllocatorDetails::PoolAllocatorImpl<
        PoolAllocatorDynamic<PoolAllocatorTrait::OneElement>,
        PoolAllocatorTrait::OneElement
    >
{
private:

    using Super = _PoolAllocatorDetails::PoolAllocatorImpl<
        PoolAllocatorDynamic<PoolAllocatorTrait::OneElement>,
        PoolAllocatorTrait::OneElement
    >;

public:

    constexpr static PoolAllocatorTrait TRAIT = PoolAllocatorTrait::OneElement;

    struct MemoryDesc {

        usize elementSizeOf = sizeof(uptr);
        usize elementAlignOf = alignof(uptr);

        template <typename T>
        ALT_FORCE_INLINE static MemoryDesc Create() noexcept {

            MemoryDesc desc{};
            desc.elementSizeOf = sizeof(T);
            desc.elementAlignOf = alignof(T);

            return desc;
        }
    };

    ALT_FORCE_INLINE PoolAllocatorDynamic(const MemoryDesc& desc) noexcept {
        ALT_ASSERT(Num::IsPowerOf2(desc.elementAlignOf));

        m_Desc.elementAlignOf = Num::Max(desc.elementAlignOf, alignof(uptr));
        m_Desc.elementSizeOf = Num::Max(desc.elementSizeOf, desc.elementAlignOf, sizeof(uptr));
    }

    PoolAllocatorDynamic(PoolAllocatorDynamic&& rhs) noexcept
        : Super(std::move(rhs))
        , m_Desc(std::move(rhs.m_Desc))
    {}

    PoolAllocatorDynamic& operator=(PoolAllocatorDynamic&& rhs) noexcept {

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

    ALT_FORCE_INLINE usize GetMinElementsSizeInBytesPerBlock() const noexcept {
        return m_Desc.elementSizeOf;
    }

    ALT_FORCE_INLINE usize GetMaxElementsSizeInBytesPerBlock() const noexcept {
        return m_Desc.elementSizeOf;
    }

private:

    MemoryDesc m_Desc;
};

} // namespace Alt

#endif // __ALT_STD_POOL_ALLOCATOR__