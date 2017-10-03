#ifndef NOR_H
#define NOR_H

#include "main.h"

class Wire;

class Nor : public Component {
public:
	Nor(std::string _name)
		: Component(_name) {}
	~Nor() = default;

	enum class PORTS {A, B, O};

	void Update() override;
	void Connect(PORTS port, wire_t wire);

	std::vector<wire_t> GetWires() override;
	std::vector<wire_t> GetInputWires() override;
	std::vector<wire_t> GetOutputWires() override;

private:
	wire_t A;
	wire_t B;
	wire_t O;
};

#endif // NOR_H
