
	inline bool renderUI() {
		bool changed = false;
RENDERUI
		return changed;
	}

	inline float getRenderWidth() {
		if (width == -1.0f) {
GETRENDERWIDTH
		}

		return width;
	}

	inline float getRenderHeight() {
		if (height == -1.0f) {
GETRENDERHEIGHT
		}

		return height;
}

	struct Parameters {
PARAMETERSSTRUCT
	};

	inline void setParameters([[maybe_unused]] Parameters& parameters) {
SETPARAMETERS
	}

	inline void getParameters([[maybe_unused]] Parameters& parameters) {
GETPARAMETERS
	}

	inline void iterateParameters([[maybe_unused]] std::function<void(const char*, float*, float)> callback) override {
ITERATEPARAMETERS
	}

SETTERS

GETTERS

private:
	float width = -1.0f;
	float height = -1.0f;
};
