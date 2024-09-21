#ifndef MACROENGINE_STORAGE_HPP
#define MACROENGINE_STORAGE_HPP

#include "ecs/i_storage.hpp"

#include <memory>
#include <vector>
#include <stdexcept>
#include <optional>
#include <functional>
#include <string>

namespace macroengine {
	template <typename T>
	class Storage final : public IStorage {
	private:
		std::vector<std::unique_ptr<T>> data;

		Storage() {}

		void insert(const Entity& entity, T&& component) {
			if (has(entity)) {
				throw std::runtime_error("Trying to insert the same component twice.");
			}

			updateCapacity(entity);

			data[entity] = std::make_unique<T>(std::move(component));
		}

		void replace(const Entity& entity, T&& component) {
			if (!has(entity)) {
				throw std::runtime_error("Trying to replace a component that hasn't been added.");
			}

			data[entity] = std::make_unique<T>(std::move(component));
		}

		void insertOrReplace(const Entity& entity, T&& component) {
			updateCapacity(entity);

			data[entity] = std::make_unique<T>(std::move(component));		
		}

		void remove(const Entity& entity) {
			if (!has(entity)) {
				throw std::runtime_error("Trying to remove a component that hasn't been added.");
			}

			data[entity] = nullptr;
		}

		bool tryRemove(const Entity& entity) {
			if (!has(entity)) {
				return false;
			}

			data[entity] = nullptr;
			return true;
		}

		T& get(const Entity& entity) const {
			if (!has(entity)) {
				throw std::runtime_error("Entity doesn't have the requested component.");
			}

			return *data[entity];
		}

		std::optional<std::reference_wrapper<T>> tryGet(const Entity& entity) const {
			if (!has(entity)) {
				return std::nullopt;
			}

			return std::ref(*data[entity]);
		}

		bool has(const Entity& entity) const {
			if (data.size() <= entity) {
				return false;
			}

			return data[entity] != nullptr;
		}

		void destroy(const Entity& entity) override {
			if (has(entity)) {
				data[entity] = nullptr;
			}
		}

		void updateCapacity(const Entity& entity) {
			if (data.size() <= entity) {
				data.resize(entity + 1);
			}
		}
		
		friend class World;
	};
}

#endif