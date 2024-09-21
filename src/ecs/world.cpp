#include "ecs/world.hpp"

#include <algorithm>
#include <stdexcept>

namespace macroengine {
	World::World() : nextEntity(0) {}

	Entity World::create() {
		Entity entity = nextEntity++;

		entitiesState.emplace_back(true);
		aliveEntities.push_back(entity);

		return entity;
	}

	World& World::destroy(const Entity &entity) {
		validateEntity(entity, "Trying to destroy an invalid entity.");
		validateEntityState(entity, "Trying to destroy an entity that's already destroyed.");
		
		entitiesState[entity] = false;
		aliveEntities.erase(std::remove(aliveEntities.begin(), aliveEntities.end(), entity));

		for (auto& storage : storages) {
			storage->destroy(entity);
		}

		return *this;
	}

	bool World::isAlive(const Entity& entity) {
		validateEntity(entity, "Trying to check the state of an invalid entity.");

		return entitiesState[entity];
	}

	void World::validateEntity(const Entity& entity, std::string error) const {
		if (entity >= nextEntity) {
			throw std::runtime_error(error);
		}
	}

	void World::validateEntityState(const Entity& entity, std::string error) const {
		if (!entitiesState[entity]) {
			throw std::runtime_error(error);
		}
	}
}