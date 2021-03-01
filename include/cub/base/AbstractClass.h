//
// Created by godsme on 2021/2/8.
//

#ifndef TRANS_DSL_2_ABSTRACTCLASS_H
#define TRANS_DSL_2_ABSTRACTCLASS_H

namespace abstract_class_details {
    struct base {
        virtual ~base() = default;
    };
}

#define ABSTRACT_CLASS(cls, ...) \
struct cls : abstract_class_details::base, __VA_ARGS__ \

#endif //TRANS_DSL_2_ABSTRACTCLASS_H
