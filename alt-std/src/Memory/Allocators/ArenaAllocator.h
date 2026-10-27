#pragma once

#ifndef __ALT_STD_ARENA_ALLOCATOR__
#define __ALT_STD_ARENA_ALLOCATOR__

#include "../../TypeTraits.h"

#include "../../Memory/Impl/MemoryPreInclude.h"

#include "../../Memory/UniquePtr.h"

#include "../../Utils/Num.h"
#include "../../Utils/EnumClassBitFlagsImpl.h"

namespace Alt {

/**
 * @brief Defines the allocation strategies for the Arena Allocator.
 */
enum class ArenaAllocatorTrait {

    /** @brief Blocks grow exponentially by a factor (2), see. 'GetGrowFactor()'. */
    ExponentialBlockSize,

    /** @brief Every block is allocated with a fixed number of bytes. */
    FixedBlockSize
};

/**
 * @brief Allows to change behavior of allocation.
 */
enum class ArenaAllocatorBits : usize {

    /** @brief Default Arena implementation. */
    None = 0,

    /** @brief Maintains additional internal blocks pool to reuse further on whole block reallocation. see. '...Release...(...)' */
    PoolForInternalBlocks = static_cast<usize>(1) << 0,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(ArenaAllocatorBits);

/**
 * @brief Templated version of ArenaAllocator.
 * 
 * @param MIN_SIZE_IN_BYTES Minimum size in bytes per block (rounded to the page size).
 * @param MAX_SIZE_IN_BYTES Maximum size in bytes per block (rounded to the page size).
 * @param BITS - see. 'ArenaAllocatorBits'.
 * 
 * @details By default if MIN_SIZE_IN_BYTES != MIN_SIZE_IN_BYTES the
 * 'ExponentialBlockSize' strategy used for new allocated blocks.
 * If MIN_SIZE_IN_BYTES == MIN_SIZE_IN_BYTES the 'FixedBlockSize' strategy
 * used. For more info see. `ArenaAllocatorTrait`.
 * 
 * @example
 * 
 * using Arena = Alt::ArenaAllocator<64, Memory::GB(2), ArenaAllocatorBits::PoolForInternalBlocks>; 
 * 
 * Arena arena;
 * 
 * Arena::Checkpoint checkpoint = arena.GetCheckpoint();
 * usize* pVal = arena.Allocate<usize>(4);
 * assert(pVal);
 * 
 * *pVal = 44;
 * 
 * arena.ResetToCheckpoint(checkpoint);
 * arena.ReleaseBytesAllToPool();
 * 
 * ...
 * 
 * arena.DeallocateBytesAll();
 * arena.DeallocateBytesExceptFirstBlockAndReset(); // useful to shrink unused memory in arena (but after all arena object was destructed)
 */
template <
    usize MIN_SIZE_IN_BYTES = Memory::KB(4),
    usize MAX_SIZE_IN_BYTES = ALT_PLATFORM_32BIT_BOOL ? Memory::GB(2) : Num::Desc<usize>::Max(),
    ArenaAllocatorBits BITS = ArenaAllocatorBits::None
>
class ArenaAllocator;

/**
 * @brief Templated version of "dynamic" ArenaAllocator - store object memory layout
 * description with arena.
 * 
 * @param TRAIT Trait of the arena - grow strategy.
 * @param BITS - see. 'ArenaAllocatorBits'.
 * @example
 * 
 * using Arena = Alt::ArenaAllocatorDynamic<Alt::ArenaAllocatorTrait::ExponentialBlockSize, ArenaAllocatorBits::PoolForInternalBlocks>;
 * 
 * Arena::MemoryDesc desc{};
 * desc.minSizeInBytes = Alt::Memory::KB(16);
 * Arena arena{ desc };
 * 
 * {
 *     Arena::UniquePtr<Entity> entity = arena.Allocate<Entity>("Text Label"_salloc);
 *     assert(entity);
 * 
 *     entity->SetText("Arena allocated Entity"_salloc);
 * }
 * 
 * arena.ReleaseBytesAllToPool(); // or arena.DeallocateBytesExceptFirstBlockAndReset();
 */
template <
    ArenaAllocatorTrait TRAIT = ArenaAllocatorTrait::ExponentialBlockSize,
    ArenaAllocatorBits bits = ArenaAllocatorBits::None
>
class ArenaAllocatorDynamic;

namespace TypeTraits {

template <
    usize MIN_SIZE_IN_BYTES,
    usize MAX_SIZE_IN_BYTES,
    ArenaAllocatorBits bits
>
struct __is_trivially_reallocatable__<ArenaAllocator<MIN_SIZE_IN_BYTES, MAX_SIZE_IN_BYTES, bits>>
    : std::true_type
{};

template <ArenaAllocatorTrait TRAIT, ArenaAllocatorBits bits>
struct __is_trivially_reallocatable__<ArenaAllocatorDynamic<TRAIT, bits>>
    : std::true_type
{};

} // namespace TypeTraits

namespace _ArenaAllocatorDetails {

template <usize MIN_SIZE_IN_BYTES, usize MAX_SIZE_IN_BYTES>
constexpr static ArenaAllocatorTrait arena_trait_v =
    MIN_SIZE_IN_BYTES == MAX_SIZE_IN_BYTES
        ? ArenaAllocatorTrait::FixedBlockSize
        : ArenaAllocatorTrait::ExponentialBlockSize;

template <ArenaAllocatorTrait TRAIT>
class ArenaAllocatorBase_;

template <>
class ArenaAllocatorBase_<ArenaAllocatorTrait::ExponentialBlockSize> {
protected:

