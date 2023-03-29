#pragma once

#include "Environment.h"
#include "map"
#include "imgui/imgui.h"
#include "Utils.h"
#include "vector"

template<typename T = int>
class ListSetting {

	private:
		map<T, string> list;
		Config* config;
		const char* settingName;
		bool needToRestart;
		T* var;

		Environment* env;

		T lastSelected;

	public:

		ListSetting(Environment* env, map<T, string> list, const char* settingName, bool needToRestart) {
			this->env = env;
			this->list = list;
			this->config = env->getConfig();
			this->settingName = settingName;
			this->needToRestart = needToRestart;
		}

		void bindVariable(T* var) {
			this->var = var;
			lastSelected = *var;
		}

		vector<const char*>* createItemsArray() {
			auto it = list.begin();

			vector<const char*>* tmp = new vector<const char*>;
			for (int i = 0; it != list.end(); i++, it++) {
				tmp->push_back(it->second.c_str());
			}
			return tmp;
		}

		void drawSetting() {
			if (this == nullptr) return;

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
						//T tmpVar;
						//Utils::parse_u32((char*)items[n], &tmpVar);
						auto it = list.begin();
						std::advance(it, item_current_idx);
						*var = it->first;
						if (lastSelected != *var && needToRestart) {
							lastSelected = *var;
							env->stopProcessing();
							env->makeReinit();
						}
					}
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			delete itemsArray;
		}

		int getSelectedSetting() {
			auto it = list.begin();
			for (int i = 0; it != list.end(); i++, it++) {
				if (it->first == *var) {
					return i;
				}
			}
			return 0;
		}
};