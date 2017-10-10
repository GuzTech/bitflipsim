#include "main.h"

/*
  NOT gate implementation.

  Schematic:

       |N|
  I ---|O|--- O
       |T|
*/

void Not::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool in;

		in = I ? I->GetValue() : false;

		if (O) {
			O->SetValue(!in, propagating);
		}

		if (!propagating) {
			needs_update = false;
		}
	}
}

void Not::Connect(PORTS port, wire_t wire, std::size_t index) {
	switch(port) {
	case PORTS::I: I = wire; wire->AddOutput(this->shared_from_base<Not>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<Not>()); break;
	default:
		std::cout << "[Error] Trying to connect to undefined port of Not "
				  << "\"" << name << "\"n";
		exit(1);
	}
}

std::vector<wire_t> Not::GetWires() {
	return {I, O};
}

std::vector<wire_t> Not::GetInputWires() {
	return {I};
}

std::vector<wire_t> Not::GetOutputWires() {
	return {O};
}
