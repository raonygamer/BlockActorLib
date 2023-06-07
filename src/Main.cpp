#include "Zenova.h"
#include "generated/initcpp.h"
#include "com.raony/blockactorlib/CustomBlockActorRegistry.hpp"

MOD_FUNCTION void ModLoad(ModContext& ctx) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}
MOD_FUNCTION void ModStart() {
	Zenova::Log::Write(Zenova::Log::Severity::Info, "BlockActorLib - Core", "{}", fmt::format(fg(fmt::color::dark_sea_green), "Initializing BlockActorLibrary v{}.{}.{}", 1, 1, 0));
	Minecraft::CustomBlockActorRegistry::initHooks();
	if (Zenova::Minecraft::version() != Zenova::Minecraft::v1_14_60_5)
		Zenova::Log::Write(Zenova::Log::Severity::Warning, "BlockActorLib - Core", "{}", fmt::format(fg(fmt::color::orange_red), "Game version isn't supported, undefined behavior can happen."));
}