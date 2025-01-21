//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <future>
#include <memory>
#include <string>


//
//	OtClipboard
//

class OtClipboard {
public:
	// access clipboard from other thread
	const char* get() {
		request.set(Message(Type::getRequest));
		return response.get().text;
	}

	void set(const char* text) {
		request.set(Message(Type::setRequest, text));
		response.get();
	}

	// process clipboard requests in main thread
	void process(GLFWwindow* window) {
		if (request.ready()) {
			auto message = request.get();

			if (message.type == Type::getRequest) {
				response.set(Message(Type::getResponse, glfwGetClipboardString(window)));

			} else if (message.type == Type::setRequest) {
				glfwSetClipboardString(window, message.text);
				response.set(Message(Type::setResponse));
			}
		}
	}

private:
	// thread-safe wrapper class for a promise/future pair
	// borrowed from https://blog.pramp.com/inter-thread-communication-in-c-futures-promises-vs-sockets-aeebfffd2107
	template <typename T>
	class PromiseAndFuture {
	public:
		// constructor
		PromiseAndFuture() {
			reset();
		}

		// reset the managed promise and future objects to new ones
		void reset() {
			promise = std::make_unique<std::promise<T>>();
			future = std::make_unique<std::future<T>>(promise->get_future());
		}

		// non-blocking function that returns whether or not the future object has a valid value
		bool ready() {
			return (future->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready);
		}

		// blocking function that retrieves and returns value
		T get() {
			T result = future->get();
			reset();
			return result;
		}

		// set the value on the managed promise object to val, thereby making the future "ready" and contain val
		void set(T val) {
			promise->set_value(val);
		}

private:
		std::unique_ptr<std::promise<T>> promise;
		std::unique_ptr<std::future<T>> future;
	};

	enum class Type {
		getRequest,
		getResponse,
		setRequest,
		setResponse
	};

	struct Message {
		Message(Type t) : type(t), text(nullptr) {}
		Message(Type t, const char* txt) : type(t), text(txt) {}

		Type type;
		const char* text;
	};

	PromiseAndFuture<Message> request;
	PromiseAndFuture<Message> response;
};
