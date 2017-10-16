#ifndef MULTIPLIER_2C_H
#define MULTIPLIER_2C_H

#include "main.h"

class Multiplier_2C : public Component {
public:
	Multiplier_2C(string _name,
				  size_t _num_bits_A,
				  size_t _num_bits_B);
	~Multiplier_2C() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, size_t index) override;

	size_t GetNumToggles() final;
	vector<wire_t> GetWires() override;
	vector<wire_t> GetInputWires() override;
	vector<wire_t> GetOutputWires() override;
	wire_t GetWire(PORTS port, size_t index) override;

private:
	size_t num_bits_A = 0;
	size_t num_bits_B = 0;

	vector<vector<comp_t>> adders;
	vector<vector<and_t>> ands;
};

#endif // MULTIPLIER_2C_H
