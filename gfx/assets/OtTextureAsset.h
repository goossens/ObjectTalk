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
#include "OtTexture.h"


//
//	OtTextureAsset
//

class OtTextureAsset : public OtAssetBase {
public:
	// access the texture
	inline OtTexture& getTexture() { return texture; }
	inline void setTexture(OtTexture& t) { texture = t; state = State::ready; }
	inline void clearTexture() { texture.clear(); state = State::missing; }
	inline void setTextureReady() { state = State::ready; }

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool canHandleVirtual = true;
	static constexpr const char* supportedFileTypes = ".jpg,.jpeg,.png,.hdr,.exr";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override  {
		try {
			// try to load the texture
			texture.load(path, true);

			// create an event handler to check on the status every frame
			asyncHandle = new uv_async_t;
			asyncHandle->data = this;

			auto status = uv_async_init(uv_default_loop(), asyncHandle, [](uv_async_t* handle){
				auto asset = (OtTextureAsset*) handle->data;

				// is post-processing done?
				if (asset->texture.isValid()) {
					// yes, it is
					asset->state = State::ready;
					asset->notify();

					// cleanup
					uv_close((uv_handle_t*) asset->asyncHandle, [](uv_handle_t* handle) {
						auto asset = (OtTextureAsset*) handle->data;
						delete (uv_fs_event_t*) handle;
						asset->asyncHandle = nullptr;
					});

				} else {
					// no, re-fire the async event
					auto status = uv_async_send(handle);
					UV_CHECK_ERROR("uv_async_send", status);
				}
			});

			UV_CHECK_ERROR("uv_async_init", status);

			// fire the async event
			status = uv_async_send(asyncHandle);
			UV_CHECK_ERROR("uv_async_send", status);

			// we are in loaded state for now
			return State::loaded;

		} catch (const OtException& exception) {
			errorMessage = exception.what();
			return State::invalid;
		}
	}

private:
	// the actual texture
	OtTexture texture;

	// async event handler
	uv_async_t* asyncHandle = nullptr;
};
