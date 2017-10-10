#include "main.h"

/*
  XOR gate implementation.

  Schematic:

  A ---|X|
       |O|--- O
  B ---|R|
*/

void Xor::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(inA ^ inB, propagating);
		}

		needs_update = false;
	}
}

void Xor::Connect(PORTS port, wire_t wire, std::size_t index) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Xor>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Xor>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<Xor>()); break;
	default:
		std::cout << "[Error] Trying to connect to undefined port of Xor "
				  << "\"" << name << "\"n";
		exit(1);
	}
}

std::vector<wire_t> Xor::GetWires() {
	return {A, B, O};
}

std::vector<wire_t> Xor::GetInputWires() {
	return {A, B};
}

std::vector<wire_t> Xor::GetOutputWires() {
	return {O};
}
