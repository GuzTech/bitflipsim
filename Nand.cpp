#include "main.h"

/*
  NAND gate implementation.

  Schematic:

  A ---|N|
       |A|
       |N|--- O
  B ---|D|
*/

void Nand::Update() {
	if (needs_update) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(!(inA & inB));
		}

		needs_update = false;
	}
}

void Nand::Connect(PORTS port, wire_t wire) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<And>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<And>()); break;
	case PORTS::O: O = wire; break;
	}
}

std::vector<wire_t> Nand::GetWires() {
	return {A, B, O};
}
