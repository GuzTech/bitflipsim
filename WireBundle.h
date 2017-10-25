#ifndef WIREBUNDLE_H
#define WIREBUNDLE_H

#include "main.h"

class WireBundle : public enable_shared_from_this<WireBundle> {
public:
	WireBundle(string _name, size_t _size);
	~WireBundle() = default;

	const string &GetName() {return name;}
	const size_t GetSize() {return wires.size();}
	const size_t GetAllocatedSize() {return wires.capacity();}
	const vector<wire_t> &GetWires() {return wires;}

	const wire_t &operator [] (size_t i) const {return wires[i];}

	void Init();
	void SetValue(int value, bool propagating = true);
private:
	string name;
	size_t size;
	vector<wire_t> wires;
};

#endif // WIREBUNDLE_H
