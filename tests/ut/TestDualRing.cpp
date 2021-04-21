//
// Created by Darwin Yuan on 2021/4/21.
//

#include <catch.hpp>
#include <cub/base/DualRing.h>

namespace {
    using namespace cub;

    struct Dummy {
        int a, b;
        auto operator==(Dummy const& rhs) const -> bool {
            return a == rhs.a && b == rhs.b;
        }

        auto operator!=(Dummy const& rhs) const -> bool {
            return !operator==(rhs);
        }
    };
}

SCENARIO("DualRing") {
    DualRing<Dummy, 3> ring;

    auto&& ring1 = ring.Ring1();
    auto&& ring2 = ring.Ring2();

    WHEN("push1") {
        REQUIRE_FALSE(ring.IsFull());
        REQUIRE(ring1.IsEmpty());
        REQUIRE(ring2.IsEmpty());

        REQUIRE(ring1.Push(1,2) == Result::SUCCESS);
        REQUIRE_FALSE(ring.IsFull());
        REQUIRE_FALSE(ring1.IsEmpty());
        REQUIRE(ring2.IsEmpty());

        REQUIRE(ring1.Push(2,3) == Result::SUCCESS);
        REQUIRE_FALSE(ring1.IsEmpty());
        REQUIRE_FALSE(ring.IsFull());
        REQUIRE(ring2.IsEmpty());

        REQUIRE(ring1.Push(3,4) == Result::SUCCESS);
        REQUIRE_FALSE(ring1.IsEmpty());
        REQUIRE(ring.IsFull());
        REQUIRE(ring2.IsEmpty());

        REQUIRE(ring1.Push(4,5) == Result::FAILED);

        REQUIRE(ring2.Push(4,5) == Result::FAILED);

        REQUIRE(ring1.GetSize() == 3);
        REQUIRE(ring2.GetSize() == 0);

        REQUIRE(ring2.Front() == nullptr);
        REQUIRE(*ring1.Front() == Dummy{1, 2});
        ring1.Pop();
        REQUIRE(*ring1.Front() == Dummy{2, 3});
        ring1.Pop();
        REQUIRE(*ring1.Front() == Dummy{3, 4});
        ring1.Pop();
        REQUIRE(ring1.Front() == nullptr);
    }

    WHEN("push2") {

        REQUIRE_FALSE(ring.IsFull());
        REQUIRE(ring1.IsEmpty());
        REQUIRE(ring2.IsEmpty());

        REQUIRE(ring2.Push(1,2) == Result::SUCCESS);
        REQUIRE_FALSE(ring.IsFull());
        REQUIRE_FALSE(ring2.IsEmpty());
        REQUIRE(ring1.IsEmpty());

        REQUIRE(ring2.Push(2,3) == Result::SUCCESS);
        REQUIRE_FALSE(ring.IsFull());
        REQUIRE_FALSE(ring2.IsEmpty());
        REQUIRE(ring1.IsEmpty());

        REQUIRE(ring2.Push(3,4) == Result::SUCCESS);
        REQUIRE(ring.IsFull());
        REQUIRE_FALSE(ring2.IsEmpty());
        REQUIRE(ring1.IsEmpty());

        REQUIRE(ring2.Push(4,5) == Result::FAILED);
        REQUIRE(ring1.Push(4,5) == Result::FAILED);

        REQUIRE(ring2.GetSize() == 3);
        REQUIRE(ring1.GetSize() == 0);

        REQUIRE(ring1.Front() == nullptr);
        REQUIRE(*ring2.Front() == Dummy{1, 2});
        ring2.Pop();
        REQUIRE(*ring2.Front() == Dummy{2, 3});
        ring2.Pop();
        REQUIRE(*ring2.Front() == Dummy{3, 4});
        ring2.Pop();
        REQUIRE(ring2.Front() == nullptr);
    }

    WHEN("push 1 & push2") {
        REQUIRE_FALSE(ring.IsFull());
        REQUIRE(ring1.IsEmpty());
        REQUIRE(ring2.IsEmpty());

        REQUIRE(ring1.Push(1,2) == Result::SUCCESS);
        REQUIRE_FALSE(ring.IsFull());
        REQUIRE_FALSE(ring1.IsEmpty());
        REQUIRE(ring2.IsEmpty());

        REQUIRE(ring2.Push(2,3) == Result::SUCCESS);
        REQUIRE_FALSE(ring.IsFull());
        REQUIRE_FALSE(ring1.IsEmpty());
        REQUIRE_FALSE(ring2.IsEmpty());

        REQUIRE(ring1.Push(3,4) == Result::SUCCESS);
        REQUIRE(ring.IsFull());
        REQUIRE_FALSE(ring1.IsEmpty());
        REQUIRE_FALSE(ring2.IsEmpty());

        WHEN("push again, should return FAILED") {
            REQUIRE(ring1.Push(4,5) == Result::FAILED);
            REQUIRE(ring2.Push(4,5) == Result::FAILED);
        }

        THEN("should be able to get size") {

            REQUIRE(ring2.GetSize() == 1);
            REQUIRE(ring1.GetSize() == 2);
        }

        THEN("should be able to read front & pop") {
            REQUIRE(*ring1.Front() == Dummy{1, 2});
            ring1.Pop();

            REQUIRE(ring2.GetSize() == 1);
            REQUIRE(ring1.GetSize() == 1);

            THEN("should be able to read front & pop more") {
                REQUIRE(*ring1.Front() == Dummy{3, 4});
                ring1.Pop();
                REQUIRE(ring1.Front() == nullptr);

                REQUIRE(*ring2.Front() == Dummy{2, 3});
                ring2.Pop();
                REQUIRE(ring2.Front() == nullptr);
            }

            THEN("after pop, should be able to push to ring1 again") {
                REQUIRE(ring1.Push(7, 8) == Result::SUCCESS);
                REQUIRE(ring1.Push(6, 8) == Result::FAILED);
                REQUIRE(ring2.Push(6, 8) == Result::FAILED);

                REQUIRE(ring2.GetSize() == 1);
                REQUIRE(ring1.GetSize() == 2);
            }

            THEN("after pop, should be able to push to ring2 again") {
                REQUIRE(ring2.Push(7, 8) == Result::SUCCESS);
                REQUIRE(ring2.Push(6, 8) == Result::FAILED);
                REQUIRE(ring1.Push(6, 8) == Result::FAILED);

                REQUIRE(ring2.GetSize() == 2);
                REQUIRE(ring1.GetSize() == 1);
            }
        }
    }
}