#include "main.h"

/*
  XOR gate implementation.

  Schematic:

  A ---|X|
       |O|--- O
  B ---|R|
*/

void Xor::Update() {
	if (needs_update) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(inA ^ inB);
		}

		needs_update = false;
	}
}

void Xor::Connect(PORTS port, wire_t wire) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Xor>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Xor>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<Xor>()); break;
	}
}

std::vector<wire_t> Xor::GetWires() {
	return {A, B, O};
}
