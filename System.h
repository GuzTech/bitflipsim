#ifndef SYSTEM_H
#define SYSTEM_H

#include "main.h"

class System {
public:

	void AddComponent(comp_t component);
	void Update();

	uint64_t GetNumToggles();
	int64_t  GetNumComponents() {return components.size();}
	int64_t  GetNumWires() {return wires.size();}
protected:

private:
	std::vector<comp_t> components;
	std::vector<wire_t> wires;
};

#endif // SYSTEM_H
