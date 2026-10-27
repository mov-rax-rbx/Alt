#pragma once

#ifndef __ALT_STD_ENTITY_POOL_ALLOCATOR__
#define __ALT_STD_ENTITY_POOL_ALLOCATOR__

#include "../../IfErr.h"
#include "../../TypeTraits.h"

#include "../../Iterators/Iterator.h"

#include "../../Memory/Impl/MemoryPreInclude.h"

#include "../../Utils/Num.h"

namespace Alt {

template <typename T>
class EntityPoolAllocator;

class EntityPoolAllocatorDynamic;

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<EntityPoolAllocator<T>>
    : std::true_type
{};

template <>
struct __is_trivially_reallocatable__<EntityPoolAllocatorDynamic>
    : std::true_type
{};

} // namespace TypeTraits

namespace _EntityPoolAllocatorDetails {

/**
 * @brief Provides the implementation logic for Pool Allocator which supports ID generation on
 * allocations and deallocation of dangling empty blocks.
 * @tparam SelfT The type associated with the end allocator.
 */
template <typename SelfT>
class EntityPoolAllocatorImpl {
private:

    struct Block;

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

public:

    template <typename T>
    class IteratorRange;

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

    /**
     * @brief Used for conversion from ID to BlockID and LocalID
     */
    struct BlockIDAndLocalID {

        usize blockID = 0;
        usize localID = 0;
    };

    /**
     * @brief A contain memory pointer with BlockID and LocalID.
     */
    struct AllocateBytesResult {

        u8* pMemory = nullptr;

        usize blockID = 0;
        usize localID = 0;
    };

    ALT_FORCE_INLINE usize GetElementSizeOf() const noexcept {
        return static_cast<const SelfT*>(this)->GetElementSizeOf();
    }

    ALT_FORCE_INLINE usize GetElementAlignOf() const noexcept {
        return static_cast<const SelfT*>(this)->GetElementAlignOf();
    }

    /**
     * @brief Returns total size of elements in block.
     */
    ALT_NODISCARD ALT_FORCE_INLINE static usize GetBlockSize(const usize blockID) noexcept {

        usize size = static_cast<usize>(1) << blockID;
        size += blockID == 0 ? 1 : 0;

        return size;
    }

