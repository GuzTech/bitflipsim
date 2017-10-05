#ifndef SYSTEM_H
#define SYSTEM_H

#include "main.h"

class System {
public:
	static System *Get();
	
	void AddComponent(comp_t component);
	void FindLongestPathInSystem();
	void Update();

	std::size_t         GetNumToggles();
	std::size_t         GetNumComponents() {return components.size();}
	std::size_t         GetNumWires() {return wires.size();}
	wire_t              GetWire(std::string wire_name);
	std::vector<wire_t> GetWires();
	std::vector<wire_t> GetInputWires() {return input_wires;}
	std::vector<wire_t> GetOutputWires() {return output_wires;}
	std::size_t         GetLongestPath() {return longest_path;}

private:
	System() {};
	System(System const&) {};
	System& operator = (System const&) {};

	static System *instance;
	
	std::map<std::string, comp_t> components;
	std::map<std::string, wire_t> wires;

	std::vector<wire_t> input_wires;
	std::vector<wire_t> output_wires;

	std::size_t longest_path = 0;
};

#endif // SYSTEM_H
