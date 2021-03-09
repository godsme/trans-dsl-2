//
// Created by godsme on 2021/3/9.
//

#include <cub/dci/FwdDeclRolesDef.h>
#include <maco/map.h>

#ifdef  __HAS_ROLES
#undef  __HAS_ROLES
#endif

#define __HAS_ROLES(...)                                                                \
__MACO_map(__FwD_dEcL_rOlE, __VA_ARGS__)