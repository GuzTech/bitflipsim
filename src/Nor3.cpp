#include "main.h"

/*
  NOR3 gate implementation.

  Schematic:

  A ---|N|
  B ---|O|--- O
  C ---|R|
*/

bool Nor3::entityGenerated = false;

void Nor3::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB, inC;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;
		inC = C ? C->GetValue() : false;

		if (O) {
			O->SetValue(!(inA | inB | inC), propagating);
		}

		needs_update = false;
	}
}

void Nor3::Connect(PORTS port, const wire_t &wire, size_t index) {
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
	case PORTS::C:
		C = wire;
		wire->AddOutput(this->shared_from_base<Nor>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		O = wire; wire->SetInput(this->shared_from_base<Nor>());
		output_wires.emplace_back(O);
		break;
	default:
		cout << "[Error] Trying to connect to undefined port of Nor3 "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void Nor3::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Nor3::GetWire(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::A: return A;
	case PORTS::B: return B;
	case PORTS::C: return C;
	case PORTS::O: return O;
	default:
		cout << "[Error] Trying to retrieve undefined port of Nor3 "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void Nor3::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the Nor3 gate are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("Nor3");
		ExpandTemplate("src/templates/VHDL/Nor3_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/Nor3.vhd");
		outfile << output;
		outfile.close();

		entityGenerated = true;
	}
}

const string Nor3::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("Nor3");
	inst.SetValue("NAME", name);
	ExpandTemplate("src/templates/VHDL/Nor3_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}
