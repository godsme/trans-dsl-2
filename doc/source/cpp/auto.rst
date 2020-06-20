**auto** 类型推演
===================

``auto`` 类型推演脱胎于模版函数的类型推演，它们的能力几乎等价（除了初始化列表的情况）。
这也就意味着，其实在 ``C++11`` 之前，``C++`` 早就具备了 ``auto`` 的能力，只是没有从语法上允许而已。

**auto** 的语意
---------------------

和直觉不同的是，对于任意表达式：``auto v = expr`` ，``v`` 的类型并不总是和 ``expr`` 所返回的类型一致。

首先，``auto`` 不可能是一个 **引用** ，无论是 **左值引用** ，还是 **右值引用** ，所以，如果 ``expr`` 返回类型里
包含任何引用，都会被舍弃。比如：

.. code-block:: c++

   Foo foo{1};
   Foo& ref = foo;
   Foo&& rref = Foo{2};

   Foo& getRef();
   Foo&& getRref();

   auto v1 = ref;        // v1 type: Foo
   auto v2 = rref;       // v2 type: Foo
   auto v3 = getRef();   // v3 type: Foo
   auto v4 = getRref();  // v4 type: Foo

其次，所有对值所修饰的 ``const`` 都会被丢弃。 比如：

.. code-block:: c++

   const Foo foo{1};
   const Foo& ref     = foo;
   const Foo&& rref   = Foo{2};
   const Foo* const p = &foo;

   auto v1 = foo;   // Foo
   auto v2 = ref;   // Foo
   auto v3 = rref;  // Foo
   auto v4 = p;     // const Foo*

究其原因，是因为这种直接给出 ``auto`` 的写法，是一种 ``copy/move`` 语意。因而，等号右边的表达式本身类型是引用，并不影响
等号左侧对象本身不是引用；同样的，等号右边表达式本身的 **constness** ，``copy/move`` 后，并不会影响新定义变量
的 **constness** 。

其推演语意，完全等价于：

.. code-block:: c++

   template <typename T>
   void f(T value);

其中 ``T`` 就是 ``auto`` ，``value`` 就是你用 ``auto`` 所定义的变量。

注意，到了 ``C++17`` 之后， 并非所有的场景下，都是 ``copy/move`` 语意， 比如 ``auto v = Foo{1}`` ，
其行为完全等价于： ``Foo v{1}`` 。具体可参见 :ref:`prvalue_material` 。


因而，更准确的说，这不是 ``copy/move`` 语意，而属于构造初始化语意。

引用及 **const**
------------------------

因而，如果你希望让新定义的变量属于引用类型，或具备 ``const`` ，则需要明确指定。比如：

.. code-block:: c++

   auto        foo  = Foo{1};
   const auto& ref  = foo;
   auto&&      rref = Foo{2};

而指针的情况则稍有特殊。

指针
----------

当你不指定指针的情况下，如果等号右侧的表达式是一个指针类型，那么左侧的变量类型当然也是一个指针。

当你明确指定指针的情况下，则是要求右侧表达式必须是一个指针类型。

.. code-block:: c++

   Foo foo{1};
   Foo* pFoo = &foo;

   auto  v1 = foo;  // v1 type: Foo
   auto  p1 = pFoo; // p1 type: Foo*
   auto* p2 = pFoo; // p2 type: Foo*
   auto* p3 = foo;  // Error: foo is not a pointer


通用引用
---------------

更为特殊的是 ``auto&& v = expr`` 的表达式。这并不必然导致 ``v`` 是一个右值引用。而是取决于 ``expr`` 的类别。

- 如果 ``expr`` 是一个 **左值** 表达式，那么 ``v`` 将是左值引用类型；
- 如果 ``expr`` 是一个 **右值** 表达式（参见 :ref:`prvalue_material`），那么 ``v`` 将会是右值引用类型。

.. code-block:: c++
   :linenos:

   Foo foo{1};
   Foo&   ref = foo;
   Foo&& rref = Foo{2};
   Foo&& getRref();
   Foo& getRef();
   Foo getFoo();

   auto&& v1 = foo;            // v1 type: Foo&
   auto&& v2 = Foo{2};         // v2 type: Foo&&
   auto&& v3 = getRref();      // v3 type: Foo&&
   auto&& v4 = getRef();       // v4 type: Foo&
   auto&& v5 = getFoo();       // v5 type: Foo&&
   auto&& v6 = ref;            // v6 type: Foo&
   atuo&& v7 = rref;           // v7 type: Foo&&

