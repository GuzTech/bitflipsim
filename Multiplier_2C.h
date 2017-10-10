#ifndef MULTIPLIER_2C_H
#define MULTIPLIER_2C_H

#include "main.h"

class Multiplier_2C : public Component {
public:
	Multiplier_2C(std::string _name,
				  std::size_t _num_bits_A,
				  std::size_t _num_bits_B);
	~Multiplier_2C() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, std::size_t index) override;

	std::size_t GetNumToggles() final;
	std::vector<wire_t> GetWires() override;
	std::vector<wire_t> GetInputWires() override;
	std::vector<wire_t> GetOutputWires() override;

private:
	std::size_t num_bits_A = 0;
	std::size_t num_bits_B = 0;

	std::vector<std::vector<comp_t>> adders;
	std::vector<std::vector<and_t>> ands;
};

#endif // MULTIPLIER_2C_H
