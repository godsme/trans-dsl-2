//
// Created by Darwin Yuan on 2020/6/28.
//

#if ENABLE_LOOP_CODE_GEN

template<SeqInt MAX_N>
auto get_(SeqInt seq, bool& isAction) -> SchedAction* {
   return nullptr;
}

template<>
auto get_<1>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      default:
         return nullptr;
   }
}

template<>
auto get_<2>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      default:
         return nullptr;
   }
}

template<>
auto get_<3>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      default:
         return nullptr;
   }
}

template<>
auto get_<4>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      default:
         return nullptr;
   }
}

template<>
auto get_<5>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      default:
         return nullptr;
   }
}

template<>
auto get_<6>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      default:
         return nullptr;
   }
}

template<>
auto get_<7>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      default:
         return nullptr;
   }
}

template<>
auto get_<8>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      default:
         return nullptr;
   }
}

template<>
auto get_<9>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      default:
         return nullptr;
   }
}

template<>
auto get_<10>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      default:
         return nullptr;
   }
}

template<>
auto get_<11>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      Loop_GeT_AcTiOn__(10);
      default:
         return nullptr;
   }
}

template<>
auto get_<12>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      Loop_GeT_AcTiOn__(10);
      Loop_GeT_AcTiOn__(11);
      default:
         return nullptr;
   }
}

template<>
auto get_<13>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      Loop_GeT_AcTiOn__(10);
      Loop_GeT_AcTiOn__(11);
      Loop_GeT_AcTiOn__(12);
      default:
         return nullptr;
   }
}

template<>
auto get_<14>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      Loop_GeT_AcTiOn__(10);
      Loop_GeT_AcTiOn__(11);
      Loop_GeT_AcTiOn__(12);
      Loop_GeT_AcTiOn__(13);
      default:
         return nullptr;
   }
}

template<>
auto get_<15>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      Loop_GeT_AcTiOn__(10);
      Loop_GeT_AcTiOn__(11);
      Loop_GeT_AcTiOn__(12);
      Loop_GeT_AcTiOn__(13);
      Loop_GeT_AcTiOn__(14);
      default:
         return nullptr;
   }
}

template<>
auto get_<16>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      Loop_GeT_AcTiOn__(10);
      Loop_GeT_AcTiOn__(11);
      Loop_GeT_AcTiOn__(12);
      Loop_GeT_AcTiOn__(13);
      Loop_GeT_AcTiOn__(14);
      Loop_GeT_AcTiOn__(15);
      default:
         return nullptr;
   }
}

template<>
auto get_<17>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      Loop_GeT_AcTiOn__(10);
      Loop_GeT_AcTiOn__(11);
      Loop_GeT_AcTiOn__(12);
      Loop_GeT_AcTiOn__(13);
      Loop_GeT_AcTiOn__(14);
      Loop_GeT_AcTiOn__(15);
      Loop_GeT_AcTiOn__(16);
      default:
         return nullptr;
   }
}

template<>
auto get_<18>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      Loop_GeT_AcTiOn__(10);
      Loop_GeT_AcTiOn__(11);
      Loop_GeT_AcTiOn__(12);
      Loop_GeT_AcTiOn__(13);
      Loop_GeT_AcTiOn__(14);
      Loop_GeT_AcTiOn__(15);
      Loop_GeT_AcTiOn__(16);
      Loop_GeT_AcTiOn__(17);
      default:
         return nullptr;
   }
}

template<>
auto get_<19>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      Loop_GeT_AcTiOn__(10);
      Loop_GeT_AcTiOn__(11);
      Loop_GeT_AcTiOn__(12);
      Loop_GeT_AcTiOn__(13);
      Loop_GeT_AcTiOn__(14);
      Loop_GeT_AcTiOn__(15);
      Loop_GeT_AcTiOn__(16);
      Loop_GeT_AcTiOn__(17);
      Loop_GeT_AcTiOn__(18);
      default:
         return nullptr;
   }
}

template<>
auto get_<20>(SeqInt seq, bool& isAction) -> SchedAction* {
   switch (seq) {
      Loop_GeT_AcTiOn__(0);
      Loop_GeT_AcTiOn__(1);
      Loop_GeT_AcTiOn__(2);
      Loop_GeT_AcTiOn__(3);
      Loop_GeT_AcTiOn__(4);
      Loop_GeT_AcTiOn__(5);
      Loop_GeT_AcTiOn__(6);
      Loop_GeT_AcTiOn__(7);
      Loop_GeT_AcTiOn__(8);
      Loop_GeT_AcTiOn__(9);
      Loop_GeT_AcTiOn__(10);
      Loop_GeT_AcTiOn__(11);
      Loop_GeT_AcTiOn__(12);
      Loop_GeT_AcTiOn__(13);
      Loop_GeT_AcTiOn__(14);
      Loop_GeT_AcTiOn__(15);
      Loop_GeT_AcTiOn__(16);
      Loop_GeT_AcTiOn__(17);
      Loop_GeT_AcTiOn__(18);
      Loop_GeT_AcTiOn__(19);
      default:
         return nullptr;
   }
}

#endif

