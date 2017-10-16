#ifndef RIPPLECARRYADDER_H
#define RIPPLECARRYADDER_H

#include "main.h"

class RippleCarryAdder : public Component {
public:
	RippleCarryAdder(string _name, size_t _num_bits);
	~RippleCarryAdder() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, size_t index) override;

	size_t GetNumToggles() final;
	vector<wire_t> GetWires() override;
	vector<wire_t> GetInputWires() override;
	vector<wire_t> GetOutputWires() override;
	wire_t GetWire(PORTS port, size_t index) override;

private:
	size_t num_bits = 0;

	vector<fa_t> full_adders;
};

#endif // RIPPLECARRYADDER_H
