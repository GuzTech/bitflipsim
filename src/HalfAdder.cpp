#include "main.h"

/*
  Half-adder implementation.

  Schematic:

  A ----+-|X|
        | |O|------ O
  B --+---|R|
      | |
      | |-|A|
      |   |N|------ Cout
      |---|D|
 */

bool HalfAdder::entityGenerated = false;

HalfAdder::HalfAdder(string _name)
	: Component(_name)
{
	xor_ha = make_shared<Xor>(name + "_xor");
	and_ha = make_shared<And>(name + "_and");
}

void HalfAdder::Update(bool propagating) {
	if (needs_update || !propagating) {
		xor_ha->Update(propagating);
		and_ha->Update(propagating);

		needs_update = false;
	}
}

void HalfAdder::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch (port) {
	case PORTS::A:
		xor_ha->Connect(PORTS::A, wire);
		and_ha->Connect(PORTS::A, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		xor_ha->Connect(PORTS::B, wire);
		and_ha->Connect(PORTS::B, wire);
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		xor_ha->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	case PORTS::Cout:
		and_ha->Connect(PORTS::O, wire);
		output_wires.emplace_back(wire);
		break;
	default:
		Error("Trying to connect to undefined port of HalfAdder \"" + name + "\"\n");
	}
}

void HalfAdder::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t HalfAdder::GetWire(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::O:
		return xor_ha->GetWire(port);
	case PORTS::Cout:
		return and_ha->GetWire(PORTS::O);
	default:
		Error("Trying to retrieve undefined port of HalfAdder \"" + name + "\".\n");
	}
}

const PORT_DIR HalfAdder::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
		return PORT_DIR::INPUT;
	case PORTS::Cout:
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in HalfAdder \"" + name + "\".\n");
	}
}

void HalfAdder::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the HalfAdder are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("HalfAdder");
		ExpandTemplate("src/templates/VHDL/HalfAdder_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/HalfAdder.vhd");
		outfile << output;
		outfile.close();

		entityGenerated = true;
	}
}

const string HalfAdder::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("HalfAdder");
	inst.SetValue("NAME", name);
	ExpandTemplate("src/templates/VHDL/HalfAdder_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
