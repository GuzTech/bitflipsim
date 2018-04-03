#include "main.h"

/*
  NAND gate implementation.

  Schematic:

  A ---|N|
       |A|
       |N|--- O
  B ---|D|
*/

bool Nand::entityGenerated = false;

void Nand::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(!(inA & inB), propagating);
		}

		needs_update = false;
	}
}

void Nand::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch (port) {
	case PORTS::A:
		A = wire;
		wire->AddOutput(this->shared_from_base<Nand>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		B = wire;
		wire->AddOutput(this->shared_from_base<Nand>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		O = wire;
		wire->SetInput(this->shared_from_base<Nand>());
		output_wires.emplace_back(O);
		break;
	default:
		Error("Trying to connect to undefined port of Nand \"" + name + "\".\n");
	}
}

void Nand::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		Error("Wire bundle \"" + wires->GetName() + " accessed with index " + to_string(wire_idx)
			  + " but has size " + to_string(wires->GetSize()) + ".\n");
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Nand::GetWire(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::A: return A;
	case PORTS::B: return B;
	case PORTS::O: return O;
	default:
		Error("Trying to retrieve undefined port of Nand \"" + name + "\".\n");
	}
}

const PORT_DIR Nand::GetPortDirection(PORTS port) const {
	switch (port) {
	case PORTS::A:
	case PORTS::B:
		return PORT_DIR::INPUT;
	case PORTS::O:
		return PORT_DIR::OUTPUT;
	default:
		Error("Trying to get port direction of undefined port in Nand \"" + name + "\".\n");
	}
}

void Nand::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the Nand gate are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("Nand");
		ExpandTemplate("src/templates/VHDL/Nand_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/Nand.vhd");
		outfile << output;
		outfile.close();

		entityGenerated = true;
	}
}

const string Nand::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("Nand");
	inst.SetValue("NAME", name);
	ExpandTemplate("src/templates/VHDL/Nand_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
