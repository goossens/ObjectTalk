//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <list>
#include <functional>


//
//	OtMessage
//

template<typename... T>
struct OtMessage {
	using handler = std::function<bool(T...)>;
};


//
//	OtPublisher
//

template<typename Tm, typename... Tr>
class OtPublisher : public OtPublisher<Tm>, public OtPublisher<Tr...> {
public:
	using OtPublisher<Tm>::listen;
	using OtPublisher<Tm>::unlisten;
	using OtPublisher<Tm>::notify;
	using OtPublisher<Tr...>::listen;
	using OtPublisher<Tr...>::unlisten;
	using OtPublisher<Tr...>::notify;
};

template<typename Tm>
class OtPublisher<Tm> {
public:
	struct listener : std::list<typename Tm::handler>::iterator {};

	template<typename T>
	std::enable_if_t<std::is_same<Tm, T>::value, listener>
	listen(typename Tm::handler handler) {
		return { listeners.insert(listeners.end(), handler) };
	}

	inline void unlisten(const listener& listener) {
		listeners.erase(listener);
	}

protected:
	template<typename T, typename... Args>
	std::enable_if_t<std::is_same<Tm, T>::value>
	notify(Args&& ...args) {
		auto i = listeners.begin();

		while (i != listeners.end()) {
			if (!std::invoke(*i, std::forward<Args>(args)...)) {
				auto target = i++;
				listeners.erase(target);

			} else {
				i++;
			}
		}
	}

private:
	std::list<typename Tm::handler> listeners;
};
