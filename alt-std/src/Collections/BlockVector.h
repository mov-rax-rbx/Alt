#pragma once

#ifndef __ALT_STD_BLOCK_VECTOR__
#define __ALT_STD_BLOCK_VECTOR__

#include "../IfErr.h"
#include "../TypeTraits.h"

#include "../Collections/Blocks.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Memory/UniquePtr.h"

namespace Alt {

template <typename T_, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = 16>
class BlockVector {
private:

	using T = TypeTraits::clean_t<T_>;

	using Blocks = typename _BlockDetails::Blocks<T>;

	using Block = typename Blocks::Block;
	using FromIDResult = typename Blocks::FromIDResult;

	static_assert(Num::IsPowerOf2(MIN_ELEMENT_CAPACITY_ON_ALLOCATE_));

public:

	using Type = T;
	using Iterator = typename Blocks::Iterator;

	constexpr static usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE = MIN_ELEMENT_CAPACITY_ON_ALLOCATE_;

	template <typename ...ArgsT>
	ALT_NODISCARD static Optional<BlockVector> AllocatePush(ArgsT&&... args) noexcept {
		iferr_scope;

		constexpr usize size = sizeof...(ArgsT);

		BlockVector result{};
		result.Reserve(Num::Max(size, MIN_ELEMENT_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

		PushVariadicImpl(result, std::forward<ArgsT>(args)...);

		return result;
	}

	ALT_NODISCARD static Optional<BlockVector> AllocateCapacity(const usize size) noexcept {
		iferr_scope;

		BlockVector result{};
		result.Reserve(Num::Max(size, MIN_ELEMENT_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

		return result;
	}

	ALT_NODISCARD static Optional<BlockVector> Allocate(const usize size) noexcept {
		iferr_scope;

		BlockVector result{};

		if (size < MIN_ELEMENT_CAPACITY_ON_ALLOCATE) {

			result.Reserve(MIN_ELEMENT_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;
		}

		result.Resize(size) iferr_return Nullopt;

		return result;
	}

	template <typename U = T,
		std::enable_if_t<std::is_constructible_v<T, U>>* = nullptr>
	ALT_NODISCARD static Optional<BlockVector> Allocate(const usize size, const U& val) noexcept {
		iferr_scope;

		BlockVector result{};

		if (size < MIN_ELEMENT_CAPACITY_ON_ALLOCATE) {

			result.Reserve(MIN_ELEMENT_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;
		}

		result.Resize(size, val) iferr_return Nullopt;

		return result;
	}

	BlockVector() noexcept = default;
	~BlockVector() noexcept = default;

	BlockVector(const BlockVector&) noexcept = delete;
	BlockVector& operator=(const BlockVector&) noexcept = delete;

	template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE = MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
	BlockVector(BlockVector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>&& rhs) noexcept {

		Memory::MoveObjectsFromFirstElementNonOverlap(m_pBlocks.blocks, rhs.m_pBlocks.blocks, rhs.m_pBlocks.blocks + MAX_BLOCK_CNT - 1);

		const usize blockOffset = rhs.m_pBlocks.pCurrentBlock - rhs.m_pBlocks.blocks;
		const usize elementOffset = rhs.m_pBlocks.pElementEndInCurrentBlock - rhs.m_pBlocks.pCurrentBlock->pMemory;

		m_pBlocks.pCurrentBlock = m_pBlocks.blocks + blockOffset;
		m_pBlocks.pElementEndInCurrentBlock = m_pBlocks.pCurrentBlock->pMemory + elementOffset;
	}

	template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE = MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
	BlockVector& operator=(BlockVector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>&& rhs) noexcept {

		if (static_cast<void*>(this) == &rhs) {
			return *this;
		}

		//m_pBlocks = nullptr;

		//Memory::MoveObjectsNonOverlap(rhs.m_pBlocks.blocks, rhs.m_pBlocks.blocks + MAX_BLOCK_CNT - 1, m_pBlocks.blocks);

		//const usize blockOffset = rhs.m_pBlocks.pCurrentBlock - rhs.m_pBlocks.blocks;
		//const usize elementOffset = rhs.m_pBlocks.pElementEndInCurrentBlock - rhs.m_pBlocks.pCurrentBlock->pMemory;

		//m_pBlocks.pCurrentBlock = m_pBlocks.blocks + blockOffset;
		//m_pBlocks.pElementEndInCurrentBlock = m_pBlocks.pCurrentBlock->pMemory + elementOffset;

		//m_pBlocks = std::move(rhs.m_pBlocks);

		return *this;
	}

	ALT_NODISCARD Optional<BlockVector> Clone() const noexcept {
		iferr_scope;

		BlockVector result{};

		if (IsEmpty()) {

			return result;
		}

		if (result.TryInitialize() == InitializeResult::Failed) {

			return Nullopt;
		}

		// TODO:
		// Move stuff for 'Blocks'
		// Finish 'Clone'
		// Finish 'MIN_ELEMENT_CAPACITY_ON_ALLOCATE'
		// Tests
		// Allocate main control blocks in first memory allocation

		usize blockID = 0;

		const Block* pBlock = m_pBlocks->blocks;
		Block* pBlockResult = result.m_pBlocks->blocks;

		for (; pBlock != m_pBlocks->pCurrentBlock; ++pBlock, blockID) {
		}

		return result;
	}

	//ALT_NODISCARD Optional<BlockVector> Clone() const noexcept {
	//	iferr_scope;

	//	if (!m_pBlocks) {

	//		return BlockVector{};
	//	}

	//	return m_pBlocks->Clone();

	//	BlockVector result{};
	//	const usize capacity = GetCapacity();

	//	T* pMemory = Memory::AlignedMalloc<T>(capacity) iferr_return Nullopt;
	//	Memory::ConstructCopyObjectsNonOverlap(m_pMemoryBegin, m_pMemoryEnd, pMemory);

	//	result.m_pMemoryBegin = pMemory;
	//	result.m_pMemoryEnd = pMemory + GetSize();
	//	result.m_pCapacityEnd = pMemory + capacity;

	//	return result;
	//}

	bool Push(const T& val) noexcept {

		return PushImpl(val);
	}

	bool Push(T&& val) noexcept {

		return PushImpl(std::move(val));
	}

	T Pop() noexcept {

		m_pBlocks->pElementEndInCurrentBlock -= 1;
		T result = Memory::MoveDestructibleObject(*m_pBlocks->pElementEndInCurrentBlock);

		const bool isFirstBlock = m_pBlocks->pCurrentBlock == m_pBlocks->blocks;
		const bool isEmptyBlock = m_pBlocks->pElementEndInCurrentBlock == m_pBlocks->pCurrentBlock->pMemory;

		if (!isFirstBlock && isEmptyBlock) {

			m_pBlocks->pCurrentBlock -= 1;

			const usize blockSize = GetBlockSize(m_pBlocks->pCurrentBlock - m_pBlocks->blocks);
			m_pBlocks->pElementEndInCurrentBlock = m_pBlocks->pCurrentBlock->pMemory + blockSize;
		}

		return result;
	}

	// TODO: Iterators to simplify block handling and better Resize which do not initialize memory
	//template <typename U = T, std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
	//bool Insert(const BlockVector<U>& blockVector, const usize i) noexcept {

	//    const usize currentSize = Size();
	//    ALT_ASSERT(i <= currentSize);

	//    const usize size = blockVector.Size();
	//    if (size == 0) {
	//        return true;
	//    }

	//    const bool isOk = Resize(currentSize + size);
	//    if (!isOk) {
	//        return false;
	//    }

	//    if (i < currentSize) {

	//        Iterator src = GetIterator();
	//        src.Advance(i);

	//        Iterator dst = GetIterator();
	//        dst.Advance(i + size);

	//        while (T* pDst = dst.Next()) {

	//            T* pSrc = src.Next();
	//            *pDst = std::move(*pSrc);
	//        }
	//    }

	//    {
	//        Iterator src = blockVector.GetIterator();

	//        Iterator dst = GetIterator();
	//        dst.Advance(i);

	//        for (usize i = 0; i < size; ++i) {

	//            U* pSrc = src.Next();
	//            T* pDst = dst.Next();

	//            *pDst = static_cast<T>(*pSrc);
	//        }
	//    }

	//    return true;
	//}

	bool IsEmpty() const noexcept {

		return !m_pBlocks || m_pBlocks->IsEmpty();
	}

	bool IsPopulated() const noexcept {

		return !IsEmpty();
	}

	usize GetSize() const noexcept {

		return m_pBlocks
			? m_pBlocks->Size()
			: 0;
	}

	usize GetCapacity() const noexcept {

		return m_pBlocks
			? m_pBlocks->Capacity()
			: 0;
	}

	void Clear() noexcept {

		if (IsEmpty()) {
			return;
		}

		Iterator iterator = GetIterator();
		while (T* pVal = iterator.Next()) {

			pVal->~T();
		}

		m_pBlocks->pCurrentBlock = m_pBlocks->blocks;
		m_pBlocks->pElementEndInCurrentBlock = m_pBlocks->blocks->pMemory;
	}

	bool Reserve(const usize n) noexcept {
		iferr_scope;

		if (n <= GetCapacity()) {

			return true;
		}

		if (TryInitialize() == InitializeResult::Failed) {

			return false;
		}

		const usize currentBlockID = m_pBlocks->pCurrentBlock - m_pBlocks->blocks;

		usize advance = n - ToID(currentBlockID, 0);
		advance = Num::SubWrap(advance, GetBlockSize(currentBlockID));

		Block* pBlock = m_pBlocks->pCurrentBlock + 1;
		usize blockID = currentBlockID + 1;

		while (advance != 0) {

			ALT_ASSERT(blockID != MAX_BLOCK_CNT);

			const usize size = GetBlockSize(blockID);
			advance = Num::SubWrap(advance, size);

			if (!pBlock->pMemory) {

				pBlock->pMemory = Memory::AlignedMalloc<T>(size) iferr_return false;
			}

			pBlock += 1;
			blockID += 1;
		}

		return true;
	}

	bool Resize(const usize n, const T& val) noexcept {

		if (TryInitialize() == InitializeResult::Failed) {

			return false;
		}

		const FromIDResult result = FromID(n);
		Block* pResultCurrentBlock = m_pBlocks->blocks + result.blockID;

		if (n <= GetSize()) {

			if (pResultCurrentBlock != m_pBlocks->pCurrentBlock) {

				const Block* pBlock = pResultCurrentBlock;
				usize blockID = result.blockID;

				Memory::DestroyObjects(pBlock->pMemory + result.idx, pBlock->pMemory + GetBlockSize(blockID));

				pBlock += 1;
				blockID += 1;

				for (; pBlock != m_pBlocks->pCurrentBlock; ++pBlock, ++blockID) {

					Memory::DestroyObjects(pBlock->pMemory, pBlock->pMemory + GetBlockSize(blockID));
				}

				Memory::DestroyObjects(pBlock->pMemory, m_pBlocks->pElementEndInCurrentBlock);
			}
			else {

				Memory::DestroyObjects(pResultCurrentBlock->pMemory + result.idx, m_pBlocks->pElementEndInCurrentBlock);
			}
		}
		else {

			if (m_pBlocks->pCurrentBlock != pResultCurrentBlock) {

				Block* pBlock = m_pBlocks->pCurrentBlock;
				usize blockID = m_pBlocks->pCurrentBlock - m_pBlocks->blocks;

				iferr_scope_on_err {

					m_pBlocks->pCurrentBlock = pBlock - 1;
					m_pBlocks->pElementEndInCurrentBlock = pBlock->pMemory + GetBlockSize(blockID - 1);
				};

				Memory::FillCloneObjectsFromFirstElement(m_pBlocks->pElementEndInCurrentBlock, pBlock->pMemory + GetBlockSize(blockID), val);

				pBlock += 1;
				blockID += 1;

				for (; blockID < result.blockID; ++pBlock, ++blockID) {

					const usize size = GetBlockSize(blockID);
					if (!pBlock->pMemory) {

						pBlock->pMemory = Memory::AlignedMalloc<T>(size) iferr_return false;
					}

					Memory::FillCloneConstructObjectsFromFirstElement(pBlock->pMemory, pBlock->pMemory + size, val);
				}

				const usize size = GetBlockSize(result.blockID);
				if (!pBlock->pMemory) {

					pBlock->pMemory = Memory::AlignedMalloc<T>(size) iferr_return false;
				}

				Memory::FillCloneConstructObjectsFromFirstElement(pBlock->pMemory, pBlock->pMemory + result.idx, val);
			}
			else {

				Memory::FillCloneConstructObjectsFromFirstElement(m_pBlocks->pElementEndInCurrentBlock, pResultCurrentBlock->pMemory + result.idx, val);
			}
		}

		if (result.idx != 0) {

			m_pBlocks->pCurrentBlock = pResultCurrentBlock;
			m_pBlocks->pElementEndInCurrentBlock = m_pBlocks->pCurrentBlock->pMemory + result.idx;
		}
		else {

			if (result.blockID != 0) {

				m_pBlocks->pCurrentBlock = pResultCurrentBlock - 1;
				m_pBlocks->pElementEndInCurrentBlock = m_pBlocks->pCurrentBlock->pMemory + GetBlockSize(result.blockID - 1);
			}
			else {

				m_pBlocks->pCurrentBlock = m_pBlocks->blocks;
				m_pBlocks->pElementEndInCurrentBlock = m_pBlocks->blocks->pMemory;
			}
		}

		return true;
	}

	bool Resize(const usize n) noexcept {

		if (TryInitialize() == InitializeResult::Failed) {

			return false;
		}

		const FromIDResult result = FromID(n);
		Block* pResultCurrentBlock = m_pBlocks->blocks + result.blockID;

		if (n <= GetSize()) {

			if (pResultCurrentBlock != m_pBlocks->pCurrentBlock) {

				const Block* pBlock = pResultCurrentBlock;
				usize blockID = result.blockID;

				Memory::DestroyObjects(pBlock->pMemory + result.idx, pBlock->pMemory + GetBlockSize(blockID));

				pBlock += 1;
				blockID += 1;

				for (; pBlock != m_pBlocks->pCurrentBlock; ++pBlock, ++blockID) {

					Memory::DestroyObjects(pBlock->pMemory, pBlock->pMemory + GetBlockSize(blockID));
				}

				Memory::DestroyObjects(pBlock->pMemory, m_pBlocks->pElementEndInCurrentBlock);
			}
			else {

				Memory::DestroyObjects(pResultCurrentBlock->pMemory + result.idx, m_pBlocks->pElementEndInCurrentBlock);
			}
		}
		else {

			if (m_pBlocks->pCurrentBlock != pResultCurrentBlock) {

				Block* pBlock = m_pBlocks->pCurrentBlock;
				usize blockID = m_pBlocks->pCurrentBlock - m_pBlocks->blocks;

				iferr_scope_on_err {

					m_pBlocks->pCurrentBlock = pBlock - 1;
					m_pBlocks->pElementEndInCurrentBlock = pBlock->pMemory + GetBlockSize(blockID - 1);
				};

				Memory::FillConstructDefaultObjects(m_pBlocks->pElementEndInCurrentBlock, pBlock->pMemory + GetBlockSize(blockID));

				pBlock += 1;
				blockID += 1;

				for (; blockID < result.blockID; ++pBlock, ++blockID) {

					const usize size = GetBlockSize(blockID);
					if (!pBlock->pMemory) {

						pBlock->pMemory = Memory::AlignedMalloc<T>(size) iferr_return false;
					}

					Memory::FillConstructDefaultObjects(pBlock->pMemory, pBlock->pMemory + size);
				}

				const usize size = GetBlockSize(result.blockID);
				if (!pBlock->pMemory) {

					pBlock->pMemory = Memory::AlignedMalloc<T>(size) iferr_return false;
				}

				Memory::FillConstructDefaultObjects(pBlock->pMemory, pBlock->pMemory + result.idx);
			}
			else {

				Memory::FillConstructDefaultObjects(m_pBlocks->pElementEndInCurrentBlock, pResultCurrentBlock->pMemory + result.idx);
			}
		}

		if (result.idx != 0) {

			m_pBlocks->pCurrentBlock = pResultCurrentBlock;
			m_pBlocks->pElementEndInCurrentBlock = m_pBlocks->pCurrentBlock->pMemory + result.idx;
		}
		else {

			if (result.blockID != 0) {

				m_pBlocks->pCurrentBlock = pResultCurrentBlock - 1;
				m_pBlocks->pElementEndInCurrentBlock = m_pBlocks->pCurrentBlock->pMemory + GetBlockSize(result.blockID - 1);
			}
			else {

				m_pBlocks->pCurrentBlock = m_pBlocks->blocks;
				m_pBlocks->pElementEndInCurrentBlock = m_pBlocks->blocks->pMemory;
			}
		}

		return true;
	}

	void ShrinkToFitBlocks(usize remainBlocks = 0) noexcept {

		if (!m_pBlocks) {
			return;
		}

		const bool isEmpty = IsEmpty();

		Block* pBlock = m_pBlocks->pCurrentBlock + (isEmpty ? 0 : 1);
		usize blockID = pBlock - m_pBlocks->blocks;

		while (remainBlocks > 0 && pBlock->pMemory && blockID < MAX_BLOCK_CNT) {

			remainBlocks -= 1;

			pBlock += 1;
			blockID += 1;
		}

		for (; pBlock->pMemory && blockID < MAX_BLOCK_CNT; ++pBlock, ++blockID) {

			Memory::AlignedFree(pBlock->pMemory);
			pBlock->pMemory = nullptr;
		}

		if (isEmpty && !m_pBlocks->pCurrentBlock->pMemory) {

			m_pBlocks = nullptr;
		}
	}

	T RemoveSwapLast(const usize i) noexcept {
		ALT_ASSERT(i < GetSize());

		T& val = operator[](i);
		T* pLast = m_pBlocks->pElementEndInCurrentBlock - 1;

		if (pLast != &val) {

			std::swap(val, *pLast);
		}

		return Pop();
	}

	// TODO: Need better
	//T Remove(const usize i) noexcept {
	//	ALT_ASSERT(i < Size());

	//	{
	//		T& val = operator[](i);
	//		T* pLast = m_pBlocks->pElementEndInCurrentBlock - 1;

	//		if (pLast == &val) {
	//			return *Pop();
	//		}
	//	}

	//	T val = std::move(operator[](i));

	//	Iterator src = GetIterator();
	//	Iterator dst = GetIterator();

	//	dst.Advance(i);
	//	src.Advance(i + 1);

	//	for (;;) {

	//		T* pSrc = src.Next();
	//		T* pDst = dst.Next();

	//		if (!pSrc) {
	//			break;
	//		}

	//		*pDst = std::move(*pSrc);
	//	}

	//	Pop();

	//	return val;
	//}

	const T& operator[](const usize i) const noexcept {
		return m_pBlocks->operator[](i);
	}

	T& operator[](const usize i) noexcept {
		return m_pBlocks->operator[](i);
	}

	Iterator GetIterator() const noexcept {
		return Iterator(m_pBlocks.Get());
	}

private:

	constexpr static usize MAX_BLOCK_CNT = Blocks::MAX_BLOCK_CNT;

	static usize GetBlockSize(const usize blockID) noexcept {
		return Blocks::GetBlockSize(blockID);
	}

	static usize ToID(const usize blockID, const usize idx) noexcept {
		return Blocks::ToID(blockID, idx);
	}

	static FromIDResult FromID(const usize i) noexcept {
		return Blocks::FromID(i);
	}

	template <typename HeadT, typename ...TailT>
	static void PushVariadicImpl(BlockVector& self, HeadT&& head, TailT&&... tail) noexcept {

		self.Push(std::forward<HeadT>(head));
		PushVariadicImpl(self, std::forward<TailT>(tail)...);
	}

	static void PushVariadicImpl(BlockVector&) noexcept {}

	enum class InitializeResult {
		Initialized,
		Failed,
		Skipped,
	};
	InitializeResult TryInitialize() noexcept {

		iferr_scope_on_err {

			m_pBlocks = nullptr;
		};

		if (m_pBlocks) {

			return InitializeResult::Skipped;
		}

		Blocks* pBlocks = Memory::AlignedMalloc<Blocks>() iferr_return InitializeResult::Failed;
		Memory::ConstructObject(pBlocks, Blocks{});

		m_pBlocks = BlocksUniquePtr{ pBlocks };

		m_pBlocks->pCurrentBlock = m_pBlocks->blocks;
		m_pBlocks->blocks->pMemory = Memory::AlignedMalloc<T>(GetBlockSize(0)) iferr_return InitializeResult::Failed;
		m_pBlocks->pElementEndInCurrentBlock = m_pBlocks->blocks->pMemory;

		return InitializeResult::Initialized;
	}

	template <typename U = T>
	bool PushImpl(U&& val) noexcept {
		iferr_scope;

		usize blockSize; 
		const InitializeResult result = TryInitialize();

		if (result == InitializeResult::Failed) {

			return false;
		}

		if (result == InitializeResult::Initialized) {

			goto value_alloc_label;
		}

		blockSize = GetBlockSize(m_pBlocks->pCurrentBlock - m_pBlocks->blocks);
		if (m_pBlocks->pElementEndInCurrentBlock == m_pBlocks->pCurrentBlock->pMemory + blockSize) {

			if (m_pBlocks->pCurrentBlock == m_pBlocks->blocks + MAX_BLOCK_CNT - 1) {

				return false;
			}

			Block* pBlock = m_pBlocks->pCurrentBlock + 1;
			if (!pBlock->pMemory) {

				const usize blockSize = GetBlockSize(pBlock - m_pBlocks->blocks);
				pBlock->pMemory = Memory::AlignedMalloc<T>(blockSize) iferr_return false;
			}

			m_pBlocks->pCurrentBlock = pBlock;
			m_pBlocks->pElementEndInCurrentBlock = pBlock->pMemory;
		}

value_alloc_label:
		new (m_pBlocks->pElementEndInCurrentBlock) T(std::forward<U>(val));
		m_pBlocks->pElementEndInCurrentBlock += 1;

		return true;
	}

private:

	template <typename T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
	friend class BlockVector;

	struct BlockUniquePtrDeleter {

		void operator()(Blocks* ptr) const noexcept {

			typename Blocks::Iterator iterator{ ptr };
			while (T* pVal = iterator.Next()) {

				pVal->~T();
			}

			ptr->~Blocks();
			Memory::AlignedFree(ptr);
		}
	};

	using BlocksUniquePtr = UniquePtr<Blocks, BlockUniquePtrDeleter>;

	BlocksUniquePtr m_pBlocks = nullptr;
};

} // namespace Alt

#endif // __ALT_STD_BLOCK_VECTOR__