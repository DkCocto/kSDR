#pragma once

#include "Config.h"
#include "ViewModel.h"
#include "Spectre.h"
#include "string"

using namespace std;

class MemoryRecordUserInterface {

	Config* config = nullptr;
	ViewModel* viewModel = nullptr;
	Spectre* spectre = nullptr;

	int selectedRecordIndex = 0;

	std::string deleteDialogTitle = "Delete record";
	std::string deleteButtonTitle = "Del";

	std::string editDialogTitle = "Edit record";
	std::string editButtonTitle = "Edit";

	std::string storeRecordDialogTitle = "Store record";

	void initEditMemRecDialog(std::string id);
	void initDeleteMemRecDialog(std::string id);
	void initStoreMemRecDialog(std::string title);
public:

	MemoryRecordUserInterface() {};
	MemoryRecordUserInterface(Config* config, ViewModel* viewModel, Spectre* spectre);

	void drawMemoryBlock(ReceiverLogic* receiverLogic);
};