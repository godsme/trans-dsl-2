//
// Created by Darwin Yuan on 2020/7/1.
//

#ifndef TRANS_DSL_2_JOINHELPER_H
#define TRANS_DSL_2_JOINHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/domain/ThreadId.h>
#include <trans-dsl/sched/domain/ThreadBitMap.h>
#include <trans-dsl/sched/action/SchedJoin.h>

TSL_NS_BEGIN

namespace details {
   template<ThreadId TID>
   struct ThreadIdBitMask {
      static_assert(TID != 0, "0 is the ID of main thread, which is not allowed to be joined");
      static_assert(TID < sizeof(ThreadBitMap) * 8, "Thread ID is out of scope");
      constexpr static ThreadBitMap value = ThreadBitMap(1) << TID;
   };

   template <ThreadId ... TIDs>
   class Join final : public SchedJoin {
      constexpr static ThreadBitMap threadBitMap = (ThreadBitMap{} | ... | ThreadIdBitMask<TIDs>::value);
      OVERRIDE(getThreadBitMap() const -> ThreadBitMap) {
         return threadBitMap;
      }
   };
}

#define __join(...) TLS_NS::details::Join<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_JOINHELPER_H
