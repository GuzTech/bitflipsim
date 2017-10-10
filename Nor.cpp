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

void Nor::Connect(PORTS port, wire_t wire, std::size_t index) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Nor>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Nor>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<Nor>()); break;
	default:
		std::cout << "[Error] Trying to connect to undefined port of Nor "
				  << "\"" << name << "\"n";
		exit(1);
	}
}

std::vector<wire_t> Nor::GetWires() {
	return {A, B, O};
}

std::vector<wire_t> Nor::GetInputWires() {
	return {A, B};
}

std::vector<wire_t> Nor::GetOutputWires() {
	return {O};
}
