//
// Created by godsme on 2021/3/9.
//

#ifndef TRANS_DSL_2_OBJECTROLES_H
#define TRANS_DSL_2_OBJECTROLES_H

#include <maco/map.h>
#include <cub/dci/Role.h>

#define __reflex_role_elem(x)                       \
    HAS_ROLE(x);                                    \
    template <typename T>                           \
    struct __SeCrEtE_field<T, x> {                  \
        static auto toRole(T const& obj) -> x& {    \
            return obj.ROLE(x);                     \
        }                                           \
    };                                              \

#define __HAS_ROLES(...)                                                                \
template<typename T> auto toRole() const -> T& {                                        \
    return __SeCrEtE_field<std::remove_reference_t<decltype(*this)>, T>::toRole(*this); \
}                                                                                       \
template<typename T, typename X> struct __SeCrEtE_field;                                \
__MACO_map(__reflex_role_elem, __VA_ARGS__)


#endif //TRANS_DSL_2_OBJECTROLES_H
