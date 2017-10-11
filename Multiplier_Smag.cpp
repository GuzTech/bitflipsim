#include "main.h"

/*
  Signed magnitude multiplier implementation.
*/

Multiplier_Smag::Multiplier_Smag(std::string _name,
								 std::size_t _num_bits_A,
								 std::size_t _num_bits_B)
	: Component(_name, _num_bits_B)
	, num_bits_A(_num_bits_A)
	, num_bits_B(_num_bits_B)
{
	// Names for the adders.
	std::string row_name_prefix = std::string(_name) + "_S_0_";
	std::string row_name = row_name_prefix + "0";

	// First row consists of #A - 2 FullAdders and 2 HalfAdders.
	std::vector<comp_t> row;
	row.push_back(std::make_shared<HalfAdder>(row_name));
	for (std::size_t a = 1; a < num_bits_A - 1; ++a) {
		row_name = row_name_prefix;
		row_name += std::to_string(a);
		
		row.push_back(std::make_shared<FullAdder>(row_name));
	}
	row_name = row_name_prefix;
	row_name += std::to_string(num_bits_A - 1);
	row.push_back(std::make_shared<HalfAdder>(row_name));

	components.push_back(row);
	row.clear();

	// The rest of the rows consist of #A FullAdders and 1 HalfAdder.
	for (std::size_t b = 1; b < num_bits_B; ++b) {
		row_name_prefix = _name + "_S_" + std::to_string(b) + "_";
		row_name = row_name_prefix + "0";
		row.push_back(std::make_shared<HalfAdder>(row_name));
		
		for (std::size_t a = 1; a < num_bits_A; ++a) {
			row_name = row_name_prefix + std::to_string(a);
			row.push_back(std::make_shared<FullAdder>(row_name));
		}

		components.push_back(row);
		row.clear();
	}

	// Create the AND gates that connect to the inputs of the adders.
	for (std::size_t b = 0; b < num_bits_B; ++b) {
		row_name_prefix = _name + "_AND_" + std::to_string(b);

		for (std::size_t a = 0; a < num_bits_A; ++a) {
			row_name = row_name_prefix + std::to_string(a);
			row.push_back(std::make_shared<And>(row_name));
		}
	}
}

void Multiplier_Smag::Update(bool propagating) {
	
}

void Multiplier_Smag::Connect(PORTS port, wire_t wire, std::size_t index) {
	if (port == PORTS::A && index >= num_bits_A) {
		std::cout << "[Error] Index " << index << " of port A is out of "
				  << "bounds for Multiplier_Smag \"" << name << "\"n";
		exit(1);
	} else if (port == PORTS::B && index >= num_bits_B) {
		std::cout << "[Error] Index " << index << " of port B is out of "
				  << "bounds for Multiplier_Smag \"" << name << "\"n";
		exit(1);
	} else if (port == PORTS::O && index >= (num_bits_A + num_bits_B)) {
		std::cout << "[Error] Index " << index << " of port O is out of "
				  << "bounds for Multiplier_Smag \"" << name << "\"n";
		exit(1);
	}

	switch(port) {
	case PORTS::A: ands[0][index]->Connect(PORTS::A, wire); break;
	case PORTS::B: ands[0][index]->Connect(PORTS::B, wire); break;
	case PORTS::O: adders[num_bits_B - 1][index]->Connect(PORTS::O, wire); break;
	default:
		std::cout << "[Error] Trying to connect to undefined port of Multiplier_Smag "
				  << "\"" << name << "\"n";
		exit(1);
	}
}

std::size_t Multiplier_Smag::GetNumToggles() {
	return 0;
}

std::vector<wire_t> Multiplier_Smag::GetWires() {
	return std::vector<wire_t>(1, nullptr);
}

std::vector<wire_t> Multiplier_Smag::GetInputWires() {
	return std::vector<wire_t>(1, nullptr);
}

std::vector<wire_t> Multiplier_Smag::GetOutputWires() {
	return std::vector<wire_t>(1, nullptr);
}
