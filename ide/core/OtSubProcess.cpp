//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtSubProcess.h"


//
//	OtSubProcess::start
//

void OtSubProcess::start(const std::string& path, const std::vector<std::string>& arguments, std::function <void(int64_t status, int signal)> onExit, std::function <void(const std::string& text)> onStdout, std::function <void(const std::string& text)> onStderr) {
	// sanity check
	if (running) {
		OtError("Can't start subprocess [{}] as one is already running", path);
	}

	// remember callbacks
	exitCallback = onExit;
	stdoutCallback = onStdout;
	stderrCallback = onStderr;

	// create the pipes to communicate with the subprocess
	UV_CHECK_ERROR("uv_pipe_init", uv_pipe_init(uv_default_loop(), &stdinPipe, 0));
	UV_CHECK_ERROR("uv_pipe_init", uv_pipe_init(uv_default_loop(), &stdoutPipe, 0));
	UV_CHECK_ERROR("uv_pipe_init", uv_pipe_init(uv_default_loop(), &stderrPipe, 0));
	stdoutPipe.data = (void*) this;
	stderrPipe.data = (void*) this;

	// setup stdio handles
	uv_stdio_container_t stdio[3];
	stdio[0].flags = UV_IGNORE;
	stdio[0].flags = (uv_stdio_flags) (UV_CREATE_PIPE | UV_READABLE_PIPE);
	stdio[0].data.stream = (uv_stream_t *) &stdinPipe;
	stdio[1].flags = (uv_stdio_flags) (UV_CREATE_PIPE | UV_WRITABLE_PIPE);
	stdio[1].data.stream = (uv_stream_t *) &stdoutPipe;
	stdio[2].flags = (uv_stdio_flags) (UV_CREATE_PIPE | UV_WRITABLE_PIPE);
	stdio[2].data.stream = (uv_stream_t *) &stderrPipe;

	// convert arguments
	char** args = new char* [arguments.size() + 2];
	args[0] = (char*) path.c_str();
	size_t i = 1;

	for (auto& argument : arguments) {
		args[i++] = (char*) argument.c_str();
	}

	args[i] = nullptr;

	// set options
	subprocess.data = (void*) this;

	uv_process_options_t options;
	options.exit_cb = onProcessExit;
	options.file = args[0];
	options.args = args;
	options.env = nullptr;
	options.cwd = nullptr;
	options.flags = UV_PROCESS_WINDOWS_HIDE_CONSOLE;
	options.stdio_count = 3;
	options.stdio = stdio;

	// start process
	auto status = uv_spawn(uv_default_loop(), &subprocess, &options);

	if (status) {
		delete [] args;
		UV_CHECK_ERROR2("uv_spawn", status, path.c_str());
	}

	running = true;

	// start reading stdoutPipe and stderrPipe
	UV_CHECK_ERROR("uv_read_start", uv_read_start((uv_stream_t*) &stdoutPipe, onAllocateMemory, onReadStdout));
	UV_CHECK_ERROR("uv_read_start", uv_read_start((uv_stream_t*) &stderrPipe, onAllocateMemory, onReadStderr));

	// cleanup
	delete [] args;
}


//
//	OtSubProcess::kill
//

void OtSubProcess::kill(int signal) {
	// sanity check
	if (!running) {
		OtError("Can't stop subprocess as it's not running");
	}

	// kill the process
	UV_CHECK_ERROR("uv_process_kill", uv_process_kill(&subprocess, signal));
}


//
//	OtSubProcess::onAllocateMemory
//

void OtSubProcess::onAllocateMemory(uv_handle_t* handle, size_t len, uv_buf_t* buf) {
	buf->base = new char[len];
	buf->len = len;
}


//
//	OtSubProcess::onReadStdout
//

void OtSubProcess::onReadStdout(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	if (nread < 0) {
		if (nread != UV_EOF) {
			UV_CHECK_ERROR("uv_read_start", nread);
		}

		uv_close((uv_handle_t*) stream, nullptr);
		delete [] buf->base;

	} else {
		OtSubProcess* self = (OtSubProcess*) stream->data;
		self->stdoutCallback(std::string(buf->base, nread));
		delete [] buf->base;
	}
}


//
//	OtSubProcess::onReadStderr
//

void OtSubProcess::onReadStderr(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	if (nread < 0) {
		if (nread != UV_EOF) {
			UV_CHECK_ERROR("uv_read_start", nread);
		}

		uv_close((uv_handle_t*) stream, nullptr);
		delete [] buf->base;

	} else {
		OtSubProcess* self = (OtSubProcess*) stream->data;
		self->stderrCallback(std::string(buf->base, nread));
		delete [] buf->base;
	}
}


//
//	OtSubProcess::onProcessExit
//

void OtSubProcess::onProcessExit(uv_process_t* process, int64_t status, int signal) {
	auto self = (OtSubProcess*) process->data;
	self->exitCallback(status, signal);
	self->running = false;
	uv_close((uv_handle_t*) process, nullptr);
}
