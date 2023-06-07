#pragma once
#include <string>
enum class BlockActorType;
class BlockActor {
public:
	static void setId(BlockActorType, const std::string&);
	static void initBlockEntities();
	static void shutdown();
};