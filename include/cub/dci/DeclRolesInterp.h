//
// Created by godsme on 2021/3/9.
//

#include <cub/dci/ObjectRoles.h>

#ifdef  __HAS_ROLES
#undef  __HAS_ROLES
#endif

#define __HAS_ROLES(...)                                                                \
__MACO_map(__reflex_role_elem, __VA_ARGS__)
