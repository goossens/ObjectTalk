
	inline void calculateSizes() {
SIZE
		initialized = true;
	}

	inline bool renderUI() {
		if (!initialized) {
			calculateSizes();
		}

		bool changed = false;
RENDERUI
		return changed;
	}

	inline float getRenderWidth() {
		if (!initialized) {
			calculateSizes();
		}

		return width;
	}

	inline float getRenderHeight() {
		if (!initialized) {
			calculateSizes();
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
	bool initialized = false;
	float width = -1.0f;
	float height = -1.0f;
VARIABLES
};
