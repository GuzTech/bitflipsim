#include "main.h"

/*
  AND gate implementation.

  Schematic:

  A ---|A|
       |N|--- O
  B ---|D|
*/

bool And::entityGenerated = false;

void And::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(inA & inB, propagating);
		}

		if (!propagating) {
			needs_update = false;
		}
	}
}

void And::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch(port) {
	case PORTS::A:
		A = wire;
		wire->AddOutput(this->shared_from_base<And>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::B:
		B = wire;
		wire->AddOutput(this->shared_from_base<And>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		O = wire; wire->SetInput(this->shared_from_base<And>());
		output_wires.emplace_back(O);
		break;
	default:
		cout << "[Error] Trying to connect to undefined port of And "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void And::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t And::GetWire(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::A: return A;
	case PORTS::B: return B;
	case PORTS::O: return O;
	default:
		cout << "[Error] Trying to retrieve undefined port of And "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void And::GenerateVHDLEntity(const string &path) const {
	// We only need to do it once, since all instances of the And gate are identical.
	if (!entityGenerated) {
		string output;
		TemplateDictionary entity("And");
		ExpandTemplate("src/templates/VHDL/And_entity.tpl", DO_NOT_STRIP, &entity, &output);

		auto outfile = ofstream(path + "/And.vhd");
		outfile << output;
		outfile.close();

		entityGenerated = true;
	}
}

const string And::GenerateVHDLInstance() const {
	string output;
	TemplateDictionary inst("And");
	inst.SetValue("NAME", name);
	ExpandTemplate("src/templates/VHDL/And_inst.tpl", DO_NOT_STRIP, &inst, &output);

	return output;
}