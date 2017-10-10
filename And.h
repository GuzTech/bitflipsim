#ifndef AND_H
#define AND_H

#include "main.h"

class And : public Component {
public:
	And(std::string _name)
		: Component(_name) {}
	~And() = default;

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

#endif // AND_H
