//
// Created by godsme on 2021/3/9.
//

#ifndef TRANS_DSL_2_ROLE_H
#define TRANS_DSL_2_ROLE_H

#include <cub/base/Keywords.h>
#include <cub/dci/RoleDef.h>

#define DEFINE_ROLE(role, ...) INTERFACE_DEF(role), ##__VA_ARGS__

#endif //TRANS_DSL_2_ROLE_H
