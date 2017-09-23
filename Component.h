#ifndef COMPONENT_H
#define COMPONENT_H

#include "main.h"

class Component : public std::enable_shared_from_this<Component> {
public:
	Component() = default;
	Component(std::string _name)
		: name(_name) {}
	virtual ~Component() = default;

	virtual void Update() =0;
	void MarkUpdate() {needs_update = true;}
	void Reset() {needs_update = false; toggle_count = 0;}

	std::string GetName() {return name;}
	uint64_t GetNumToggles() {return toggle_count;}
	virtual std::vector<wire_t> GetWires() =0;
protected:
	template <typename Derived> std::shared_ptr<Derived> shared_from_base() {
		return std::static_pointer_cast<Derived>(shared_from_this());
	}

	bool needs_update = false;
	uint64_t toggle_count = 0;
	std::string name;
};

#endif // COMPONENT_H
