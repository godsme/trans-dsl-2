.. _user-friendly:

用户接口改进
==========================

同步操作或谓词
-------------------

- 对于 ``sync action`` 和 ``predicate`` ，可以直接使用函数和lambda，同时也允许使用 ``仿函数`` ；

比如：

.. code-block::

   // normal function
   auto SyncAction1(TransactionInfo const&) -> Status {
      return Result::SUCCESS;
   }

   // lambda sync action
   auto SyncAction2 = [](TransactionInfo const&) -> Status {
      return Result::SUCCESS;
   };

   // lambda predicate
   auto IsTrue = [](TransactionInfo const&) -> bool {
      return true;
   };

   __sequential
     ( __sync(SyncAction1)
     , __optional(IsTrue, __sync(SyncAction2))
     , ...
     );



- 对于 ``atom action`` ， 无需从任何接口类继承，而是直接定义相关函数即可。

比如，对于异步Action，你只需要定义如下方法，却不需要继承任何接口：

.. code-block::

   struct AsyncAction1 {
      auto exec(TransactionInfo const&) -> Status;
      auto handleEvent(TransactionInfo const&, Event const&) -> Status;
      auto kill(TransactionInfo const&, Status cause) -> void;
   };

   __sequential
     ( __asyn(AsyncAction1)
     , __sync(SyncAction2)
     , ...
     );

这一方面免除了用户的负担，更重要的是，用户无需为不必要的虚函数付出任何额外代价。

**__procedure**
-----------------------------

在 ``1.x`` 时，过程分为两种: ``__procedure`` 和 ``__prot_procedure`` ，它们有完全一致的结构： 前面时正常的 `Action` ，
最后都有一个 ``__finally`` 。它们之间的区别在于，``__procedure`` 无法从错误中恢复。而 ``__prot_procedure`` 可以。

但这其中一个麻烦是：由于两者最后都是 ``__finally`` ，导致在阅读代码时，无法迅速的知道这是一个 ``__procedure`` ，还是一个
``__prot_procedure`` ，而它们的区别无非是是否可以通过 ``__finally`` 里的操作恢复之前的错误而已。

所以，``2.0`` 变更了定义它们的方式：不再有 ``__prot_procedure`` 。如果你需要从错误中恢复，最后不要使用 ``__finally`` ，而
使用 ``__recover`` 。比如：

.. code-block::

   // 原来的 __procedure 语意
   __procedure
   ( __sync(Action1)
   , __asyn(Action2)
   , __finally(__asyn(Action3)));

   // 原来的 __prot_procedure 语意
   __procedure
   ( __sync(Action1)
   , __asyn(Action2)
   , __recover(__asyn(Action3)));


更加自由的编写代码
------------------------------

在 ``1.x`` 时，如果操作是一个 ``__sequential`` 或 ``__procedure`` ，则必须明确指明，比如：

.. code-block::

   __transaction
   ( __sequential
       ( __asyn(Action1)
       , __asyn(Action2))
   );

   __transaction
   ( __procedure
      ( __sequential
          ( __asyn(Action1)
          , __asyn(Action2)),
      , __finally(
          __sequential
            ( __asyn(Action1)
            , __asyn(Action2)))
   );


当然，这可以通过让 ``__transaction`` 默认就组合了一个 ``__procedure`` 或者某些部分默认就组合了 ``__sequential`` 来
简化用户负担。但这样做至少有如下缺点：

首先，你只能默认组合一个。即你一旦默认组合了 ``__procedure`` ，则其必须有 ``__finally`` ，这对于只是 ``__sequential`` 的
情况就很麻烦；反之亦然。

其次，引入了不必要的代价。因为一旦你组合了一个 ``__procedure`` 或者 ``__sequential`` ，无论你需不需它，你都在付出它们所占用的
内存开销和运行时代价。比如下面这种最简单的情况，

.. code-block::

   __transaction
   ( __apply(Fragment));

如果 ``__transaction`` 背后默认组合了一个 ``__sequential`` ，则在付出不必要的代价。


``2.0`` 对此进行了改进。用户可以自由的编写代码。而不用关心你是一个 ``__sequential`` , 还是一个 ``__procedure`` ，或
干脆就是最简单的单一 ``action`` 。比如：

.. code-block::

   // simple case
   __transaction
   ( __asyn(Action1));

   // sequential
   __transaction
   ( __asyn(Action1)
   , __asyn(Action2));

   // procedure
   __transaction
   ( __asyn(Action1)
   , __finally(__asyn(Action2)));

框架可以自动感知你的代码结构。如果你有多个 ``action`` 则会自动按照 ``__sequential`` 的方式调度，如果最后是 ``__finally`` 或
``__recover`` ，则会自动引入 ``__procedure`` 的调度，而如果你只是最简单的情况，则按照最简单的情况调度。

也就是说，用户可以在自由书写的同时，却不用担心付出任何不必要的代价。

而这样的能力无处不在，比如 **片段** ：

.. code-block::

   // __procedure
   __def(Fragment1) __as
   ( __asyn(Action1)
   , __recover(__asyn(Action2)));

   // __sequential
   __def(Fragment2) __as
   ( __asyn(Action1)
   , __asyn(Action2));

   // just a simple one
   __def(Fragment3) __as
   ( __asyn(Action1) );

   // __procedure
   // main action of __procedure is __sequential
   __def(Fragment1) __as
   ( __asyn(Action1)
   , __asyn(Action2)
   , __recover(__asyn(Action3)));

   // __procedure
   // both main action & recover part are __sequential
   __def(Fragment1) __as
   ( __asyn(Action1)
   , __asyn(Action2)
   , __recover
      ( __asyn(Action3)
      , __asyn(Action4)));

或者 ``__optional`` ，

.. code-block::

   // __optional with a __sequential
   __optional
   ( __is_failed
   , __asyn(Action1)
   , __asyn(Action2));

   // __optional with a __procedure,
   // and the main action of the
   // __procedure is a __sequential
   __optional
   ( __is_failed
   , __asyn(Action1)
   , __asyn(Action2)
   , __finally(__asyn(Action3)));

事实上，任何可以组合其它 `Action` 的关键字里，比如 ``__time_guard`` ， ``__fork`` ， ``__safe`` , ``__void`` , ``__case`` 等等，都拥有
这样的能力。

多线程
++++++++++++++

在一个项目里，会存在多个 ``__transaction`` ，其中一部分是单线程的，而另外一部分是多线程的。多线程调度器无论从内存，还是运行时资源
消耗都要明显高于单线程。

在 ``1.x`` 时，为了在单线程场景不必付出多线程代价，需要由用户自己通过 ``__transaction``
和 ``__mt_transaction`` 来区分。

``2.0`` 则免除了用户的这种负担。如果你的代码中 ``__fork`` 了其它线程，``__transaction`` 会自动选择资源消耗更大的
多线程调度器，否则，将不需要付出这种不必要的代价。

.. code-block::

   // multi-thread
   __transaction
   ( __fork(THREAD1, __asyn(Action1))
   , __asyn(Action2))
   , __finally(__on_fail(__asyn(Action3))))

   // single-thread
   __transaction
   ( __asyn(Action1)
   , __asyn(Action2))
   , __finally(__on_fail(__asyn(Action3))))

