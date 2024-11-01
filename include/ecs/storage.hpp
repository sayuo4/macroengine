#ifndef MACROENGINE_STORAGE_HPP
#define MACROENGINE_STORAGE_HPP

#include "ecs/i_storage.hpp"

#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <optional>
#include <functional>
#include <string>

namespace macroengine {
	template <typename T>
	class Storage final : public IStorage {
	private:
		std::vector<std::unique_ptr<T>> sequentialData;
		std::unordered_map<Entity, std::unique_ptr<T>> sparseData;

		Storage() {}

		bool isSequential(const Entity &entity) const {
			return entity < 100'000;
		}

		void allocateComponent(const Entity &entity, T &&component) {
			std::unique_ptr<T> componentPtr = std::make_unique<T>(std::move(component));

			if (isSequential(entity)) {
				updateSequentialDataCapacity(entity);
				sequentialData[entity] = std::move(componentPtr);
			} else {
				sparseData[entity] = std::move(componentPtr);
			}
		}

		void deallocateComponent(const Entity &entity) {
			if (isSequential(entity)) {
				sequentialData[entity] = nullptr;
			} else {
				sparseData.erase(entity);
			}
		}

		std::unique_ptr<T> &getComponent(const Entity &entity) {
			if (isSequential(entity)) {
				return sequentialData[entity];
			}

			return sparseData[entity];
		}		

		void insert(const Entity &entity, T &&component) {
			if (has(entity)) {
				throw std::runtime_error("Trying to insert the same component twice.");
			}

			allocateComponent(entity, std::move(component));
		}

		void replace(const Entity &entity, T &&component) {
			if (!has(entity)) {
				throw std::runtime_error("Trying to replace a component that hasn't been added.");
			}

			allocateComponent(entity, std::move(component));
		}

		void insertOrReplace(const Entity &entity, T &&component) {
			allocateComponent(entity, std::move(component));
		}

		void remove(const Entity &entity) {
			if (!has(entity)) {
				throw std::runtime_error("Trying to remove a component that hasn't been added.");
			}

			deallocateComponent(entity);
		}

		bool tryRemove(const Entity &entity) {
			if (!has(entity)) {
				return false;
			}

			deallocateComponent(entity);
			return true;
		}

		T &get(const Entity &entity) {
			if (!has(entity)) {
				throw std::runtime_error("Entity doesn't have the requested component.");
			}

			return *getComponent(entity);
		}

		auto tryGet(const Entity &entity) -> std::optional<std::reference_wrapper<T>> {
			if (!has(entity)) {
				return std::nullopt;
			}

			return std::ref(*getComponent(entity));
		}

		bool has(const Entity &entity) const {
			if (isSequential(entity)) {
				if (sequentialData.size() <= entity) {
					return false;
				}

				return sequentialData[entity] != nullptr;
			}

			return sparseData.find(entity) != sparseData.end();
		}

		void destroy(const Entity &entity) override {
			if (has(entity)) {
				deallocateComponent(entity);
			}
		}

		void updateSequentialDataCapacity(const Entity &entity) {
			if (sequentialData.size() <= entity) {
				sequentialData.resize(entity + 1);
			}
		}
		
		friend class World;
	};
}

#endif