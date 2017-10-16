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

void Not::Connect(PORTS port, wire_t wire, size_t index) {
	switch (port) {
	case PORTS::I: I = wire; wire->AddOutput(this->shared_from_base<Not>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<Not>()); break;
	default:
		cout << "[Error] Trying to connect to undefined port of Not "
			 << "\"" << name << "\"n";
		exit(1);
	}
}

vector<wire_t> Not::GetWires() {
	return {I, O};
}

vector<wire_t> Not::GetInputWires() {
	return {I};
}

vector<wire_t> Not::GetOutputWires() {
	return {O};
}

wire_t Not::GetWire(PORTS port, size_t index) {
	switch (port) {
	case PORTS::I: return I;
	case PORTS::O: return O;
	default:
		cout << "[Error] Trying to retrieve undefined port of Not "
			 << "\"" << name << "\"n";
		exit(1);
	}
}
