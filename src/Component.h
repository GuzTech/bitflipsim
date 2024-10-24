#ifndef COMPONENT_H
#define COMPONENT_H

#include "main.h"

class Component : public enable_shared_from_this<Component> {
public:
	Component(string _name)
		: name(_name) {}
	Component(string _name, size_t _longest_path)
		: name(_name)
		, longest_path(_longest_path) {}
	virtual ~Component() = default;

	virtual void Update(bool propagating = true) =0;
	virtual void Connect(PORTS port, const wire_t &wire, size_t index = 0) =0;
	virtual void Connect(PORTS port, const wb_t &wires, size_t port_idx = 0, size_t wire_idx = 0) =0;
	virtual void Connect(PORTS port, const wb_t &wires, size_t port_begin_idx, size_t port_end_idx, size_t wire_begin_idx) {};
	void MarkUpdate() {needs_update = true;}
	void Reset() {needs_update = false;}

	const string &GetName() const {return name;}
	const size_t GetLongestPath() const {return longest_path;}
	const virtual vector<wire_t> GetWires() const;
	const virtual vector<wire_t> GetInputWires() const {return input_wires;}
	const vector<wire_t> &GetInternalWires() const {return internal_wires;}
	const vector<wire_t> &GetOutputWires() const {return output_wires;}
	const virtual wire_t GetWire(PORTS port, size_t index = 0) const =0;
	const virtual PORT_DIR GetPortDirection(PORTS port) const =0;

	void PrintDebugAfterUpdate(bool value) {print_debug = value;}
	virtual void PrintDebug() const {};

	virtual void GenerateVHDLEntity(const string &path) const {};
	const virtual string GenerateVHDLInstance() const =0;
protected:
	template <typename Derived> shared_ptr<Derived> shared_from_base() {
		return static_pointer_cast<Derived>(shared_from_this());
	}

	virtual void CheckIfIndexIsInRange(PORTS port, size_t index) const {return;}
	void GenerateAssignments(const PORTS port,
							 const size_t port_width,
							 const string &signal_name,
							 TemplateDictionary &inst,
							 const bool last_port = false) const;

	string name;
	bool needs_update = false;
	size_t longest_path = 1; // Default path length is 1.

	bool print_debug = false;

	vector<wire_t> input_wires;
	vector<wire_t> internal_wires;
	vector<wire_t> output_wires;
};

#endif // COMPONENT_H
