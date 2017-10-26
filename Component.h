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
	void MarkUpdate() {needs_update = true;}
	void Reset() {needs_update = false; toggle_count = 0;}

	const string &GetName() const {return name;}
	const virtual size_t GetNumToggles() {return toggle_count;}
	const size_t GetLongestPath() const {return longest_path;}
	const virtual vector<wire_t> GetWires() const =0;
	const virtual vector<wire_t> GetInputWires() const =0;
	const vector<wire_t> &GetOutputWires() const {return output_wires;}
	const virtual wire_t GetWire(PORTS port, size_t index = 0) const =0;
protected:
	template <typename Derived> shared_ptr<Derived> shared_from_base() {
		return static_pointer_cast<Derived>(shared_from_this());
	}

	string name;
	bool needs_update = false;
	size_t toggle_count = 0;
	size_t longest_path = 1; // Default path length is 1.

	vector<wire_t> output_wires;
};

#endif // COMPONENT_H
