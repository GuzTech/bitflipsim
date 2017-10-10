#ifndef XNOR_H
#define XNOR_H

#include "main.h"

class Xnor : public Component {
public:
	Xnor(std::string _name)
		: Component(_name) {}
	~Xnor() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, std::size_t index = 0) override;

	std::vector<wire_t> GetWires() override;
	std::vector<wire_t> GetInputWires() override;
	std::vector<wire_t> GetOutputWires() override;

private:
	wire_t A;
	wire_t B;
	wire_t O;
};

#endif // XNOR_H
