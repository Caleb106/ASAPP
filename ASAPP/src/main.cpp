#pragma once
#include "game/controls.h"
#include "game/globals.h"
#include "game/resources.h"
#include "game/settings.h"
#include "game/window.h"
#include "interfaces/localinventory.h"
#include "items/items.h"

#include "interfaces/basetravelmap.h"

int main()
{

	asa::globals::gameBaseDirectory = std::filesystem::path(
		"F:\\SteamLibrary\\steamapps\\common\\ARK Survival Ascended");

	asa::window::GetHandle(60, true);
	// asa::window::SetForeground();
	asa::settings::LoadGameUserSettings();
	asa::settings::LoadActionMappings();

	asa::resources::assetsDir = std::filesystem::path(
		"C:\\dev\\ASAPP\\ASAPP\\src\\assets");
	asa::resources::Init();

	asa::items::Init();

	auto map = asa::interfaces::BaseTravelMap();


	map.SelectResult(0);


	return 0;
}