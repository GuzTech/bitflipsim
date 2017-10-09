#ifndef COMPONENT_H
#define COMPONENT_H

#include "main.h"

class Component : public std::enable_shared_from_this<Component> {
public:
	Component(std::string _name)
		: name(_name) {}
	Component(std::string _name, std::size_t _longest_path)
		: name(_name)
		, longest_path(_longest_path) {}
	virtual ~Component() = default;

	virtual void Update(bool propagating = true) =0;
	void MarkUpdate() {needs_update = true;}
	void Reset() {needs_update = false; toggle_count = 0;}

	std::string GetName() {return name;}
	virtual std::size_t GetNumToggles() {return toggle_count;}
	std::size_t GetLongestPath() {return longest_path;}
	virtual std::vector<wire_t> GetWires() =0;
	virtual std::vector<wire_t> GetInputWires() =0;
	virtual std::vector<wire_t> GetOutputWires() =0;
protected:
	template <typename Derived> std::shared_ptr<Derived> shared_from_base() {
		return std::static_pointer_cast<Derived>(shared_from_this());
	}

	bool needs_update = false;
	std::size_t toggle_count = 0;
	std::size_t longest_path = 1; // Default path length is 1.
	std::string name;
};

#endif // COMPONENT_H
