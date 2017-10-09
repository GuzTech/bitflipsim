#ifndef RIPPLECARRYADDER_H
#define RIPPLECARRYADDER_H

#include "main.h"

class Wire;

class RippleCarryAdder : public Component {
public:
	RippleCarryAdder(std::string _name, std::size_t _num_bits);
	~RippleCarryAdder() = default;

	enum class PORTS {A, B, Cin, S, Cout};

	void Update(bool propagating) override;
	void Connect(PORTS port, wire_t wire, std::size_t index);

	std::size_t GetNumToggles() final;
	std::vector<wire_t> GetWires() override;
	std::vector<wire_t> GetInputWires() override;
	std::vector<wire_t> GetOutputWires() override;

private:
	std::size_t num_bits = 0;

	std::vector<fa_t> full_adders;
};

#endif // RIPPLECARRYADDER_H
