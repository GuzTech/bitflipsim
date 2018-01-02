#ifndef WIREBUNDLE_H
#define WIREBUNDLE_H

#include "main.h"

class WireBundle : public enable_shared_from_this<WireBundle> {
public:
	enum class REPR {TWOS_COMPLEMENT, ONES_COMPLEMENT, SIGNED_MAGNITUDE};

	WireBundle(string _name, size_t _size, REPR _repr = REPR::TWOS_COMPLEMENT);
	~WireBundle() = default;

	const string &GetName() const {return name;}
	const size_t GetSize() const {return wires.size();}
	const size_t GetAllocatedSize() const {return wires.capacity();}
	const vector<wire_t> &GetWires() const {return wires;}
	const int64_t GetValue() const;
	const int64_t Get2CValue() const;
	const REPR GetRepresentation() const {return repr;};
	const bool IsInputBundle() const {return is_input_bundle;}
	const bool IsOutputBundle() const {return is_output_bundle;}

	const wire_t &operator [] (size_t i) const {return wires[i];}
	const optional<size_t> GetWireIndex(const wire_t &wire) const;

	void Init();
	void SetValue(int64_t value, bool propagating = true);
	void SetRepresentation(REPR _repr) {repr = _repr;}
	void SetAsInputBundle() {is_input_bundle = true;}
	void SetAsOutputBundle() {is_output_bundle = true;}
private:
	string name;
	size_t size;
	vector<wire_t> wires;
	REPR repr; // Number representation format.
	bool is_input_bundle = false;
	bool is_output_bundle = false;
};

#endif // WIREBUNDLE_H
