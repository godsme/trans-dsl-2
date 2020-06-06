//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_TSL_NS_H
#define TRANS_DSL_2_TSL_NS_H

#define TSL_NS tsl
#define TSL_NS_BEGIN namespace TSL_NS {
#define TSL_NS_END   }

#define USING_TSL_NS using namespace TSL_NS;
#define FWD_DECL_TSL(type) namespace TSL_NS { struct type; }

#endif //TRANS_DSL_2_TSL_NS_H
