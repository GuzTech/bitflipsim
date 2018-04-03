#ifndef SYSTEM_H
#define SYSTEM_H

#include "main.h"

class System {
public:
	void AddComponent(comp_t component);
	void AddWireBundle(wb_t wires);
	void FindLongestPathInSystem();
	void FindInitialState();
	void Update();

	const size_t GetNumToggles() const;
	const size_t GetNumComponents() const {return components.size();}
	const comp_t GetComponent(const string &comp_name) const;
	const vector<comp_t> GetComponents() const;
	const size_t GetNumWires() const {return wires.size();}
	const wire_t GetWire(const string &wire_name) const;
	const wire_map_t &GetWires() const {return wires;}
	const vector<wire_t> &GetInputWires() const {return input_wires;}
	const vector<wire_t> &GetAllInputWires() const {return all_input_wires;}
	const vector<wire_t> &GetOutputWires() const {return output_wires;}
	const vector<wire_t> &GetAllOutputWires() const {return all_output_wires;}
	const wb_t GetWireBundle(const string &bundle_name) const;
	const wb_map_t &GetWireBundles() const {return wire_bundles;}
	const vector<wb_t> &GetInputWireBundles() const {return input_bundles;}
	const vector<wb_t> &GetOutputWireBundles() const {return output_bundles;}
	const size_t GetLongestPath() const {return longest_path;}

	const void GenerateVHDL(const string &config_filename, const string &path) const;
protected:

private:
	comp_map_t components;
	wire_map_t wires;
	wb_map_t   wire_bundles;

	vector<wire_t> input_wires; // Only wires that have been defined as input wires (not part of wire bundles).
	vector<wire_t> all_input_wires; // All input wires, including those of input wire bundles.
	vector<wire_t> output_wires; // Only wires that have been defined as output wires (not part of wire bundles).
	vector<wire_t> all_output_wires; // All output wires, including those of output wire bundles.

	vector<wb_t> input_bundles;
	vector<wb_t> output_bundles;
	vector<wb_t> internal_bundles;

	size_t longest_path = 0;
};

#endif // SYSTEM_H
