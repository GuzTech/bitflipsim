#include "main.h"

/*
  XNOR gate implementation.

  Schematic:

  A ---|X|
       |N|
       |O|--- O
  B ---|R|
*/

void Xnor::Update() {
	if (needs_update) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(!(inA ^ inB));
		}

		needs_update = false;
	}
}

void Xnor::Connect(PORTS port, wire_t wire) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Xnor>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Xnor>()); break;
	case PORTS::O: O = wire; break;
	}
}

std::vector<wire_t> Xnor::GetWires() {
	return {A, B, O};
}
