#include "main.h"

const vector<wire_t> Component::GetWires() const {
	vector<wire_t> wires;

	// Add all input wires.
	wires.insert(wires.end(),
				 input_wires.begin(),
				 input_wires.end());

	// Add all internal wires.
	wires.insert(wires.end(),
				 internal_wires.begin(),
				 internal_wires.end());

	// Add all output wires.
	wires.insert(wires.end(),
				 output_wires.begin(),
				 output_wires.end());

	return wires;
}
