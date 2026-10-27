#pragma once

#ifndef __ALT_STD_BLOCKS__
#define __ALT_STD_BLOCKS__

#include "../TypeTraits.h"

#include "../Iterators/Iterator.h"

#include "../Utils/Num.h"
#include "../Utils/Slice.h"
#include "../Utils/Optional.h"

namespace Alt {
namespace _BlockDetails {

template <typename T>
struct Blocks {
public:

	struct Block;
	struct FromIDResult;

	class Iterator;

	static usize GetBlockSize(const usize blockID) noexcept {
		return blockID == 0 ? 2 : static_cast<usize>(1) << blockID;
	}

	static usize ToID(const usize blockID, const usize idx) noexcept {

		// in 2^0 we store 2 elements
		return blockID == 0
			? idx
			: (static_cast<usize>(1) << blockID) + idx;
	}

	static FromIDResult FromID(const usize i) noexcept {

		// in 2^0 we store 2 elements
		FromIDResult result;

		result.blockID = i < 2
			? 0
			: Num::Log2(i);

		const usize baseID = i < 2
			? 0
			: (static_cast<usize>(1) << result.blockID);

		result.idx = i - baseID;

		return result;
	}

	Blocks() noexcept = default;

	bool IsEmpty() const noexcept {
		return pElementEndInCurrentBlock == blocks->pMemory;
	}

	usize Size() const noexcept {

		const usize blockID = pCurrentBlock - blocks;
		const usize idx = pElementEndInCurrentBlock - pCurrentBlock->pMemory;

		return ToID(blockID, idx);
	}

	usize Capacity() const noexcept {

		usize blockID = pCurrentBlock - blocks;
		for (Block* pBlock = pCurrentBlock; pBlock->pMemory && blockID < MAX_BLOCK_CNT; ++blockID, ++pBlock) {
		}

		return ToID(blockID, 0);
	}

	const T& operator[](const usize i) const noexcept {
		ALT_ASSERT(i < Size());

		const FromIDResult result = FromID(i);
		return blocks[result.blockID].pMemory[result.idx];
	}

	T& operator[](const usize i) noexcept {
		ALT_ASSERT(i < Size());

		const FromIDResult result = FromID(i);
		return blocks[result.blockID].pMemory[result.idx];
	}

	class IteratorRange {
	public:

		IteratorRange(const Blocks* pBlocks) noexcept
			: m_pBlocks(pBlocks)
		{
			m_pCurrentBlock = m_pBlocks->blocks;
			m_BlockID = 0;
		}

		Optional<Slice<T>> Next() noexcept {

			if (!m_pBlocks) {

				return Nullopt;
			}

			if (m_pCurrentBlock == m_pBlocks->pCurrentBlock) {

				Optional<Slice<T>> result = m_pCurrentBlock->pMemory != m_pBlocks->pElementEndInCurrentBlock
					? Slice<T>{ m_pCurrentBlock->pMemory, m_pBlocks->pElementEndInCurrentBlock }
					: Optional<Slice<T>>::Null();

				m_pBlocks = nullptr;

				return result;
			}

			Slice<T> result{
				m_pCurrentBlock->pMemory,
				GetBlockSize(m_BlockID)
			};

			m_pCurrentBlock += 1;
			m_BlockID += 1;

			return result;
		}

	private:

		friend class Iterator;

		const Blocks* m_pBlocks = nullptr;

		const Block* m_pCurrentBlock = nullptr;
		usize m_BlockID = 0;
	};

	class Iterator {
	public:

		Iterator(const Blocks* pBlocks) noexcept
			: m_IteratorRange(pBlocks)
		{
			if (!pBlocks) {
				return;
			}

			m_Range = Slice<T>{ pBlocks->blocks->pMemory, (usize)0 };
		}

		T* Next() noexcept {

			if (m_Range.IsEmpty()) {

				if (Optional<Slice<T>> range = m_IteratorRange.Next()) {

					m_Range = *range;
				}
				else {

					return nullptr;
				}
			}

			T* pResult = m_Range.Data();
			m_Range = Slice<T>{ m_Range.Data() + 1, m_Range.GetSize() - 1 };

			return pResult;
		}

		void Advance(const usize n) noexcept {

			if (!m_IteratorRange.m_pBlocks) {
				return;
			}

			const usize blockIDInIteration = m_IteratorRange.m_BlockID == 0
				? m_IteratorRange.m_BlockID
				: m_IteratorRange.m_BlockID - 1;

			const Block* pCurrentBlockInIteration = m_IteratorRange.m_BlockID == 0
				? m_IteratorRange.m_pCurrentBlock
				: m_IteratorRange.m_pCurrentBlock - 1;

			const usize i = ToID(blockIDInIteration, m_Range.Data() - pCurrentBlockInIteration->pMemory) + n;
			const FromIDResult result = FromID(i);

			m_IteratorRange.m_pCurrentBlock = pCurrentBlockInIteration + result.blockID;
			m_IteratorRange.m_BlockID = result.blockID;

			T* ptr = m_IteratorRange.m_pCurrentBlock->pMemory + result.idx;

			if (m_IteratorRange.m_pCurrentBlock != m_IteratorRange.m_pBlocks->pCurrentBlock) {

				m_Range = Slice<T>{ ptr, Blocks::GetBlockSize(result.blockID) - result.idx };

				m_IteratorRange.m_pCurrentBlock += 1;
				m_IteratorRange.m_BlockID += 1;
			}
			else {

				m_Range = Slice<T>{ ptr, m_IteratorRange.m_pBlocks->pElementEndInCurrentBlock };
				m_IteratorRange.m_pBlocks = nullptr;
			}
		}

	private:

		IteratorRange m_IteratorRange;
		Slice<T> m_Range;
	};

public:

	constexpr static usize MAX_BLOCK_CNT = Num::Desc<usize>::Bits();
	constexpr static usize DIGITS = MAX_BLOCK_CNT;

public:

	struct Block {

		T* pMemory = nullptr;
	};

	struct FromIDResult {

		usize blockID;
		usize idx;
	};

public:

	Block blocks[MAX_BLOCK_CNT - 1]{};

	Block* pCurrentBlock = nullptr;
	T* pElementEndInCurrentBlock = nullptr;
};

} // namespace _BlockDetails
} // namespace Alt

#endif // __ALT_STD_BLOCKS__