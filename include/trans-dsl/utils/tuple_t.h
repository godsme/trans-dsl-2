//
// Created by Darwin Yuan on 2020/8/31.
//

#ifndef TRANS_DSL_2_TUPLE_T_H
#define TRANS_DSL_2_TUPLE_T_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/ebo.h>
#include <trans-dsl/utils/integral_c.h>
#include <trans-dsl/utils/type_c.h>
#include <trans-dsl/utils/size_c.h>
#include <utility>

TSL_NS_BEGIN

template<typename ... Ts>
constexpr bool Is_Empty_Class = (std::is_empty_v<Ts> && ...);

struct tuple_tag {};

namespace detail {
   template<typename Xn, typename ... Xs>
   struct tuple_env;

   template<std::size_t ... Xn, typename ... Xs>
   struct tuple_env<std::index_sequence<Xn...>, Xs...> {
      template<std::size_t I> struct EBI {};
      struct instance : ebo<EBI<Xn>, Xs>... {
         template<std::size_t I> using ebi = EBI<I>;

         constexpr instance() = default;

         template<typename ... Ys>
         constexpr instance(Ys&& ... ys) : ebo<ebi<Xn>, Xs>{ys} ... {}
      };
   };

   template<typename ... Xs>
   using tuple_env_t = typename detail::tuple_env<std::index_sequence_for<Xs...>, Xs...>::instance;
}

template<typename ... Xs>
struct tuple  : private detail::tuple_env_t<Xs...> {
   constexpr static std::size_t Size = sizeof...(Xs);
   using tag_type = tuple_tag;
   using base = detail::tuple_env_t<Xs...>;
   using base::base;

   template<std::size_t N>
   constexpr auto get() const noexcept -> decltype(auto) {
      return detail::ebo_get<typename tuple<Xs...>::template ebi<N>>(*this);
   }

private:
   template<std::size_t ... I, typename Ys>
   constexpr auto tuple_equals(std::index_sequence<I...>, Ys const& ys) const noexcept {
      return bool_c<(Is_True_V<decltype(get<I>() == ys.template get<I>())> && ...)>;
   }

public:
   template<typename ... Ys>
   constexpr auto operator==(tuple<Ys...> const& rhs) const noexcept {
      if constexpr (std::is_same_v<tuple<Xs...>, tuple<Ys...>>) {
         return tuple_equals(std::index_sequence_for<Xs...>{}, rhs);
      } else {
         return false_c;
      }
   }

   template<typename Ys>
   constexpr auto operator!=(Ys const& rhs) const noexcept {
      return !operator==(rhs);
   }
};

template<typename ... Xs>
tuple(Xs&& ...) -> tuple<std::decay_t<Xs>...>;

template<typename T>
struct Is_Tuple : false_type {};

template<typename ... Xs>
struct Is_Tuple<tuple<Xs...>> : true_type {};

template<typename T>
constexpr bool Is_Tuple_v = Is_Tuple<std::decay_t<T>>::value();

template<typename T>
constexpr bool is_tuple(T const&) {
   return bool_c<Is_Tuple_v<T>>;
}

template<typename ... Xs>
constexpr auto make_tuple(Xs&& ... xs) -> tuple<std::decay_t<Xs>...> {
   return tuple<std::decay_t<Xs>...>{std::forward<Xs>(xs) ...};
}

template<typename ... Xs>
constexpr tuple<type_c_t<Xs>...> tuple_t{};

template<typename ... Ts>
constexpr bool Is_Empty_Object = (std::is_empty_v<std::decay_t<Ts>> && ...);

template<std::size_t N, typename ... Xs>
constexpr auto get(tuple<Xs...> const& xs) noexcept -> decltype(auto) {
return xs.template get<N>();
}

namespace detail {
   template<std::size_t ... I, typename Xs, typename Ys>
   constexpr auto tuple_matches(std::index_sequence<I...>, Xs const& xs, Ys const& ys) noexcept {
      return ((get<I>(xs) == get<I>(ys)) && ...);
   }
}

template<typename ... Xs, typename ... Ys>
constexpr auto matches(tuple<Xs...> const& lhs, tuple<Ys...> const& rhs) noexcept {
   if constexpr (std::is_same_v<tuple<Xs...>, tuple<Ys...>>) {
      return detail::tuple_matches(std::index_sequence_for<Xs...>{}, lhs, rhs);
   } else {
      return false;
   }
}

template<std::size_t I, typename Xs>
using tuple_element_t = std::decay_t<decltype(get<I>(std::declval<Xs>()))>;

template<typename ... Xs>
constexpr auto tuple_size(tuple<Xs...> const&) {
return size_c<sizeof...(Xs)>;
}

namespace detail {
   template<typename ... Xs, typename ... Ys, std::size_t ... Xn, std::size_t ... Yn>
   constexpr auto tuple_cat(tuple<Xs...> const& xs, tuple<Ys...> const& ys, std::index_sequence<Xn...>, std::index_sequence<Yn...>) {
   return tuple<Xs..., Ys...>{get<Xn>(xs)..., get<Yn>(ys)...};
}
}

template<typename ... Xs, typename ... Ys>
constexpr auto tuple_cat(tuple<Xs...> const& xs, tuple<Ys...> const& ys) {
return detail::tuple_cat(xs,
   ys,
   std::index_sequence_for<Xs...>{},
   std::index_sequence_for<Ys...>{});
}

namespace detail {
   template<typename ... Xs, typename X, std::size_t ... Xn>
   constexpr auto tuple_append(X const& x, tuple<Xs...> const& xs, std::index_sequence<Xn...>) {
      return tuple<Xs..., X>{get<Xn>(xs)..., x};
   }
}

template <typename X, typename ... Xs>
constexpr static auto tuple_append(X const& x, tuple<Xs...> const& xs) {
   return detail::tuple_append(x, xs, std::index_sequence_for<Xs...>{});
}

template<typename ... Xs, typename ... Ys>
constexpr auto operator+(tuple<Xs...> const& xs, tuple<Ys...> const& ys) {
return tuple_cat(xs, ys);
}

template<typename ... Xs, typename X>
constexpr auto operator+(tuple<Xs...> const& xs, X const& x) {
return tuple_append(x, xs);
}

TSL_NS_END

#endif //TRANS_DSL_2_TUPLE_T_H
