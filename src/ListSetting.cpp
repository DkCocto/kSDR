#include "ListSetting.h"

ListSetting::ListSetting(Environment* env, map<int, string> list, const char* settingName, bool needToRestart) {
	this->env = env;
	this->list = list;
	this->config = env->getConfig();
	this->settingName = settingName;
	this->needToRestart = needToRestart;
}

uint32_t lastSelected = 0;

void ListSetting::bindVariable(int* var) {
	this->var = var;
	lastSelected = *var;
}

vector<const char*>* ListSetting::createItemsArray() {	
	map <int, string> ::iterator it = list.begin();
	
	vector<const char*>* tmp = new vector<const char*>;
	for (int i = 0; it != list.end(); i++, it++) {
		tmp->push_back(it->second.c_str());
	}
	return tmp;
}

void ListSetting::drawSetting() {
	if (var == nullptr) return;

	vector<const char*>* itemsArray = createItemsArray();

	const char** items = itemsArray->data();

	int item_current_idx = getSelectedSetting(); // Here we store our selection data as an index.

	const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
	string title = string(settingName);
	if (needToRestart) title.append(" (*)");
	if (ImGui::BeginCombo(title.c_str(), combo_preview_value, 0)) {
		for (int n = 0; n < list.size(); n++) {
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected)) {
				item_current_idx = n;
				uint32_t tmpVar;
				Utils::parse_u32((char*)items[n], &tmpVar);
				*var = tmpVar;
				if (lastSelected != tmpVar && needToRestart) {
					lastSelected = tmpVar;
					env->stopProcessing();
					env->makeReload();
				}
			}
			if (is_selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	delete itemsArray;
}

int ListSetting::getSelectedSetting() {
	map <int, string> ::iterator it = list.begin();
	for (int i = 0; it != list.end(); i++, it++) {
		if (it->second.compare(to_string(*var)) == 0) {
			//printf("%d\r\n", *var);
			return i;
		}
	}
	return 0;
}