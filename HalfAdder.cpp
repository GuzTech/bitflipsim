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

void HalfAdder::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(inA ^ inB, propagating);
		}
		if (Cout) {
			Cout->SetValue(inA & inB, propagating);
		}

		needs_update = false;
	}
}

void HalfAdder::Connect(PORTS port, const wire_t &wire, size_t index) {
	switch (port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::Cout: Cout = wire; wire->SetInput(this->shared_from_base<HalfAdder>()); break;
	default:
		cout << "[Error] Trying to connect to undefined port of HalfAdder "
			 << "\"" << name << "\"\n";
		exit(1);
	}
}

void HalfAdder::Connect(PORTS port, const wb_t &wires, size_t port_idx, size_t wire_idx) {
	if (wire_idx >= wires->GetSize()) {
		cout << "[Error] Wire bundle \"" << wires->GetName()
			 << " accessed with index " << wire_idx
			 << " but has size " << wires->GetSize() << '\n';
		exit(1);
	}

	const wire_t &wire = (*wires)[wire_idx];
	Connect(port, wire, port_idx);
}

vector<wire_t> HalfAdder::GetWires() {
	return {A, B, O, Cout};
}

vector<wire_t> HalfAdder::GetInputWires() {
	return {A, B};
}

vector<wire_t> HalfAdder::GetOutputWires() {
	return {O, Cout};
}

wire_t HalfAdder::GetWire(PORTS port, size_t index) {
	switch (port) {
	case PORTS::A:    return A;
	case PORTS::B:    return B;
	case PORTS::O:    return O;
	case PORTS::Cout: return Cout;
	default:
		cout << "[Error] Trying to retrieve undefined port of HalfAdder "
			 << "\"" << name << "\"\n";
		exit(1);
	}
}
