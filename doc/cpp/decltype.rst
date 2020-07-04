.. _decltype:

**decltype**
======================

``decltype`` 是 `C++11` 加入的一个重要特性。 它允许求一切合法表达式的类型。从而，让从类型到值，从值到类型形成了一个闭环，
极大的扩展了泛型编程的能力。

``C++`` 规范中，对于 ``decltype`` 类型推演规则的定义如下：

   1. 若实参为无括号的 **标识表达式** 或无括号的 **类成员访问表达式** ，则 ``decltype`` 产生以此表达式命名的实体的类型。
      若无这种实体，或该实参指名某个重载函数，则程序非良构。

   2. 若实参是其他类型为 ``T`` 的任何表达式，且

      a) 若 表达式 的值类别为 **速亡值** ，则 ``decltype`` 产生 ``T&&`` ；
      b) 若 表达式 的值类别为 **左值** ，则 ``decltype`` 产生 ``T&`` ；
      c) 若 表达式 的值类别为 **纯右值** ，则 ``decltype`` 产生 ``T`` 。

      若表达式是 **纯右值** ，则不从该纯右值 **物质化** 临时对象：这种纯右值无结果对象。

   注意如果对象的名字带有括号，则它被当做通常的 **左值** 表达式，从而 ``decltype(x)`` 和 ``decltype((x))`` 通常是不同的类型。


这些规则，初看起来，有些让人困惑。但如果真的理解了背后的机制，其实非常容易理解。

``decltype`` 有两种表达方法：

1. 有括号：``decltype((expr))``
2. 无括号：``decltype(expr)``


有括号语意
-----------------

有括号的表达方法，语意是简单而统一的：它站在表达式类别的角度求类型。

1. 如果表达式属于 **纯右值** ，结果必然是 **非引用** 类型；
2. 如果表达式属于 **泛左值** ，结果必然是 **引用** 类型；

   - 如果表达式属于 **左值** ，结果必然是 **左值引用** ；
   - 如果表达式属于 **速亡值** ，结果必然是 **右值引用** ；


.. code-block:: c++

   struct Foo { int a; };

   using Func  = Foo& ();
   using Array = char[2];

   enum class E { OK, FAIL };

   const Foo f_v();
   Foo&      f_ref();
   Foo&&     f_r();

   int        a    = 0;
   const int  b    = 1;
   const Foo  foo  = {10};
   Foo&&      rref = Foo{1};
   const Foo& ref  = foo;
   char       c[2] = {1, 2};
   int*       p    = &a;
   const Foo* pFoo = &foo;

   // 左值
   decltype((a))       v1;   // int&
   decltype((foo))     v2;   // const Foo&
   decltype((foo.a))   v3;   // const int&
   decltype((f_ref())) v4;   // Foo&
   decltype((f_r))     v5;   // Foo&& (&)()
   decltype((c))       v6;   // char (&)[2]
   decltype((a += 10)) v7;   // int&
   decltype((++a))     v8;   // int&
   decltype((c[1]))    v9;   // char&
   decltype((*p))      v10;  // int&
   decltype((p))       v11;  // int*&
   decltype((pFoo))    v12;  // const Foo*&
   decltype((pFoo->a)) v13;  // const int&
   decltype((Foo::a))  v14;  // int&
   decltype((rref))    v15;  // Foo&
   decltype((ref))     v16;  // const Foo&
   decltype((a > 0 ? a : b))              v;  // int&
   decltype((static_cast<Func&&>(f_ref))) f;  // Foo& (&)()

   // 纯右值
   decltype((1+2))         v1;   // int
   decltype((Foo{10}))     v2;   // Foo
   decltype((f_v()))       v3;   // const Foo
   decltype((Array{0, 1})) v4;   // char[2]
   decltype((a++))         v5;   // int
   decltype((&b))          v6;   // const int*
   decltype((OK))          v7;   // E
   decltype((a > 0 ? 10 : Foo{0}.a)) v;  // int

   // 速亡值
   decltype((Foo{10}.a))    v1;  // int&&
   decltype((f_r()))        v2;  // Foo&&
   decltype((Array{}[0]))   v3;  // char&&
   decltype((std::move(a))) v4;  // int&&
   decltype((a > 0 ? Foo{1}.a : Foo{0}.a)) v;  // int&&


这其中，最有趣的是 ``decltype((rref))`` ，``rref`` 本身的类型是一个右值引用 ``Foo&&`` ，但做为左值表达式，
它的类型却是 ``Foo&`` ，而这一点，请参见 :ref:`右值引用变量 <rvalue-ref-var>`。

无括号语意
-----------------

无括号的情况下，除了一种例外，其它情况下，都与有括号场景一致。

这个例外就是对于变量（包括常量）名字的直接求类型。这种情况，会返回变量被定义时的类型。


.. code-block:: c++

   struct Foo { int a; };

   using Func  = Foo& ();
   using Array = char[2];

   const Foo f_v();
   Foo&      f_ref();
   Foo&&     f_r();

   int a           = 0;
   const int  b    = 1;
   const Foo  foo  = {10};
   Foo&&      rref = Foo{1};
   const Foo& ref  = foo;
   char       c[2] = {1, 2};
   int*       p    = &a;
   const Foo* pFoo = &foo;

   decltype(a)        v1;   // int
   decltype(b)        v2;   // const int
   decltype(foo)      v3;   // const Foo
   decltype(ref)      v4;   // const Foo&
   decltype(rref)     v5;   // Foo&&
   decltype(c)        v6;   // char[2]
   decltype(p)        v7;   // int*
   decltype(foo.a)    v8;   // int
   decltype(ref.a)    v9;   // int
   decltype(rref.a)   v10;  // int
   decltype(pFoo)     v11;  // const Foo*
   decltype(pFoo->a)  v12;  // int
   decltype(Foo{1).a) v13;  // int
   decltype(Foo::a)   v14;  // int

从例子中不难看出，对于所有的变量访问，无论直接还是间接，
由于每个变量在定义时都有自己的类型，因而求类型的结果就是这些变量被定义时的类型。

所以，之所以会出现有括号，无括号两种用法，正是因为每一个被定义的变量，都面临着两种需求：

1. 它们被定义时的类型
2. 整体做为一个表达式的类型（一定是泛左值）

前者是不关心表达式的，比如 ``decltype(Foo{1}.a)`` ，它只关心 ``a`` 被定义时的类型：``int`` ；
而不关心整个表达式本身是一个 ``xvalue`` ，因而表达式必然应该是一种右值引用类型：``int&&`` 。

正是对于变量有这两种需求的存在，而其它表达式没有这样的问题，所以，才专门为变量定义了两种求类型的方法。而对于其它表达式则两种方式无差别。
