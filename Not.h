#ifndef NOT_H
#define NOT_H

#include "main.h"

class Not : public Component {
public:
	Not(string _name)
		: Component(_name) {
		needs_update = true;
	}
	~Not() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const vector<wire_t> GetWires() const override;
	const vector<wire_t> GetInputWires() const override;
	const wire_t GetWire(PORTS port, size_t index = 0) const override;

private:
	wire_t I;
	wire_t O;
};

#endif // NOT_H
