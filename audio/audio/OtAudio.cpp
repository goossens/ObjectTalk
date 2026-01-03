//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <set>

#include "nlohmann/json.hpp"

#include "OtException.h"
#include "OtLog.h"
#include "OtPath.h"
#include "OtText.h"

#include "OtAudio.h"
#include "OtCircuitPin.h"
#include "OtCircuitUtils.h"


//
//	OtAudio::OtAudio
//

OtAudio::OtAudio() {
	// setup the signal processor
	dsp.setSignalProvider([this](OtAudioBuffer& buffer) {
		provideSignal(buffer);
	});

	dsp.start();
}


//
//	OtAudio::provideSignal
//

void OtAudio::provideSignal(OtAudioBuffer& buffer) {
	// thread safety
	std::lock_guard<std::mutex> guard(mutex);

	// measure processing time
	OtMeasureStopWatch timer;

	// see if resorting is required
	if (needsSorting) {
		sortCircuitsTopologically();
		needsSorting = false;
	}

	buffer.clear();

	// execute all circuits (in depth-first order)
	for (auto& circuit : circuits) {
		try {
			circuit->error.clear();
			circuit->execute();

			// if the circuit is an output circuit, get the stream and mix it
			if (circuit->category == OtCircuitClass::Category::output) {
				circuit->eachInput([&](OtCircuitPin pin) {
					if (pin->isSourceConnected()) {
						auto source = pin->getSource();

						if (source->buffer) {
							buffer.mix(*source->buffer);
						}
					}
				});
			}

		} catch (OtException& e) {
			circuit->error = e.getShortErrorMessage();
		}
	}

	static int report = 0;

	if (++report == 100) {
		static constexpr float factor = OtAudioSettings::sampleRate / OtAudioSettings::bufferSize / 1000.0f * 100.0f;
		processingTime += timer.elapsed() * factor;
		report = 0;
	}
}


//
//	OtAudio::~OtAudio
//

OtAudio::~OtAudio() {
	dsp.stop();
	clear();
}


//
//	OtAudio::clear
//

void OtAudio::clear() {
	// reset audio
	wireIndex.clear();
	pinIndex.clear();
	circuitIndex.clear();
	wires.clear();
	circuits.clear();
	metadata = "{}";
}


//
//	OtAudio::load
//

void OtAudio::load(const std::string& path) {
	// load scene from file
	auto basedir = OtPath::getParent(path);
	loadFromString(OtText::load(path), basedir);
}


//
//	OtAudio::loadFromString
//

void OtAudio::loadFromString(const std::string& string, std::string& basedir) {
	// stop audio processing
	dsp.stop();

	// clear circuits
	clear();

	// treat an empty file as a blank circuits
	if (string.size()) {
		// parse json
		auto data = nlohmann::json::parse(string);

		// extract metadata
		if (data.contains("metadata")) {
			metadata = data["metadata"].dump();
		}

		// restore each circuit
		for (auto& circuit : data["circuits"]) {
			restoreCircuit(circuit, true, &basedir);
		}

		// restore wires
		for (auto& wire : data["wires"]) {
			uint32_t wireId = wire["id"];
			uint32_t fromId = wire["from"];
			uint32_t toId = wire["to"];

			if (pinIndex.count(fromId) == 0) {
				OtLogError("Invalid 'from' pin ID [{}] in wire [{}]", fromId, wireId);

			} else if (pinIndex.count(toId) == 0) {
				OtLogError("Invalid 'to' pin ID [{}] in wire [{}]", toId, wireId);
			}

			createWire(fromId, toId, wireId);
		}
	}

	// set the sorting flag
	needsSorting = true;

	// restart audio processing
	dsp.start();
}


//
//	OtAudio::save
//

void OtAudio::save(const std::string& path) {
	// create json outline
	auto data = nlohmann::json::object();
	data["metadata"] = nlohmann::json::parse(metadata.c_str());
	auto circuitsJSON = nlohmann::json::array();
	auto wiresJSON = nlohmann::json::array();

	// save all circuits
	auto basedir = OtPath::getParent(path);

	eachCircuit([&](OtCircuit circuit) {
		circuitsJSON.push_back(circuit->serialize(&basedir));
	});

	// save all wires
	eachWire([&](OtWire& wire) {
		wiresJSON.push_back(wire->serialize(&basedir));
	});

	data["circuits"] = circuitsJSON;
	data["wires"] = wiresJSON;

	// write circuits to file
	OtText::save(path, data.dump(1, '\t'));
}


