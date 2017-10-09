#ifndef NAND_H
#define NAND_H

#include "main.h"

class Nand : public Component {
public:
	Nand(std::string _name)
		: Component(_name) {}
	~Nand() = default;

	enum class PORTS {A, B, O};

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire);

	std::vector<wire_t> GetWires() override;
	std::vector<wire_t> GetInputWires() override;
	std::vector<wire_t> GetOutputWires() override;

private:
	wire_t A;
	wire_t B;
	wire_t O;
};

#endif // NAND_H
