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

	const wire_t GetWire(PORTS port, size_t index) const override;

private:
	void CheckIfIndexIsInRange(PORTS port, size_t index) const override;
	void Generate2CBoothHardware();

	size_t num_bits_A = 0;
	size_t num_bits_B = 0;
	size_t num_bits_O = 0;
	size_t num_encoders = 0;
	size_t num_decoders_per_row = 0;
	size_t num_ppt_adders = 0;
	size_t adder_size_level_0 = 0;
	size_t final_adder_size = 0;
	size_t sign_extend_B_input_size = 0;

	vector<b_enc_t> encoders;
	vector<b_r4d_t> decoders;
	vector<csa_t> cs_adders;
	rca_t final_adder;
	not_t se_not;
	wire_t se_not_o;
	wire_t hardcoded_1;
};

#endif // MULTIPLIER_2C_BOOTH_H
