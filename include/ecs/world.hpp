#ifndef MACROENGINE_WORLD_HPP
#define MACROENGINE_WORLD_HPP

#include "ecs/types/entity.hpp"
#include "ecs/i_storage.hpp"
#include "ecs/storage.hpp"

#include <vector>
#include <typeindex>
#include <string>
#include <optional>
#include <functional>

namespace macroengine {
	class World final {
	private:
		std::vector<IStorage*> storages;
		
		Entity nextEntity;
		
		std::vector<bool> entityAliveStates;
		std::vector<bool> entityActiveStates;

		std::vector<Entity> entities;
		std::vector<Entity> aliveEntities;
		std::vector<Entity> activeEntities;

		void validateEntity(const Entity& entity, std::string error) const;
		void validateEntityAliveState(const Entity& entity, std::string error) const;

		template <typename T>
		Storage<T>& getStorage() {
			static Storage<T> storage;
			static bool isRegistered = false;

			if (!isRegistered) {
				storages.emplace_back(&storage);
				isRegistered = true;
			}

			return storage;
		}

		template <typename... T>
		auto getFromVec(std::vector<Entity>& vec) -> std::vector<std::tuple<T&...>> {
			std::vector<std::tuple<T&...>> result;
			result.reserve(vec.size());

			for (const auto& entity : vec) {
				if ((getStorage<T>().has(entity) && ...)) {
					result.emplace_back(*getStorage<T>().data[entity]...);
				}
			}

			return result;
		}

	public:
		World();

		Entity create();

		World& destroy(const Entity& entity);
		World& destroyAll();

		bool tryDestroy(const Entity& entity);
		
		World& activate(const Entity& entity);
		World& activateAll();

		World& disable(const Entity& entity);
		World& disableAll();

		bool tryActivate(const Entity& entity);
		bool tryDisable(const Entity& entity);

		bool isAlive(const Entity& entity);
		bool isActive(const Entity& entity);

		std::vector<Entity> getAllEntities();
		std::vector<Entity> getAliveEntities();
		std::vector<Entity> getActiveEntities();

		std::size_t getEntitiesCount();
		std::size_t getAliveEntitiesCount();
		std::size_t getActiveEntitiesCount();

		template <typename T>
		World& insert(const Entity& entity, T&& component) {
			validateEntity(entity, "Trying to insert a component to an invalid entity.");
			validateEntityAliveState(entity, "Trying to insert a component to a destroyed entity.");

			getStorage<T>().insert(entity, std::move(component));
		
			return *this;
		}

		template <typename T>
		World& replace(const Entity& entity, T&& component) {
			validateEntity(entity, "Trying to replace a component in an invalid entity.");
			validateEntityAliveState(entity, "Trying to replace a component in a destroyed entity.");

			getStorage<T>().replace(entity, std::move(component));

			return *this;
		}

		template <typename T>
		World& insertOrReplace(const Entity& entity, T&& component) {
			validateEntity(entity, "Trying to insert/replace a component in an invalid entity.");
			validateEntityAliveState(entity, "Trying to insert/replace a component in a destroyed entity.");			
			
			getStorage<T>().insertOrReplace(entity, std::move(component));
		
			return *this;
		}

		template <typename T>
		World& remove(const Entity& entity) {
			validateEntity(entity, "Trying to remove a component from an invalid entity.");
			validateEntityAliveState(entity, "Trying to remove a component from a destroyed entity.");

			getStorage<T>().remove(entity);	
			
			return *this;
		}

		template <typename T>
		bool tryRemove(const Entity& entity) {
			validateEntity(entity, "Trying to remove a component from an invalid entity.");
			validateEntityAliveState(entity, "Trying to remove a component from a destroyed entity.");

			return getStorage<T>().tryRemove(entity);
		}

		template <typename T>
		T& get(const Entity& entity) {
			validateEntity(entity, "Trying to get a component from an invalid entity.");
			validateEntityAliveState(entity, "Trying to get a component from a destroyed entity.");

			return getStorage<T>().get(entity);
		}

		template <typename... T>
		std::tuple<T&...> get(const Entity& entity) {
			validateEntity(entity, "Trying to get a components from an invalid entity.");
			validateEntityAliveState(entity, "Trying to get a components from a destroyed entity.");

			if ((getStorage<T>().has(entity) && ...)) {
				throw std::runtime_error("Entity doesn't have the requested components.");
			}

			return std::make_tuple(std::ref(*getStorage<T>().data[entity])...);
		}

		template <typename... T>
		auto get() -> std::vector<std::tuple<T&...>> {
			return getFromVec<T...>(activeEntities);
		}

		template <typename... T>
		auto getAll() -> std::vector<std::tuple<T&...>> {
			return getFromVec<T...>(aliveEntities);
		}

		template <typename T>
		auto tryGet(const Entity& entity) -> std::optional<std::reference_wrapper<T>> {
			validateEntity(entity, "Trying to get a component from an invalid entity.");
			validateEntityAliveState(entity, "Trying to get a component from a destroyed entity.");

			return getStorage<T>().tryGet(entity);
		}

		template <typename... T>
		auto tryGet(const Entity& entity) -> std::optional<std::tuple<T&...>> {
			validateEntity(entity, "Trying to get a components from an invalid entity.");
			validateEntityAliveState(entity, "Trying to get a components from a destroyed entity.");
		
			if (!has<T...>(entity)) {
				return std::nullopt;
			}

			return std::make_tuple(std::ref(*getStorage<T>().data[entity])...);
		}

		template <typename T>
		bool has(const Entity& entity) {
			validateEntity(entity, "Trying to check a component from in invalid entity.");
			validateEntityAliveState(entity, "Trying to check a component in a destroyed entity.");

			return getStorage<T>().has(entity);
		}

		template <typename... T>
		bool has(const Entity& entity) {
			validateEntity(entity, "Trying to check a components from in invalid entity.");
			validateEntityAliveState(entity, "Trying to check a components in a destroyed entity.");

			return (getStorage<T>().has(entity) && ...);
		}
	};
}

#endif
