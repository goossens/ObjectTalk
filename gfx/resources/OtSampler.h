//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "SDL3/SDL.h"


//
//	OtSampler
//

class OtSampler {
public:
	// filtering options
	enum class Filter {
		none,
		nearest,
		linear,
		anisotropic
	};

	enum class Addressing {
		none,
		repeat,
		mirror,
		clamp
	};

	// constructors
	OtSampler() = default;
	OtSampler(Filter filter, Addressing addressing) : requestedFilter(filter), requestedAddressingX(addressing), requestedAddressingY(addressing) {}
	OtSampler(Filter filter, Addressing addressingX, Addressing addressingY) : requestedFilter(filter), requestedAddressingX(addressingX), requestedAddressingY(addressingY) {}

	// clear the object
	void clear();

	// see if sampler is valid
	inline bool isValid() { return sampler != nullptr; }

	// access options
	inline void setFilter(Filter filter) { requestedFilter = filter; }
	inline Filter getFilter() { return requestedFilter; };

	inline void setAddressing(Addressing addressing) {
		requestedAddressingX = addressing;
		requestedAddressingY = addressing;
	}

	inline void setAddressingX(Addressing addressing) { requestedAddressingX = addressing; }
	inline void setAddressingY(Addressing addressing) { requestedAddressingY = addressing; }
	inline Addressing getAddressingX() { return requestedAddressingX; };
	inline Addressing getAddressingY() { return requestedAddressingY; };

	inline void setMinMaxLod(float mnLod, float mxLod) { minLod = mnLod; maxLod = mxLod; }
	inline float getMinLod() { return minLod; }
	inline float getMaxLod() { return maxLod; }

private:
	// sampler
	std::shared_ptr<SDL_GPUSampler> sampler;

	// properties
	Filter requestedFilter = Filter::linear;
	Filter currentFilter = Filter::none;
	Addressing requestedAddressingX = Addressing::repeat;
	Addressing requestedAddressingY = Addressing::repeat;
	Addressing currentAddressingX = Addressing::none;
	Addressing currentAddressingY = Addressing::none;
	float minLod = 0;
	float maxLod = 0;

	// memory manage SDL resource
	void assign(SDL_GPUSampler* newSampler);

	// get the raw sampler object
	friend class OtComputePass;
	friend class OtRenderPass;
	SDL_GPUSampler* getSampler();
};
