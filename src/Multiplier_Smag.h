#ifndef MULTIPLIER_SMAG_H
#define MULTIPLIER_SMAG_H

#include "main.h"

class Multiplier_Smag : public Component {
public:
	enum class MUL_TYPE {CARRY_PROPAGATE, CARRY_SAVE};
	
	Multiplier_Smag(string _name,
					size_t _num_bits_A,
					size_t _num_bits_B,
					MUL_TYPE type = MUL_TYPE::CARRY_PROPAGATE);
	~Multiplier_Smag() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index = 0) const override;

	void GenerateVHDLEntity(const string &path) const override;
	const string GenerateVHDLInstance() const override;

private:
	void CheckIfIndexIsInRange(PORTS port, size_t index) const override;
	void GenerateCarryPropagateArrayHardware();
	void GenerateCarrySaveArrayHardware();
	void gen2();

	size_t num_bits_A = 0;
	size_t num_bits_B = 0;
	size_t num_bits_O = 0;
	size_t num_adder_levels = 0;
	size_t num_and_levels = 0;
	size_t num_adders_per_level = 0;
	size_t num_ands_per_level = 0;

	vector<csa_t> cs_adders;
	vector<rca_t> rc_adders;

	vector<vector<comp_t>> adders;
	vector<vector<and_t>> ands;
	xor_t sign = nullptr;

	MUL_TYPE type = MUL_TYPE::CARRY_PROPAGATE;

	static bool entityGenerated; // Used for HDL generation.
};

#endif // MULTIPLIER_SMAG_H
