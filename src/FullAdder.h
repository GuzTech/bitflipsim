#ifndef FULLADDER_H
#define FULLADDER_H

#include "main.h"

class FullAdder : public Component {
public:
	FullAdder(string _name);
	~FullAdder() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index = 0) const override;
	const PORT_DIR GetPortDirection(PORTS port) const override;

	void GenerateVHDLEntity(const string &path) const override;
	const string GenerateVHDLInstance() const override;

private:
	xor_t xor_ab = nullptr;
	xor_t xor_cin = nullptr;
	and_t and_cin = nullptr;
	and_t and_ab = nullptr;
	or_t or_cout = nullptr;

	wire_t iw_1 = nullptr;
	wire_t iw_2 = nullptr;
	wire_t iw_3 = nullptr;

	static bool entityGenerated; // Used for HDL generation.
};

#endif // FULLADDER_H
