#pragma once

#include "byte_range.hpp"
#include "character.hpp"
#include "character_iterator.hpp"
#include <bits/iterator_concepts.h>
#include <compare>
#include <iterator>
#include <type_traits>

namespace u {

template<enc::encoding E, class T>
struct basic_string_view;

namespace internal {

template<class D, enc::encoding E, class It>
struct string_common_base {
	using value_type = character_view<E, It>;
	using iterator = character_iterator<E, It>;
	using size_type = std::size_t;
	using base_iterator_value_type = std::iter_value_t<It>;
private:
	/*constexpr T* data() const {
		return ((D*)this)->data();
	}*/

	constexpr It raw_begin() const {
		return ((D*)this)->raw_begin();
	}

	constexpr It raw_end() const {
		return ((D*)this)->raw_end();
	}

public:

	static constexpr size_type npos = size_type(-1);

	constexpr iterator begin() const {
		return u::make_character_iterator<E>(raw_begin(), raw_end());
	}

	constexpr iterator end() const {
		return u::make_character_iterator_end<E>(raw_begin(), raw_end());
	}

	constexpr bool empty() const { return raw_begin() == raw_end(); }

	constexpr value_type operator [] (size_type pos) const {
		auto it = begin();
		std::advance(it, pos);
		return *it;
	}

	constexpr value_type at(size_type pos) const {
		auto it = begin();
		std::advance(it, pos);
		return *it;
	}

	constexpr value_type front() const {
		return *begin();
	}

	/*constexpr value_type back() const {
		auto it = begin();
		std::advance(it, size() - 1);
		return *it;
	}*/

	constexpr D substr(size_type pos = 0, size_type n = npos) const {
		auto b = begin();
		std::advance(b, pos);

		iterator e;
		if(n == npos) e = end();
		else {
			e = b;
			std::advance(e, n);
		}

		return { b, e };
	}

	constexpr size_type
	find(const string_common_base& s, size_type pos = 0) const {
		iterator b = begin();
		std::advance(b, pos);
		iterator e = b;
		std::advance(e, std::ranges::distance(s));

		while(e <= end()) {
			auto res =
				u::byte_range{ b.base(), e.base() }
				<=>
				u::byte_range{ s.begin().base(), s.end().base() };
			if(res == 0) return pos;

			++pos;
			++b;
			++e;
		}

		return npos;
	}

	constexpr size_type find(value_type c, size_type pos = 0) const {
		return find(
			basic_string_view<E, base_iterator_value_type> {
				c.data(), c.size()
			},
			pos
		);
	}

	constexpr std::strong_ordering
	operator <=> (const string_common_base& that) const {
		return
		u::byte_range { raw_begin(), raw_end() }
		<=>
		u::byte_range { that.raw_begin(), that.raw_end() };
	}

	constexpr bool
	operator == (const string_common_base& that) const = default;

	constexpr std::strong_ordering
	operator <=> (const std::ranges::range auto& that) const {
		return
		u::byte_range { raw_begin(), raw_end() }
		<=>
		u::byte_range { that };
	}

	constexpr bool
	operator == (const std::ranges::range auto& that) const {
		return (*this <=> that) == 0;
	}

	constexpr std::strong_ordering
	operator <=> (const base_iterator_value_type* that) const {
		return
		u::byte_range { raw_begin(), raw_end() }
		<=>
		u::byte_range { std::basic_string_view { that } };
	}

	constexpr bool
	operator == (const base_iterator_value_type* that) const {
		return (*this <=> that) == 0;
	}
};

}

}