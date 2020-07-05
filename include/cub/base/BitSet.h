//
// Created by Darwin Yuan on 2020/7/4.
//

#ifndef TRANS_DSL_2_BITSET_H
#define TRANS_DSL_2_BITSET_H

#include <cub/cub_ns.h>
#include <type_traits>
#include <cstddef>

CUB_NS_BEGIN

template<typename T, typename = std::enable_if<std::is_integral_v<T>>>
struct BitSet {
public:
   constexpr BitSet(T value = 0) : value{value} {}

   auto enable(unsigned char n) -> void {
      value |= T(1 << n);
   }

   auto clear(unsigned char n) -> void {
      value &= T(~(1 << n));
   }

   auto resize(unsigned char n) -> void {
      value &= T((1 << n) - 1);
   }

   constexpr auto isEnabled(unsigned char n) const -> bool {
      return value & T(1 << n);
   }

   constexpr auto empty() const -> bool {
      return value == 0;
   }

   constexpr auto getValue() const -> T {
      return value;
   }

   constexpr auto nonEmpty() const -> bool {
      return value != 0;
   }

   constexpr auto firstEnabled() const -> unsigned char {
      unsigned char i=0;
      for(; !isEnabled(i); i++);
      return i;
   }

   friend constexpr BitSet operator|(const BitSet& lhs, const BitSet& rhs) {
      return lhs.value | rhs.value;
   }

   static constexpr size_t max = sizeof(T) * 8;

private:
   T value = 0;
};


CUB_NS_END

#endif //TRANS_DSL_2_BITSET_H
