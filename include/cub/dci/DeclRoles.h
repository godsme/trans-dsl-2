//
// Created by godsme on 2021/3/9.
//

#include <cub/dci/ObjectRoles.h>

#ifdef  __HAS_ROLES
#undef  __HAS_ROLES
#endif

#define __HAS_ROLES(...)                                                                \
template<typename T> auto toRole() const -> T& {                                        \
    return __SeCrEtE_field<std::remove_reference_t<decltype(*this)>, T>::toRole(*this); \
}                                                                                       \
template<typename T, typename X> struct __SeCrEtE_field;                                \
__MACO_map(__reflex_role_elem, __VA_ARGS__)
