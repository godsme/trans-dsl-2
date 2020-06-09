//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_ROLE_H
#define TRANS_DSL_2_ROLE_H

#include <cub/base/Keywords.h>

#define DEFINE_ROLE(role) DEF_INTERFACE(role)

#define ROLE(role) __dci_get##role()
#define ROLE_PROTO_TYPE(role) role& ROLE(role) const
#define USE_ROLE(role) virtual ROLE_PROTO_TYPE(role) = 0
#define HAS_ROLE(role) USE_ROLE(role)

//////////////////////////////////////////////////////////////////
#define IMPL_ROLE(role)                                       \
ROLE_PROTO_TYPE(role) override                                \
{                                                             \
   return const_cast<role&>(static_cast<const role&>(*this)); \
}

#endif //TRANS_DSL_2_ROLE_H
