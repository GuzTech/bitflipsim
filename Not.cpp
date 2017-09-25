#include "main.h"

/*
  NOT gate implementation.

  Schematic:

       |N|
  I ---|O|--- O
       |T|
*/

void Not::Update() {
	if (needs_update) {
		bool in;

		in = I ? I->GetValue() : false;

		if (O) {
			O->SetValue(!in);
		}

		needs_update = false;
	}
}

void Not::Connect(PORTS port, wire_t wire) {
	switch(port) {
	case PORTS::I: I = wire; wire->AddOutput(this->shared_from_base<Not>()); break;
	case PORTS::O: O = wire; break;
	}
}

std::vector<wire_t> Not::GetWires() {
	return {I, O};
}
