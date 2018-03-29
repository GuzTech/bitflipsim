#ifndef RIPPLECARRYSUBTRACTER_H
#define RIPPLECARRYSUBTRACTER_H

#include "main.h"

class RippleCarrySubtracter : public Component {
public:
	RippleCarrySubtracter(string _name, size_t _num_bits);
	~RippleCarrySubtracter() = default;

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

	rca_t adder;
	vector<not_t> nots;
	wire_t fixed_one;

	static bool entityGenerated; // Used for generating HDL
};

#endif // RIPPLECARRYSUBTRACTER_H
