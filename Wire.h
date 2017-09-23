#ifndef WIRE_H
#define WIRE_H

#include "main.h"

class Wire {
public:
	Wire() = default;
	Wire(std::string _name)
		: name(_name) {}
	~Wire() = default;

	const bool GetValue();
	const bool HasChanged();

	void SetValue(bool val);
	void SetDrives(comp_t component);

	std::string GetName() {return name;}
	uint64_t GetNumToggles() {return toggle_count;}

private:
	bool curr_value = false;
	bool has_changed = false;

	uint64_t toggle_count = 0;
	std::string name;

	std::weak_ptr<Component> drives;
};

#endif // WIRE_H
