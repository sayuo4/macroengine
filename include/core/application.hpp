#ifndef MACROENGINE_APPLICATION_HPP
#define MACROENGINE_APPLICATION_HPP

#include "ecs/entity.hpp"
#include "ecs/world.hpp"
#include "core/query.hpp"
#include "core/without.hpp"
#include "core/update.hpp"
#include "core/registry.hpp"

#include <vector>
#include <functional>
#include <memory>
#include <iostream>

#define MACROENGINE_ADD_SYSTEM(vector, ...)				  		  \
	vector.emplace_back([this, system]() {					  	  \
		system(__VA_ARGS__); 									  \
	});															  \
																  \
	return *this;												  \

namespace macroengine {
	class App final {
	private:
		static std::shared_ptr<App> inst;
		Registry registry;

		std::vector<std::function<void()>> startupSystems;
		std::vector<std::function<void()>> frameUpdateSystems;
		std::vector<std::function<void()>> fixedUpdateSystems;

		bool isRunning;

		App();

		template <typename... T, typename... U>
		Query<T...> &getQuery(Exclude<U...> &&exclude = Exclude{}) {
			static Query<T...> query;
			query.data.clear();

			query.data = world.get<T...>(std::forward<Exclude<U...>>(exclude));

			return query;
		}

	public:
		World world;

		static App &init();
		static App &get();
		
		App &addStartupSystem(std::function<void()> system);
		
		template <typename... T>
		App &addStartupSystem(std::function<void(Query<T...>)> system) {
			MACROENGINE_ADD_SYSTEM(
				startupSystems,
				getQuery<T...>()
			)
		}

		template <typename... T, typename... U>
		App &addStartupSystem(std::function<void(Query<T...>, Without<U...>)> system) {
			MACROENGINE_ADD_SYSTEM(
				startupSystems,
				getQuery<T...>(Exclude<U...>{}), Without<U...>{}
			)
		}

		App &addStartupSystem(std::function<void(Registry&)> system);
		
		template <typename... T>
		App &addStartupSystem(std::function<void(Registry&, Query<T...>)> system) {
			MACROENGINE_ADD_SYSTEM(
				startupSystems,
				registry, getQuery<T...>()
			)
		}

		template <typename... T, typename... U>
		App &addStartupSystem(std::function<void(Registry&, Query<T...>, Without<U...>)> system) {
			MACROENGINE_ADD_SYSTEM(
				startupSystems,
				registry, getQuery<T...>(Exclude<U...>{}), Without<U...>{}
			)
		}


		App &addSystem(std::function<void()> system, Update updateType = Update::Frame);

		template <typename... T>
		App &addSystem(std::function<void(Query<T...>)> system, Update updateType = Update::Frame) {
			MACROENGINE_ADD_SYSTEM(
				updateType == Update::Frame ? frameUpdateSystems : fixedUpdateSystems,
				getQuery<T...>()
			);
		}

		template <typename... T, typename... U>
		App &addSystem(std::function<void(Query<T...>, Without<U...>)> system, Update updateType = Update::Frame) {
			MACROENGINE_ADD_SYSTEM(
				updateType == Update::Frame ? frameUpdateSystems : fixedUpdateSystems,
				getQuery<T...>(Exclude<U...>{}), Without<U...>{}
			)
		}

		App &addSystem(std::function<void(Registry&)> system, Update updateType = Update::Frame);

		template <typename... T>
		App &addSystem(std::function<void(Registry&, Query<T...>)> system, Update updateType = Update::Frame) {
			MACROENGINE_ADD_SYSTEM(
				updateType == Update::Frame ? frameUpdateSystems : fixedUpdateSystems,
				registry, getQuery<T...>()
			)
		}

		template <typename... T, typename... U>
		App &addSystem(std::function<void(Registry&, Query<T...>, Without<U...>)> system, Update updateType = Update::Frame) {
			MACROENGINE_ADD_SYSTEM(
				updateType == Update::Frame ? frameUpdateSystems : fixedUpdateSystems,
				registry, getQuery<T...>(Exclude<U...>{}), Without<U...>{}
			)
		}
		
		App &run() {
			for (auto& system : startupSystems) {
				system();
			}
			
			return *this;
		}
	};
}

#undef MACROENGINE_ADD_SYSTEM
#endif