#ifndef WIRE_H
#define WIRE_H

#include "main.h"

class Wire {
public:
	Wire(std::string _name)
		: name(_name) {}
	~Wire() = default;

	void SetValue(bool val);
	void SetInput(comp_t component) {input = component;}
	void AddOutput(comp_t component);
	void SetAsInputWire() {is_input_wire = true;}
	void SetAsOutputWire() {is_output_wire = true;}

	const bool           GetValue();
	const bool           HasChanged();
	std::string          GetName() {return name;}
	std::size_t          GetNumToggles() {return toggle_count;}
	comp_wt              GetInput() {return input;}
	std::vector<comp_wt> GetOutputs() {return outputs;}
	const bool           IsInputWire() {return is_input_wire;}
	const bool           IsOutputWire() {return is_output_wire;}

private:
	bool curr_value = false;
	bool has_changed = false;
	bool is_input_wire = false;
	bool is_output_wire = false;

	std::size_t toggle_count = 0;
	std::string name;

	comp_wt input;
	std::vector<comp_wt> outputs;
};

#endif // WIRE_H
