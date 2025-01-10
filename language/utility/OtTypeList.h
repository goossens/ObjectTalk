//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	OtTypeList
//

template <typename... Ts>
class OtTypeList;

namespace detail
{
	template<int N, typename T, typename... Ts>
	struct indexOf;

	template<int N, typename T, typename C>
	struct indexOf<N, T, C> {
		constexpr static int value = (std::is_same<T,C>::value)? N : -1;
	};

	template<int N, typename T, typename C, typename... Ts>
	struct indexOf<N, T, OtTypeList<C, Ts...>> {
		constexpr static int value = (std::is_same<T, C>::value) ? N : indexOf<N+1, T, OtTypeList<Ts...>>::value;
	};
}

template<typename T, typename TL>
constexpr int OtTypeListIndexOf() {
	return detail::indexOf<0, T, TL>::value;
}