//
//	OtAudio::createCircuit
//

OtCircuit OtAudio::createCircuit(const std::string& name, float x, float y) {
	// create a new circuit
	auto circuit = OtCircuitFactory::createCircuit(name);
	circuits.emplace_back(circuit);
	needsSorting = true;

	circuit->x = x;
	circuit->y = y;
	circuit->needsPlacement = true;
	circuit->needsSizing = true;

	// index circuit and pins
	indexCircuit(circuit);

	// return new mode
	return circuit;
}


//
//	OtAudio::deleteCircuit
//

void OtAudio::deleteCircuit(OtCircuit circuit) {
	// find the circuit
	auto i = std::find_if(circuits.begin(), circuits.end(), [circuit](OtCircuit candidate) {
		return candidate->id == circuit->id;
	});

	// remove registered pins and possible wires
	(*i)->eachInput([this](OtCircuitPin pin) {
		deleteWires(pin);
		pinIndex.erase(pin->id);
	});

	(*i)->eachOutput([this](OtCircuitPin pin) {
		deleteWires(pin);
		pinIndex.erase(pin->id);
	});

	// remove specified circuit
	circuitIndex.erase(circuit->id);
	circuits.erase(i);
	needsSorting = true;
}


//
//	OtAudio::deleteCircuits
//

void OtAudio::deleteCircuits(const std::vector<uint32_t>& circuitIDs) {
	for (auto id : circuitIDs) {
		deleteCircuit(id);
	}
}


//
//	OtAudio::hasCycle
//

bool OtAudio::hasCycle(OtCircuitClass* circuit, OtCircuitClass* newTarget) {
	// function result
	bool cycle = false;

	// only process each circuit once
	if (!circuit->permanentMark) {
		// check for cycles
		if (circuit->temporaryMark) {
			cycle = true;

		} else {
			// temporarily mark the circuit so we can detect cycles
			circuit->temporaryMark = true;

			// visit all circuits it depends on
			circuit->eachInput([&](OtCircuitPin pin) {
				if (!cycle && pin->sourcePin != nullptr) {
					cycle = hasCycle(pin->sourcePin->circuit);
				}
			});

			// also check the possible new wire (if required)
			if (!cycle && newTarget) {
				cycle = hasCycle(newTarget);
			}

			// set proper flags
			circuit->temporaryMark = false;
			circuit->permanentMark = true;
		}
	}

	return cycle;
}


//
//	OtAudio::causesCycle
//

bool OtAudio::causesCycle(OtCircuitPin from, OtCircuitPin to) {
	// based on https://en.wikipedia.org/wiki/Topological_sorting

	// clear all flags
	for (auto& circuit : circuits) {
		circuit->permanentMark = false;
		circuit->temporaryMark = false;
	}

	// visit the circuits in the proposed wire (depth first)
	if (hasCycle(to->circuit, from->circuit)) {
		// we have a cycle
		return true;
	}

	// no cycle detected
	return false;
}


//
//	OtAudio::isWireValid
//

bool OtAudio::isWireValid(OtCircuitPin from, OtCircuitPin to) {
	if (from->isInput() || to->isOutput()) {
		return false;

	} else if (to->isSourceConnected()) {
		return false;

	} else if (from->type != to->type) {
		return false;

	} else if (causesCycle(from, to)) {
		return false;
	}

	return true;
}


//
//	OtAudio::createWire
//

OtWire OtAudio::createWire(OtCircuitPin from, OtCircuitPin to, uint32_t id) {
	// add new wire
	auto wire = std::make_shared<OtWireClass>(from, to, id);
	wires.emplace_back(wire);
	wireIndex[wire->id] = wire;
	wire->connect();

	// set the flags
	to->circuit->needsSizing = true;
	needsSorting = true;

	// return new wire
	return wire;
}


//
//	OtAudio::findWire
//