    /**
     * @brief Composes BlockID and LocalID to ID.
     */
    ALT_NODISCARD ALT_FORCE_INLINE static usize BlockIDAndLocalIDToID(const usize blockID, const usize localID) noexcept {

        usize size = static_cast<usize>(1) << blockID;
        size -= blockID == 0 ? 1 : 0;

        return size + localID;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize BlockIDAndLocalIDToID(const BlockIDAndLocalID& blockIDAndLocalID) noexcept {

        return BlockIDAndLocalIDToID(blockIDAndLocalID.blockID, blockIDAndLocalID.localID);
    }

    /**
     * @brief Decomposes ID to BlockID and LocalID.
     */
    ALT_NODISCARD ALT_FORCE_INLINE static BlockIDAndLocalID IDToBlockIDAndLocalID(const usize id) noexcept {

        BlockIDAndLocalID result{};

        result.blockID = Num::Log2(id);
        const usize blockPrevSize = static_cast<usize>(1) << result.blockID;

        result.localID = id;
        result.localID -= result.blockID == 0 ? 0 : blockPrevSize;

        return result;
    }

    ALT_FORCE_INLINE EntityPoolAllocatorImpl() noexcept = default;

    EntityPoolAllocatorImpl(const EntityPoolAllocatorImpl&) noexcept = delete;
    EntityPoolAllocatorImpl& operator=(const EntityPoolAllocatorImpl&) noexcept = delete;

    EntityPoolAllocatorImpl(EntityPoolAllocatorImpl&& rhs) noexcept
        : EntityPoolAllocatorImpl(Rule5Move, rhs)
    {}

    EntityPoolAllocatorImpl& operator=(EntityPoolAllocatorImpl&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    ~EntityPoolAllocatorImpl() noexcept {

        Destroy();
    }

    /**
     * @brief Allocates and constructs an object in the pool.
     * @tparam T The type of the object to construct.
     * @tparam Args Variadic arguments used for constructing the object of type T.
     * @param args The values passed directly into the constructor of T.
     * @return A pointer to the newly constructed object, or `nullptr` if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Entity* pEnt = pool.Allocate<Entity>("Entity 0") iferr_return;
     * assert(pEnt);
     */
    template <typename T, typename ...Args>
    ALT_NODISCARD T* Allocate(Args&&... args) noexcept(std::is_nothrow_constructible_v<T>) {
        iferr_scope;

        ALT_ASSERT(sizeof(T) <= GetElementSizeOf());
        ALT_ASSERT(alignof(T) <= GetElementAlignOf());

        AllocateBytesResult result = AllocateBytes() iferr_return nullptr;
        return Memory::ConstructObject<T>(result.pMemory, std::forward<Args>(args)...);
    }

    template <typename T>
    ALT_NODISCARD T* Allocate(T&& data) noexcept(std::is_nothrow_move_constructible_v<T>) {
        iferr_scope;

        ALT_ASSERT(sizeof(T) <= GetElementSizeOf());
        ALT_ASSERT(alignof(T) <= GetElementAlignOf());

        AllocateBytesResult result = AllocateBytes() iferr_return nullptr;
        return Memory::ConstructObject<T>(result.pMemory, std::forward<T>(data));
    }

    /**
     * @brief Destroys and deallocates an object from the specified block.
     * @tparam T The type of the object being destroyed.
     * @param blockID The ID of the memory block containing the object.
     * @param pMemory The pointer to the start of the memory for object T.
     */
    template <typename T>
    void Deallocate(const usize blockID, T* pMemory) noexcept(std::is_nothrow_destructible_v<T>) {

        if (!pMemory) {
            return;
        }

        ALT_ASSERT(
            reinterpret_cast<u8*>(pMemory) >= m_Blocks.blocks[blockID].pMemory &&
            reinterpret_cast<u8*>(pMemory) < m_Blocks.blocks[blockID].pMemory + GetBlockSize(blockID) * GetElementSizeOf()
        );

        Memory::DestroyObject(*pMemory);
        DeallocateBytesByBlockIDAndPtr(blockID, pMemory);
    }

    /**
     * @brief Allocates memory for specified pool sizeof and alignment.
     * @return An optional containing `AllocateBytesResult` on success, or `Nullopt` if no memory is available.
     */
    ALT_NODISCARD Optional<AllocateBytesResult> AllocateBytes() noexcept {
        iferr_scope;

        const usize blockID = Num::Count0BitsRight(m_VacantBlocks);
        if (blockID >= BITS - 1) {
            return Nullopt;
        }

        const usize size = GetBlockSize(blockID);
        Block& block = m_Blocks.blocks[blockID];

        if (!block.pMemory) {

            block = AllocateBlock(size) iferr_return Nullopt;
            m_BlocksSize += 1;
        }

        m_BlocksWhichHaveAtLeastOneElement |= static_cast<usize>(1) << blockID;

        const usize localID = size - block.availableSizeInElements;
        block.availableSizeInElements -= 1;

        if (block.availableSizeInElements == 0) {

            m_VacantBlocks &= ~(static_cast<usize>(1) << blockID);
        }

        u8* pMemory;
        if (block.freePtr != 0) {

            pMemory = reinterpret_cast<u8*>(block.freePtr);
            block.freePtr = *reinterpret_cast<uptr*>(block.freePtr);
        }
        else {

            pMemory = block.pMemory + localID * GetElementSizeOf();
        }

        AllocateBytesResult result{};
        result.pMemory = pMemory;
        result.blockID = blockID;
        result.localID = (pMemory - block.pMemory) / GetElementSizeOf();

        return result;
    }

    /**
     * @brief Returns a memory segment to the pool and updates free list pointers.
     * @param blockID The ID of the block containing the pointer.
     * @param pMemory Pointer to the memory being returned.
     */
    void DeallocateBytesByBlockIDAndPtr(const usize blockID, void* pMemory) noexcept {
        ALT_ASSERT(blockID < BITS - 1);

        Block& block = m_Blocks.blocks[blockID];

        m_VacantBlocks |= static_cast<usize>(1) << blockID;
        block.availableSizeInElements += 1;

        *reinterpret_cast<uptr*>(pMemory) = block.freePtr;
        block.freePtr = reinterpret_cast<uptr>(pMemory);

        if (block.availableSizeInElements == GetBlockSize(blockID)) {

            block.freePtr = 0;
            m_BlocksWhichHaveAtLeastOneElement &= ~(static_cast<usize>(1) << blockID);
        }
    }

    /**
     * @brief Returns a memory segment to the pool and updates free list pointers.
     * @param id The ID of the entity in pool.
     */
    void DeallocateBytesByID(const usize id) noexcept {

        const BlockIDAndLocalID result = IDToBlockIDAndLocalID(id);

        Block& block = m_Blocks.blocks[result.blockID];
        DeallocateBytesByBlockIDAndPtr(result.blockID, block.pMemory + result.localID * GetElementSizeOf());
    }

    /**
     * @brief Finds the BlockID and LocalID for a given pointer.
     * @param pMemory The memory address to resolve. Pointer must be allocated from pool.
     * @return A `BlockIDAndLocalID` struct containing the calculated offsets.
     */
    ALT_NODISCARD BlockIDAndLocalID FindBlockIDAndLocalIDByPtr(const void* pMemory) const noexcept {

        const usize blockID = FindBlockIDByPtr(pMemory);
        const Block& block = m_Blocks.blocks[blockID];

        BlockIDAndLocalID result{};
        result.blockID = blockID;
        result.localID = (reinterpret_cast<const u8*>(pMemory) - block.pMemory) / GetElementSizeOf();

        return result;
    }

    /**
     * @brief Return maximum possible allocated number of blocks.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize GetMaxPossibleNumOfBlocks() noexcept {

        return BITS - 1;
    }

    /**
     * @brief Return currently allocated number of blocks.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetAllocatedNumOfBlocks() const noexcept {

        return m_BlocksSize;
    }

    /**
     * @brief Deallocates memory of all dangling blocks that currently contain no active elements.
     */
    Optional<usize> DeallocateBytesAllBlocksUnused() noexcept {

        const usize numBlocksToExcept = BITS - Num::Count0BitsLeft(m_BlocksWhichHaveAtLeastOneElement);
        const bool isDeallocateBlocks = numBlocksToExcept < m_BlocksSize;

        DeallocateBytesAllBlocksExcept(numBlocksToExcept);

        return isDeallocateBlocks ? MakeOptional(numBlocksToExcept) : Nullopt;
    }

    /**
     * @brief Deallocates memory of all dangling blocks that currently contain no active elements except for the very first vacant one.
     */
    Optional<usize> DeallocateBytesAllBlocksUnusedExceptFirstUnused() noexcept {

        const usize numBlocksToExcept = BITS - Num::Count0BitsLeft(m_BlocksWhichHaveAtLeastOneElement) + 1;
        const bool isDeallocateBlocks = numBlocksToExcept < m_BlocksSize;

        DeallocateBytesAllBlocksExcept(numBlocksToExcept);

        return isDeallocateBlocks ? MakeOptional(numBlocksToExcept) : Nullopt;
    }

    /**
     * @brief Deallocates unused blocks and releases the internal state of remaining active blocks.
     * @param numBlocksToExcept The number of base blocks to except from deallocating.
     */
    void DeallocateBytesExceptBlocksAndRelease(const usize numBlocksToExcept) noexcept {

        DeallocateBytesAllBlocksExcept(numBlocksToExcept);
        ReleaseAllBlocksExcept(0);
    }

    /**
     * @brief Deallocates unused blocks.
     * @param numBlocksToExcept The number of base blocks to except from deallocating.
     */
    void DeallocateBytesAllBlocksExcept(usize numBlocksToExcept) noexcept {

        numBlocksToExcept = Num::Min(numBlocksToExcept, m_BlocksSize);

        for (usize iBlock = numBlocksToExcept; iBlock < m_BlocksSize; ++iBlock) {

            DeallocateBlock(m_Blocks.blocks[iBlock]);
        }

        const usize mask = (static_cast<usize>(1) << numBlocksToExcept) - 1;
        m_VacantBlocks |= ~mask;
        m_BlocksWhichHaveAtLeastOneElement &= mask;

        m_BlocksSize = numBlocksToExcept;
    }

    /**
     * @brief Relases the blocks without deallocationg the underlying memory.
     * @param numBlocksToExcept The number of base blocks to except from releasing.
     */
    void ReleaseAllBlocksExcept(usize numBlocksToExcept) noexcept {

        numBlocksToExcept = Num::Min(numBlocksToExcept, m_BlocksSize);

        for (usize iBlock = numBlocksToExcept; iBlock < m_BlocksSize; ++iBlock) {

            Block& block = m_Blocks.blocks[iBlock];

            block.availableSizeInElements = GetBlockSize(iBlock);
            block.freePtr = 0;
        }

        const usize mask = (static_cast<usize>(1) << numBlocksToExcept) - 1;
        m_VacantBlocks |= ~mask;
        m_BlocksWhichHaveAtLeastOneElement &= mask;
    }

    /**
     * @brief Retrieves current usage statistics.
     * @return A `Stats` struct with statistics.
     */
    ALT_NODISCARD Stats GetStats() const noexcept {

        Stats stats{};
        stats.numBlocks = m_BlocksSize;

        for (usize iBlock = 0; iBlock < m_BlocksSize; ++iBlock) {

            const Block& block = m_Blocks.blocks[iBlock];

            stats.availableElements += block.availableSizeInElements;
            stats.consumedElements += GetBlockSize(iBlock) - block.availableSizeInElements;
        }

        return stats;
    }

    /**
     * @brief Creates an iterator range over a specified segment of the pool.
     * @tparam T The type of elements within the range.
     * @param offset The starting index (absolute position) in the pool where the range begins.
     * @param size The total number of elements to include in the iteration range.
     * @return An IteratorRange<T> object providing a forward-moving iterator over the specified segment.
     * @example
     * 
     * Alt::Pool::IteratorRange<Entity> range = pool.GetIteratorRange<Entity>(40, 4);
     * assert(range.GetSizeHint() == 10);
     */
    template <typename T>
    ALT_NODISCARD ALT_FORCE_INLINE IteratorRange<T> GetIteratorRange(const usize offset, const usize size) const noexcept {

        return IteratorRange<T>{ *this, offset, size };
    }

    template <typename T>
    struct IteratorRangeResult {

        usize blockID = 0;
        usize localID = 0;

        T* pResult = nullptr;
    };

    /**
     * @brief Provides a forward iterator over a specific segment of the pool.
     * @tparam T The type of elements being iterated. Must be a valid type within the allocator's constraints (sizeof and alignment).
     */
    template <typename T>
    class IteratorRange final : public Iterator<IteratorRange<T>, IteratorRangeResult<T>, IIteratorForwardTrait, IIteratorExactSizeTrait> {
    public:

        using Result = IteratorRangeResult<T>;

        ALT_FORCE_INLINE IteratorRange(const EntityPoolAllocatorImpl<SelfT>& self, const usize offset, const usize size) noexcept
            : m_pBlocks(self.m_Blocks.blocks)
            , m_BlocksWhichHaveAtLeastOneElement(self.m_BlocksWhichHaveAtLeastOneElement)
            , m_Size(size)
            , m_SizeOf(self.GetElementSizeOf())
        {
            ALT_ASSERT(sizeof(T) <= self.GetElementSizeOf());
            ALT_ASSERT(alignof(T) <= self.GetElementAlignOf());

            const BlockIDAndLocalID decomposed = self.IDToBlockIDAndLocalID(offset);

            const usize mask = (static_cast<usize>(0b10) << decomposed.blockID) - 1;
            m_BlocksWhichHaveAtLeastOneElement &= ~mask;

            m_BlockID = decomposed.blockID;
            m_LocalID = decomposed.localID;

            const usize blockSize = Num::Min(GetBlockSize(decomposed.blockID) - decomposed.localID, m_Size);
            m_SizeCorrection = blockSize + decomposed.localID;
            m_Size -= blockSize;

            m_pMemory = m_pBlocks[decomposed.blockID].pMemory + decomposed.localID * self.GetElementSizeOf();
            m_pMemoryEnd = m_pMemory + blockSize * self.GetElementSizeOf();
        }

        ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
            return GetSize();
        }

        /**
         * @brief Retrieves the next element and advances the start boundary.
         * @return An Alt::Optional<Result> containing the value, or Alt::Nullopt if the range is exhausted.
         */
        ALT_FORCE_INLINE Optional<Result> Next() noexcept {

            if (m_pMemory == m_pMemoryEnd) {

                if (m_Size == 0) {
                    return Nullopt;
                }

                UpdateStateForNextBlock();
            }

            const usize localID = m_LocalID;
            m_LocalID += 1;

            T* pResult = reinterpret_cast<T*>(m_pMemory);
            m_pMemory += m_SizeOf;

            Result result;
            result.blockID = m_BlockID;
            result.localID = localID;
            result.pResult = pResult;

            return result;
        }

        /**
         * @brief Skips a specified number of elements forward in the current view.
         * @param advance_ The number of elements to skip.
         * @return The actual number of elements successfully skipped.
         */
        ALT_FORCE_INLINE usize Advance(usize advance) noexcept {

            usize result = 0;

            for (;;) {

                const usize localAdvance = Num::Min(static_cast<usize>(m_pMemoryEnd - m_pMemory) / m_SizeOf, advance);

                advance -= localAdvance;
                result += localAdvance;

                m_LocalID += localAdvance;
                m_pMemory += localAdvance * m_SizeOf;

                if (m_pMemory != m_pMemoryEnd || m_Size == 0) {
                    break;
                }

                UpdateStateForNextBlock();
            }

            return result;
        }

        ALT_FORCE_INLINE usize GetSize() const noexcept {

            return (m_SizeCorrection - m_LocalID) + m_Size;
        }

        ALT_FORCE_INLINE bool IsEmpty() const noexcept {

            return GetSize() == 0;
        }

    private:

        ALT_FORCE_INLINE static usize GetBlockSize(const usize blockID) noexcept {

            usize size = static_cast<usize>(1) << blockID;
            size += blockID == 0 ? 1 : 0;

            return size;
        }

        ALT_FORCE_INLINE void UpdateStateForNextBlock() noexcept {

            const usize blockID = Num::Count0BitsRight(m_BlocksWhichHaveAtLeastOneElement);
            ALT_ASSERT(blockID < BITS - 1);

            m_BlocksWhichHaveAtLeastOneElement &= ~(static_cast<usize>(1) << blockID);

            m_BlockID = blockID;
            m_LocalID = 0;

            const usize blockSize = Num::Min(GetBlockSize(blockID), m_Size);
            m_SizeCorrection = blockSize;
            m_Size -= blockSize;

            m_pMemory = m_pBlocks[blockID].pMemory;
            m_pMemoryEnd = m_pBlocks[blockID].pMemory + blockSize * m_SizeOf;
        }

    private:

        const Block* m_pBlocks;
        usize m_BlocksWhichHaveAtLeastOneElement;

        usize m_SizeCorrection;

        usize m_Size;
        usize m_SizeOf;

        usize m_BlockID;
        usize m_LocalID;

        u8* m_pMemory;
        u8* m_pMemoryEnd;
    };

private:

    ALT_FORCE_INLINE usize FindBlockIDByPtr(const void* pMemory) const noexcept {

        usize iBlock = 0;
        bool isInBlock = false;

        while (!isInBlock && iBlock < m_BlocksSize) {

            const Block& block = m_Blocks.blocks[iBlock];

            isInBlock =
                pMemory >= block.pMemory &&
                pMemory < block.pMemory + GetBlockSize(iBlock) * GetElementSizeOf()
            ;

            iBlock += 1;
        }

        ALT_ASSERT(isInBlock);

        return iBlock - 1;
    }

    ALT_FORCE_INLINE Optional<Block> AllocateBlock(const usize sizeInElements) const noexcept {
        iferr_scope;

        const usize elementSizeOf = GetElementSizeOf();
        const usize elementAlignOf = GetElementAlignOf() <= Alt::Memory::MallocDefaultAlignment()
            ? 1
            : GetElementAlignOf();

        Block block{};

        usize totalSizeInBytes = Num::MulOverflowCheck(sizeInElements, elementSizeOf) iferr_return Nullopt;
        totalSizeInBytes = Num::AddOverflowCheck(totalSizeInBytes, elementAlignOf - 1) iferr_return Nullopt;

        u8* pMemory = Memory::Malloc<u8>(totalSizeInBytes) iferr_return Nullopt;

        const usize alignmentOffsetInBytes = Memory::Align(pMemory, elementAlignOf);

        block.pMemory = pMemory + alignmentOffsetInBytes;
        block.availableSizeInElements = sizeInElements;
        block.alignmentOffsetInBytes = alignmentOffsetInBytes;

        return block;
    }

    ALT_FORCE_INLINE static void DeallocateBlock(Block& block) noexcept {

        Memory::Free(block.pMemory - block.alignmentOffsetInBytes);
        block.pMemory = nullptr;
    }

    ALT_FORCE_INLINE EntityPoolAllocatorImpl(Rule5Move_t, EntityPoolAllocatorImpl& rhs) noexcept
        : m_VacantBlocks(Exchange(rhs.m_VacantBlocks, Num::Desc<usize>::Max()))
        , m_BlocksWhichHaveAtLeastOneElement(Exchange(rhs.m_BlocksWhichHaveAtLeastOneElement, 0))
        , m_Blocks(Exchange(rhs.m_Blocks, Blocks{}))
        , m_BlocksSize(Exchange(rhs.m_BlocksSize, 0))
    {}

    ALT_FORCE_INLINE void Assign(Rule5Move_t, EntityPoolAllocatorImpl& rhs) noexcept {

        m_VacantBlocks = Exchange(rhs.m_VacantBlocks, Num::Desc<usize>::Max());
        m_BlocksWhichHaveAtLeastOneElement = Exchange(rhs.m_BlocksWhichHaveAtLeastOneElement, 0);
        m_Blocks = Exchange(rhs.m_Blocks, Blocks{});
        m_BlocksSize = Exchange(rhs.m_BlocksSize, 0);
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        if (m_BlocksSize == 0) {
            return;
        }

        for (usize iBlock = 0; iBlock < m_BlocksSize; ++iBlock) {

            DeallocateBlock(m_Blocks.blocks[iBlock]);
        }

        m_VacantBlocks = Num::Desc<usize>::Max();
        m_BlocksWhichHaveAtLeastOneElement = 0;

        m_BlocksSize = 0;
    }

private:

    constexpr static usize BITS = Num::Desc<usize>::Bits();

    constexpr static usize BLOCK_ID_NONE = BITS;
    constexpr static usize BLOCKS_LOG2 = Num::Log2Constexpr(BITS);

    constexpr static usize BITS_TO_WORDID_RIGHT_SHIFT = Num::Count0BitsRightConstexpr(BITS);
    constexpr static usize BITS_TO_BITIDX_IN_WORD_MASK = BITS - 1;

    struct Block {

        u8* pMemory = nullptr;
        uptr freePtr = 0;

        usize availableSizeInElements = 0;
        usize alignmentOffsetInBytes = 0;
    };

    struct Blocks {

        // sum(2^0...2^(BITS - 1)) == 2^BITS - 1, in 2^0 we store 2 elements see. 'GetBlockSize(...)'
        Block blocks[BITS - 1]{};
    };

    usize m_VacantBlocks = Num::Desc<usize>::Max();
    usize m_BlocksWhichHaveAtLeastOneElement = 0;

    Blocks m_Blocks;
    usize m_BlocksSize = 0;
};

} // namespace _EntityPoolAllocatorDetails

namespace TypeTraits {

template <typename SelfT>
struct __is_trivially_reallocatable__<_EntityPoolAllocatorDetails::EntityPoolAllocatorImpl<SelfT>>
    : std::true_type
{};

} // namespace TypeTraits

/**
 * @brief A Entity Pool Allocator for using a pool based allocation strategy and manipulate with blocks,
 * iterations over defined range of entities.
 * @tparam T The specific type of entity to be stored in the pool.
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::EntityPoolAllocator<Entity> pool;
 * Entity* pEnt = pool.Allocate<Entity>() iferr_return;
 * 
 * AllocateResult result = pool.AllocateWithResult<Entity>() iferr_return;
 * pool.DeallocateBytesByBlockIDAndPtr(result.blockID, result.pMemory);
 */
template <typename T>
class EntityPoolAllocator : public _EntityPoolAllocatorDetails::EntityPoolAllocatorImpl<EntityPoolAllocator<T>> {
private:

