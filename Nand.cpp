#include "main.h"

/*
  NAND gate implementation.

  Schematic:

  A ---|N|
       |A|
       |N|--- O
  B ---|D|
*/

void Nand::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(!(inA & inB), propagating);
		}

		needs_update = false;
	}
}

void Nand::Connect(PORTS port, wire_t wire, std::size_t index) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<Nand>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<Nand>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<Nand>()); break;
	default:
		std::cout << "[Error] Trying to connect to undefined port of Nand "
				  << "\"" << name << "\"n";
		exit(1);
	}
}

std::vector<wire_t> Nand::GetWires() {
	return {A, B, O};
}

std::vector<wire_t> Nand::GetInputWires() {
	return {A, B};
}

std::vector<wire_t> Nand::GetOutputWires() {
	return {O};
}
