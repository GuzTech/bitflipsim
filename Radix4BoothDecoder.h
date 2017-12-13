#ifndef RADIX4_BOOTH_DECODER
#define RADIX4_BOOTH_DECODER

#include "main.h"

class Radix4BoothDecoder : public Component {
public:
	Radix4BoothDecoder(string _name,
					   size_t _num_bits);
	~Radix4BoothDecoder() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const vector<wire_t> GetWires() const override;
	const vector<wire_t> GetInputWires() const override;
	const wire_t GetWire(PORTS port, size_t index) const override;

private:
	void GenerateDecoderHardware();

	size_t num_bits = 0;
	size_t num_bits_O = 0;

	vector<xnor_t> yj_neg;
	vector<or_t> yj_x1b;
	vector<or3_t> yj_m1_z_x2b;
	vector<nand_t> ppt_j;

	vector<wire_t> internal_wires;
};

#endif // RADIX4_BOOTH_DECODER
