#ifndef MACROENGINE_TYPE_TRAITS_HPP
#define MACROENGINE_TYPE_TRAITS_HPP

#include "core/component.hpp"

#include <type_traits>
#include <utility>
#include <tuple>

namespace macroengine {
	template <typename T, typename U, typename = void>
	struct has_ecs_type : std::false_type {};

	template <typename T, typename U>
	struct has_ecs_type<T, U, std::void_t<typename T::ecs_type>> : std::is_same<typename T::ecs_type, U> {};

	template <typename T, typename U>
	constexpr bool has_ecs_type_v = has_ecs_type<T, U>::value;

	template<typename T, typename... Args>
	struct contains;

	template<typename T>
	struct contains<T> {
	    static constexpr bool value = false;
	};

	template<typename T, typename U, typename... Args>
	struct contains<T, U, Args...> {
	    static constexpr bool value = std::is_same<T, U>::value || contains<T, Args...>::value;
	};

	template <typename... T>
	constexpr bool contains_v = contains<T...>::value;

	template<typename... Args>
	struct is_unique;

	template<>
	struct is_unique<> {
	    static constexpr bool value = true;
	};

	template<typename T, typename... Args>
	struct is_unique<T, Args...> {
	    static constexpr bool value = !contains<T, Args...>::value && is_unique<Args...>::value;
	};

	template <typename... T>
	constexpr bool is_unique_v = is_unique<T...>::value;
}

#endif
