//
// Created by godsme on 2021/3/9.
//

#include <catch.hpp>
#include <cub/dci/ObjectRoles.h>

namespace {
    struct Role1 {
        auto get() const -> int {
            return 1;
        }
    };

    struct Role2 {
        auto get() const -> int {
            return 2;
        }
    };

    struct ObjectRoles {
        __HAS_ROLES(Role1, Role2);
        virtual ~ObjectRoles() = default;
    };

    struct RealObject : Role1, Role2, ObjectRoles {
        IMPL_ROLE(Role1);
        IMPL_ROLE(Role2);
    };
}

SCENARIO("should be able to get role") {
    RealObject object;
    ObjectRoles& roles = object;

    REQUIRE(roles.toRole<Role1>().get() == 1);
    REQUIRE(roles.toRole<Role2>().get() == 2);
}