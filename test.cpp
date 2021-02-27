/*#include <initializer_list>
#include <sstream>
#include <string>
#include <iostream>
#include <iterator>
#include <string_view>
#include <type_traits>
#include "include/cxx_util/mb/string.hpp"
#include "include/cxx_util/mb/string_view.hpp"
//#include "include/cxx_util/iterator.hpp"
#include <assert.h>
#include "include/cxx_util/containers/join.hpp"
#include <cstring>
#include <vector>
#include "include/cxx_util/convert.hpp"
#include "include/cxx_util/containers/concepts.hpp"*/

#include <bits/stdint-uintn.h>
#include <iostream>
#include <assert.h>
#include <iterator>
#include <stdint.h>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <list>
#include "include/cxx_util/bytes.hpp"
#include "include/cxx_util/int.hpp"
#include "include/cxx_util/bit.hpp"
#include "include/cxx_util/encoding/utf8.hpp"
#include "include/cxx_util/encoding/utf16.hpp"
#include "include/cxx_util/byte_iterator.hpp"
#include "include/cxx_util/encoding/encoding.hpp"
#include "include/cxx_util/containers/concepts.hpp"


//
template<class It>
struct is_input_iterator : std::false_type {};

template<std::input_iterator It>
struct is_input_iterator<It> : std::true_type {};
//

//
template<class It>
struct is_indirectly_readable : std::false_type {};

template<std::indirectly_readable It>
struct is_indirectly_readable<It> : std::true_type {};
//

//
template<class It, class T>
struct is_output_iterator : std::false_type {};

template<class T, std::output_iterator<T> It>
struct is_output_iterator<It, T> : std::true_type {};
//

//
template<class It>
struct is_random_access_iterator : std::false_type {};

template<std::random_access_iterator It>
struct is_random_access_iterator<It> : std::true_type {};
//

template<std::input_iterator It>
void check_input_iterator() {}

template<std::input_iterator It> requires( std::sized_sentinel_for<It, It> )
void check_sized_sentinel_for() {}
template<std::random_access_iterator It>
void check_random_access_iterator() {}
template<std::contiguous_iterator It>
void check_contiguous_iterator() {}
template<std::indirectly_readable It>
void check_indirectly_readable() {}

template<std::ranges::range It>
void check_range() {}

template<std::ranges::sized_range It>
void check_sized_range() {}

//template<u::byte_range It>
//void check_byte_range() {}

void byte_iterator() {
	using RAIt = u::byte_iterator<int*>;

	check_input_iterator< RAIt> ();
	check_indirectly_readable< RAIt >();
	static_assert( is_indirectly_readable< RAIt >::value );
	static_assert( is_input_iterator< RAIt >::value );
	static_assert( not is_output_iterator< RAIt, int >::value );
	//static_assert( is_random_access_iterator< RAIt >::value );

	static_assert( u::is_input_iterator_of_type_convertible_to_byte<RAIt>::value );

	using ListIt = u::byte_iterator<std::list<int>::iterator>;

	static_assert( not is_random_access_iterator< ListIt >::value );

	static constexpr int arr[] { 0x1100FF, 2, 4 };

	constexpr auto beg = u::byte_iterator(std::begin(arr));
	constexpr auto end = u::byte_iterator(std::end(arr));

	static_assert(u::bytes { arr } == u::bytes { arr } );

	static_assert(decltype(beg)::base_iterator_value_type_size == sizeof(int));
	static_assert(std::distance(beg, end) == 3 * sizeof(int));

	constexpr u::byte_iterator bi { std::begin(arr) };

	static_assert(*bi == std::byte{ 0xFF });
	static_assert(*(bi + 1) == std::byte{ 0x00 });
	static_assert(*(bi + 2) == std::byte{ 0x11 });
	static_assert(*(bi + 3) == std::byte{ 0x00 });

	static_assert(*(bi + 4) == std::byte{ 2 });
	static_assert(*(bi + 8) == std::byte{ 4 });
}

void bit() {
	{
		constexpr unsigned val = 0xF102F304;
		static_assert(u::change_endianness(val) == 0x04F302F1);
	}
	{
		static_assert(u::equals<0,0,1,1>(0b01010011, 0));
		static_assert(u::equals<0,1,0,1,0,0>(0b01010011, 2));

		static_assert(u::equalsl<uint8_t>(0b0101, 4, 0b01010011));
		static_assert(u::equalsl<0,1,0,1>(uint8_t(0b01010011)));

		static_assert(u::equalsr<uint8_t>(0b0101, 4, 0b00000101));
		static_assert(u::equalsr<0,1,0,1>(0b00000101));
	}
}

