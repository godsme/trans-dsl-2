//
// Created by godsme on 2021/3/10.
//

#ifndef TRANS_DSL_2_ROLEEXPORTER_H
#define TRANS_DSL_2_ROLEEXPORTER_H

#include <type_traits>

#define __DEF_ROLE_EXPORTER                                                              \
private: template<typename T, typename R> struct __SeCrEtE_field;                        \
public:  template<typename T> auto toRole() const -> T& {                                \
    return __SeCrEtE_field<std::remove_reference_t<decltype(*this)>, T>::toRole(*this);  \
}

#endif //TRANS_DSL_2_ROLEEXPORTER_H
