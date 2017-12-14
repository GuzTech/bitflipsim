#ifndef HALFADDER_H
#define HALFADDER_H

#include "main.h"

class HalfAdder : public Component {
public:
	HalfAdder(string _name);
	~HalfAdder() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index = 0) const override;

private:
	xor_t xor_ha = nullptr;
	and_t and_ha = nullptr;
};

#endif // HALFADDER_H