OtWire OtAudio::findWire(OtCircuitPin from, OtCircuitPin to) {
	// find the wire
	auto i = std::find_if(wires.begin(), wires.end(), [&](OtWire& candidate) {
		return candidate->from == from && candidate->to == to;
	});

	return *i;
}


//
//	OtAudio::deleteWire
//

void OtAudio::deleteWire(OtWire wire) {
	// ensure circuit size is recalculated
	wire->to->circuit->needsSizing = true;

	// remove specified wire
	wire->disconnect();
	wireIndex.erase(wire->id);

	wires.erase(std::remove_if(wires.begin(), wires.end(), [wire](OtWire& candidate) {
		return candidate == wire;
	}), wires.end());

	// set sorting flag
	needsSorting = true;
}

void OtAudio::deleteWire(OtCircuitPin from, OtCircuitPin to) {
	// find the wire
	auto i = std::find_if(wires.begin(), wires.end(), [&](OtWire& candidate) {
		return candidate->from == from && candidate->to == to;
	});

	// ensure circuit size is recalculated
	to->circuit->needsSizing = true;

	deleteWire(*i);
}


//
//	OtAudio::deleteWires
//

void OtAudio::deleteWires(OtCircuitPin any) {
	// find wires
	std::vector<OtWire> associatedWires;

	for (auto& wire : wires) {
		if (wire->from == any || wire->to == any) {
			associatedWires.push_back(wire);
		}
	}

	// remove discovered wires
	for (auto& wire : associatedWires) {
		deleteWire(wire);
	}
}


//
//	OtAudio::redirectWire
//

void OtAudio::redirectWire(OtWire wire, uint32_t newTo) {
	// redirect wire
	wire->to->circuit->needsSizing = true;
	wire->disconnect();
	wire->redirectTo(pinIndex[newTo]);
	wire->connect();

	// set flags
	wire->to->circuit->needsSizing = true;
	needsSorting = true;
}


//
//	OtAudio::selectAll
///

void OtAudio::selectAll() {
	for (auto& circuit : circuits) {
		circuit->selected = true;
	};
}


//
//	OtAudio::deselectAll
//

void OtAudio::deselectAll() {
	for (auto& circuit : circuits) {
		circuit->selected = false;
	};
}


//
//	OtAudio::select
//

void OtAudio::select(uint32_t id, bool deselect) {
	if (deselect) {
		deselectAll();
	}

	circuitIndex[id]->selected = true;
}

void OtAudio::select(const std::vector<uint32_t>& circuitIDs, bool deselect) {
	if (deselect) {
		deselectAll();
	}

	for (auto id : circuitIDs) {
		circuitIndex[id]->selected = true;
	}
}

void OtAudio::select(int x1, int y1, int x2, int y2) {
	deselectAll();

	for (auto& circuit : circuits) {
		circuit->selected =
			x1 < (circuit->x + circuit->w) && x2 > circuit->x &&
			y1 < (circuit->y + circuit->h) && y2 > circuit->y;
	};
}


//
//	OtAudio::deselect
//

void OtAudio::deselect(uint32_t id) {
	circuitIndex[id]->selected = false;
}


//
//	OtAudio::getSelected
//

std::vector<uint32_t> OtAudio::getSelected() {
	std::vector<uint32_t> result;

	for (auto& circuit : circuits) {
		if (circuit->selected) {
			result.push_back(circuit->id);
		}
	}

	return result;
}


//
//	OtAudio::archiveCircuit
//

std::string OtAudio::archiveCircuit(uint32_t circuit) {
	std::vector<uint32_t> selection = {circuit};
	return archiveCircuits(selection);
}


//
//	OtAudio::archiveCircuits
//

std::string OtAudio::archiveCircuits(const std::vector<uint32_t>& circuitIDs) {
	auto circuitsJSON = nlohmann::json::array();
	auto wiresJSON = nlohmann::json::array();
	std::set<OtWire> associatedWires;

	for (auto id : circuitIDs) {
		auto circuit = circuitIndex[id];
		circuitsJSON.push_back(circuit->serialize());

		eachWire([&](OtWire& wire) {
			if (wire->from->circuit->id == id || wire->to->circuit->id == id) {
				associatedWires.insert(wire);
			}
		});
	}

	for (auto& wire : associatedWires) {
		wiresJSON.push_back(wire->serialize());
	}

	auto data = nlohmann::json::object();
	data["circuits"] = circuitsJSON;
	data["wires"] = wiresJSON;
	return data.dump();
}


