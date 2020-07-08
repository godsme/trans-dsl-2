过程监控
========

在一个现实的系统中，对于个事务的执行过程往往有各种类型的追踪，比如， 统计每个操作的运行时间，
一个重要的步骤需要汇报给网关软件，以进行相关的指标统计，或者干脆就是调试的目的:当出现问题时，希望知道究竟是哪个步骤出了问题。

将相关的代码都放入每个操作无疑会严重的污染操作的实现。从单一职责的角度，每个类都只应该关注一件事情。
上述的需求都应该作为不同的关注点，从具体的业务代码中分离出去。

很明显， **观察者模式** 是解决这类问题的良药。每个不同的过程监控需求都应该当做一个独立的观察者：
比如，调试的观察者，某种指标统计的观察者，单步操作运行时间的观察者，等等等等。

**TransactionListener**
---------------------------

每个观察者应该实现一个或者多个如下方法。其定义如下:

.. code-block::

   auto onActionStarting(ActionId, TransactionInfo const&) -> void;
   auto onActionEventConsumed(ActionId, TransactionInfo const&, Event const&) -> void;
   auto onActionDone(ActionId, TransactionInfo const&, Status) -> void
   auto onActionStopped(ActionId, TransactionInfo const&, Status) -> void
   auto onActionKilled(ActionId, TransactionInfo const&, Status) -> void


注意，你无需继承自任何接口，直接在你的观察者类中选择实现你关心的方法，比如下面的观察者只关心 ``Action Done`` 事件：

.. code-block::

   struct MyObserver {
      auto onActionDone(ActionId, TransactionInfo const&, Status) -> void {
         // blah...
      }
   };


.. list-table::
   :widths: 30  60
   :header-rows: 1

   * - 方法
     - 被调用时机
   * - ``onActionStarting``
     - 一个操作开始执行之前( ``exec`` 被调用之前)
   * - ``onActionEventConsumed``
     - 一个事件被 ``handleEvent`` 接受(无论其返回 ``SUCCESS`` ，``CONTINUE`` 还是失败)
   * - ``onActionDone``
     - 一个操作已经运行结束，无论是成功还是失败。
   * - ``onActionStopped``
     - 一个操作已经被中止，无论成功或失败。
   * - ``onActionKilled``
     - 一个操作已经被暴力杀掉 （ ``kill`` 被调用之后）


**__with_id**
----------------------

你应该已经注意到，观察者的每一个方法都有一个类型为 ``ActionId`` 的参数。
这个参数是为了唯一的标识一个操作。问题是，这些观察者如何知道哪个 ``Action ID`` 对应哪个操作?

答案是：由用户自己指定，指定的方式则是通过 ``__with_id`` 。通过 ``__with_id`` ，
你不仅可以知道一个 ``Action ID`` 对应了哪个操作，你还可以选择是否观察一个操作。 比如:

.. code-block::

   const ActionId ID_ACTION1 = 1;
   const ActionId ID_ACTION4 = 2;

   __transaction
   ( __procedure
       ( __with_id(ID_ACTION1,__asyn(Action1))
       , __asyn(Action2))
       , __finally(__rsp(Action3)))
   , __with_id(ID_ACTION4, __asyn(Action4)));


在这个例子中，用户指定了要观察 ``Action1`` 和 ``Action4`` ，并分别为它们指定了 ``Action ID`` 。
随后，当此事务运行时，将只向观察者报告这两个操作有关的事件。

任意的粒度
++++++++++++++

在上一个例子中，两个被观察的操作都是用户定义的基本操作。但这并不是 `Transaction DSL` 的约束。

事实上，你可以观察的粒度可以是任意的。比如, 在下面的例子中，我们设置的观察粒度是整个事务:

.. code-block::

   __transaction
   ( __with_id
      ( ID_TRANS
      , __procedure
          ( __asyn(Action1)
          , __asyn(Action2))
          , __finally(__rsp(Action3)))
      , __asyn(Action4)));


无论观察的粒度设为多大，在此粒度上的一切事件，都会汇报给观察者。

嵌套
-----------

如果观察者观察了一个大粒度的操作，同时，它依然可以观察此操作中更小粒度的操作。
比如，在下面的例子中， ``ID_SEQ`` 所代表的操作，就属于 ``ID_TRANS`` 所代表的操作的子操作。

.. code-block::

   __transaction
   ( __with_id
       ( ID_TRANS
       , __procedure
           ( __with_id
               ( ID_SEQ
               , __asyn(Action1)
               , __asyn(Action2))
           , __finally(__rsp(Action3)))
       , __asyn(Action4)));

这种情况下，发生在子操作上的一切事件，会同时汇报给子操作和其父操作。

.. _register_of_observer:

观察者的定义及注册
---------------------

一个简单的事实是，任何一个 ``Transaction DSL`` 的关键字，背后都对应着一个内部 ``Action`` 的实现。只要你使用了它，你就潜在
地为之付出空间和性能代价。

