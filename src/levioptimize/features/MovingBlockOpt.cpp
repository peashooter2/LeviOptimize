#include "features.h"
#include "ll/api/memory/Hook.h"
#include "mc/network/packet/BlockActorDataPacket.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/actor/BlockActor.h"
#include "mc/world/level/block/actor/MovingBlockActor.h"

namespace lo::moving_block_opt {

thread_local bool updatePacketFlag = false;

LL_TYPE_INSTANCE_HOOK(
    BlockActorGetServerUpdatePacketHook,
    ll::memory::HookPriority::Normal,
    BlockActor,
    &BlockActor::getServerUpdatePacket,
    std::unique_ptr<BlockActorDataPacket>,
    BlockSource& bs
) {
    updatePacketFlag = true;
    auto rtn         = origin(bs);
    updatePacketFlag = false;
    return rtn;
}

LL_TYPE_INSTANCE_HOOK(
    MovingBlockActorSaveHook,
    ll::memory::HookPriority::Normal,
    MovingBlockActor,
    "?save@MovingBlockActor@@UEBA_NAEAVCompoundTag@@@Z",
    bool,
    CompoundTag& tag
) {
    if (!updatePacketFlag) {
        return origin(tag);
    }

    if (!::BlockActor::save(tag)) return false; // NOLINT

    Block* block            = ll::memory::dAccess<Block*>(this, 200);
    Block* extraBlock       = ll::memory::dAccess<Block*>(this, 208);
    tag["movingBlock"]      = block->getSerializationId();
    tag["movingBlockExtra"] = extraBlock->getSerializationId();

    BlockPos& pos     = ll::memory::dAccess<BlockPos>(this, 58 * sizeof(int));
    tag["pistonPosX"] = pos.x;
    tag["pistonPosY"] = pos.y;
    tag["pistonPosZ"] = pos.z;
    tag["expanding"]  = ll::memory::dAccess<bool>(this, 244);

    return true;
}

struct MovingBlockOpt::Impl {
    ll::memory::HookRegistrar<BlockActorGetServerUpdatePacketHook, MovingBlockActorSaveHook> r;
};

void MovingBlockOpt::call(bool enable) {
    if (enable) {
        if (!impl) impl = std::make_unique<Impl>();
    } else {
        impl.reset();
    }
}

MovingBlockOpt::MovingBlockOpt()  = default;
MovingBlockOpt::~MovingBlockOpt() = default;

} // namespace lo::moving_block_opt
