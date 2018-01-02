#ifndef CARRY_SAVE_ADDER
#define CARRY_SAVE_ADDER

#include "main.h"

class CarrySaveAdder : public Component {
public:
	CarrySaveAdder(string _name,
				   size_t _num_bits);
	~CarrySaveAdder() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index) const override;

	void PrintDebug() const override;

	void GenerateVHDLEntity() const override;
	void GenerateVHDLInstance() override;
private:
	void CheckIfIndexIsInRange(PORTS port, size_t index) const override;
	size_t num_bits = 0;

	vector<fa_t> full_adders;

	// Used for generating HDL
	static bool entityGenerated;
};

#endif // CARRY_SAVE_ADDER
