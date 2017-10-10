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

void HalfAdder::Connect(PORTS port, wire_t wire, std::size_t index) {
	switch (port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::S: S = wire; wire->SetInput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::Cout: Cout = wire; wire->SetInput(this->shared_from_base<HalfAdder>()); break;
	default:
		std::cout << "[Error] Trying to connect to undefined port of HalfAdder "
				  << "\"" << name << "\"n";
		exit(1);
	}
}

std::vector<wire_t> HalfAdder::GetWires() {
	return {A, B, S, Cout};
}

std::vector<wire_t> HalfAdder::GetInputWires() {
	return {A, B};
}

std::vector<wire_t> HalfAdder::GetOutputWires() {
	return {S, Cout};
}
