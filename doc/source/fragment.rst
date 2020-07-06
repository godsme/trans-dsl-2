片段
=======

到目前为止，我们给出的例子虽然并不非常复杂，但某些例子的描述已经开始显的不太清晰。

而良好软件设计的核心原则之一就是 **清晰性** ，对于其它编程语言而言，满足清晰性的一个重要措施就是允许用户定义更小的，职责更加单一的小函数，
或者小类。

而通过 `Transaction DSL` ，你同样可以将一个复杂过程的任何一个局部进行提取，以定义更小的，职责更加单一的子操作： **片段** (`Fragment`)。

**__def**
-----------------

定义一个片段的方式，就是通过 ``__def`` 。比如:

.. code-block::

   __def(Fragment) __as
   ( __asyn(Action1)
   , __asyn(Action2));

从例子中可以看出，一个片段的定义包含两个部分：首先，``__def`` 的参数，定义了一个 **片段** 的 **名字** ；
然后在 ``__as`` 里，定义了这个片段所执行的操作。

之所以会有 ``__as`` ，主要是作为一门以 `C++` 为母语言的 DSL，在 `C++` 的语法范围内，尚未找到一种更好的实现方法。

**__apply**
------------------

一旦一个 **片段** 被定义，就可以在另外一段操作中引用它。比如:

.. code-block::

    __transaction
    ( __asyn(Action1)
    , __apply(Fragment));


现在，我们就可以将 procedure 中的例子的两个片段提取出来:

.. code-block::

   __def(Fragment1) __as
   ( __asyn(Action1)
   , __asyn(Action2))
   , __finally(__rsp(Action3)));

   __def(Fragment2) __as
   ( __asyn(Action5)
   , __finally(__sync(Action6)));


最后，我们的事务描述就变成了下面的样子。和之前的例子对比一下，是否清晰了许多。

.. code-block::

   __transaction
   ( __apply(Fragment1)
   , __asyn(Action4)
   , __apply(Fragment2));

**__params**
--------------------

对于软件设计而言，另外一个重要的指标是 **可重用性** 。之前定义的子操作，虽然最早的出发点是为了清晰性，但事实上，也间接的让这些片段变得可以复用。

但一旦到了复用的层次，我们就需要更加强大的设施。比如，存在如下两个事务:

.. code-block::

  __transaction
  ( __time_guard
      ( TIMER_1
      , __asyn(Action2)
      , __asyn(Action3))
  , __asyn(Action4));

  __transaction
  ( __concurrent
      ( __asyn(Action1)
      , __time_guard
          ( TIMER_2
          , __asyn(Action4)
          , __asyn(Action3))));

稍微仔细的观察，可以发现，两个事务有一段非常相似的过程:

.. code-block::

  __time_guard
  ( TIMER_1
  , __asyn(Action2)
  , __asyn(Action3));

  __time_guard
  ( TIMER_2
  , __asyn(Action4)
  , __asyn(Action3));

两段子操作看起来很相似，但中间有两处细节是不同的。对于这类问题，我们可以将它们合二为一，然后将差异参数化:

.. code-block::

   __def(Seq2, __params(__timer_id(TIMER), __action(ACTION))) __as
   ( __time_guard
       ( TIMER
       , __asyn(ACTION)
       , __asyn(Action3)));

在这个例子中，我们定义了一个名为 ``Seq2`` 的片段， ``__params`` 里放置的是它的两个参数： ``TIMER`` 和 ``ACTION`` 。
前者的修饰 ``__timer_id`` 说明参数是一个 ``Timer ID`` ， 后者则由 ``__action`` 修饰，说明它是一个操作。
这些修饰就相当于其它编程语言里的 参数类型说明。

这两个参数随后在 ``__as`` 里所定义的操作里得到了使用，其中 ``TIMER`` 被用在了 ``__time_guard`` 里，
而 ``ACTION`` 则用在了第一个 ``__asyn`` 里。

**__with**
-----------------

调用一个带参数的片段，同样是使用 ``__apply`` ，不过，需要通过 ``__with`` 来指明实参。
现在，我们就可以将之前的两个事务修改为:

.. code-block::

  __transaction
  ( __apply(Seq2, __with(TIMER_1, Action2))
  , __asyn(Action4));

  __transaction
  ( __concurrent
      ( __asyn(Action1)
     , __apply(Seq2, __with(TIMER_2, Action4)));


