#ifndef RIPPLECARRYADDER_H
#define RIPPLECARRYADDER_H

#include "main.h"

class RippleCarryAdder : public Component {
public:
	RippleCarryAdder(string _name, size_t _num_bits);
	~RippleCarryAdder() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index) const override;
	const PORT_DIR GetPortDirection(PORTS port) const override;

	void PrintDebug() const override;

	void GenerateVHDLEntity(const string &path) const override;
	const string GenerateVHDLInstance() const override;
private:
	size_t num_bits = 0;

	vector<fa_t> full_adders;

	// Used for generating HDL
	static bool entityGenerated;
};

#endif // RIPPLECARRYADDER_H
