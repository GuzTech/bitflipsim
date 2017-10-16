#include "main.h"

/*
  Half-adder implementation.

  Schematic:

  A ----+-|X|
        | |O|------ S
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

		if (S) {
			S->SetValue(inA ^ inB, propagating);
		}
		if (Cout) {
			Cout->SetValue(inA & inB, propagating);
		}

		needs_update = false;
	}
}

void HalfAdder::Connect(PORTS port, wire_t wire, size_t index) {
	switch (port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::S: S = wire; wire->SetInput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::Cout: Cout = wire; wire->SetInput(this->shared_from_base<HalfAdder>()); break;
	default:
		cout << "[Error] Trying to connect to undefined port of HalfAdder "
			 << "\"" << name << "\"\n";
		exit(1);
	}
}

vector<wire_t> HalfAdder::GetWires() {
	return {A, B, S, Cout};
}

vector<wire_t> HalfAdder::GetInputWires() {
	return {A, B};
}

vector<wire_t> HalfAdder::GetOutputWires() {
	return {S, Cout};
}

wire_t HalfAdder::GetWire(PORTS port, size_t index) {
	switch (port) {
	case PORTS::A:    return A;
	case PORTS::B:    return B;
	case PORTS::S:    return S;
	case PORTS::Cout: return Cout;
	default:
		cout << "[Error] Trying to retrieve undefined port of HalfAdder "
			 << "\"" << name << "\"\n";
		exit(1);
	}
}
