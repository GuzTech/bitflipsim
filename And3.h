#ifndef AND3_H
#define AND3_H

#include "main.h"

class And3 : public Component {
public:
	And3(string _name)
		: Component(_name) {}
	~And3() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index = 0) const override;

private:
	wire_t A;
	wire_t B;
	wire_t C;
	wire_t O;
};

#endif // AND3_H
