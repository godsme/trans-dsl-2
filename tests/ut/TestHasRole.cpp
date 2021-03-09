//
// Created by godsme on 2021/3/9.
//

#include <catch.hpp>


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

    struct Role3 {
        auto get() const -> int {
            return 3;
        }
    };

    struct Role4 {
        auto get() const -> int {
            return 4;
        }
    };


    struct ObjectRoles {
#include <cub/dci/DeclRoles.h>
        __DEF_ROLE_EXPORTER
        __HAS_ROLES(Role1, Role2)
        __HAS_ROLES(Role3, Role4)
        virtual ~ObjectRoles() = default;
    };

    struct RealObject : ObjectRoles
#include <cub/dci/AggregateRoles.h>
    __HAS_ROLES(Role1, Role2)
    __HAS_ROLES(Role3, Role4)
    {
#include <cub/dci/ImplRoles.h>
        __HAS_ROLES(Role1, Role2)
        __HAS_ROLES(Role3, Role4)
    };
}

SCENARIO("should be able to get role") {
    RealObject object;
    ObjectRoles& roles = object;

    REQUIRE(roles.toRole<Role1>().get() == 1);
    REQUIRE(roles.toRole<Role2>().get() == 2);
    REQUIRE(roles.toRole<Role3>().get() == 3);
    REQUIRE(roles.toRole<Role4>().get() == 4);
}