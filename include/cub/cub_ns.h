//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_CUB_NS_H
#define TRANS_DSL_2_CUB_NS_H

#define CUB_NS cub

#define CUB_NS_BEGIN namespace CUB_NS {
#define CUB_NS_END   }

#define USING_CUB_NS using namespace CUB_NS;
#define FWD_DECL_CUB(type) namespace CUB_NS { struct type; }

#endif //TRANS_DSL_2_CUB_NS_H