    struct Block {

        u8* pMemory = nullptr;
        Block* pPrev = nullptr;

        usize totalSizeInBytes = 0;
        usize availableSizeInBytes = 0;

        usize sizeInBytesPerfectForNextGrow = 0;

        ALT_FORCE_INLINE Block(
            u8* pMemory,
            Block* pPrev,

            usize totalSizeInBytes,
            usize availableSizeInBytes,

            usize sizeInBytesPerfectForNextGrow
        ) noexcept
            : pMemory(pMemory)
            , pPrev(pPrev)

            , totalSizeInBytes(totalSizeInBytes)
            , availableSizeInBytes(availableSizeInBytes)

            , sizeInBytesPerfectForNextGrow(sizeInBytesPerfectForNextGrow)
        {}

        ALT_FORCE_INLINE usize GetGrowFactor() const noexcept {
            return 2;
        }

        ALT_FORCE_INLINE usize GetSizeInBytesPerfectForNextGrow() const noexcept {
            return sizeInBytesPerfectForNextGrow;
        }

        ALT_FORCE_INLINE void CalculateNextBlockSize(usize& sizeInBytesBlock, const usize maxSizeInBytes) const noexcept {

            sizeInBytesBlock = Num::MulWrap(GetSizeInBytesPerfectForNextGrow(), GetGrowFactor(), maxSizeInBytes);
        }
    };
};

template <>
class ArenaAllocatorBase_<ArenaAllocatorTrait::FixedBlockSize> {
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
            usize availableSizeInBytes,

            usize
        ) noexcept
            : pMemory(pMemory)
            , pPrev(pPrev)

            , totalSizeInBytes(totalSizeInBytes)
            , availableSizeInBytes(availableSizeInBytes)
        {}

        ALT_FORCE_INLINE usize GetGrowFactor() const noexcept {
            return 1;
        }

        ALT_FORCE_INLINE usize GetSizeInBytesPerfectForNextGrow() const noexcept {
            return 0;
        }

        ALT_FORCE_INLINE constexpr void CalculateNextBlockSize(usize&, const usize) const noexcept {
        }
    };
};

template <ArenaAllocatorTrait TRAIT>
class ArenaAllocatorBase : public ArenaAllocatorBase_<TRAIT> {
protected:

    using Block = typename ArenaAllocatorBase_<TRAIT>::Block;

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

        usize numBlocks = 0;      /**< Total number of blocks currently allocated. */

        usize consumedBytes = 0;  /**< Effectively used number of bytes. */
        usize availableBytes = 0; /**< Remaining bytes available in the most recently allocated block. */

        usize wastedBytes = 0;    /**< Total unused bytes in previously allocated blocks. */

        /**
         * @brief Calculates the sum of all memory managed by the allocator.
         * @return The total capacity (consumed + wasted + available).
         */
        ALT_NODISCARD ALT_FORCE_INLINE usize GetTotal() const noexcept {
            return consumedBytes + wastedBytes + availableBytes;
        }
    };

    /**
     * @brief A snapshot of the allocator's state - can be used for scoped memory management.
     */
    struct Checkpoint {
    private:

        template <typename, ArenaAllocatorTrait, ArenaAllocatorBits>
        friend class ArenaAllocatorImpl;

        Block* pBlock = nullptr;
        usize availableSizeInBytes = 0;
    };

    ALT_NODISCARD ALT_FORCE_INLINE static usize GetExtraBlockSizeOf() noexcept {
        return sizeof(Block);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize GetExtraBlockAlignOf() noexcept {
        return alignof(Block);
    }
};

template <typename SelfT, ArenaAllocatorTrait TRAIT, ArenaAllocatorBits bits>
class ArenaAllocatorImpl;

template <typename SelfT, ArenaAllocatorTrait TRAIT>
class ArenaAllocatorImpl<SelfT, TRAIT, ArenaAllocatorBits::None> : public ArenaAllocatorBase<TRAIT> {
private:

    using Super = ArenaAllocatorBase<TRAIT>;

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

protected:

    using Block = typename Super::Block;

public:

    using Stats = typename Super::Stats;
    using Checkpoint = typename Super::Checkpoint;

    ALT_FORCE_INLINE usize GetMinSizeInBytes() const noexcept {
        return static_cast<const SelfT*>(this)->GetMinSizeInBytes();
    }

    ALT_FORCE_INLINE usize GetMaxSizeInBytes() const noexcept {
        return static_cast<const SelfT*>(this)->GetMaxSizeInBytes();
    }

    ALT_FORCE_INLINE ArenaAllocatorImpl() noexcept = default;

    ArenaAllocatorImpl(const ArenaAllocatorImpl&) noexcept = delete;
    ArenaAllocatorImpl& operator=(const ArenaAllocatorImpl&) noexcept = delete;

    ArenaAllocatorImpl(ArenaAllocatorImpl&& rhs) noexcept
        : ArenaAllocatorImpl(Rule5Move, rhs)
    {}

