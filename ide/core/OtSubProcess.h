//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "OtLibuv.h"


//
//	OtSubProcess
//

class OtSubProcess {
public:
	// start a specified subprocess with parameters
	void start(
		const std::filesystem::path& path,
		const std::vector<std::string>& arguments,
		std::function<void(int64_t status, int signal)> onExit,
		std::function<void(const std::string& text)> onStdout,
		std::function<void(const std::string& text)> onStderr);

	// kill a subprocess
	void kill(int signal);

	// see if a subprocess is still running
	bool isRunning() { return running; }

private:
	// pipes to communicate with subprocess
	uv_pipe_t stdoutPipe;
	uv_pipe_t stderrPipe;

	// sub process control
	uv_process_t subprocess;
	bool running = false;

	// callbacks
	std::function<void(int64_t status, int signal)> exitCallback;
	std::function<void(const std::string& text)> stdoutCallback;
	std::function<void(const std::string& text)> stderrCallback;

	// static callback functions for libuv
	static void onAllocateMemory(uv_handle_t* handle, size_t len, uv_buf_t* buf);
	static void onReadStdout(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void onReadStderr(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void onProcessExit(uv_process_t* process, int64_t status, int signal);
};
