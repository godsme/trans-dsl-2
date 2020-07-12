//
// Created by Darwin Yuan on 2020/7/11.
//

#include <catch.hpp>
#include <cub/type-list/TypeListPipeLine.h>
#include <cub/type-list/InfiniteIntList.h>

namespace {
   using namespace cub;

   template<typename T> struct S;
   template<typename ... Ts> struct Result;
   template<typename T, auto V> struct Tag;
   template<typename T>
   struct Transformer {
      using type = Tag<typename T::first, T::second>;
   };

   template<typename T> struct Identity { using type = T; };
   template<typename ... Ts>
   struct PipeLineTest1 {
      using type = __TL_Raw_Pipeline__(Ts..., ZipWith<__infinite_list(int, 1)>, Transform<Transformer>)
                   __TL_OutputTo(Result);
   };

   TEST_CASE("pipe-line") {
      REQUIRE(std::is_same_v<Result<Tag<int, 1>, Tag<double, 2>, Tag<short, 3>>, PipeLineTest1<int, double, short>::type>);
      using type = __TL_Pipeline(__infinite_list(long, 1), Drop<5>, Take<5>, Elem<0>); //__TL_OutputTo(Result);

      S<type> ss;
   }
}