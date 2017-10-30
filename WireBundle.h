#ifndef WIREBUNDLE_H
#define WIREBUNDLE_H

#include "main.h"

class WireBundle : public enable_shared_from_this<WireBundle> {
public:
	enum class REPR {TWOS_COMPLEMENT, ONES_COMPLEMENT, SIGNED_MAGNITUDE};

	WireBundle(string _name, size_t _size, REPR _repr = REPR::TWOS_COMPLEMENT);
	~WireBundle() = default;

	const string &GetName() {return name;}
	const size_t GetSize() {return wires.size();}
	const size_t GetAllocatedSize() {return wires.capacity();}
	const vector<wire_t> &GetWires() {return wires;}
	const int64_t GetValue() const;

	const wire_t &operator [] (size_t i) const {return wires[i];}

	void Init();
	void SetValue(int64_t value, bool propagating = true);
private:
	string name;
	size_t size;
	vector<wire_t> wires;
	REPR repr;
};

#endif // WIREBUNDLE_H
