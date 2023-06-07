#pragma once
#include "Zenova.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <fmt/color.h>
#include "mcpe/ActorBlock.h"
#include "mcpe/BlockActor.h"
#include "mcpe/BlockActorFactory.h"

enum class BlockActorType;

namespace Minecraft {
	class CustomBlockActorDefinitionBase {
	public:
		virtual void setId() = 0;
	};

	class CustomBlockActorDefinition :
		public CustomBlockActorDefinitionBase {
		std::string mIdentifier;
	public:
		CustomBlockActorDefinition(const std::string& identifier) :
			mIdentifier(identifier) {}

		virtual void setId() override;
	};


	class CustomBlockActorRegistry {
		friend class CustomBlockActorDefinitionBase;
		friend class CustomBlockActorDefinition;
	private:
		static std::shared_ptr<BlockActor>(*_createBlockEntity)(BlockActorType, const BlockPos&, const BlockLegacy&);
		static std::shared_ptr<BlockActor> createBlockEntity(BlockActorType type, const BlockPos& pos, const BlockLegacy& block);

		static void (*_initBlockEntities)();
		static void initBlockEntities();

		static void (*_shutdown)();
		static void shutdown();

		inline static std::unordered_map<std::string, CustomBlockActorDefinitionBase*> mCustomBlockActorsDefBase;
		inline static std::vector<int> sOccupiedBlockActorId = { 44 }; //workaround for BlockActorType, 44 is the last of the BlockActorType + 1

		inline static bool hooksInitialized = false;
	public:
		inline static std::unordered_map<std::string, int> mBlockActorIdentifierToId; //workaround for BlockActor::setId
		inline static std::unordered_map<int, std::string> mBlockActorIdToIdentifier; //workaround for BlockActor::setId

		static void initHooks() {
			if (hooksInitialized) return;
			hooksInitialized = true;
			Zenova_Hook(BlockActor::initBlockEntities, &initBlockEntities, &_initBlockEntities);
			Zenova_Hook(BlockActorFactory::createBlockEntity, &createBlockEntity, &_createBlockEntity);
		}

		__declspec(dllexport) static int getNextBlockActorId() {
			return sOccupiedBlockActorId[sOccupiedBlockActorId.size() - 1] + 1;
		}

		__declspec(dllexport) static int registerBlockActorWithIdentifier(const std::string& identifier, const std::string& modName = "Unknown") {
			int nextId = getNextBlockActorId();
			mBlockActorIdentifierToId[identifier] = nextId;
			mBlockActorIdToIdentifier[nextId] = identifier;
			sOccupiedBlockActorId.push_back(nextId);
			mCustomBlockActorsDefBase[identifier] = new CustomBlockActorDefinition(identifier);
			Zenova::Log::Write(Zenova::Log::Severity::Info, "BlockActorLib - Core", "{}", fmt::format(fg(fmt::color::aquamarine), "Mod \"{}\" registered a block entity named \"{}\" with TypeID of: {}", modName, identifier, nextId));
			return nextId;
		}

		__declspec(dllexport) static int getIdFromIdentifier(const std::string& identifier) {
			auto& iterator = mBlockActorIdentifierToId.find(identifier);
			if (iterator != mBlockActorIdentifierToId.end())
				return iterator->second;
			else
				return -1;
		}

		__declspec(dllexport) static BlockActorType getTypeIdFromIdentifier(const std::string& identifier) {
			return (BlockActorType)getIdFromIdentifier(identifier);
		}
	};

	void (*CustomBlockActorRegistry::_initBlockEntities)() = nullptr;
	void CustomBlockActorRegistry::initBlockEntities() {
		_initBlockEntities();
		for (auto& blockActordef : mCustomBlockActorsDefBase)
			blockActordef.second->setId();
	}

	void (*CustomBlockActorRegistry::_shutdown)() = nullptr;
	void CustomBlockActorRegistry::shutdown() {
		_shutdown();
	}

	std::shared_ptr<BlockActor>(*CustomBlockActorRegistry::_createBlockEntity)(BlockActorType, const BlockPos&, const BlockLegacy&) = nullptr;
	std::shared_ptr<BlockActor> CustomBlockActorRegistry::createBlockEntity(BlockActorType type, const BlockPos& pos, const BlockLegacy& block) {
		if (mBlockActorIdToIdentifier.find((int)type) != mBlockActorIdToIdentifier.end()) {
			return ((ActorBlock&)block).newBlockEntity(pos);
		}
		else return _createBlockEntity(type, pos, block);;
	}

	void CustomBlockActorDefinition::setId() {
		BlockActor::setId((BlockActorType)Minecraft::CustomBlockActorRegistry::mBlockActorIdentifierToId[mIdentifier], mIdentifier);
	}
};

