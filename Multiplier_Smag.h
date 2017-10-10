#ifndef MULTIPLIER_SMAG_H
#define MULTIPLIER_SMAG_H

#include "main.h"

class Multiplier_Smag : public Component {
public:
	Multiplier_Smag(std::string _name,
					std::size_t _num_bits_A,
					std::size_t _num_bits_B);
	~Multiplier_Smag() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, std::size_t index) override;

	std::size_t GetNumToggles() final;
	std::vector<wire_t> GetWires() override;
	std::vector<wire_t> GetInputWires() override;
	std::vector<wire_t> GetOutputWires() override;

private:
	std::size_t num_bits_A = 0;
	std::size_t num_bits_B = 0;

	std::vector<std::vector<comp_t>> components;
};

#endif // MULTIPLIER_SMAG_H
