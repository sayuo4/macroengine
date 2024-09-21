#include "ecs/world.hpp"

#include <algorithm>
#include <stdexcept>

namespace macroengine {
	World::World() : nextEntity(0) {}

	Entity World::create() {
		Entity entity = nextEntity++;

		entityAliveStates.emplace_back(true);
		entityActiveStates.emplace_back(true);

		entities.push_back(entity);
		aliveEntities.push_back(entity);
		activeEntities.push_back(entity);

		return entity;
	}

	World& World::destroy(const Entity& entity) {
		validateEntity(entity, "Trying to destroy an invalid entity.");
		validateEntityAliveState(entity, "Trying to destroy an entity that's already destroyed.");
		
		entityAliveStates[entity] = false;
		entityActiveStates[entity] = false;

		aliveEntities.erase(std::remove(aliveEntities.begin(), aliveEntities.end(), entity));
		activeEntities.erase(std::remove(activeEntities.begin(), activeEntities.end(), entity));

		for (const auto& storage : storages) {
			storage->destroy(entity);
		}

		return *this;
	}

	bool World::tryDestroy(const Entity& entity) {
		validateEntity(entity, "Trying to destroy an invalid entity.");
		
		if (!entityAliveStates[entity]) {
			return false;
		}

		entityAliveStates[entity] = false;
		entityActiveStates[entity] = false;

		aliveEntities.erase(std::remove(aliveEntities.begin(), aliveEntities.end(), entity));
		activeEntities.erase(std::remove(activeEntities.begin(), activeEntities.end(), entity));

		for (const auto& storage : storages) {
			storage->destroy(entity);
		}

		return true;
	}

	World& World::destroyAll() {
		for (const auto& entity : aliveEntities) {
			destroy(entity);
		}

		return *this;
	}

	World& World::activate(const Entity& entity) {
		validateEntity(entity, "Trying to activate an invalid entity.");
		validateEntityAliveState(entity, "Trying to activate a destroyed entity.");

		if (entityActiveStates[entity]) {
			throw std::runtime_error("Trying to activate an entity that's already activated.");
		}

		entityActiveStates[entity] = true;
		activeEntities.push_back(entity);

		return *this;
	}

	World& World::activateAll() {
		for (const auto& entity : aliveEntities) {
			tryActivate(entity);
		}

		return *this;
	}

	World& World::disable(const Entity& entity) {
		validateEntity(entity, "Trying to disable an invalid entity.");
		validateEntityAliveState(entity, "Trying to disable a destroyed entity.");

		if (!entityActiveStates[entity]) {
			throw std::runtime_error("Trying to disable an entity that's already disabled.");
		}

		entityActiveStates[entity] = false;
		activeEntities.erase(std::remove(activeEntities.begin(), activeEntities.end(), entity));

		return *this;
	}

	World& World::disableAll() {
		for (const auto& entity : activeEntities) {
			disable(entity);
		}

		return *this;
	}

	bool World::tryActivate(const Entity& entity) {
		validateEntity(entity, "Trying to activate an invalid entity.");
		validateEntityAliveState(entity, "Trying to activate a destroyed entity.");

		if (entityActiveStates[entity]) {
			return false;
		}

		entityActiveStates[entity] = false;
		activeEntities.push_back(entity);

		return true;
	}

	bool World::tryDisable(const Entity& entity) {
		validateEntity(entity, "Trying to disable an invalid entity.");
		validateEntityAliveState(entity, "Trying to disable a destroyed entity.");

		if (!entityActiveStates[entity]) {
			return false;
		}

		entityActiveStates[entity] = false;
		activeEntities.erase(std::remove(activeEntities.begin(), activeEntities.end(), entity));

		return true;
	}

	bool World::isAlive(const Entity& entity) {
		validateEntity(entity, "Trying to check the alive state of an invalid entity.");

		return entityAliveStates[entity];
	}

	bool World::isActive(const Entity& entity) {
		validateEntity(entity, "Trying to check the active state of an invalid entity.");
		validateEntityAliveState(entity, "Trying to check the active state of a destroyed entity.");

		return entityActiveStates[entity];
	}

	std::vector<Entity> World::getAllEntities() {
		return entities;
	}

	std::vector<Entity> World::getAliveEntities() {
		return aliveEntities;
	}

	std::vector<Entity> World::getActiveEntities() {
		return activeEntities;
	}

	std::size_t World::getEntitiesCount() {
		return nextEntity;
	}

	std::size_t World::getAliveEntitiesCount() {
		return aliveEntities.size();
	}

	std::size_t World::getActiveEntitiesCount() {
		return activeEntities.size();
	}

	void World::validateEntity(const Entity& entity, std::string error) const {
		if (entity >= nextEntity) {
			throw std::runtime_error(error);
		}
	}

	void World::validateEntityAliveState(const Entity& entity, std::string error) const {
		if (!entityAliveStates[entity]) {
			throw std::runtime_error(error);
		}
	}
}