    ArenaAllocatorImpl& operator=(ArenaAllocatorImpl&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    ~ArenaAllocatorImpl() noexcept {

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
     * Alt::ArenaAllocator<> arena;
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
     * Alt::ArenaAllocator<> arena;
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
        ALT_ASSERT(Num::IsPowerOf2(alignment));

        usize alignmentOffset = 0;
        usize sizeInBytesBlock = GetMinSizeInBytes();

        if (!m_pLastBlock) {
            goto block_alloc_label;
        }

        alignmentOffset = Memory::Align(m_pLastBlock->pMemory, alignment);
        ALT_ASSERT(!Num::IsOverflowCheckAdd(alignment, sizeInBytes));

        if (alignmentOffset + sizeInBytes <= m_pLastBlock->availableSizeInBytes) {

            u8* pMemory = m_pLastBlock->pMemory + alignmentOffset;

            m_pLastBlock->pMemory += alignmentOffset + sizeInBytes;
            m_pLastBlock->availableSizeInBytes -= alignmentOffset + sizeInBytes;

            return pMemory;
        }

        m_pLastBlock->CalculateNextBlockSize(sizeInBytesBlock, GetMaxSizeInBytes());

block_alloc_label:
        return AllocateBlock(sizeInBytes, alignment, sizeInBytesBlock);
    }

    /**
     * @brief Deallocates all memory in the arena.
     * @note Must be called after object destruction
     */
    void DeallocateBytesAll() noexcept {

        while (m_pLastBlock) {

            Block* pBlock = m_pLastBlock;
            m_pLastBlock = pBlock->pPrev;

            DeallocateBlock(*pBlock);
        }
    }

    /**
     * @brief Deallocates all memory except first allocated block and reset
     * block state for the future reusing.
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
    }

    /**
     * @brief Deallocates all memory except block with largest number of bytes and
     * reset block state for the future reusing.
     * @note Must be called after object destruction
     */
    void DeallocateBytesExceptLargestBlockAndReset() noexcept {

        Block* pMaxBlock = nullptr;
        usize maxTotalSizeInBytes = 0;

        while (m_pLastBlock) {

            if (m_pLastBlock->totalSizeInBytes > maxTotalSizeInBytes) {

                if (pMaxBlock) {

                    DeallocateBlock(*pMaxBlock);
                }

                pMaxBlock = m_pLastBlock;
                maxTotalSizeInBytes = pMaxBlock->totalSizeInBytes;

                m_pLastBlock = pMaxBlock->pPrev;
            }
            else {

                Block* pBlock = m_pLastBlock;
                m_pLastBlock = pBlock->pPrev;

                DeallocateBlock(*pBlock);
            }
        }

        if (pMaxBlock) {

            pMaxBlock->pPrev = nullptr;
            m_pLastBlock = pMaxBlock;

            ResetBlock(*m_pLastBlock);
        }

        ALT_ASSERT(m_pLastBlock == pMaxBlock);
    }

    /**
     * @brief Creates `Checkpoint` of the current state of the arena.
     * @return A `Checkpoint` object storing current state.
     */
    ALT_NODISCARD ALT_FORCE_INLINE Checkpoint GetCheckpoint() const noexcept {

        Checkpoint result{};
        result.pBlock = m_pLastBlock;
        result.availableSizeInBytes = m_pLastBlock ? m_pLastBlock->availableSizeInBytes : 0;

        return result;
    }

    /**
     * @brief Rolls back the allocator to a previously captured checkpoint.
     * @param checkpoint The `Checkpoint` to revert to.
     * @details Deallocates all blocks allocated after the checkpoint and resets the checkpointed block's pointer.
     */
    void DeallocateBytesToCheckpoint(const Checkpoint& checkpoint) noexcept {

        while (m_pLastBlock != checkpoint.pBlock) {

            Block* pBlock = m_pLastBlock;
            m_pLastBlock = pBlock->pPrev;

            DeallocateBlock(*pBlock);
        }

        if (m_pLastBlock) {

            const usize offset = checkpoint.availableSizeInBytes - m_pLastBlock->availableSizeInBytes;

            m_pLastBlock->pMemory -= offset;
            m_pLastBlock->availableSizeInBytes = checkpoint.availableSizeInBytes;
        }
    }

    /**
     * @brief Resets the allocator to a previously captured checkpoint.
     * @param checkpoint The `Checkpoint` to revert to.
     * @details If the current block is the checkpointed block, it simply resets the pointer. Otherwise,
     * it resest whole memory block.
     */
    void ResetToCheckpoint(const Checkpoint& checkpoint) noexcept {

        if (!m_pLastBlock) {
            return;
        }

        if (m_pLastBlock == checkpoint.pBlock) {

            const usize offset = checkpoint.availableSizeInBytes - m_pLastBlock->availableSizeInBytes;

            m_pLastBlock->pMemory -= offset;
            m_pLastBlock->availableSizeInBytes = checkpoint.availableSizeInBytes;

            return;
        }

        ResetBlock(*m_pLastBlock);
    }

    /**
     * @brief Retrieves current usage statistics.
     * @return A `Stats` struct with statistics.
     */
    ALT_NODISCARD Stats GetStats() const noexcept {

        if (!m_pLastBlock) {
            return Stats{};
        }

        Stats stats{};
        stats.numBlocks = 1;
        stats.consumedBytes = m_pLastBlock->totalSizeInBytes - m_pLastBlock->availableSizeInBytes;
        stats.availableBytes = m_pLastBlock->availableSizeInBytes;

        const Block* pBlock = m_pLastBlock->pPrev;

        while (pBlock) {

            stats.numBlocks += 1;
            stats.consumedBytes += pBlock->totalSizeInBytes - pBlock->availableSizeInBytes;
            stats.wastedBytes += pBlock->availableSizeInBytes;

            pBlock = pBlock->pPrev;
        }

        return stats;
    }

private:

    ALT_FORCE_INLINE static void ResetBlock(Block& block) noexcept {

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
    }

    ALT_FORCE_INLINE static void DeallocateBlock(const Block& block) noexcept {

        const usize offset = block.totalSizeInBytes - block.availableSizeInBytes;

        // itself block is aligned, so we need to offset the pointer to base address
        Memory::Free(block.pMemory - offset);
    }

    ALT_FORCE_INLINE u8* AllocateBlock(const usize sizeInBytes, const usize alignment, const usize sizeInBytesBlock) noexcept {

        // malloc aligns memory suitable for any object type with fundamental alignment
        constexpr static bool isMallocAlreadyAlignedToBlock = alignof(Block) <= Memory::MallocDefaultAlignment();

        const usize defaultBlockAlignmentMalloc = isMallocAlreadyAlignedToBlock
            ? 1
            : alignof(Block);

        const usize extraSizeInBytes = alignment <= alignof(Block)
            ? (defaultBlockAlignmentMalloc - 1) + sizeof(Block)
            : (defaultBlockAlignmentMalloc - 1) + sizeof(Block) + (alignment - 1);

        usize size;
        if (Num::IsOverflowCheckAdd(extraSizeInBytes, sizeInBytes, &size)) {
            return nullptr;
        }

        size = Num::Max(size, sizeInBytesBlock);

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

        const usize alignmentOffset = Memory::Align(pMemory, alignment);

        u8* pMemoryElement = pMemory + alignmentOffset;
        pMemory += alignmentOffset + sizeInBytes;
        availableSizeInBytes -= alignmentOffset + sizeInBytes;

        Block block{
            pMemory,
            m_pLastBlock,
            size,
            availableSizeInBytes,
            sizeInBytesBlock,
        };

        m_pLastBlock = Memory::ConstructObject<Block>(pMemoryBlock, block);

        return pMemoryElement;
    }

    ALT_FORCE_INLINE ArenaAllocatorImpl(Rule5Move_t, ArenaAllocatorImpl& rhs) noexcept
        : m_pLastBlock(Exchange(rhs.m_pLastBlock, nullptr))
    {}

    ALT_FORCE_INLINE void Assign(Rule5Move_t, ArenaAllocatorImpl& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        DeallocateBytesAll();

        m_pLastBlock = Exchange(rhs.m_pLastBlock, nullptr);
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        DeallocateBytesAll();
    }

private:

    Block* m_pLastBlock = nullptr;
};

template <typename SelfT, ArenaAllocatorTrait TRAIT>
class ArenaAllocatorImpl<SelfT, TRAIT, ArenaAllocatorBits::PoolForInternalBlocks> : public ArenaAllocatorBase<TRAIT> {
private:

