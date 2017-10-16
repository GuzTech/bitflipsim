#include "main.h"

/*
  NOR gate implementation.

  Schematic:

  A ---|N|
       |O|--- O
  B ---|R|
*/

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

void Nor::Connect(PORTS port, wire_t wire, size_t index) {
	switch (port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Nor>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Nor>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<Nor>()); break;
	default:
		cout << "[Error] Trying to connect to undefined port of Nor "
			 << "\"" << name << "\"n";
		exit(1);
	}
}

vector<wire_t> Nor::GetWires() {
	return {A, B, O};
}

vector<wire_t> Nor::GetInputWires() {
	return {A, B};
}

vector<wire_t> Nor::GetOutputWires() {
	return {O};
}

wire_t Nor::GetWire(PORTS port, size_t index) {
	switch (port) {
	case PORTS::A: return A;
	case PORTS::B: return B;
	case PORTS::O: return O;
	default:
		cout << "[Error] Trying to retrieve undefined port of Nor "
			 << "\"" << name << "\"n";
		exit(1);
	}
}
