//
// Created by godsme on 2021/3/9.
//

#include <cub/dci/DeclRoles.h>

#ifdef  __HAS_ROLES
#undef  __HAS_ROLES
#endif

#define __HAS_ROLES(...)                                                                \
__MACO_map(__ReFlEx_RoLe_ElEm, __VA_ARGS__)
