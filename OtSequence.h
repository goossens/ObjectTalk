//	ObjectTalk Scripting Language
//	Copyright 1993-2018 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


#pragma once


//
//  Include files
//

#if __cplusplus>= 201402L
#include <utility>

#else
namespace std
{
	// see https://github.com/taocpp/sequences/tree/master/include/tao/seq
	template<typename T, T... Ns>
	struct integer_sequence
	{
		using value_type = T;
		static constexpr std::size_t size() noexcept { return sizeof...(Ns); }
	};

	template<std::size_t... Ns>
	using index_sequence = integer_sequence<std::size_t, Ns...>;

	namespace impl
	{
		template<bool V, bool E>
		struct generate_sequence;

		template<>
		struct generate_sequence<false, true>
		{
			template<typename T, T M, T N, std::size_t S, T... Ns>
			using f = integer_sequence<T, Ns...>;
		};

		template<>
		struct generate_sequence<true, true>
		{
			template<typename T, T M, T N, std::size_t S, T... Ns>
			using f = integer_sequence<T, Ns..., S>;
		};

		template<>
		struct generate_sequence<false, false>
		{
			template<typename T, T M, T N, std::size_t S, T... Ns>
			using f = typename generate_sequence<(N & (M / 2)) != 0, (M / 2) == 0>::template f<T, M / 2, N, 2 * S, Ns..., (Ns + S)...>;
		};

		template<>
		struct generate_sequence<true, false>
		{
			template<typename T, T M, T N, std::size_t S, T... Ns>
			using f = typename generate_sequence<(N & (M / 2)) != 0, (M / 2) == 0>::template f<T, M / 2, N, 2 * S + 1, Ns..., (Ns + S)..., 2 * S>;
		};

		template<typename T, T N>
		struct memoize_sequence
		{
			static_assert(N < T(1 << 20), "N too large");
			using type = typename generate_sequence<false, false>::template f<T, (N <T(1 <<1)) ? T(1 <<1) : (N <T(1 <<2)) ? T(1 <<2) : (N <T(1 <<3)) ? T(1 <<3) : (N <T(1 <<4)) ? T(1 <<4) : (N <T(1 <<5)) ? T(1 <<5) : (N <T(1 <<6)) ? T(1 <<6) : (N <T(1 <<7)) ? T(1 <<7) : (N <T(1 <<8)) ? T(1 <<8) : (N <T(1 <<9)) ? T(1 <<9) : (N <T(1 <<10)) ? T(1 <<10) : T(1 <<20), N, 0>;
		};
	}

	template<typename T, T N>
	using make_integer_sequence = typename impl::memoize_sequence<T, N>::type;

	template<std::size_t N>
	using make_index_sequence = make_integer_sequence<std::size_t, N>;

	template<typename... Ts>
	using index_sequence_for = make_index_sequence<sizeof...(Ts)>;
}
#endif
