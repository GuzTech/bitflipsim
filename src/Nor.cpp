#include "main.h"

/*
  NOR gate implementation.

  Schematic:

  A ---|N|
       |O|--- O
  B ---|R|
*/

bool Nor::entityGenerated = false;

void Nor::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(!(inA | inB), propagating);
		}

		needs_update = false;
	}
}

void Nor::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch (port) {
	case PORTS::A:
		A = wire;
		wire->AddOutput(this->shared_from_base<Nor>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		B = wire;
		wire->AddOutput(this->shared_from_base<Nor>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		O = wire;
		wire->SetInput(this->shared_from_base<Nor>());
		output_wires.emplace_back(O);
		break;
	default:
		Error("Trying to connect to undefined port of Nor \"" + name + "\".\n");
	}
}

void Nor::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Nor::GetWire(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::A: return A;
	case PORTS::B: return B;
	case PORTS::O: return O;
	default:
		Error("Trying to retrieve undefined port of Nor \"" + name + "\".\n");
	}
}

const PORT_DIR Nor::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
		return PORT_DIR::INPUT;
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in Nor \"" + name + "\".\n");
	}
}

void Nor::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the Nor gate are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("Nor");
		ExpandTemplate("src/templates/VHDL/Nor_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/Nor.vhd");
		outfile << output;
		outfile.close();

		entityGenerated = true;
	}
}

const string Nor::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("Nor");
	inst.SetValue("NAME", name);
	ExpandTemplate("src/templates/VHDL/Nor_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
