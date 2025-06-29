// === Storage (Page-Based Sparse Set) ===
// Storage is a data structure optimized for fast insertions, removals,
// and lookups using a sparse set approach with paging for memory efficiency.
//
// Template parameters:
// - T: the type of elements stored.
// - PAGE_SIZE: the number of entities per sparse page.
//
// This structure supports:
// - insert(entity, value): inserts a value associated with a given entity ID (throws if entity already exists).
// - remove(entity): removes the value associated with a given entity ID (throws if not found).
// - at(entity): accesses the value by entity ID (throws if not found).
// - contains(entity): checks if a given entity ID exists in the storage.
// - iteration over all stored elements using begin() and end().
//
// Internally, it uses:
// - a dense vector to store data compactly (for fast iteration), and
// - a sparse array split into pages to map entity IDs to dense indices.

#ifndef MACROENGINE_STORAGE_HPP
#define MACROENGINE_STORAGE_HPP

#include <vector>
#include <array>
#include <memory>
#include <utility>

#include "ecs/i_storage.hpp"
#include "ecs/entity_id.hpp"

namespace macroengine {
	template <typename T, std::size_t PAGE_SIZE = 1024>
	struct Storage final : public IStorage {
	public:
		static_assert(PAGE_SIZE > 0, "PAGE_SIZE must be greater than zero");
		
		// Inserts a value associated with a given entity ID.
		// Throws std::logic_error if the entity is already stored.
		void insert(EntityID entity, const T& value) {
			const auto pageIndex = getPageIndex(entity);
			sparsePages.resize(pageIndex + 1);

			auto& pagePtr = sparsePages[pageIndex];
			
			// Creates new page if it doesn't exist
			if (!pagePtr) {
			    pagePtr = std::make_unique<SparsePage>();
			    pagePtr->fill(INVALID_DENSE_INDEX);
			}

			const auto offset = getEntityOffset(entity);

			if ((*pagePtr)[offset] != INVALID_DENSE_INDEX)
				throw std::logic_error("Storage: entity already inserted");

			dense.emplace_back(value, entity);
			(*pagePtr)[offset] = dense.size() - 1;
		}

		// Removes the value associated with a given entity ID.
		// Throws if the entity is not stored.
		void remove(EntityID entity) {
			const auto denseIndex = getDenseIndexOrThrow(entity);
			const auto lastIndex = dense.size() - 1;

			// Moves last element to the removed position to keep dense vector compact
			if (denseIndex != lastIndex) {
				dense[denseIndex] = std::move(dense[lastIndex]);

				const auto movedEntity = dense[denseIndex].second;
				const auto movedPageIndex = getPageIndex(movedEntity);
				const auto movedOffset = getEntityOffset(movedEntity);
				
				(*sparsePages[movedPageIndex])[movedOffset] = denseIndex;
			}

			dense.pop_back();

			const auto pageIndex = getPageIndex(entity);
			const auto offset = getEntityOffset(entity);

			(*sparsePages[pageIndex])[offset] = INVALID_DENSE_INDEX;
		}

		// Accesses the value associated with a given entity ID.
		// Throws if the entity is not stored.
		T& at(EntityID entity) {
			return dense[getDenseIndexOrThrow(entity)].first;
		}

		const T& at(EntityID entity) const {
			return dense[getDenseIndexOrThrow(entity)].first;
		}

		// Returns true if a value is associated with the given entity ID.
		bool contains(EntityID entity) const {
			const auto pageIndex = getPageIndex(entity);
			const auto offset = getEntityOffset(entity);

			return pageIndex < sparsePages.size() &&
			       sparsePages[pageIndex] &&
			       (*sparsePages[pageIndex])[offset] != INVALID_DENSE_INDEX;
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
		const std::vector<std::pair<T, EntityID>>& dense_data() const {
		    return dense;
		}

	private:
		constexpr static std::size_t INVALID_DENSE_INDEX = static_cast<EntityID>(-1);

		using SparsePage = std::array<std::size_t, PAGE_SIZE>;

		std::vector<std::unique_ptr<SparsePage>> sparsePages;
		std::vector<std::pair<T, EntityID>> dense;

		inline std::size_t getPageIndex(EntityID entity) const {
			return entity / PAGE_SIZE;
		}

		inline std::size_t getEntityOffset(EntityID entity) const {
			return entity % PAGE_SIZE;
		}

		std::size_t getDenseIndexOrThrow(EntityID entity) const {
			constexpr const char* ERROR_MSG = "Storage: no value associated with given entity";

			const auto pageIndex = getPageIndex(entity);

			if (pageIndex >= sparsePages.size() || !sparsePages[pageIndex])
				throw std::out_of_range(ERROR_MSG);

			const auto offset = getEntityOffset(entity);
			const auto denseIndex = (*sparsePages[pageIndex])[offset];

			if (denseIndex == INVALID_DENSE_INDEX)
				throw std::out_of_range(ERROR_MSG);

			return denseIndex;
		}
	};
}

#endif