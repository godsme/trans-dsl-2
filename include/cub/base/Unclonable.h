//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_UNCLONABLE_H
#define TRANS_DSL_2_UNCLONABLE_H

#define DISALLOW_CLONE(cls) \
cls(cls const&) = delete; \
cls(cls&&) = delete; \
cls& operator=(cls const&) = delete; \
cls& operator=(cls&&) = delete

#endif //TRANS_DSL_2_UNCLONABLE_H
