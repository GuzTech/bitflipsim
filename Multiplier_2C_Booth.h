#ifndef MULTIPLIER_2C_BOOTH_H
#define MULTIPLIER_2C_BOOTH_H

#include "main.h"

class Multiplier_2C_Booth : public Component {
public:
	Multiplier_2C_Booth(string _name,
						size_t _num_bits_A,
						size_t _num_bits_B);
	~Multiplier_2C_Booth() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const size_t GetNumToggles() final;
	const vector<wire_t> GetWires() const override;
	const vector<wire_t> GetInputWires() const override;
	const wire_t GetWire(PORTS port, size_t index) const override;

private:
	void Generate2CBoothHardware();

	size_t num_bits_A = 0;
	size_t num_bits_B = 0;
	size_t num_bits_O = 0;

	vector<wire_t> internal_wires;
};

#endif // MULTIPLIER_2C_BOOTH_H
