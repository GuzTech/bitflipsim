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

void Xnor::Connect(PORTS port, wire_t wire, size_t index) {
	switch (port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Xnor>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Xnor>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<Xnor>()); break;
	default:
		cout << "[Error] Trying to connect to undefined port of Xnor "
			 << "\"" << name << "\"n";
		exit(1);
	}
}

vector<wire_t> Xnor::GetWires() {
	return {A, B, O};
}

vector<wire_t> Xnor::GetInputWires() {
	return {A, B};
}

vector<wire_t> Xnor::GetOutputWires() {
	return {O};
}

wire_t Xnor::GetWire(PORTS port, size_t index) {
	switch (port) {
	case PORTS::A: return A;
	case PORTS::B: return B;
	case PORTS::O: return O;
	default:
		cout << "[Error] Trying to retrieve undefined port of Xnor "
			 << "\"" << name << "\"n";
		exit(1);
	}
}
