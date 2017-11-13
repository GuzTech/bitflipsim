#ifndef MULTIPLIER_2C_H
#define MULTIPLIER_2C_H

#include "main.h"

class Multiplier_2C : public Component {
public:
	enum class MUL_TYPE {ARRAY_SIGN_EXTEND, ARRAY_INVERSION};
	
	Multiplier_2C(string _name,
				  size_t _num_bits_A,
				  size_t _num_bits_B,
				  MUL_TYPE type = MUL_TYPE::ARRAY_SIGN_EXTEND);
	~Multiplier_2C() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const size_t GetNumToggles() final;
	const vector<wire_t> GetWires() const override;
	const vector<wire_t> GetInputWires() const override;
	const wire_t GetWire(PORTS port, size_t index) const override;

private:
	void GenerateSignExtendHardware();
	void GenerateInversionHardware();

	size_t num_bits_A = 0;
	size_t num_bits_B = 0;
	size_t num_bits_O = 0;
	size_t num_adder_levels = 0;
	size_t num_and_levels = 0;
	size_t num_adders_per_level = 0;
	size_t num_ands_per_level = 0;

	vector<vector<comp_t>> adders;
	vector<vector<and_t>> ands;
	vector<xor_t> input_2C_xors_A;
	vector<xor_t> input_2C_xors_B;
	vector<ha_t> input_2C_adders_A;
	vector<ha_t> input_2C_adders_B;
	vector<xor_t> output_2C_xors;
	vector<ha_t> output_2C_adders;
	xor_t output_2C_adder_xor = nullptr;
	xor_t different_sign = nullptr;
	vector<wire_t> internal_wires;

	MUL_TYPE type = MUL_TYPE::ARRAY_SIGN_EXTEND;
};

#endif // MULTIPLIER_2C_H
