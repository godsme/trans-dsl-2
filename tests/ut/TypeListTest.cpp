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
   template<typename T>
   struct Transformer {
      using type = typename T::first;
   };
   template<typename ... Ts>
   struct PipeLineTest1 {
      using type = __TL_Pipeline__(Ts..., ZipWith<InfiniteValueList<int, 1>>, Transform<Transformer>)
                   __TL_OutputTo__(Result);
   };

   TEST_CASE("pipe-line") {
      REQUIRE(std::is_same_v<Result<int, double, short>,PipeLineTest1<int, double, short>::type>);
   }
}