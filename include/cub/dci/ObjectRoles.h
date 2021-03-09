//
// Created by godsme on 2021/3/9.
//

#ifndef TRANS_DSL_2_OBJECT_ROLES_H
#define TRANS_DSL_2_OBJECT_ROLES_H

#include <maco/map.h>
#include <cub/dci/RoleDef.h>

#define __reflex_role_elem(x)                       \
    HAS_ROLE(x);                                    \
    template <typename T>                           \
    struct __SeCrEtE_field<T, x> {                  \
        static auto toRole(T const& obj) -> x& {    \
            return obj.ROLE(x);                     \
        }                                           \
    };                                              \

#endif //TRANS_DSL_2_OBJECT_ROLES_H





