#ifndef NAND_H
#define NAND_H

#include "main.h"

class Nand : public Component {
public:
	Nand(string _name)
		: Component(_name) {}
	~Nand() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, size_t index = 0) override;

	vector<wire_t> GetWires() override;
	vector<wire_t> GetInputWires() override;
	vector<wire_t> GetOutputWires() override;
	wire_t GetWire(PORTS port, size_t index = 0) override;

private:
	wire_t A;
	wire_t B;
	wire_t O;
};

#endif // NAND_H
