#include "main.h"

bool Wire::declarationGenerated = false;

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

		for (const auto &c : comp_outputs) {
			if (const auto &spt = c.lock()) {
				spt->MarkUpdate();
			}
		}

		for (const auto &w : wire_outputs) {
			w->SetValue(val, true);
		}
	}
}

void Wire::AddOutput(comp_t component) {
	comp_outputs.emplace_back(component);
	num_outputs = wire_outputs.size() + comp_outputs.size();
}

void Wire::AddOutput(wire_t wire) {
	wire_outputs.emplace_back(wire);
	num_outputs = wire_outputs.size() + comp_outputs.size();
}

void Wire::GenerateVHDLDeclaration() const {
	// Signal declarations only happen once.
	if (!declarationGenerated) {
		string output;
		TemplateDictionary decl("Wire");
		ExpandTemplate("src/templates/VHDL/Wire_decl.tpl", DO_NOT_STRIP, &decl, &output);
		cout << output;

		declarationGenerated = true;
	}
}

void Wire::GenerateVHDLIOAssignment() const {
	
}
