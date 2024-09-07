#include "roblox/classes/classes.hpp"
#include "utils/configs/configs.hpp"

#include <filesystem>
#include <iostream>
#include <thread>

int main(/* credits to AGC / A GREAT CHAOS / KEN CARSON / mogus (lua env) */)
{
	//std::string appdata = antagonist::utils::appdata_path();
	//if (!std::filesystem::exists(appdata + "\\antagonist"))
	//{
	//	std::filesystem::create_directory(appdata + "\\antagonist");
	//}

	//if (!std::filesystem::exists(appdata + "\\antagonist\\configs"))
	//{
	//	std::filesystem::create_directory(appdata + "\\antagonist\\configs");
	//}

	antagonist::roblox::init();

	printf("[debug]: press enter to close console [closes cheat]");
	std::cin.get();

	//system("pause");
}