    using Super = _EntityPoolAllocatorDetails::EntityPoolAllocatorImpl<EntityPoolAllocator<T>>;

public:

    ALT_FORCE_INLINE EntityPoolAllocator() noexcept = default;

    ALT_FORCE_INLINE usize GetElementSizeOf() const noexcept {
        return Num::Max(sizeof(T), alignof(T), sizeof(uptr));
    }

    ALT_FORCE_INLINE usize GetElementAlignOf() const noexcept {
        return Num::Max(alignof(T), alignof(uptr));
    }

    struct AllocateResult {

        T* pMemory = nullptr;

        usize blockID = 0;
        usize localID = 0;
    };

    template <typename U, typename ...Args>
    ALT_NODISCARD Optional<AllocateResult> AllocateWithResult(Args&&... args) noexcept(std::is_nothrow_constructible_v<T>) {
        iferr_scope;

        static_assert(sizeof(U) <= sizeof(T), "");
        static_assert(alignof(U) <= alignof(T), "");

        typename Super::AllocateBytesResult res = this->AllocateBytes() iferr_return Nullopt;
        U* pMemory = Memory::ConstructObject<U>(res.pMemory, std::forward<Args>(args)...);

        AllocateResult result{};
        result.pMemory = pMemory;
        result.blockID = res.blockID;
        result.localID = res.localID;

        return result;
    }

