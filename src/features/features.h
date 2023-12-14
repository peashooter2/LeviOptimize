#include <memory>


namespace lo {

namespace moving_block_fix {
struct MovingBlockFix {
    struct Impl;
    std::unique_ptr<Impl> impl;

    void call(bool);
    MovingBlockFix();
    ~MovingBlockFix();
};
} // namespace moving_block_fix

namespace hopper_item_fix {
struct HopperItemFix {
    struct Impl;
    std::unique_ptr<Impl> impl;

    void call(bool);
    HopperItemFix();
    ~HopperItemFix();
};
} // namespace hopper_item_fix

} // namespace lo