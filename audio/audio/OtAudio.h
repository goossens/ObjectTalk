//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "OtMeasure.h"

#include "OtCircuit.h"
#include "OtCircuitPin.h"
#include "OtCircuitFactory.h"
#include "OtDsp.h"
#include "OtWire.h"


//
//	OtAudio
//

class OtAudio {
public:
	// constructor/destructor
	OtAudio();
	~OtAudio();

	// handle concurrent access
	inline void lock() { mutex.lock(); }
	inline void unlock() { mutex.unlock(); }

	// register a circuit type
	template <typename T>
	inline void registerCircuitType(const char* category, const char* name) {
		OtCircuitFactory::registerType(
			category,
			name,
			[]() {
				auto circuit = std::make_shared<T>();
				circuit->configure();
				return circuit;
			});
	}

	// clear all circuits
	void clear();

	// load and save circuits
	void load(const std::string& path);
	void loadFromString(const std::string& string, std::string& basedir);
	void save(const std::string& path);

	// manipulate circuits
	OtCircuit createCircuit(const std::string& name, float x, float y);
	inline void deleteCircuit(uint32_t id) { deleteCircuit(circuitIndex[id]); }
	void deleteCircuit(OtCircuit circuit);
	void deleteCircuits(const std::vector<uint32_t>& circuitIDs);
	inline size_t getCircuitCount() {return circuits.size(); }

	// check wires
	inline bool causesCycle(uint32_t from, uint32_t to) { return causesCycle(pinIndex[from], pinIndex[to]); }
	bool causesCycle(OtCircuitPin from, OtCircuitPin to);
	inline bool isWireValid(uint32_t from, uint32_t to) { return isWireValid(pinIndex[from], pinIndex[to]); }
	bool isWireValid(OtCircuitPin from, OtCircuitPin to);

	// manipulate wires
	inline OtWire createWire(uint32_t from, uint32_t to, uint32_t id=0) { return createWire(pinIndex[from], pinIndex[to], id); }
	OtWire createWire(OtCircuitPin from, OtCircuitPin to, uint32_t id=0);

	inline void deleteWire(uint32_t id) { deleteWire(wireIndex[id]); }
	void deleteWire(OtWire wire);
	inline void deleteWire(uint32_t from, uint32_t to) { deleteWire(pinIndex[from], pinIndex[to]); }
	void deleteWire(OtCircuitPin from, OtCircuitPin to);
	inline void deleteWires(uint32_t any) { deleteWires(pinIndex[any]); }
	void deleteWires(OtCircuitPin pin);

	inline void redirectWire(uint32_t id, uint32_t newTo) { redirectWire(wireIndex[id], newTo); }
	void redirectWire(OtWire wire, uint32_t newTo);

	// search for wires
	inline OtWire findWire(uint32_t from, uint32_t to) { return findWire(pinIndex[from], pinIndex[to]); }
	OtWire findWire(OtCircuitPin from, OtCircuitPin to);

	// access circuits and pins
	inline OtCircuit getCircuit(uint32_t id) { return circuitIndex[id]; }
	inline OtCircuitPin getPin(uint32_t id) { return pinIndex[id]; }

	// handle selections
	void selectAll();
	void deselectAll();
	void select(uint32_t id, bool deselect=true);
	void select(const std::vector<uint32_t>& circuitIDs, bool deselect=true);
	void select(int x1, int y1, int x2, int y2);
	void deselect(uint32_t id);
	inline bool isCircuitSelected(uint32_t id) { return circuitIndex[id]->selected; }

	std::vector<uint32_t> getSelected();
	inline size_t getSelectedCount() { return getSelected().size(); }
	inline bool hasSelected() { return getSelected().size() != 0; }
	inline bool hasMultipleSelected() { return getSelected().size() > 1; }

	// iterate through circuits and wires
	inline void eachCircuit(std::function<void(OtCircuit)> callback) {
		for (auto& circuit : circuits) {
			callback(circuit);
		}
	}

	inline void eachWire(std::function<void(OtWire&)> callback) {
		for (auto& wire : wires) {
			callback(wire);
		}
	}

	// archive/restore circuits to/from a string (support for do/undo/redo operations)
	std::string archiveCircuit(uint32_t circuit);
	std::string archiveCircuits(const std::vector<uint32_t>& circuitIDs);
	void restoreCircuits(const std::string& json);
	std::vector<uint32_t> duplicateCircuits(const std::string& json);

	// get average processing time (as a percentage of the threads time)
	float getAverageProcessingTime() { return processingTime.average(); }

private:
	// properties
	std::vector<OtCircuit> circuits;
	std::vector<OtWire> wires;

	std::unordered_map<uint32_t, OtCircuit> circuitIndex;
	std::unordered_map<uint32_t, OtCircuitPin> pinIndex;
	std::unordered_map<uint32_t, OtWire> wireIndex;

	bool needsSorting = false;

	// metadata for editor
	std::string metadata = "{}";

	// signal processor to get the sound out
	OtDsp dsp;
	void provideSignal(OtAudioBuffer& buffer);
	std::mutex mutex;
	OtMeasureAvarage<float, 100> processingTime;

	// (un)index a circuit and its pins
	void indexCircuit(OtCircuit circuit);
	void unindexCircuit(OtCircuit circuit);

	// helper functions for depth-first searches and topological sorting
	bool hasCycle(OtCircuitClass* circuit, OtCircuitClass* newTarget=0);
	bool visitCircuit(OtCircuit circuit, std::vector<OtCircuit>& circuitIDs);
	void sortCircuitsTopologically();

	// restore a circuit from its JSON data
	OtCircuit restoreCircuit(nlohmann::json data, bool restoreIDs=true, std::string* basedir=nullptr);
};
