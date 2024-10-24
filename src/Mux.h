#ifndef MUX_H
#define MUX_H

#include "main.h"

class Mux : public Component {
public:
	Mux(string _name)
		: Component(_name) {}
	~Mux() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index = 0) const override;
	const PORT_DIR GetPortDirection(PORTS port) const override;

	void GenerateVHDLEntity(const string &path) const override;
	const string GenerateVHDLInstance() const override;

private:
	wire_t A;
	wire_t B;
	wire_t S;
	wire_t O;
};

#endif // MUX_H
