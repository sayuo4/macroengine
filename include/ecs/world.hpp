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
		std::vector<bool> entitiesState;
		std::vector<Entity> aliveEntities;

		void validateEntity(const Entity& entity, std::string error) const;
		void validateEntityState(const Entity& entity, std::string error) const;

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

	public:
		World();

		Entity create();
		World& destroy(const Entity& entity);
		bool isAlive(const Entity& entity);

		template <typename T>
		World& insert(const Entity& entity, T&& component) {
			validateEntity(entity, "Trying to insert a component to an invalid entity.");
			validateEntityState(entity, "Trying to insert a component to a destroyed entity.");

			getStorage<T>().insert(entity, std::move(component));
		
			return *this;
		}

		template <typename T>
		World& replace(const Entity& entity, T&& component) {
			validateEntity(entity, "Trying to replace a component in an invalid entity.");
			validateEntityState(entity, "Trying to replace a component in a destroyed entity.");

			getStorage<T>().replace(entity, std::move(component));

			return *this;
		}

		template <typename T>
		World& insertOrReplace(const Entity& entity, T&& component) {
			validateEntity(entity, "Trying to insert/replace a component in an invalid entity.");
			validateEntityState(entity, "Trying to insert/replace a component in a destroyed entity.");			
			
			getStorage<T>().insertOrReplace(entity, std::move(component));
		
			return *this;
		}

		template <typename T>
		World& remove(const Entity& entity) {
			validateEntity(entity, "Trying to remove a component from an invalid entity.");
			validateEntityState(entity, "Trying to remove a component from a destroyed entity.");

			getStorage<T>().remove(entity);	
			
			return *this;
		}

		template <typename T>
		bool tryRemove(const Entity& entity) {
			validateEntity(entity, "Trying to remove a component from an invalid entity.");
			validateEntityState(entity, "Trying to remove a component from a destroyed entity.");

			return getStorage<T>().tryRemove(entity);
		}

		template <typename T>
		T& get(const Entity& entity) {
			validateEntity(entity, "Trying to get a component from an invalid entity.");
			validateEntityState(entity, "Trying to get a component from a destroyed entity.");

			return getStorage<T>().get(entity);
		}

		template <typename T>
		std::optional<std::reference_wrapper<T>> tryGet(const Entity& entity) {
			validateEntity(entity, "Trying to get a component from an invalid entity.");
			validateEntityState(entity, "Trying to get a component from a destroyed entity.");

			return getStorage<T>().tryGet(entity);
		}

		template <typename T>
		bool has(const Entity& entity) {
			validateEntity(entity, "Trying to check a component from in invalid entity.");
			validateEntityState(entity, "Trying to check a component in a destroyed entity.");

			return getStorage<T>().has(entity);
		}
	};
}

#endif
