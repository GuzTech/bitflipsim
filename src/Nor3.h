#ifndef NOR3_H
#define NOR3_H

#include "main.h"

class Nor3 : public Component {
public:
	Nor3(string _name)
		: Component(_name) {}
	~Nor3() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index = 0) const override;

	void GenerateVHDLEntity() const override;
	void GenerateVHDLInstance() override;

private:
	wire_t A;
	wire_t B;
	wire_t C;
	wire_t O;

	// Used for generating HDL
	static bool entityGenerated;
};

#endif // NOR3_H
