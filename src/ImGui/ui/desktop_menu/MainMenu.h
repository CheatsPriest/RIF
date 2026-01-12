#pragma once
#include <ImGui/Window.hpp>
#include <nfd.hpp>
#include <string>
#include <configs/SearchConfig.hpp>
#include <list>
#include <core/Core.hpp>
#include <chrono>

class MainMenu {

private:
	std::string search_input;
    std::list<fs::path> inital_folders;
    SearchConfig& config;
    SynonymsSettings& syn_setting;
	SearchStats& stats;

	std::chrono::steady_clock::time_point start_point;
	std::chrono::steady_clock::time_point end_point;

	int depth;
	bool respect_registers;

	Core async_core;

    void foldersChose();
	void startSearch();
	void resizeThreadPool();
	
	void optionsMenu();
	void duringTheSearch();

	void drawProgressSearchBar();
public:
	MainMenu() : config(SearchConfig::get()), syn_setting(SynonymsSettings::get()), stats(SearchStats::get()), depth(0), respect_registers(false) {
		NFD_Init();
		start_point = std::chrono::steady_clock::now();
		end_point = std::chrono::steady_clock::now();
	}
	void draw();
	~MainMenu() {
		NFD_Quit();
	}
};