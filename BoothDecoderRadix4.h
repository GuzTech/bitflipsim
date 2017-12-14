#ifndef BOOTH_DECODER_RADIX4
#define BOOTH_DECODER_RADIX4

#include "main.h"

class BoothDecoderRadix4 : public Component {
public:
	BoothDecoderRadix4(string _name);
	~BoothDecoderRadix4() = default;

	void Update(bool propagating) override;
	void Connect(PORTS port, const wire_t &wire, size_t index = 0) override;
	void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) override;

	const size_t GetNumToggles() final;
	const wire_t GetWire(PORTS port, size_t index) const override;

private:
	xnor_t yj_neg;
	xnor_t yj_m1_neg;
	or_t yj_x1b;
	or3_t yj_m1_z_x2b;
	nand_t ppt_j;

	wire_t yj_neg_o;      // Output of yj_neg.
	wire_t yj_m1_neg_o;   // Output of yj_m1_neg.
	wire_t yj_x1b_o;      // Output of yj_x1b.
	wire_t yj_m1_z_x2b_o; // Output of yj_m1_z_x2b.
};

#endif // BOOTH_DECODER_RADIX4
