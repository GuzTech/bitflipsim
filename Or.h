#ifndef OR_H
#define OR_H

#include "main.h"

class Or : public Component {
public:
	Or(std::string _name)
		: Component(_name) {}
	~Or() = default;

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

#endif // OR_H
