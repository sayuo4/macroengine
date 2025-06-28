// === Page-Based Sparse Set ===
// SparseSet is a data structure optimized for fast insertions, removals,
// and lookups using a sparse set approach with paging for memory efficiency.
//
// Template parameters:
// - T: type of the elements stored in the set.
// - PAGE_SIZE: number of elements per page in the sparse array.
//
// This structure supports:
// - insert(index, value): inserts a value at a given index (throws if index exists).
// - remove(index): removes the value at given index (throws if not found).
// - at(index): accesses element by index (throws if index not valid).
// - contains(index): checks if index exists in the set.
// - iteration over all stored elements using begin() and end().
//
// Internally, it uses a dense vector to store values compactly, and a
// sparse array split into pages to map indices to positions in dense vector.

#ifndef SPARSE_SET_HPP
#define SPARSE_SET_HPP

#include <vector>
#include <array>
#include <memory>
#include <utility>

namespace macroengine {
	template <typename T, std::size_t PAGE_SIZE = 1024>
	struct SparseSet {
	public:
		static_assert(PAGE_SIZE > 0, "PAGE_SIZE must be greater than zero");
		
		// Inserts a value at a given index
		// Throws std::logic_error if index is already occupied.
		void insert(const std::size_t index, const T& value) {
			auto pageIndex = getPageIndex(index);
			sparsePages.resize(pageIndex + 1);

			auto& pagePtr = sparsePages[pageIndex];
			
			// Creates new page if it doesn't exist
			if (!pagePtr) {
			    pagePtr = std::make_unique<SparsePage>();
			    pagePtr->fill(INVALID_INDEX);
			}

			if ((*pagePtr)[getEntityOffset(index)] != INVALID_INDEX)
				throw std::logic_error("SparseSet: index already inserted");

			dense.emplace_back(value, index);
			(*pagePtr)[getEntityOffset(index)] = dense.size() - 1;
		}

		// Removes value at index
		// Throws if index is not found.
		void remove(std::size_t index) {
			std::size_t denseIndex = getDenseIndexOrThrow(index);
			std::size_t lastIndex = dense.size() - 1;

			// Moves last element to the removed position to keep dense vector compact
			if (denseIndex != lastIndex) {
				dense[denseIndex] = std::move(dense[lastIndex]);

				std::size_t movedSparseIndex = dense[denseIndex].second;
				std::size_t movedPageIndex = getPageIndex(movedSparseIndex);
				std::size_t movedOffset = getEntityOffset(movedSparseIndex);
				(*sparsePages[movedPageIndex])[movedOffset] = denseIndex;
			}

			dense.pop_back();

			const std::size_t pageIndex = getPageIndex(index);
			const std::size_t offset = getEntityOffset(index);
			(*sparsePages[pageIndex])[offset] = INVALID_INDEX;
		}

		// Accesses value by index
		// Throws if index is not valid.
		T& at(std::size_t index) {
			return dense[getDenseIndexOrThrow(index)].first;
		}

		const T& at(std::size_t index) const {
			return dense[getDenseIndexOrThrow(index)].first;
		}

		// Checks if index is contained in the set
		bool contains(std::size_t index) const {
			const auto pageIndex = getPageIndex(index);
			const auto offset = getEntityOffset(index);

			return pageIndex < sparsePages.size() &&
			       sparsePages[pageIndex] &&
			       (*sparsePages[pageIndex])[offset] != INVALID_INDEX;
		}

		// Iterators to iterate over stored elements
		auto begin() const {
			return dense.begin();
		}

		auto end() const {
			return dense.end();
		}

		// Number of stored elements
		std::size_t size() const {
			return dense.size();
		}

		// Returns a const reference to the internal dense storage.
		// This allows read-only access for iteration or inspection.
		// Modifying the returned data directly is not allowed, as it may
		// break the internal consistency between dense and sparse structures.
		// Use insert() and remove() methods to modify the data safely.
		const std::vector<std::pair<T, std::size_t>>& dense_data() const {
		    return dense;
		}

	private:
		constexpr static std::size_t INVALID_INDEX = static_cast<std::size_t>(-1);

		using SparsePage = std::array<std::size_t, PAGE_SIZE>;

		std::vector<std::unique_ptr<SparsePage>> sparsePages;
		std::vector<std::pair<T, std::size_t>> dense;

		inline std::size_t getPageIndex(const std::size_t index) const {
			return index / PAGE_SIZE;
		}

		inline std::size_t getEntityOffset(const std::size_t index) const {
			return index % PAGE_SIZE;
		}

		std::size_t getDenseIndexOrThrow(std::size_t index) const {
			const std::size_t pageIndex = getPageIndex(index);
			const std::size_t offset = getEntityOffset(index);

			if (pageIndex >= sparsePages.size() ||
			!sparsePages[pageIndex])
				throw std::out_of_range("SparseSet: index out of bounds");

			std::size_t denseIndex = (*sparsePages[pageIndex])[offset];

			if (denseIndex == INVALID_INDEX)
				throw std::out_of_range("SparseSet: no value at given index");

			return denseIndex;
		}
	};
}

#endif