void _utf16() {
	using namespace enc;
	constexpr std::u16string_view str { u"a" };
	constexpr std::u16string_view wave { u"🌊" }; 

	static_assert( size<utf16>(str) == 1 );
	static_assert( size<utf16>(wave) == 2 );

	static_assert( read_codepoint<utf16>(str) == 'a' );
	static_assert( read_codepoint<utf16>(wave) == 0x1F30A );
}

static_assert(enc::is_encoding_v<enc::utf8>);
static_assert(enc::is_encoding_v<enc::utf16>);
static_assert(enc::is_encoding_v<enc::ascii>);

#include "include/cxx_util/vw/string_view.hpp"

void vw_string_view() {
	constexpr vw::ascii_string_view str_ascii { "Hello" };
	static_assert(str_ascii.size() == 5);

	static_assert(str_ascii.front() == std::string_view{ "H" });
	static_assert(str_ascii[1] == std::string_view{ "e" });
	static_assert(str_ascii[2] == std::string_view{ "l" });
	static_assert(str_ascii[3] == std::string_view{ "l" });
	static_assert(str_ascii.back() == std::string_view{ "o" });
	static_assert(str_ascii.substr(2, 2) == vw::ascii_string_view { "ll" });


	constexpr vw::utf8_string_view str_utf8 { u8"Привет" };
	static_assert(str_utf8.size() == 6);

	static_assert(str_utf8.front() == std::u8string_view{ u8"П" } );
	static_assert(str_utf8[1] == std::string_view{ "р" });
	static_assert(str_utf8[2] == std::string_view{ "и" });
	static_assert(str_utf8[3] == std::string_view{ "в" });
	static_assert(str_utf8[4] == std::string_view{ "е" });
	static_assert(str_utf8.back() == std::string_view{ "т" });
	static_assert(str_utf8.substr(1, 4) == vw::utf8_string_view { u8"риве" });

	static_assert(str_utf8.find(vw::utf8_string_view { u8"вет" }) == 3);
	static_assert(str_utf8.find(vw::utf8_string_view { u8"" }, 6) == 6);

	constexpr auto _3 = str_utf8[3];

	static_assert(str_utf8.find(_3) == 3);
}

#include "include/cxx_util/vw/string.hpp"

consteval int vw_string() {
	vw::utf8_string D = u8"D";
	assert(D.size() == 1);

	vw::utf8_string_view D_sv = D;

	vw::utf8_string hello = u8"Привет";
	assert(hello == u8"Привет");
	assert(hello.size() == 6);
	assert(hello.find(vw::utf8_string_view { u8"вет" }) == 3);
	assert(hello.find(vw::utf8_string_view { u8"" }, 6) == 6);
	//assert(hello[0] == std::u8string_view { u8"П" });
 
	return 0;
}

