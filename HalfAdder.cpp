#include "main.h"

/*
  Half-adder implementation.

  Schematic:

  A ----+-|X|
        | |O|------ S
  B --+---|R|
      | |
      | |-|A|
      |   |N|------ C
      |---|D|
 */

void HalfAdder::Update() {
	if (needs_update) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (S) {
			S->SetValue(inA ^ inB);
		}
		if (C) {
			C->SetValue(inA & inB);
		}

		needs_update = false;
	}
}

void HalfAdder::Connect(PORTS port, wire_t wire) {
	switch (port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::S: S = wire; wire->SetInput(this->shared_from_base<HalfAdder>()); break;
	case PORTS::C: C = wire; wire->SetInput(this->shared_from_base<HalfAdder>()); break;
	}
}

std::vector<wire_t> HalfAdder::GetWires() {
	return {A, B, S, C};
}
