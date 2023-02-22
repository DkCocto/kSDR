#pragma once

#include "string"
#include "map"
#include "imgui/imgui.h"
#include "Config.h"
#include "Utils.h"
#include "vector"
#include "iostream"
#include <functional>

using namespace std;

class ListSetting {

	private:
		map<int, string> list;
		Config* config;
		const char* settingName;
		bool needToRestart;
		int* var;

		function<void(int)> l;

	public:

		ListSetting(Config* config, map<int, string> list, const char* settingName, bool needToRestart);
		void bindVariable(int* var);
		vector<const char*>* createItemsArray();
		void drawSetting();
		int getSelectedSetting();
};