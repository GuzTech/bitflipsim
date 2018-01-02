#ifndef AND_H
#define AND_H

#include "main.h"

class And : public Component {
public:
	And(string _name)
		: Component(_name)
	{}
	~And() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const wire_t GetWire(PORTS port, size_t index = 0) const override;

	void GenerateVHDLEntity() const override;
	void GenerateVHDLInstance() override;

private:
	wire_t A;
	wire_t B;
	wire_t O;

	// Used for generating HDL
	static bool entityGenerated;
};

#endif // AND_H
