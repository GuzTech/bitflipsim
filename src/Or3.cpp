#include "main.h"

/*
  OR3 gate implementation.

  Schematic:

  A ---|O|
  B ---| |--- O
  C ---|R|
*/

bool Or3::entityGenerated = false;

void Or3::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB, inC;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;
		inC = C ? C->GetValue() : false;

		if (O) {
			O->SetValue(inA | inB | inC, propagating);
		}

		needs_update = false;
	}
}

void Or3::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch (port) {
	case PORTS::A:
		A = wire;
		wire->AddOutput(this->shared_from_base<Or3>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		B = wire;
		wire->AddOutput(this->shared_from_base<Or3>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::C:
		C = wire;
		wire->AddOutput(this->shared_from_base<Or3>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		O = wire;
		wire->SetInput(this->shared_from_base<Or3>());
		output_wires.emplace_back(O);
		break;
	default:
		Error("Trying to connect to undefined port of Or3 \"" + name + "\".\n");
	}
}

void Or3::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Or3::GetWire(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::A: return A;
	case PORTS::B: return B;
	case PORTS::C: return C;
	case PORTS::O: return O;
	default:
		Error("Trying to retrieve undefined port of Or3 \"" + name + "\".\n");
	}
}

const PORT_DIR Or3::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
	case PORTS::C:
		return PORT_DIR::INPUT;
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in Or3 \"" + name + "\".\n");
	}
}

void Or3::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the Or3 gate are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("Or3");
		ExpandTemplate("src/templates/VHDL/Or3_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/Or3.vhd");
		outfile << output;
		outfile.close();

		entityGenerated = true;
	}
}

const string Or3::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("Or3");
	inst.SetValue("NAME", name);
	ExpandTemplate("src/templates/VHDL/Or3_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
