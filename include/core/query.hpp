#ifndef MACROENGINE_QUERY_HPP
#define MACROENGINE_QUERY_HPP

#include "core/type_traits.hpp"

#include <type_traits>
#include <vector>
#include <tuple>

namespace macroengine {
	template <typename... T>
	struct Query final {
	public:
		static_assert(sizeof...(T) >= 1, "'Query' must have 1 component or more");
		static_assert(std::conjunction_v<has_ecs_type<T, Component>...>, "'Query' components must have 'ecs_type' set to 'Component'");
		static_assert(std::conjunction_v<is_unique<T...>>, "'Query' component must be unique");

		auto begin() {
			return data.begin();
		}

		auto end() {
			return data.end();
		}

		std::size_t count() const {
			return data.size();
		}
	
	private:
		std::vector<std::tuple<T&...>> data;
		
		Query() {};

		friend class App;
	};
}

#endif