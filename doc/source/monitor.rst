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

每个观察者都应该实现一个名为 ``TransactionListener`` 的接口。其定义如下:

.. code-block::

   struct TransactionListener {
      virtual void onActionStarting(ActionId) {}
      virtual void onActionStarted(ActionId) {}
      virtual void onActionEventConsumed(ActionId, Event const&) {}
      virtual void onActionDone(ActionId, Status) {}
      virtual void onActionStartStopping(ActionId, Status) {}
      virtual void onActionStoppingStarted(ActionId) {}
      virtual void onActionEventConsumed(ActionId, Event const&) {}
      virtual void onActionStopped(ActionId, Status) {}
      virtual void onActionInterrupted(const ActionId) {}

      virtual ~TransactionListener() = default;
   };

这个接口的方法分为三组，共 9 个。它的默认首先均为空，每个观察者可以选择自己感兴趣的方法进行实现，忽略那些自己不关系的。

**__with_id**
----------------------

你应该已经注意到， ``TransactionListener`` 的每一个方法都有一个类型为 ``ActionId`` 的参数。
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