/*void ascii_util() {
	mb::ascii_string str = "Hello world!";

	char first = str[0];
	assert(first == 'H');
	assert(str.size() == std::strlen("Hello world!"));

	mb::ascii_string str0 = u8"Yeah, we can do that...";
	str0 += "!";
	assert(str0 == "Yeah, we can do that...!");
}

void utf8_util() {
	char8_t buf[]{ 0xF0, 0x9F, 0x8C, 0x8A, 'a', 'b', 'c' };

	assert(4 == util::utf8::first_char_width(buf, buf + std::extent_v<decltype(buf)>).size);

	assert(0x1F30A == util::utf8::first_code_point(buf).codepoint);

	for(int i = 0; i < 4; i++)
		assert(enc::request_result::unexpected_src_end == util::utf8::first_code_point(buf, buf + i).result);

	char8_t invalid[]{ 0b10000000 };
	assert(enc::request_result::invalid_input == util::utf8::first_code_point(invalid).result);
}

void utf16_util() {
	char16_t buf[]{ 0xD83D, 0xDE02, 'a', 'b', 'c' };

	assert(2 == util::utf16::first_char_width(buf, buf + std::extent_v<decltype(buf)>).size);
	assert(0x1F602 == util::utf16::first_code_point(buf, buf + 2).codepoint);

	mb::utf16_string_view str = u"Hello";
}

void mb_string() {
	{
		mb::utf8_string s0 = u8"abcd";
		mb::utf8_string s1 = u8"bcd";

		std::vector<mb::utf8_string> utfs{s0, s1};

		assert(
			util::join<mb::utf8_string>(utfs.begin(), utfs.end(), mb::utf8_string(u8"_"))
			==
			u8"abcd_bcd"
		);

		s1.insert(s1.begin(), u8"a");
		assert(s1 == s0);

		static_assert(std::is_same_v<bool, decltype(s0 == s1)>, "");
		static_assert(std::is_same_v<bool, decltype(s0 != s1)>, "");
		static_assert(std::is_same_v<bool, decltype(s0 >  s1)>, "");
		static_assert(std::is_same_v<bool, decltype(s0 <  s1)>, "");
		static_assert(std::is_same_v<bool, decltype(s0 >= s1)>, "");
		static_assert(std::is_same_v<bool, decltype(s0 <= s1)>, "");

		mb::utf8_string_view s1sv = s1.to_string_view();
		s0 = s1sv;

		assert(s0.size() == s1.size());
		assert(s0 == s1);
	}

	{
		mb::utf8_string u8str = u8"Привет Hello 😇!";
		mb::utf8_string_view u8str_v = u8str;

		assert(u8str.compare(u8str_v) == 0);

		auto ch0 = u8str[0];
		auto ch1 = u8str[1];

		assert(ch0 != ch1);
		assert(ch0 < ch1);
		assert(u8str[9] == u8str[10]);

		assert(u8str + u8" Исэнмэ!" == u8"Привет Hello 😇! Исэнмэ!");
		assert(u8" Исэнмэ!" + u8str == u8" Исэнмэ!Привет Hello 😇!");

		assert(u8str.size() == 6 + 1 + 5 + 1 + 1 + 1);
		assert(u8str.operator std::basic_string_view<char8_t>().size() == 6*2 + 1 + 5 + 1 + 4 + 1);

		std::u8string std = u8"std";
		assert(u8str + std == u8"Привет Hello 😇!std");
	}

	{
		mb::ascii_string empty_ascii_str = "";
		assert(empty_ascii_str.size() == 0);
	}

	{
		mb::utf16_string u16_hello_world = u"Hello world!";
		std::wstring_view converted_strv = u16_hello_world;

		assert(converted_strv.size() == u16_hello_world.size());

		assert(
			(u16_hello_world.template to_string<enc::ascii>()).size()
			==
			std::strlen("Hello world!")*2
		);
	}

	{
		mb::utf8_string u8_hello_world = u8"Hello world!";

		assert(
			(u8_hello_world.template to_string<enc::ascii>())
			==
			"Hello world!"
		);

		std::u8string str_from_rvalue_u8 = mb::utf8_string{u8"Hello?"}.to_string<char8_t>();
		assert(str_from_rvalue_u8.size() == 6);
	}

	{
		std::istringstream ss{"Hello"};
		mb::utf8_string str;
		ss >> str;
		assert(str == u8"Hello");
	}
}

void convert() {
	util::convert::from<std::filesystem::path>("").template to<std::string>();
	util::convert::from<mb::utf8_string>(u8"").template to<std::string>();
	util::convert::from<mb::utf16_string>(u"").template to<std::wstring>();
	util::convert::from<mb::utf8_string_view>(u8"").template to<std::filesystem::path>();
	util::convert::from<mb::utf8_string_view>(u8"").template to<mb::utf8_string>();*/
	//bool val = util::convert::convert_to<bool>(mb::utf8_string_view(u8""));
	//static_assert(util::is_convertible_to_v<bool, mb::utf8_string_view>);

	//static_assert(util::convert::is_convertible_to_v<bool, bool>);
//}

//#include "include/cxx_util/concepts.hpp"


int main() {
	//check_range<util::bytes_visitor_range<std::vector<int>>>();
	//check_sized_range<util::bytes_visitor_range<std::vector<int>>>();
	//check_byte_range<util::bytes_visitor_range<std::string_view>>();
	//std::span s{ v.begin(), v.end() };
	//static_assert(std::is_same_v<int, S<long>::type>);

	//util::common_reference<int&, int&>::type;

	//util::common_reference<int&, int&>::type;
	byte_iterator();
	bit();
	constexpr int r = vw_string();
	//utf8();
	//utf16();
	//vw_string_view();
	//check_for_container_concept<std::string>();
	//check_for_container_concept<mb::utf8_string>();
	//utf8_util();
	//utf16_util();
	//mb_string();
	//convert();
}