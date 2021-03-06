#pragma once

#include <cstddef>
#include <limits>
#include <pthread.h>
#include <type_traits>
#include "parameter_pack/at.hpp"
#include "parameter_pack/index_of.hpp"

namespace u {

template<typename... Ts> 
struct parameter_pack;

namespace internal {
	template<typename PP>
	struct is_parameter_pack : std::false_type {};
	
	template<typename... Ts0>
	struct is_parameter_pack<parameter_pack<Ts0...>> : std::true_type {};
}

template<>
struct parameter_pack<> {
	static constexpr std::size_t size = 0;
	static constexpr bool empty = true;

	template<class T>
	static constexpr std::size_t index_of = u::not_found;

	template<class T>
	static constexpr bool contains = false;

	template<class T>
	using append_back = parameter_pack<T>;

	template<class T>
	using append_front = parameter_pack<T>;

	template<typename PP>
	requires(internal::is_parameter_pack<PP>::value)
	using concat = PP;

	template<std::size_t Index>
	requires(Index == 0)
	using from = parameter_pack<>;

	template<std::size_t Index>
	requires(Index == 0)
	using until = parameter_pack<>;
};

namespace internal {
	// concat
	template<typename PP0, typename PP1>
	struct concat;

	template<typename...Ts0, typename...Ts1>
	struct concat<parameter_pack<Ts0...>, parameter_pack<Ts1...>> {
		using type = parameter_pack<Ts0..., Ts1...>;
	};

	// from
	template<std::size_t Index, typename... Ts>
	struct from;

	template<typename... Ts>
	struct from<0, Ts...> {
		using type = parameter_pack<Ts...>;
	};

	template<std::size_t Index, typename T, typename... Ts>
	requires(Index > 0)
	struct from<Index, T, Ts...> {
		using type = typename from<Index - 1, Ts...>::type;
	};

	// until
	template<
		std::size_t Index,
		typename... Ts
	>
	struct until;

	template<typename... Ts>
	struct until<0, Ts...> {
		using type = parameter_pack<>;
	};

	template<std::size_t Index, typename T, typename... Ts>
	requires(Index > 0)
	struct until<Index, T, Ts...> {
		using type = typename concat<
			parameter_pack<T>, 
			typename until<Index - 1, Ts...>::type
		>::type;
	};
}

template<typename... Ts>
struct parameter_pack {
	static constexpr std::size_t size = sizeof...(Ts);
	static constexpr bool empty = false;

	template<std::size_t Index>
	using at = typename u::at<Index, Ts...>;

	using front = at<0>;
	using back = at<size - 1>;

	template<typename T>
	static constexpr std::size_t index_of = u::index_of<T, Ts...>;

	template<typename T>
	static constexpr bool contains = index_of<T> != not_found;

	template<typename... Ts0>
	using append_back = parameter_pack<Ts..., Ts0...>;

	template<typename... Ts0>
	using append_front = parameter_pack<Ts0..., Ts...>;

	template<typename PP>
	requires(internal::is_parameter_pack<PP>::value)
	using concat = typename internal::concat<parameter_pack<Ts...>, PP>::type;

	template<std::size_t Index>
	using from = typename internal::from<Index, Ts...>::type;

	template<std::size_t Index>
	using until = typename internal::until<Index, Ts...>::type;
};

}