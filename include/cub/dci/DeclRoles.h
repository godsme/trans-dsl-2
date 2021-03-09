//
// Created by godsme on 2021/3/9.
//

#ifndef TRANS_DSL_2_OBJECT_ROLES_H
#define TRANS_DSL_2_OBJECT_ROLES_H

#include <maco/map.h>
#include <cub/dci/RoleDef.h>

#define __ReFlEx_RoLe_ElEm(x)                       \
    HAS_ROLE(x);                                    \
    template <typename T>                           \
    struct __SeCrEtE_field<T, x> {                  \
        static auto toRole(T const& obj) -> x& {    \
            return obj.ROLE(x);                     \
        }                                           \
    };                                              \

#define __DEF_ROLE_EXPORTER \
template<typename T, typename X> struct __SeCrEtE_field;                                \
template<typename T> auto toRole() const -> T& {                                        \
    return __SeCrEtE_field<std::remove_reference_t<decltype(*this)>, T>::toRole(*this); \
}

#endif //TRANS_DSL_2_OBJECT_ROLES_H