//
//	OtAudio::restoreCircuit
//

OtCircuit OtAudio::restoreCircuit(nlohmann::json data, bool restoreIDs, std::string* basedir) {
	// create a new circuit
	auto circuit = OtCircuitFactory::createCircuit(data["type"]);
	circuit->deserialize(data, restoreIDs, basedir);
	circuits.emplace_back(circuit);
	needsSorting = true;

	circuit->needsPlacement = true;
	circuit->needsSizing = true;

	// index circuit and pins
	indexCircuit(circuit);

	// return new circuit
	return circuit;
}


//
//	OtAudio::restoreCircuits
//

void OtAudio::restoreCircuits(const std::string& json) {
	auto data = nlohmann::json::parse(json);

	// restore each circuit
	for (auto& circuit : data["circuits"]) {
		restoreCircuit(circuit);
	}

	// restore wires
	for (auto& wire : data["wires"]) {
		createWire(wire["from"], wire["to"], wire["id"]);
	}
}


//
//	OtAudio::duplicateCircuits
//

std::vector<uint32_t> OtAudio::duplicateCircuits(const std::string& json) {
	auto data = nlohmann::json::parse(json);
	std::vector<uint32_t> circuitIDs;

	// restore each circuit
	for (auto& circuit : data["circuits"]) {
		auto newCircuit = restoreCircuit(circuit, false);
		newCircuit->x += 50.0f;
		newCircuit->y += 50.0f;
		circuitIDs.push_back(newCircuit->id);
	}

	// create list of circuit IDs
	return circuitIDs;
}


//
//	OtAudio::indexCircuit
//

void OtAudio::indexCircuit(OtCircuit circuit) {
	// index circuit and its pins
	circuitIndex[circuit->id] = circuit;

	circuit->eachInput([this](OtCircuitPin pin) {
		pinIndex[pin->id] = pin;
	});

	circuit->eachOutput([this](OtCircuitPin pin) {
		pinIndex[pin->id] = pin;
	});
}


//
//	OtAudio::unindexCircuit
//

void OtAudio::unindexCircuit(OtCircuit circuit) {
	circuitIndex.erase(circuitIndex.find(circuit->id));

	circuit->eachInput([this](OtCircuitPin pin) {
		pinIndex.erase(pinIndex.find(pin->id));
	});

	circuit->eachOutput([this](OtCircuitPin pin) {
		pinIndex.erase(pinIndex.find(pin->id));
	});
}


//
//	OtAudio::visitCircuit
//

bool OtAudio::visitCircuit(OtCircuit circuit, std::vector<OtCircuit>& circuitIDs) {
	// function result
	bool cycle = false;

	// only process each circuit once
	if (!circuit->permanentMark) {
		// check for cycles
		if (circuit->temporaryMark) {
			cycle = true;

		} else {
			// temporarily mark the circuit so we can detect cycles
			circuit->temporaryMark = true;

			// visit all circuits it depends on
			circuit->eachInput([&](OtCircuitPin pin) {
				if (!cycle && pin->sourcePin != nullptr) {
					cycle = visitCircuit(circuitIndex[pin->sourcePin->circuit->id], circuitIDs);
				}
			});

			// set proper flags and add circuit to list (if required)
			if (!cycle) {
				circuit->temporaryMark = false;
				circuit->permanentMark = true;
				circuitIDs.push_back(circuit);
			}
		}
	}

	return cycle;
}


//
//	OtAudio::sortCircuitsTopologically
//

void OtAudio::sortCircuitsTopologically() {
	// based on https://en.wikipedia.org/wiki/Topological_sorting

	// clear all flags
	for (auto& circuit : circuits) {
		circuit->permanentMark = false;
		circuit->temporaryMark = false;
	}

	// place to store sorted circuits
	std::vector<OtCircuit> sortedCircuits;

	// process all circuits
	for (auto& circuit : circuits) {
		if (visitCircuit(circuit, sortedCircuits)) {
			return;
		}
	}

	// sort was successful
	circuits.swap(sortedCircuits);
}
