#include "MemoryRecordUserInterface.h"

MemoryRecordUserInterface::MemoryRecordUserInterface(Config* config, ViewModel* viewModel, Spectre* spectre) {
	this->config = config;
	this->viewModel = viewModel;
	this->spectre = spectre;
}

Config::MemoryRecord recordToCommit;

void MemoryRecordUserInterface::drawMemoryBlock(ReceiverLogic* receiverLogic) {

	if (ImGui::Button("Store frequency")) {
		spectre->disableControlForID(DISABLE_CONTROL_STORE_FREQ);
		recordToCommit = Config::MemoryRecord {
			Utils::getPrettyFreq(((int)receiverLogic->getSelectedFreqNew())),
			(float)receiverLogic->getSelectedFreqNew(),
			viewModel->receiverMode,
			viewModel->filterWidth
		};
		ImGui::OpenPopup(storeRecordDialogTitle.c_str());
	}

	initStoreMemRecDialog(storeRecordDialogTitle);

	//ImGuiTableFlags flags = ;

	auto& memoryVector = config->memoryVector;

	ImGui::Spacing();

	if (ImGui::BeginTable("Memory Records Table", 2, NULL)) {
		// The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
		//ImGui::TableSetupColumn("Name");
		//ImGui::TableSetupColumn("Controls");
		//ImGui::TableHeadersRow();
		for (int row = 0; row < memoryVector.size(); row++) {
			ImGui::PushID(row);
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);

			ImGui::Text(string(to_string(row) + ". ").c_str());
			ImGui::SameLine();

			//Colorize button if memory item was selected
			if ((int)receiverLogic->getSelectedFreqNew() == (int)memoryVector[row].freq) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::ColorConvertU32ToFloat4(config->colorTheme.memoryItemSelectedColor));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorConvertU32ToFloat4(config->colorTheme.memoryItemSelectedColor));
			}

			if (ImGui::Button(memoryVector[row].desc.c_str())) {
				spectre->executeMemoryRecord(memoryVector[row], receiverLogic);
			}

			//Uncolorize button if memory item was not selected
			if ((int)receiverLogic->getSelectedFreqNew() == (int)memoryVector[row].freq) {
				ImGui::PopStyleColor(); ImGui::PopStyleColor();
			}

			string toolTipText("Freq: ");
			toolTipText.append(Utils::getPrettyFreq((int)memoryVector[row].freq)).append(" Hz");

			if (ImGui::IsItemHovered()) ImGui::SetTooltip(toolTipText.c_str());

			ImGui::TableSetColumnIndex(1);
			// + to_string(row)
			string dynamicEditTitle = editDialogTitle;
			if (ImGui::Button((editButtonTitle).c_str())) {
				spectre->disableControlForID(DISABLE_CONTROL_EDIT_FREQ);
				selectedRecordIndex = row;
				recordToCommit = config->memoryVector[selectedRecordIndex];
				ImGui::OpenPopup(dynamicEditTitle.c_str());
			}

			initEditMemRecDialog(dynamicEditTitle);

			ImGui::SameLine();
			string dynamicDelTitle = deleteDialogTitle;
			if (ImGui::Button((deleteButtonTitle).c_str())) {
				spectre->disableControlForID(DISABLE_CONTROL_DEL_FREQ);
				selectedRecordIndex = row;
				ImGui::OpenPopup(dynamicDelTitle.c_str());
			}

			initDeleteMemRecDialog(dynamicDelTitle);

			ImGui::Spacing();
			ImGui::PopID();
		}
		ImGui::EndTable();
	}
}

static int item_current_idx = 0;

void MemoryRecordUserInterface::initEditMemRecDialog(std::string id) {
	if (ImGui::BeginPopupModal(id.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Please, set new values for the memory record.\n\n");

		auto& record = config->memoryVector[selectedRecordIndex];

		ImGui::Text(string("Current description: ").append(record.desc).append("\n").c_str());
		
		ImGui::InputText("New desc", &recordToCommit.desc);
		ImGui::InputFloat("Frequency", &recordToCommit.freq, 500.0f, 1000.0f, "%.0f");

		// Using the generic BeginCombo() API, you have full control over how to display the combo contents.
		// (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
		// stored in the object itself, etc.)
		const char* items[] = { "USB", "LSB", "AM", "nFM" };
		item_current_idx = recordToCommit.modulation; // Here we store our selection data as an index.
		const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
		if (ImGui::BeginCombo("Modulation", combo_preview_value)) {
			for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected)) {
					recordToCommit.modulation = n;
					item_current_idx = n;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::SliderInt("Filter width", &recordToCommit.filterWidth, 0, 12000);

		ImGui::Spacing(); ImGui::Spacing();

		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			spectre->enableControlForID(DISABLE_CONTROL_EDIT_FREQ);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(120, 0))) {
			config->editRecord(recordToCommit, selectedRecordIndex);
			spectre->enableControlForID(DISABLE_CONTROL_EDIT_FREQ);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}

void MemoryRecordUserInterface::initDeleteMemRecDialog(std::string id) {
	if (ImGui::BeginPopupModal(id.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Do you want to delete the selected record?\n\n");
		
		auto& record = config->memoryVector[selectedRecordIndex];

		ImGui::Text(string("Description: ").append(record.desc).append("\n").c_str());
		ImGui::Text(string("Frequency: ").append(Utils::getPrettyFreq((int)record.freq)).append("\n\n").c_str());

		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			spectre->enableControlForID(DISABLE_CONTROL_DEL_FREQ);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Yes", ImVec2(120, 0))) {
			config->deleteRecord(selectedRecordIndex);
			spectre->enableControlForID(DISABLE_CONTROL_DEL_FREQ);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}

void MemoryRecordUserInterface::initStoreMemRecDialog(std::string id) {
	if (ImGui::BeginPopupModal(id.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Please, enter name to store the current frequency:\n\n");

		ImGui::InputText("Description", &recordToCommit.desc);
		ImGui::InputFloat("Frequency", &recordToCommit.freq, 500.0f, 1000.0f, "%.0f");

		ImGui::Spacing(); ImGui::Spacing();

		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			spectre->enableControlForID(DISABLE_CONTROL_STORE_FREQ);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Store", ImVec2(120, 0))) {
			config->storeRecord(recordToCommit);
			spectre->enableControlForID(DISABLE_CONTROL_STORE_FREQ);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}
