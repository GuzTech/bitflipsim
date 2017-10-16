#include "main.h"

/*
  AND gate implementation.

  Schematic:

  A ---|A|
       |N|--- O
  B ---|D|
*/

void And::Update(bool propagating) {
	if (needs_update || !propagating) {
		bool inA, inB;

		inA = A ? A->GetValue() : false;
		inB = B ? B->GetValue() : false;

		if (O) {
			O->SetValue(inA & inB, propagating);
		}

		if (!propagating) {
			needs_update = false;
		}
	}
}

void And::Connect(PORTS port, wire_t wire, size_t index) {
	switch(port) {
	case PORTS::A: A = wire; wire->AddOutput(this->shared_from_base<And>()); break;
	case PORTS::B: B = wire; wire->AddOutput(this->shared_from_base<And>()); break;
	case PORTS::O: O = wire; wire->SetInput(this->shared_from_base<And>()); break;
	default:
		cout << "[Error] Trying to connect to undefined port of And "
			 << "\"" << name << "\"n";
		exit(1);
	}
}

vector<wire_t> And::GetWires() {
	return {A, B, O};
}

vector<wire_t> And::GetInputWires() {
	return {A, B};
}

vector<wire_t> And::GetOutputWires() {
	return {O};
}
