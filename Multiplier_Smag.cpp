#include "main.h"

/*
  Signed magnitude multiplier implementation.
*/

Multiplier_Smag::Multiplier_Smag(string _name,
								 size_t _num_bits_A,
								 size_t _num_bits_B)
	: Component(_name)
	, num_bits_A(_num_bits_A - 1)
	, num_bits_B(_num_bits_B - 1)
	, num_bits_O(num_bits_A + num_bits_B + 1)
	, num_adder_levels(num_bits_B - 1)
	, num_and_levels(num_bits_B)
	, num_adders_per_level(num_bits_A)
	, num_ands_per_level(num_bits_A)
{
	sign = make_shared<Xor>(_name + "sign");

	// Names for the adders.
	string row_name_prefix = string(_name) + "_S_0_";
	string row_name = row_name_prefix + "0";

	// First row consists of #A - 2 FullAdders and 2 HalfAdders.
	vector<comp_t> adders_row;

	adders_row.push_back(make_shared<HalfAdder>(row_name));
	for (size_t a = 1; a < num_adders_per_level - 1; ++a) {
		row_name = row_name_prefix;
		row_name += to_string(a);
		
		adders_row.push_back(make_shared<FullAdder>(row_name));
	}
	row_name = row_name_prefix;
	row_name += to_string(num_adders_per_level - 1);
	adders_row.push_back(make_shared<HalfAdder>(row_name));

	adders.push_back(adders_row);
	adders_row.clear();

	// The rest of the rows consist of #A FullAdders and 1 HalfAdder.
	for (size_t b = 1; b < num_adder_levels; ++b) {
		row_name_prefix = _name + "_S_" + to_string(b) + "_";
		row_name = row_name_prefix + "0";
		adders_row.push_back(make_shared<HalfAdder>(row_name));
		
		for (size_t a = 1; a < num_adders_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);
			adders_row.push_back(make_shared<FullAdder>(row_name));
		}

		adders.push_back(adders_row);
		adders_row.clear();
	}

	// Create the AND gates that connect to the inputs of the adders.
	vector<and_t> ands_row;

	// First level of AND gates is slightly different, so handle it
	// separately. The first AND gate is directly connected to bit 0
	// of the result.
	row_name_prefix = _name + "_AND_0_";
	ands_row.push_back(make_shared<And>(row_name_prefix + "0"));

	for (size_t a = 1; a < num_ands_per_level; ++a) {
		row_name = row_name_prefix + to_string(a);

		auto and_gate = make_shared<And>(row_name);
		auto and_wire = make_shared<Wire>(row_name + "_O");
		and_gate->Connect(PORTS::O, and_wire);
		adders[0][a-1]->Connect(PORTS::A, and_wire);
		ands_row.push_back(and_gate);
	}
	ands.push_back(ands_row);
	ands_row.clear();

	// Now handle the rest of the AND gate levels.
	for (size_t b = 1; b < num_and_levels; ++b) {
		row_name_prefix = _name + "_AND_" + to_string(b) + "_";

		for (size_t a = 0; a < num_ands_per_level; ++a) {
			row_name = row_name_prefix + to_string(a);

			auto and_gate = make_shared<And>(row_name);
			auto and_wire = make_shared<Wire>(row_name + "_O");
			and_gate->Connect(PORTS::O, and_wire);
			adders[b-1][a]->Connect(PORTS::B, and_wire);
			ands_row.push_back(and_gate);
		}

		ands.push_back(ands_row);
	}

	// Create the connections between Cout and Cin of the adders.
	for (size_t b = 0; b < num_adder_levels; ++b) {
		row_name_prefix = _name + "_Cout_" + to_string(b) + "_";
		const bool last_b = b == (num_adder_levels - 1);
		
		if (b == 0) {
			// First row has 2 HalfAdders so the connections
			// are slightly different.
			for (size_t a = 0; a < num_adders_per_level; ++a) {
				row_name = row_name_prefix + to_string(a);

				const bool second_to_last_a = a == (num_adders_per_level - 2);
				const bool last_a = a == (num_adders_per_level - 1);

				auto wire = make_shared<Wire>(row_name);
				adders[b][a]->Connect(PORTS::Cout, wire);

				if (last_a) {
					adders[b+1][a]->Connect(PORTS::A, wire);
				} else {
					if (second_to_last_a) {
						adders[b][a+1]->Connect(PORTS::A, wire);
					} else {
						adders[b][a+1]->Connect(PORTS::Cin, wire);
					}
				}
			}
		} else {
			// The rest of the rows all have the same connections.
			for (size_t a = 0; a < num_bits_A; ++a) {
				row_name = row_name_prefix + to_string(a);

				const bool last_a = a == (num_adders_per_level - 1);

				// The very last one FullAdder connects to the output, so only
				// connect the a wire here if it's not the last FullAdder.
				if (!(last_a && last_b)) {
					auto wire = make_shared<Wire>(row_name);
					adders[b][a]->Connect(PORTS::Cout, wire);

					if (last_a) {
						// The last FullAdder connects to the A input of the
						// adder of the next level instead of the Cin port.
						adders[b+1][a]->Connect(PORTS::A, wire);
					} else {
						adders[b][a+1]->Connect(PORTS::Cin, wire);
					}
				} 
			}
		}
	}
}

void Multiplier_Smag::Update(bool propagating) {
	
}

void Multiplier_Smag::Connect(PORTS port, wire_t wire, size_t index) {
	if (port == PORTS::A && index >= num_bits_A) {
		cout << "[Error] Index " << index << " of port A is out of "
			 << "bounds for Multiplier_Smag \"" << name << "\"n";
		exit(1);
	} else if (port == PORTS::B && index >= num_bits_B) {
		cout << "[Error] Index " << index << " of port B is out of "
			 << "bounds for Multiplier_Smag \"" << name << "\"n";
		exit(1);
	} else if (port == PORTS::O && index >= num_bits_O) {
		cout << "[Error] Index " << index << " of port O is out of "
			 << "bounds for Multiplier_Smag \"" << name << "\"n";
		exit(1);
	}

	switch(port) {
	case PORTS::A:
		if (index == (num_adders_per_level - 1)) {
			sign->Connect(PORTS::A, wire); break;
		} else {
			ands[0][index]->Connect(PORTS::A, wire); break;
		}
	case PORTS::B:
		if (index == (num_adders_per_level - 1)) {
			sign->Connect(PORTS::B, wire); break;
		} else {
			ands[0][index]->Connect(PORTS::B, wire); break;
		}
	case PORTS::O:
		if (index == (num_adders_per_level - 1)) {
			sign->Connect(PORTS::O, wire); break;
		} else {
			adders[num_adder_levels - 1][index]->Connect(PORTS::O, wire); break;
		}
	default:
		cout << "[Error] Trying to connect to undefined port of Multiplier_Smag "
			 << "\"" << name << "\"n";
		exit(1);
	}
}

size_t Multiplier_Smag::GetNumToggles() {
	return 0;
}

vector<wire_t> Multiplier_Smag::GetWires() {
	return vector<wire_t>(1, nullptr);
}

vector<wire_t> Multiplier_Smag::GetInputWires() {
	return vector<wire_t>(1, nullptr);
}

vector<wire_t> Multiplier_Smag::GetOutputWires() {
	return vector<wire_t>(1, nullptr);
}