``__with_id`` 也不例外。并且在一些现实项目中，维测类需求大都可以通过 ``观察者`` 的方式监控所有 ``Transaction`` 的运行。
因而会有很多 ``观察者`` 。每个 ``观察者`` 关注的 ``Action ID`` 也不尽相同。

另外，在运行时，这些维测类 ``观察者`` 可能会随时被关闭，也可能随时被打开。而我们希望它们被关闭时，``Transaction`` 在运行时无需
为之付出任何代价（最好一个指令，一个字节都不付出）。

为了达到这个目标，框架要求你定义每一个 ``观察者`` 时，需要通过 ``ObservedActionIdRegistry`` 来指明你关心的 ``Action ID`` 。
比如：

.. code-block::

   struct MyListener1 : ObservedActionIdRegistry<ID_TRANS, ID_SEQ> {
        auto onActionDone(ActionId aid, TransactionInfo const&, Status) -> void {
            switch(aid) {
            case ID_TRANS: // blah...
            case ID_SEQ:   // blah...
            }
        }
   };

   struct MyListener2 : ObservedActionIdRegistry<ID_TRANS> {
        auto onActionStarting(ActionId aid, TransactionInfo const&) -> void {
            switch(aid) {
            case ID_TRANS: // blah...
            }
        }
   };

例子中，``MyListener1`` 关心 2 个 ``Action ID`` : ``ID_TRANS`` 和 ``ID_SEQ`` ；而 ``MyListener2`` 只关心 ``ID_TRANS`` 。
这需要通过继承 ``ObservedActionIdRegistry`` 并在模版参数里指明。


然后，你可以通过 ``__bind_listener`` ，将这些 ``观察者`` 注册给一个 ``Transaction`` ：

.. code-block::

   __bind_listener(Transaction1, __listeners(MyListener1, MyListener2));


如果 ``Transaction1`` 的定义如下：

.. code-block::

   __def(Transaction1) __as_trans
   ( __with_id
       ( ID_TRANS
       , __procedure
           ( __with_id
               ( ID_SEQ
               , __with_id(ID_1, __asyn(Action1))
               , __with_id(ID_2, __asyn(Action2)))
           , __finally(__rsp(Action3)))
       , __with_id(ID_4, __asyn(Action4))));


那么 ``bind_listener`` 之后， 框架发现 ``ID_1`` , ``ID_2``, ``ID_4`` 完全没有任何 ``观察者`` 关心，会立即将对应的
``__with_id`` 给优化掉。也就是说，无论从空间消耗，还是运行时性能，都完全等价于下面的 ``Transaction`` :

.. code-block::

   __def(Transaction1) __as_trans
   ( __with_id
       ( ID_TRANS
       , __procedure
           ( __with_id
               ( ID_SEQ
               , __asyn(Action1)
               , __asyn(Action2))
           , __finally(__rsp(Action3)))
       , __asyn(Action4)));


即便对于剩下的 ``__with_id`` ，如果一个 ``观察者`` 并不关注它，框架同样会知道这一点，为之生成的运行时代码里，将不会有与之有关的
任何一个指令。比如：``MyListener2`` 只关注 ``ID_TRANS`` ，而不关注 ``ID_SEQ`` ，那么当与 ``ID_SEQ`` 有关的任何事件，
框架将不会通知给 ``MyListener2`` ，内部生成的指令完全不会进行任何判断或尝试，而是从机器指令级别，就将其排除出去。

更进一步，由于 ``MyListener2`` 只关注 ``ID_TRANS`` 里的 ``onActionStarting`` ，因而，与此事件无关的任何其它事件，
比如 ``onActionDone`` 等等，也会在编译时，从机器指令的层面就消除了与之有关的任何指令。也就是说，你不会为之付出一个指令的代价。

综上所述，通过用户在定义一个 ``观察者`` 时，明确的指明自己关心的 ``Action ID`` ，框架将会保证，你无需为你不关注的事情付出任何一丁点
代价。

由此，很容易产生一个结论：对于任何一个 ``Transaction`` ，如果没有 ``观察者`` 关注它，那么其中所有的 ``__with_id`` 都会被优化掉。
因而上面的 ``Transaction1`` 无论从空间到性能，将完全等价于：

.. code-block::

   __def(Transaction1) __as_trans
   ( __procedure
       ( __asyn(Action1)
       , __asyn(Action2)
       , __finally(__rsp(Action3)))
   , __asyn(Action4));

因而，如果你的系统需要在运行时，随时关闭和打开监控类需求。那么你只需要在开关关闭时，使用没有绑定任何 ``观察者`` 的 ``Transaction``，
而在开关打开时，使用绑定了 ``观察者`` 的 ``Transaction`` 。从而，让你的系统在开关关闭时，不为之付出哪怕一个指令的代价。

