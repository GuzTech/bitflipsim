#include "main.h"

/*
  XNOR gate implementation.

  Schematic:

  A ---|X|
       |N|
       |O|--- O
  B ---|R|
*/

void Xnor::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(!(inA ^ inB), propagating);
		}

		needs_update = false;
	}
}

void Xnor::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch (port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Xnor>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Xnor>()); break;
	case PORTS::O:
		O = wire; wire->SetInput(this->shared_from_base<Xnor>());
		output_wires.emplace_back(O);
		break;
	default:
		cout << "[Error] Trying to connect to undefined port of Xnor "
			 << "\"" << name << "\"\n";
		exit(1);
	}
}

void Xnor::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << '\n';
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

const vector<wire_t> Xnor::GetWires() const {
	return {A, B, O};
}

const vector<wire_t> Xnor::GetInputWires() const {
	return {A, B};
}

const wire_t Xnor::GetWire(PORTS port, size_t index) const {
	switch (port) {
	case PORTS::A: return A;
	case PORTS::B: return B;
	case PORTS::O: return O;
	default:
		cout << "[Error] Trying to retrieve undefined port of Xnor "
			 << "\"" << name << "\"\n";
		exit(1);
	}
}
