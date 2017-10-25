#ifndef WIRE_H
#define WIRE_H

#include "main.h"

class Wire {
public:
	Wire(string _name, wb_t bundle = nullptr)
		: name(_name)
		, part_of_bundle(bundle) {}
	~Wire() = default;

	void SetValue(bool val, bool propagating = true);
	void SetInput(comp_t component) {input = component;}
	void AddOutput(comp_t component);
	void SetAsInputWire() {is_input_wire = true;}
	void SetAsOutputWire() {is_output_wire = true;}

	const bool GetValue();
	const bool  HasChanged();
	const string &GetName() {return name;}
	size_t GetNumToggles() {return toggle_count;}
	comp_wt GetInput() {return input;}
	vector<comp_wt> GetOutputs() {return outputs;}
	wb_t GetWireBundle() {return part_of_bundle;}
	size_t GetNumOutputs() {return num_outputs;};
	const bool IsInputWire() {return is_input_wire;}
	const bool IsOutputWire() {return is_output_wire;}

private:
	bool curr_value = false; // The current value on the wire.
	bool prev_value = false; // The value on the wire before propagation started.
	bool has_changed = false; // True if the value that is set is different from the current value.
	bool is_input_wire = false; // True if this wire is connected to the global input.
	bool is_output_wire = false; // True if this wire is connected to the global output.

	size_t toggle_count = 0; // Tracks how many times this wire has changed its value.
	string name; // Name of this wire.

	comp_wt input; // The component that drives this wire.
	vector<comp_wt> outputs; // The components that are driven by this wire.
	size_t num_outputs = 1; // The number of components that are driven by this wire.
	wb_t part_of_bundle = nullptr; // Indicates whether this wire is part of a bundle.
};

#endif // WIRE_H
