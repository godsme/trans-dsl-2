//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_SINGLETON_H
#define TRANS_DSL_2_SINGLETON_H

#include <cub/cub_ns.h>
#include <cstdint>
#include <cub/base/Unclonable.h>

CUB_NS_BEGIN

template<typename T>
struct Singleton {
   static T& getInstance() {
      static T instance;
      return instance;
   }

   DISALLOW_CLONE(Singleton);

protected:
   Singleton() = default;
};

CUB_NS_END

#define DEF_SINGLETON(object, ...) struct object : ::CUB_NS::Singleton<object>, ##__VA_ARGS__

#endif //TRANS_DSL_2_SINGLETON_H
