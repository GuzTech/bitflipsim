#ifndef SYSTEM_H
#define SYSTEM_H

#include "main.h"

class System {
public:

	void AddComponent(comp_t component);
	void FindLongestPathInSystem();
	void FindInitialState();
	void Update();

	size_t GetNumToggles();
	size_t GetNumComponents() {return components.size();}
	comp_t GetComponent(const string &comp_name);
	vector<comp_t> GetComponents();
	size_t GetNumWires() {return wires.size();}
	wire_t GetWire(const string &wire_name);
	wb_t GetWireBundle(const string &bundle_name);
	const vector<wire_t> &GetWires() {return all_wires;}
	const vector<wire_t> &GetInputWires() {return input_wires;}
	const vector<wire_t> &GetOutputWires() {return output_wires;}
	size_t GetLongestPath() {return longest_path;}
protected:

private:
	map<string, comp_t> components;
	map<string, wire_t> wires;
	map<string, wb_t> wire_bundles;

	vector<wire_t> all_wires;
	vector<wire_t> input_wires;
	vector<wire_t> output_wires;

	size_t longest_path = 0;
};

#endif // SYSTEM_H
