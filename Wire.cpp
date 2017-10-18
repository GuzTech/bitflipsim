#include "main.h"

const bool Wire::GetValue() {
	return curr_value;
}

const bool Wire::HasChanged() {
	return has_changed;
}

void Wire::SetValue(bool val, bool propagating) {
	if (propagating) {
		has_changed = curr_value ^ val;
	} else {
		has_changed = prev_value ^ val;
		prev_value = val;
	}

	curr_value = val;

	if (has_changed) {
		if (!propagating) {
			toggle_count += num_outputs;
		}

		for (const auto &c : outputs) {
			if (const auto &spt = c.lock()) {
				spt->MarkUpdate();
			}
		}
	}
}

void Wire::AddOutput(comp_t component) {
	outputs.push_back(component);
	num_outputs = outputs.size();
}
