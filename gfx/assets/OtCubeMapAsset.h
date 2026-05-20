//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtException.h"

#include "OtAsset.h"
#include "OtAssetBase.h"
#include "OtCubeMap.h"


//
//	OtCubeMapAsset
//

class OtCubeMapAsset : public OtAssetBase {
public:
	// access the cubemap
	inline OtCubeMap& getCubeMap() { return cubemap; }

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".dds,.hdr,.json,.ktx";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override {
		try {
			// try to load the cubemap asynchronously
			cubemap.load(path, true);

			// see if the cubemap needs postprocessing (in which case it's not ready yet)
			if (cubemap.isValid()) {
				return State::ready;

			} else {
				// it does, create an event handler to check on the status every frame
				postProcessingEventHandle = new uv_async_t;
				postProcessingEventHandle->data = this;

				auto status = uv_async_init(uv_default_loop(), postProcessingEventHandle, [](uv_async_t* handle){
					auto asset = (OtCubeMapAsset*) handle->data;

					// is post-processing done?
					if (asset->cubemap.isValid()) {
						// yes, it is
						asset->state = State::ready;
						asset->notify();

						// cleanup
						uv_close((uv_handle_t*) asset->postProcessingEventHandle, [](uv_handle_t* handle) {
							auto asset = (OtCubeMapAsset*) handle->data;
							delete (uv_fs_event_t*) handle;
							asset->postProcessingEventHandle = nullptr;
						});

					} else {
						// no, re-fire the async event
						auto status = uv_async_send(handle);
						UV_CHECK_ERROR("uv_async_send", status);
					}
				});

				UV_CHECK_ERROR("uv_async_init", status);

				// fire the async event
				status = uv_async_send(postProcessingEventHandle);
				UV_CHECK_ERROR("uv_async_send", status);

				// we are in loaded state for now
				return State::loaded;
			}

		} catch (const OtException& exception) {
			errorMessage = exception.what();
			return State::invalid;
		}
	}

private:
	// the actual cubemap
	OtCubeMap cubemap;

	// post-processing event handler
	uv_async_t* postProcessingEventHandle = nullptr;
};
