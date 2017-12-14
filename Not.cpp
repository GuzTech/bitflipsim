#include "main.h"

/*
  NOT gate implementation.

  Schematic:

       |N|
  I ---|O|--- O
       |T|
*/

void Not::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool in;

		in = I ? I->GetValue() : false;

		if (O) {
			O->SetValue(!in, propagating);
		}

		if (!propagating) {
			needs_update = false;
		}
	}
}

void Not::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch (port) {
	case PORTS::I:
		I = wire;
		wire->AddOutput(this->shared_from_base<Not>());
		input_wires.emplace_back(wire);
		break;
	case PORTS::O:
		O = wire; wire->SetInput(this->shared_from_base<Not>());
		output_wires.emplace_back(O);
		break;
	default:
		cout << "[Error] Trying to connect to undefined port of Not "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}

void Not::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << ".\n";
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const wire_t Not::GetWire(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::I: return I;
	case PORTS::O: return O;
	default:
		cout << "[Error] Trying to retrieve undefined port of Not "
			 << "\"" << name << "\".\n";
		exit(1);
	}
}
