#include "main.h"

/*
  AND gate implementation.

  Schematic:

  A ---|A|
       |N|--- O
  B ---|D|
*/

void And::Update() {
	if (needs_update) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(inA & inB);
		}

		needs_update = false;
	}
}

void And::Connect(PORTS port, wire_t wire) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<And>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<And>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<And>()); break;
	}
}

std::vector<wire_t> And::GetWires() {
	return {A, B, O};
}

std::vector<wire_t> And::GetInputWires() {
	return {A, B};
}

std::vector<wire_t> And::GetOutputWires() {
	return {O};
}