正是因为这样的写法，允许等号右侧是任意合法的表达式，而等号左侧总是可以根据表达式类别，推演出合适的引用类型。所以这种写法被称做 **通用引用** 。

其中，我们可以清晰的看出，虽然 ``ref`` 和 ``rref`` 分别被定义为 **左值引用** 和 **右值引用** ，但它们做为左值来讲，是等价的。都是左值引用。
具体可参考 :ref:`右值引用变量 <rvalue-ref-var>` 。

初始化列表
-------------

由于初始化列表不是一个表达式，因而类型也就无从谈起。所以 ``C++14`` 对其做了特殊的规定：

- 如果使用直接初始化的方式，比如：`auto i{1}` ，则初始化列表只允许有一个元素，其等价于 ``auto i = 1``；
- 如果使用拷贝初始化的方式，比如：`auto v = {1, 2}`，则初始化列表允许有多个同一类型的元素。
  其等价于 ``std::initializer_list<int> v = {1, 2}`` 。

decltype(auto)
--------------------

由于 ``auto`` 推演总是会丢弃 **引用** 及 ``const`` 信息，明确给出 **引用** 又总是得到一个引用。明确给出 ``const`` ，
则总是得到一个 ``const`` 类型。这对于想精确遵从等号后面类型的情况非常不便，尤其在进行泛型编程时，很难通过 `auto` 符合通用的情况。

而 ``decltype`` 恰恰相反，它总是能准确捕捉右侧表达式的类型（参见 :ref:`decltype` ）。因而，我们可以这样写：

.. code-block:: c++

   Foo foo{1};
   const Foo& ref = foo;
   Foo&& rref = Foo{2};
   int a = 0;

   decltype(foo)    v1 = foo;   // Foo
   decltype((foo))  v2 = foo;   // Foo&
   decltype(ref)    v3 = ref;   // const Foo&
   decltype(rref)   v4 = rref;  // Foo&&
   decltype((rref)) v5 = rref;  // Foo&
   decltype(1+2)    v6 = 1 + 2; // int

   decltype((a > 0 ? Foo{0}.a : Foo{1}.a)) v7 = \
      a > 0 ? Foo{0}.a : Foo{1}.a; // int&&

但这样的写法，总是要把右边的表达式在 ``decltype`` 里重复写一遍，才能做到。到了 ``C++14`` ，
推出了一种新的写法：``decltype(auto)`` ， 其中 ``auto`` 是一个自动占位符，代表等号右侧的表达式，这就大大简化了程序员的工作：

.. code-block:: c++

   decltype(auto)   v1 = foo;    // Foo
   decltype(auto)   v2 = (foo);  // Foo&
   decltype(auto)   v7 = (a > 0 ? Foo{0}.a : Foo{1}.a); // int&&


函数返回值类型的自动推演
--------------------------

到了 ``C++14`` 之后，对于普通函数的返回值自动推演，可以通过 ``auto`` 来完成，比如：

.. code-block:: c++

   auto f() { return Foo{1}.a; } // 返回值类型为int

当然，如果希望返回值类型运用 ``decltype`` 规则，则可以用 ``decltype(auto)`` 。比如：

.. code-block:: c++

   auto f() -> decltype(auto) { // 返回值为int&&
     return (Foo{1}.a);
   }


非类型模版参数
--------------------

.. code-block:: c++

   template <auto V>
   struct C
   {
      // ....
   };

   C<10>   a; // C<int>
   C<'c'>  b; // C<char>
   C<true> c; // C<bool>


函数模版的便捷写法
-------------------

.. code-block:: c++

   template <typename T1, typename T2>
   auto add(T1 lhs, T2 rhs) {
      return lhs + rhs;
   }


到了 ``C++20`` ，允许让普通函数可以有更加便捷的写法：

.. code-block:: c++

   auto add(auto lhs, auto rhs) {
      return lhs + rhs;
   }


当然，如果你想指明两个参数属于同一种类型，但另外的参数没有这样的约束，则仍然需要写模版头：

.. code-block:: c++

   template <typename T>
   auto f(T a, auto b, T c, auto d); // a, c 必须同一类型，b, d 各自有各自类型





