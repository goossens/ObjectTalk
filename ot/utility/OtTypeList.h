//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
		constexpr static int value = (std::is_same<T, C>::value) ?
		N : indexOf<N+1, T, OtTypeList<Ts...>>::value;
	};
}

template<typename T, typename TL>
constexpr int OtTypeListIndexOf() {
	return detail::indexOf<0, T, TL>::value;
}

/*
template <>
class OtTypeList<> {
public:
	static constexpr int id = -1;

	template <typename T>
	static constexpr int indexOf() { return id; }
};

template <typename T, typename... Ts>
class OtTypeList<T, Ts...> {
private:
	using remainingTypes = OtTypeList<Ts...>;

	static constexpr int id = sizeof...(Ts);

	template<typename U, bool dummy>
	struct Helper {
		static constexpr int value = remainingTypes::template indexOf<U>();
	};

	template<bool dummy>
	struct Helper<T, dummy> {
		static constexpr int value = id;
	};

public:
	template <typename F>
	static constexpr int indexOf() {
		return Helper<F, true>::value;
	};
};
*/
