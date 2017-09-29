#ifndef SYSTEM_H
#define SYSTEM_H

#include "main.h"

class System {
public:

	void AddComponent(comp_t component);
	void Update();

	std::size_t GetNumToggles();
	std::size_t GetNumComponents() {return components.size();}
	std::size_t GetNumWires() {return wires.size();}
	wire_t      GetWire(std::string wire_name);
protected:

private:
	std::map<std::string, comp_t> components;
	std::map<std::string, wire_t> wires;
};

#endif // SYSTEM_H
