#ifndef HALFADDER_H
#define HALFADDER_H

#include "main.h"

class Wire;

class HalfAdder : public Component {
public:
	HalfAdder(std::string _name)
		: Component(_name) {}
	~HalfAdder() = default;

	enum class PORTS {A, B, S, C};

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire);

	std::vector<wire_t> GetWires() override;
	std::vector<wire_t> GetInputWires() override;
	std::vector<wire_t> GetOutputWires() override;

private:
	wire_t A;
	wire_t B;
	wire_t S;
	wire_t C;
};

#endif // HALFADDER_H
