#ifndef MACROENGINE_I_STORAGE
#define MACROENGINE_I_STORAGE

#include "ecs/types/entity.hpp"

namespace macroengine {
	class IStorage {
	protected:
		virtual ~IStorage() = default;
		virtual void destroy(const Entity& entity) = 0;

		friend class World;
	};
}

#endif