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
	std::map<std::string, comp_t> components;
	std::map<std::string, wire_t> wires;
};

#endif // SYSTEM_H
