#include "main.h"

const bool Wire::GetValue() {
	return curr_value;
}

const bool Wire::HasChanged() {
	return has_changed;
}

void Wire::SetValue(bool val) {
	has_changed = curr_value ^ val;
	curr_value = val;

	if (has_changed) {
		toggle_count++;

		if (auto spt = drives.lock()) {
			spt->MarkUpdate();
		}
	}
}

void Wire::SetDrives(comp_t component) {
	drives = component;
}
