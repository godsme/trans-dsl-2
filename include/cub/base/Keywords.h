//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_KEYWORDS_H
#define TRANS_DSL_2_KEYWORDS_H

#include <cub/cub_ns.h>

CUB_NS_BEGIN

namespace details
{
   template<typename T>
   struct Interface
   {
      virtual ~Interface() = default;
   };
}

#define INTERFACE_DEF(Intf)  struct Intf : ::CUB_NS::details::Interface<Intf>

#define DEFAULT(...) virtual auto __VA_ARGS__
#define ABSTRACT(...) virtual auto __VA_ARGS__ = 0

#define OVERRIDE(...) auto __VA_ARGS__ override

#define EXTENDS(...) , ##__VA_ARGS__
#define IMPLEMENTS(...) EXTENDS(__VA_ARGS__)

#define FWD_DECL(ns, cls) namespace ns { struct cls; }

CUB_NS_END

#endif //TRANS_DSL_2_KEYWORDS_H
