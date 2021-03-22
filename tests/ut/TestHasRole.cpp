////
//// Created by godsme on 2021/3/9.
////
//
//#include <catch.hpp>
//#include <cub/dci/RoleExporter.h>
//#include <cub/dci/Role.h>
//
//namespace {
//    #include <cub/dci/FwdDeclRolesInterp.h>
//    #include "AllRoles.h"
//
//    DEFINE_ROLE(ObjectRoles) {
//        __DEF_ROLE_EXPORTER
//
//        #include <cub/dci/DeclRolesInterp.h>
//        #include "AllRoles.h"
//    };
//
//    struct Role1 {
//        auto get() const -> int {
//            return 1;
//        }
//    };
//
//    struct Role2 {
//        auto get() const -> int {
//            return 2;
//        }
//    };
//
//    struct Role3 {
//        auto get() const -> int {
//            return 3;
//        }
//    };
//
//    struct Role4 {
//        auto get() const -> int {
//            return 4;
//        }
//    };
//
//    #include <cub/dci/AggregateRolesInterp.h>
//    struct Dummy {};
//    struct Group1Object : Dummy
//    #include "RoleGroup1.h"
//    {};
//
//    struct Group2Object : Dummy
//    #include "RoleGroup2.h"
//    {};
//
//    struct RealObject : ObjectRoles, Group1Object, Group2Object
//    {
//        #include <cub/dci/ImplRolesInterp.h>
//        #include "AllRoles.h"
//    };
//}
//
//SCENARIO("should be able to get role") {
//    RealObject object;
//    ObjectRoles& roles = object;
//
//    REQUIRE(roles.toRole<Role1>().get() == 1);
//    REQUIRE(roles.toRole<Role2>().get() == 2);
//    REQUIRE(roles.toRole<Role3>().get() == 3);
//    REQUIRE(roles.toRole<Role4>().get() == 4);
//}