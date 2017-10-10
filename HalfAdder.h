#ifndef HALFADDER_H
#define HALFADDER_H

#include "main.h"

class HalfAdder : public Component {
public:
	HalfAdder(std::string _name)
		: Component(_name) {}
	~HalfAdder() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, std::size_t index = 0) override;

	std::vector<wire_t> GetWires() override;
	std::vector<wire_t> GetInputWires() override;
	std::vector<wire_t> GetOutputWires() override;

private:
	wire_t A;
	wire_t B;
	wire_t S;
	wire_t Cout;
};

#endif // HALFADDER_H
