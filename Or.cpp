#include "main.h"

/*
  OR gate implementation.

  Schematic:

  A ---|O|
       | |--- O
  B ---|R|
*/

void Or::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(inA | inB, propagating);
		}

		needs_update = false;
	}
}

void Or::Connect(PORTS port, wire_t wire, std::size_t index) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Or>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Or>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<Or>()); break;
	default:
		std::cout << "[Error] Trying to connect to undefined port of Or "
				  << "\"" << name << "\"n";
		exit(1);
	}
}

std::vector<wire_t> Or::GetWires() {
	return {A, B, O};
}

std::vector<wire_t> Or::GetInputWires() {
	return {A, B};
}

std::vector<wire_t> Or::GetOutputWires() {
	return {O};
}
