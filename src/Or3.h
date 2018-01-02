#ifndef OR3_H
#define OR3_H

#include "main.h"

class Or3 : public Component {
public:
	Or3(string _name)
		: Component(_name) {}
	~Or3() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index = 0) const override;

	void GenerateVHDLEntity(const string &path) const override;
	const string GenerateVHDLInstance() const override;

private:
	wire_t A;
	wire_t B;
	wire_t C;
	wire_t O;

	// Used for generating HDL
	static bool entityGenerated;
};

#endif // OR3_H
