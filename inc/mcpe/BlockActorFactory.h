#pragma once

#include <memory>

enum class BlockActorType;

class BlockActor;
class BlockLegacy;
class BlockPos;

class BlockActorFactory {
public:
    static std::shared_ptr<BlockActor> createBlockEntity(BlockActorType, const BlockPos&, const BlockLegacy&);
};