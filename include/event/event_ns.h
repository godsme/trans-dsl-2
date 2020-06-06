//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_EVENT_NS_H
#define TRANS_DSL_2_EVENT_NS_H

#define EV_NS ev

#define EV_NS_BEGIN namespace EV_NS {
#define EV_NS_END   }

#define USING_EV_NS using namespace EV_NS;
#define FWD_DECL_EV(type) namespace EV_NS { struct type; }

#endif //TRANS_DSL_2_EVENT_NS_H
