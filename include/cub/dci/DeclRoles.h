//
// Created by godsme on 2021/3/9.
//

#ifndef TRANS_DSL_2_OBJECT_ROLES_H
#define TRANS_DSL_2_OBJECT_ROLES_H

#include <maco/map.h>
#include <cub/dci/RoleDef.h>

#define __ReFlEx_RoLe_ElEm(r)                       \
    private: HAS_ROLE(r);                           \
    template <typename T>                           \
    struct __SeCrEtE_field<T, r> {                  \
        static auto toRole(T const& obj) -> r& {    \
            return obj.ROLE(r);                     \
        }                                           \
    };                                              \

#define __DEF_ROLE_EXPORTER                                                              \
private: template<typename T, typename R> struct __SeCrEtE_field;                        \
public:  template<typename T> auto toRole() const -> T& {                                \
    return __SeCrEtE_field<std::remove_reference_t<decltype(*this)>, T>::toRole(*this);  \
}

#endif //TRANS_DSL_2_OBJECT_ROLES_H





