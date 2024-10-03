#ifndef MACROENGINE_WITHOUT_HPP
#define MACROENGINE_WITHOUT_HPP

#include "core/type_traits.hpp"

namespace macroengine {
	template <typename... T>
	struct Without {
	public:
		static_assert(sizeof...(T) >= 1, "'Without' must have 1 component or more");
		static_assert(std::conjunction_v<has_ecs_type<T, Component>...>, "'Without' components must have 'ecs_type' set to 'Component'");
		static_assert(std::conjunction_v<is_unique<T...>>, "'Without' component must be unique");
		
	private:
		Without() = default;

		friend class App;
	};
}

#endif