    template <typename U>
    ALT_NODISCARD Optional<AllocateResult> AllocateWithResult(U&& data) noexcept(std::is_nothrow_move_constructible_v<U>) {
        iferr_scope;

        static_assert(sizeof(U) <= sizeof(T), "");
        static_assert(alignof(U) <= alignof(T), "");

        typename Super::AllocateBytesResult res = this->AllocateBytes() iferr_return Nullopt;
        U* pMemory = Memory::ConstructObject<U>(res.pMemory, std::forward<U>(data));

        AllocateResult result{};
        result.pMemory = pMemory;
        result.blockID = res.blockID;
        result.localID = res.localID;

        return result;
    }
};

/**
 * @brief A Entity Pool Allocator for using a pool based allocation strategy and manipulate with blocks, allows
 * non compile time description of size and alignment for elements, iterations over defined range of entities.
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::EntityPoolAllocatorDynamic pool{ Alt::EntityPoolAllocatorDynamic::MemoryDesc::Create() };
 * Entity* pEnt = pool.Allocate<Entity>() iferr_return;
 * 
 * AllocateResult result = pool.AllocateWithResult<Entity>() iferr_return;
 * pool.DeallocateBytesByBlockIDAndPtr(result.blockID, result.pMemory);
 */
class EntityPoolAllocatorDynamic : public _EntityPoolAllocatorDetails::EntityPoolAllocatorImpl<EntityPoolAllocatorDynamic> {
private:

    using Super = _EntityPoolAllocatorDetails::EntityPoolAllocatorImpl<EntityPoolAllocatorDynamic>;

public:

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

    ALT_FORCE_INLINE EntityPoolAllocatorDynamic(const MemoryDesc& desc) noexcept {

        m_Desc.elementAlignOf = Num::Max(desc.elementAlignOf, sizeof(uptr));
        m_Desc.elementSizeOf = Num::Max(desc.elementSizeOf, desc.elementAlignOf, sizeof(uptr));
    }

    EntityPoolAllocatorDynamic(EntityPoolAllocatorDynamic&& rhs) noexcept
        : Super(std::move(rhs))
        , m_Desc(std::move(rhs.m_Desc))
    {}

    EntityPoolAllocatorDynamic& operator=(EntityPoolAllocatorDynamic&& rhs) noexcept {

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

private:

    MemoryDesc m_Desc;
};

} // namespace Alt

#endif // __ALT_STD_ENTITY_POOL_ALLOCATOR__