另外，框架不仅仅能够感知你是一个多线程，还是一个单线程。还能够感知你的线程数量。从而，会按照你实际的线程数量进行空间和性能优化。从而保证
空间及性能的最优化。

**inline __sequential**
+++++++++++++++++++++++++++++++

在我们编写一个复杂的 ``__transaction`` 时，无论是因为 **复用** 目的，还是因为 **代码清晰** 的目的，往往会抽取很多 **片段** 。比如，
本来有这样一个 ``__transaction`` ：

.. code-block::

   // 多个Action，所以背后是一个__sequential
   __transaction
   ( __asyn(Action1)
   , __asyn(Action2)
   , __asyn(Action3)
   , __asyn(Action4)
   , __asyn(Action5)
   , __asyn(Action6)));


由于合理的原因，我们提取了两个片段：

.. code-block::

   // 多个Action，所以背后是一个__sequential
   __transaction
   ( __asyn(Action1)
   , __apply(Fragment1)
   , __asyn(Action4)
   , __apply(Fragment2));

   // 多个Action，所以背后是一个__sequential
   __def(Fragment1) __as
   ( __asyn(Action2)
   , __asyn(Action3));

   // 多个Action，所以背后是一个__sequential
   __def(Fragment2) __as
   ( __asyn(Action5)
   , __asyn(Action6));


这样，展开之后，会形成这样的结构：

.. code-block::

   // 多个Action，所以背后是一个__sequential
   __transaction
   ( __sequential
       ( __asyn(Action1)
       , __sequential
           ( __asyn(Action2)
           , __asyn(Action3))
       , __asyn(Action4)
       , __sequential
           ( __asyn(Action5)
           , __asyn(Action6))));

这就意味着，在（为了好的原因）提取片段的同时，你也在额外付出空间和性能代价。而这样的情况，基于现实项目的经验非常常见。

``2.0`` 针对等价语意的 ``__sequential`` 进行了自动 ``inline`` 处理，即，如果 ``__sequential`` 嵌套 ``__sequential`` ，
内层的 ``__sequential`` 会被展开（ ``inline`` ) 到外层的 ``__sequential`` 里。

对于上面的例子，经过 ``inline`` 处理之后，会自动恢复到与没有提取片段之前完全一样的结构上。

而在下面例子中的4个 ``transaction`` 完全等价，无论从语意，内存占用和性能开销，都完全一样。

.. code-block::

   __def(Fork2, __params(__action(ACTION1), __action(ACTION2))) __as
   ( __fork(1, __asyn(ACTION1))
   , __fork(2, __asyn(ACTION2)));

   //////////////////////////////////////////////////////
   __transaction
   ( __apply(Fork2, __with(AsyncAction1, AsyncAction4))
   , __asyn(AsyncAction2)
   , __join());

   __transaction
   ( __fork(1, __asyn(AsyncAction1))
   , __fork(2, __asyn(AsyncAction4))
   , __asyn(AsyncAction2)
   , __join());

   __transaction
   ( __sequential
       ( __fork(1, __asyn(AsyncAction1))
       , __fork(2, __asyn(AsyncAction4))
       , __asyn(AsyncAction2)
       , __join()));

   __transaction
   ( __sequential(
       __sequential
         ( __fork(1, __asyn(AsyncAction1))
         , __fork(2, __asyn(AsyncAction4)))
         , __asyn(AsyncAction2))
   , __join());

这样，就让程序员可以基于好的理由，自由的提取任何片段，而不用担心付出任何资源代价。

除了 ``__sequential`` 以外， ``__loop`` 里的 ``__sequential`` 也可以进行 ``inline`` ，比如：

.. code-block::

   __def(Fragment) __as
   ( __asyn(Action2)
   , __asyn(Action3));

   __loop
   ( __asyn(Action1)
   , __apply(Fragment)
   , __asyn(Action4)
   , __while(__is_failed));

与下面的形式，无论从语意，还是资源消耗，都完全相同：

.. code-block::

   __loop
   ( __asyn(Action1)
   , __asyn(Action2)
   , __asyn(Action3)
   , __asyn(Action4)
   , __while(__is_failed));


**__with_id 优化**
---------------------------

参见 :ref:`register_of_observer`