    using Super = ArenaAllocatorBase<TRAIT>;
    
    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

protected:

    using Block = typename Super::Block;

public:

    using Stats = typename Super::Stats;
    using Checkpoint = typename Super::Checkpoint;

    ALT_FORCE_INLINE usize GetMinSizeInBytes() const noexcept {
        return static_cast<const SelfT*>(this)->GetMinSizeInBytes();
    }

    ALT_FORCE_INLINE usize GetMaxSizeInBytes() const noexcept {
        return static_cast<const SelfT*>(this)->GetMaxSizeInBytes();
    }

    ALT_FORCE_INLINE usize GetGrowFactor() const noexcept {
        return 2;
    }

    ALT_FORCE_INLINE ArenaAllocatorImpl() noexcept = default;

    ArenaAllocatorImpl(const ArenaAllocatorImpl&) noexcept = delete;
    ArenaAllocatorImpl& operator=(const ArenaAllocatorImpl&) noexcept = delete;

    ArenaAllocatorImpl(ArenaAllocatorImpl&& rhs) noexcept
        : ArenaAllocatorImpl(Rule5Move, rhs)
    {}

    ArenaAllocatorImpl& operator=(ArenaAllocatorImpl&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    ~ArenaAllocatorImpl() noexcept {

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
     * Alt::ArenaAllocator<> arena;
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
     * Alt::ArenaAllocator<> arena;
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
        ALT_ASSERT(Num::IsPowerOf2(alignment));

        usize alignmentOffset = 0;
        usize sizeInBytesBlock = GetMinSizeInBytes();

        if (!m_pLastBlock) {
            goto block_alloc_label;
        }

        alignmentOffset = Memory::Align(m_pLastBlock->pMemory, alignment);
        ALT_ASSERT(!Num::IsOverflowCheckAdd(alignment, sizeInBytes));

        if (alignmentOffset + sizeInBytes <= m_pLastBlock->availableSizeInBytes) {

            u8* pMemory = m_pLastBlock->pMemory + alignmentOffset;

            m_pLastBlock->pMemory += alignmentOffset + sizeInBytes;
            m_pLastBlock->availableSizeInBytes -= alignmentOffset + sizeInBytes;

            return pMemory;
        }

        m_pLastBlock->CalculateNextBlockSize(sizeInBytesBlock, GetMaxSizeInBytes());

block_alloc_label:

        if (m_pLastBlockPool) {

            if (u8* pMemory = AcquireBlockFromPool(sizeInBytes, alignment, sizeInBytesBlock)) {

                return pMemory;
            }
        }

        return AllocateBlock(sizeInBytes, alignment, sizeInBytesBlock);
    }

    /**
     * @brief Deallocates all including pool memory in the arena.
     * @note Must be called after object destruction
     */
    void DeallocateBytesAll() noexcept {

        DeallocateBytesAllInPool();
        DeallocateBytesAllInUse();
    }

    /**
     * @brief Deallocates only pool memory in the arena.
     * @note Must be called after object destruction
     */
    void DeallocateBytesAllInPool() noexcept {

        while (m_pLastBlockPool) {

            Block* pBlock = m_pLastBlockPool;
            m_pLastBlockPool = pBlock->pPrev;

            DeallocateBlock(*pBlock);
        }
    }

    /**
     * @brief Deallocates only used memory in the arena.
     * @note Must be called after object destruction
     */
    void DeallocateBytesAllInUse() noexcept {

        while (m_pLastBlock) {

            Block* pBlock = m_pLastBlock;
            m_pLastBlock = pBlock->pPrev;

            DeallocateBlock(*pBlock);
        }
    }

    /**
     * @brief Deallocates all including pool memory except first allocated block and reset
     * block state for the future reusing.
     * @note Must be called after object destruction
     */
    void DeallocateBytesExceptFirstBlockAndReset() noexcept {

        if (!m_pLastBlock) {

            Block* pBlock = m_pLastBlockPool;

            m_pLastBlock = pBlock;
            m_pLastBlockPool = pBlock->pPrev;

            pBlock->pPrev = nullptr;

            DeallocateBytesAllInPool();

            return;
        }

        DeallocateBytesAllInPool();

        while (m_pLastBlock->pPrev) {

            Block* pBlock = m_pLastBlock;
            m_pLastBlock = pBlock->pPrev;

            DeallocateBlock(*pBlock);
        }

        ResetBlock(*m_pLastBlock);
    }

    /**
     * @brief Releases all memory in use to the pool memory.
     * @note Must be called after object destruction
     */
    void ReleaseBytesAllToPool() noexcept {

        if (!m_pLastBlock) {
            return;
        }

        while (m_pLastBlock) {

            Block* pBlock = m_pLastBlock;
            m_pLastBlock = pBlock->pPrev;

            ResetBlock(*pBlock);

            pBlock->pPrev = m_pLastBlockPool;
            m_pLastBlockPool = pBlock;
        }
    }

    /**
     * @brief Creates `Checkpoint` of the current state of the arena.
     * @return A `Checkpoint` object storing current state.
     */
    ALT_NODISCARD ALT_FORCE_INLINE Checkpoint GetCheckpoint() const noexcept {

        Checkpoint result{};
        result.pBlock = m_pLastBlock;
        result.availableSizeInBytes = m_pLastBlock ? m_pLastBlock->availableSizeInBytes : 0;

        return result;
    }

    /**
     * @brief Rolls back the allocator to a previously captured checkpoint.
     * @param checkpoint The `Checkpoint` to revert to.
     * @details Releases all blocks allocated to the pool after the checkpoint and resets the checkpointed block's pointer.
     */
    void ResetToCheckpointAndReleaseToPool(const Checkpoint& checkpoint) noexcept {

        while (m_pLastBlock != checkpoint.pBlock) {

            Block* pBlock = m_pLastBlock;
            m_pLastBlock = pBlock->pPrev;

            ResetBlock(*pBlock);

            pBlock->pPrev = m_pLastBlockPool;
            m_pLastBlockPool = pBlock;
        }

        if (m_pLastBlock) {

            const usize offset = checkpoint.availableSizeInBytes - m_pLastBlock->availableSizeInBytes;

            m_pLastBlock->pMemory -= offset;
            m_pLastBlock->availableSizeInBytes = checkpoint.availableSizeInBytes;
        }
    }

    /**
     * @brief Resets the allocator to a previously captured checkpoint.
     * @param checkpoint The `Checkpoint` to revert to.
     * @details If the current block is the checkpointed block, it simply resets the pointer. Otherwise,
     * it resest whole memory block.
     */
    void ResetToCheckpoint(const Checkpoint& checkpoint) noexcept {

        if (!m_pLastBlock) {
            return;
        }

        if (m_pLastBlock == checkpoint.pBlock) {

            const usize offset = checkpoint.availableSizeInBytes - m_pLastBlock->availableSizeInBytes;

            m_pLastBlock->pMemory -= offset;
            m_pLastBlock->availableSizeInBytes = checkpoint.availableSizeInBytes;

            return;
        }

        ResetBlock(*m_pLastBlock);
    }

    /**
     * @brief Retrieves current usage statistics.
     * @return A `Stats` struct with statistics.
     */
    ALT_NODISCARD Stats GetStats() const noexcept {

        Stats stats{};

        if (m_pLastBlock) {

            stats.numBlocks = 1;
            stats.consumedBytes = m_pLastBlock->totalSizeInBytes - m_pLastBlock->availableSizeInBytes;
            stats.availableBytes = m_pLastBlock->availableSizeInBytes;

            const Block* pBlock = m_pLastBlock->pPrev;

            while (pBlock) {

                stats.numBlocks += 1;
                stats.consumedBytes += pBlock->totalSizeInBytes - pBlock->availableSizeInBytes;
                stats.wastedBytes += pBlock->availableSizeInBytes;

                pBlock = pBlock->pPrev;
            }
        }

        if (m_pLastBlockPool) {

            const Block* pBlock = m_pLastBlockPool;

            while (pBlock) {

                stats.numBlocks += 1;
                stats.availableBytes += pBlock->availableSizeInBytes;

                pBlock = pBlock->pPrev;
            }
        }

        return stats;
    }

private:

    ALT_FORCE_INLINE static void ResetBlock(Block& block) noexcept {

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
    }

    ALT_FORCE_INLINE static void DeallocateBlock(const Block& block) noexcept {

        const usize offset = block.totalSizeInBytes - block.availableSizeInBytes;

        // itself block is aligned, so we need to offset the pointer to base address
        Memory::Free(block.pMemory - offset);
    }

    ALT_FORCE_INLINE u8* AllocateBlock(const usize sizeInBytes, const usize alignment, const usize sizeInBytesBlock) noexcept {

        // malloc aligns memory suitable for any object type with fundamental alignment
        constexpr static bool isMallocAlreadyAlignedToBlock = alignof(Block) <= Memory::MallocDefaultAlignment();

        const usize defaultBlockAlignmentMalloc = isMallocAlreadyAlignedToBlock
            ? 1
            : alignof(Block);

        const usize extraSizeInBytes = alignment <= alignof(Block)
            ? (defaultBlockAlignmentMalloc - 1) + sizeof(Block)
            : (defaultBlockAlignmentMalloc - 1) + sizeof(Block) + (alignment - 1);

        usize size;
        if (Num::IsOverflowCheckAdd(extraSizeInBytes, sizeInBytes, &size)) {
            return nullptr;
        }

        size = Num::Max(size, sizeInBytesBlock);

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

        const usize alignmentOffset = Memory::Align(pMemory, alignment);

        u8* pMemoryElement = pMemory + alignmentOffset;
        pMemory += alignmentOffset + sizeInBytes;
        availableSizeInBytes -= alignmentOffset + sizeInBytes;

        Block block{
            pMemory,
            m_pLastBlock,
            size,
            availableSizeInBytes,
            sizeInBytesBlock,
        };

        m_pLastBlock = Memory::ConstructObject<Block>(pMemoryBlock, block);

        return pMemoryElement;
    }

    ALT_FORCE_INLINE u8* AcquireBlockFromPool(const usize sizeInBytes, const usize alignment, const usize sizeInBytesBlock) noexcept {
        ALT_ASSERT(m_pLastBlockPool);

        // malloc aligns memory suitable for any object type with fundamental alignment
        constexpr static bool isMallocAlreadyAlignedToBlock = alignof(Block) <= Memory::MallocDefaultAlignment();

        const usize defaultBlockAlignmentMalloc = isMallocAlreadyAlignedToBlock
            ? 1
            : alignof(Block);

        const usize extraSizeInBytes = alignment <= alignof(Block)
            ? (defaultBlockAlignmentMalloc - 1) + sizeof(Block)
            : (defaultBlockAlignmentMalloc - 1) + sizeof(Block) + (alignment - 1);

        usize size;
        if (Num::IsOverflowCheckAdd(extraSizeInBytes, sizeInBytes, &size)) {
            return nullptr;
        }

        size = Num::Max(size, sizeInBytesBlock);

        while (m_pLastBlockPool) {

            if (size <= m_pLastBlockPool->totalSizeInBytes) {
                break;
            }

            Block* pBlock = m_pLastBlockPool;
            m_pLastBlockPool = pBlock->pPrev;

            DeallocateBlock(*pBlock);
        }

        Block* pBlock = m_pLastBlockPool;
        if (!pBlock) {
            return nullptr;
        }

        m_pLastBlockPool = pBlock->pPrev;
        pBlock->pPrev = m_pLastBlock;

        m_pLastBlock = pBlock;

        u8* pMemory = pBlock->pMemory;

        const usize alignmentOffset = Memory::Align(pMemory, alignment);

        u8* pMemoryElement = pMemory + alignmentOffset;

        pBlock->pMemory += alignmentOffset + sizeInBytes;
        pBlock->availableSizeInBytes -= alignmentOffset + sizeInBytes;

        return pMemoryElement;
    }

    ALT_FORCE_INLINE ArenaAllocatorImpl(Rule5Move_t, ArenaAllocatorImpl& rhs) noexcept
        : m_pLastBlock(Exchange(rhs.m_pLastBlock, nullptr))
        , m_pLastBlockPool(Exchange(rhs.m_pLastBlockPool, nullptr))
    {}

    ALT_FORCE_INLINE void Assign(Rule5Move_t, ArenaAllocatorImpl& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        DeallocateBytesAll();

        m_pLastBlock = Exchange(rhs.m_pLastBlock, nullptr);
        m_pLastBlockPool = Exchange(rhs.m_pLastBlockPool, nullptr);
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        DeallocateBytesAll();
    }

private:

    Block* m_pLastBlock = nullptr;
    Block* m_pLastBlockPool = nullptr;
};

} // namespace _ArenaAllocatorDetails

namespace TypeTraits {

template <typename SelfT, ArenaAllocatorTrait TRAIT, ArenaAllocatorBits bits>
struct __is_trivially_reallocatable__<_ArenaAllocatorDetails::ArenaAllocatorImpl<SelfT, TRAIT, bits>>
    : std::true_type
{};

} // namespace TypeTraits

/**
 * @brief Templated version of ArenaAllocator.
 * 
 * @param MIN_SIZE_IN_BYTES Minimum size in bytes per block.
 * @param MAX_SIZE_IN_BYTES Maximum size in bytes per block (can be ignored if the allocation size exceeds this limit).
 * @param BITS - see. 'ArenaAllocatorBits'.
 * 
 * @details By default if MIN_SIZE_IN_BYTES != MIN_SIZE_IN_BYTES the
 * 'ExponentialBlockSize' strategy used for new allocated blocks.
 * If MIN_SIZE_IN_BYTES == MIN_SIZE_IN_BYTES the 'FixedBlockSize' strategy
 * used. For more info see. `ArenaAllocatorTrait`.
 * 
 * @example
 * 
 * using Arena = Alt::ArenaAllocator<64, Memory::GB(2), ArenaAllocatorBits::PoolForInternalBlocks>; 
 * 
 * Arena arena;
 * 
 * Arena>::Checkpoint checkpoint = arena.GetCheckpoint();
 * usize* pVal = arena.Allocate<usize>(4);
 * assert(pVal);
 * 
 * *pVal = 44;
 * 
 * arena.ResetToCheckpoint(checkpoint);
 * arena.ReleaseBytesAllToPool();
 * 
 * ...
 * 
 * arena.DeallocateBytesAll();
 * arena.DeallocateBytesExceptFirstBlockAndReset(); // useful to shrink unused memory in arena (but after all arena object was destructed)
 */
template <
    usize MIN_SIZE_IN_BYTES,
    usize MAX_SIZE_IN_BYTES,
    ArenaAllocatorBits bits
>
class ArenaAllocator
    : public _ArenaAllocatorDetails::ArenaAllocatorImpl<
        ArenaAllocator<MIN_SIZE_IN_BYTES, MAX_SIZE_IN_BYTES, bits>,
        _ArenaAllocatorDetails::arena_trait_v<MIN_SIZE_IN_BYTES, MAX_SIZE_IN_BYTES>,
        bits
    >
{
private:

    static_assert(MIN_SIZE_IN_BYTES <= MAX_SIZE_IN_BYTES, "");

    struct IgnoreInitialization_t{};
    constexpr static IgnoreInitialization_t IgnoreInitialization = IgnoreInitialization_t{};

public:

    constexpr static ArenaAllocatorTrait TRAIT = _ArenaAllocatorDetails::arena_trait_v<MIN_SIZE_IN_BYTES, MAX_SIZE_IN_BYTES>;

    ALT_FORCE_INLINE ArenaAllocator CreateFromPreciseMemoryDesc() noexcept {

        ArenaAllocator self{ IgnoreInitialization };
        self.m_MinSizeInBytes = MIN_SIZE_IN_BYTES;
        self.m_MinSizeInBytes = MAX_SIZE_IN_BYTES;

        return self;
    }

    ALT_FORCE_INLINE ArenaAllocator() noexcept {

        m_MinSizeInBytes = Memory::RoundUpToPageSizeWrap(MIN_SIZE_IN_BYTES);
        m_MaxSizeInBytes = Memory::RoundUpToPageSizeWrap(MAX_SIZE_IN_BYTES);
    }

    ALT_FORCE_INLINE usize GetMinSizeInBytes() const noexcept {
        return m_MinSizeInBytes;
    }

    ALT_FORCE_INLINE usize GetMaxSizeInBytes() const noexcept {
        return m_MaxSizeInBytes;
    }

private:

    ALT_FORCE_INLINE ArenaAllocator(const IgnoreInitialization_t) noexcept
    {}

private:

    usize m_MinSizeInBytes;
    usize m_MaxSizeInBytes;
};

/**
 * @brief Templated version of "dynamic" ArenaAllocator - store object memory layout
 * description with arena.
 * 
 * @param TRAIT Trait of the arena - grow strategy.
 * @param BITS - see. 'ArenaAllocatorBits'.
 * @example
 * 
 * using Arena = Alt::ArenaAllocatorDynamic<Alt::ArenaAllocatorTrait::ExponentialBlockSize, ArenaAllocatorBits::PoolForInternalBlocks>;
 * 
 * Arena::MemoryDesc desc{};
 * desc.minSizeInBytes = Alt::Memory::KB(16);
 * Arena arena{ desc };
 * 
 * {
 *     Arena::UniquePtr<Entity> entity = arena.Allocate<Entity>("Text Label"_salloc);
 *     assert(entity);
 * 
 *     entity->SetText("Arena allocated Entity"_salloc);
 * }
 * 
 * arena.ReleaseBytesAllToPool(); // or arena.DeallocateBytesExceptFirstBlockAndReset();
 */
template <ArenaAllocatorBits bits>
class ArenaAllocatorDynamic<ArenaAllocatorTrait::ExponentialBlockSize, bits>
    : public _ArenaAllocatorDetails::ArenaAllocatorImpl<
        ArenaAllocatorDynamic<ArenaAllocatorTrait::ExponentialBlockSize, bits>,
        ArenaAllocatorTrait::ExponentialBlockSize,
        bits
    >
{
private:

    using Super = _ArenaAllocatorDetails::ArenaAllocatorImpl<
        ArenaAllocatorDynamic<ArenaAllocatorTrait::ExponentialBlockSize, bits>,
        ArenaAllocatorTrait::ExponentialBlockSize,
        bits
    >;

    struct IgnoreInitialization_t{};
    constexpr static IgnoreInitialization_t IgnoreInitialization = IgnoreInitialization_t{};

public:

    constexpr static ArenaAllocatorTrait TRAIT = ArenaAllocatorTrait::ExponentialBlockSize;

    struct MemoryDesc {

        usize minSizeInBytes = Memory::KB(4);
        usize maxSizeInBytes = ALT_PLATFORM_32BIT_BOOL ? Memory::GB(2) : Num::Desc<usize>::Max();
    };

    ALT_FORCE_INLINE ArenaAllocatorDynamic CreateFromPreciseMemoryDesc(const MemoryDesc& desc) noexcept {

        const usize minSizeInBytes = Num::Min(desc.minSizeInBytes, desc.maxSizeInBytes);
        const usize maxSizeInBytes = Num::Max(desc.minSizeInBytes, desc.maxSizeInBytes);

        ArenaAllocatorDynamic self{ IgnoreInitialization };
        self.m_Desc.minSizeInBytes = minSizeInBytes;
        self.m_Desc.maxSizeInBytes = maxSizeInBytes;

        return self;
    }

    ALT_FORCE_INLINE ArenaAllocatorDynamic(const MemoryDesc& desc) noexcept {

        const usize minSizeInBytes = Num::Min(desc.minSizeInBytes, desc.maxSizeInBytes);
        const usize maxSizeInBytes = Num::Max(desc.minSizeInBytes, desc.maxSizeInBytes);

        m_Desc.minSizeInBytes = Memory::RoundUpToPageSizeWrap(minSizeInBytes);
        m_Desc.maxSizeInBytes = Memory::RoundUpToPageSizeWrap(maxSizeInBytes);
    }

    ArenaAllocatorDynamic(ArenaAllocatorDynamic&& rhs) noexcept
        : Super(std::move(rhs))
        , m_Desc(std::move(rhs.m_Desc))
    {}

    ArenaAllocatorDynamic& operator=(ArenaAllocatorDynamic&& rhs) noexcept {

        if (this == &rhs) {
            return *this;
        }

        Super::operator=(std::move(rhs));
        m_Desc = std::move(rhs.m_Desc);

        return *this;
    }

    ALT_FORCE_INLINE usize GetMinSizeInBytes() const noexcept {
        return m_Desc.minSizeInBytes;
    }

    ALT_FORCE_INLINE usize GetMaxSizeInBytes() const noexcept {
        return m_Desc.maxSizeInBytes;
    }

private:

    ALT_FORCE_INLINE ArenaAllocatorDynamic(const IgnoreInitialization_t) noexcept
    {}

private:

    MemoryDesc m_Desc;
};

template <ArenaAllocatorBits bits>
class ArenaAllocatorDynamic<ArenaAllocatorTrait::FixedBlockSize, bits>
    : public _ArenaAllocatorDetails::ArenaAllocatorImpl<
        ArenaAllocatorDynamic<ArenaAllocatorTrait::FixedBlockSize, bits>,
        ArenaAllocatorTrait::FixedBlockSize,
        bits
    >
{
private:

    using Super = _ArenaAllocatorDetails::ArenaAllocatorImpl<
        ArenaAllocatorDynamic<ArenaAllocatorTrait::FixedBlockSize, bits>,
        ArenaAllocatorTrait::FixedBlockSize,
        bits
    >;

    struct IgnoreInitialization_t{};
    constexpr static IgnoreInitialization_t IgnoreInitialization = IgnoreInitialization_t{};

public:

    constexpr static ArenaAllocatorTrait TRAIT = ArenaAllocatorTrait::FixedBlockSize;

    struct MemoryDesc {

        usize sizeInBytes = Memory::KB(4);
    };

    ALT_FORCE_INLINE ArenaAllocatorDynamic CreateFromPreciseMemoryDesc(const MemoryDesc& desc) noexcept {

        ArenaAllocatorDynamic self{ IgnoreInitialization };
        self.m_Desc.sizeInBytes = desc.sizeInBytes;

        return self;
    }

    ALT_FORCE_INLINE ArenaAllocatorDynamic(const MemoryDesc& desc) noexcept {

        m_Desc.sizeInBytes = Memory::RoundUpToPageSizeWrap(desc.sizeInBytes);
    }

    ArenaAllocatorDynamic(ArenaAllocatorDynamic&& rhs) noexcept
        : Super(std::move(rhs))
        , m_Desc(std::move(rhs.m_Desc))
    {}

    ArenaAllocatorDynamic& operator=(ArenaAllocatorDynamic&& rhs) noexcept {

        if (this == &rhs) {
            return *this;
        }

        Super::operator=(std::move(rhs));
        m_Desc = std::move(rhs.m_Desc);

        return *this;
    }

    ALT_FORCE_INLINE usize GetMinSizeInBytes() const noexcept {
        return m_Desc.sizeInBytes;
    }

    ALT_FORCE_INLINE usize GetMaxSizeInBytes() const noexcept {
        return m_Desc.sizeInBytes;
    }

private:

    ALT_FORCE_INLINE ArenaAllocatorDynamic(const IgnoreInitialization_t) noexcept
    {}

private:

    MemoryDesc m_Desc;
};

} // namespace Alt

#endif // __ALT_STD_ARENA_ALLOCATOR__