
过程控制
=============

在一个较为复杂的事务中，一些操作，只有在一定的条件下才会执行；或者，在不同的条件下执行的时机不同。

如图所示的过程中，Action2 就是一个可选的操作，它只有在系统的某个开关打开的情况下才会执行。


**__optional**
-------------------

对于这样的情况，可以使用 ``__optional`` 来描述:

.. code-block:: c++

   bool ShouldExecAction2(const TransactionInfo&) {
      return SystemConfig::shouldExecAction2();
   }

   __sequential
     ( __req(Action1)
     , __optional(ShouldExecAction2, __asyn(Action2)) , __asyn(Action3)
     , __asyn(Action4)
     , __rsp(Action5));


从代码中可以看出， ``__optional`` 有两个参数：第一个参数是一个谓词，即一个返回值是一个 `bool` 的函数，
如果为真，则 ``__optional`` 的第二个参数: 一个 Action 将会得到执行。这是 ``C++`` 解决这类问题的常用手段。

如果谓词是一个 ``仿函数`` ，即一个类，Transaction DSL 对其有着和基本操作一样的约束，即它也必须是自满足的。
所以，它必须亲自访问环境来确定一个条件是否成立。

**TransactionInfo**
+++++++++++++++++++++

无论是谓词还是基本操作，这些需要用户定义的类，都必须是子满足的，所以， 它们自身计算所需的信息都必须亲历其为的到环境中查找。
由于 Transaction 自身也是环境的一部分，所以 Transaction 必须通过参数将自身的信息传递给基本操作或谓词，
从而让它们有能力得到一切需要的信息，这就是 TransactionInfo 的由来。

TransactionInfo 是一个接口类。通过它，你首先可以获取到实例标识（Instance ID）。
因为有些系统对于同种类型的领域对象会创建多个实例，而每个实例都可 能会有自己的 Transaction；
通过 Instance ID，用户定义的类就可以知道当前的 Transaction 属于哪个实例。

另外，Transaction 会通过 TransactionInfo 告知自身的运行时状态：是成功还是失败，如果失败，是什么原因导致的失败等等信息。

路径选择
------------

现在我们让事情更为复杂一些。

在图所示的事务中，Action2 即可以在 Action3 之前运行，也可以在它之后运行，究竟选择哪种方式，取决于相应的配置。


.. code-block:: c++

   bool IsAction2RunFirst(const TransactionInfo& info) {
    Object* object = Respository::findInstance(info.getInstanceId());
    return object->isAction2RunFirst();
   }

   __sequential
     ( __req(Action1)
     , __optional(IsAction2RunFirst,
         __sequential
           ( __asyn(Action2)
           , __asyn(Action3)))
     , __optional(__not(IsAction2RunFirst),
         __sequential
           ( __asyn(Action3)
           , __asyn(Action2)))
     , __rsp(Action4));

首先，在这个例子中，谓词的实现使用了 TransactionInfo 来获取 Instance ID， 进而通过它找到了对象，从而完成了判断。

其次，我们使用了两个顺序的 ``__optional`` 来进行路径选择，这两条路径是互斥 的。而互斥的保证是通过 ``__not`` 来完成的。

**__switch**
+++++++++++++++++

当一段流程存在多条路径选择时，我们可以选择使用多个 ``__optional`` 来决定执行路径。

但是，我们从例子中同样可以看出，每个 ``__optional`` 都必须有自己的谓词，当谓词是一种互斥关系时，
我们需要在不同的 ``__optional`` 里，使用 ``__not`` 来描述这种 互斥。无疑，这会让事务程序员觉得麻烦。

另外，多个并列的 ``__optional`` 无法让事务代码的阅读者直观的看出这些路径之间的互斥关系。

所以，当存在多条互斥的路径时，最好应该使用 ``__switch`` 来描述:

.. code-block:: c++

   __sequential
     ( __req(Action1)
     , __switch( __case( IsAction2RunFirst,
                   __sequential
                     ( __asyn(Action2)
                     , __asyn(Action3)))
               , __otherwise(
                  __sequential
                     ( __asyn(Action3)
                     , __asyn(Action2))))
    , __rsp(Action4));


从代码中可以看出，在一个 ``__switch`` 里，一条路径可以使用 ``__case`` 来描述， 而 ``__case`` 则和 ``__optional`` 一样，
存在两个参数：谓词和操作。

当存在多条路径时，``__case`` 的顺序则非常重要：Transaction DSL 会按照顺序依次匹配，一旦找到一条路径，将会执行其操作，
并忽略其它路径，即便其它路径的 谓词也可能匹配。

如果所有的 ``__case`` 谓词都不匹配，则 ``__switch`` 会返回事务的当前状态。 ``__otherwise`` 则是一个语法糖，
用来描述无条件匹配。所以，它应该作为一个 ``__switch`` 的最后一条路径，否则，在它之后的任何 ``__case`` 都不会得到调用。

``__switch`` 要求至少两条路径选择。如果只存在一条路径时，使用 ``__optional`` 。

找到合适的描述方式
+++++++++++++++++++++++++++

在一个通用编程语言中，在面临路径选择时，你可以找到多种等价的描述方式。为了让程序简洁，直观，我们应该选择最恰当的那一种。

同样的，对于本例，我们可以找到它的等价描述方式。如图所示:


.. code-block:: c++

  __sequential
    ( __req(Action1)
    , __optional(IsAction2RunFirst, __asyn(Action2))
    , __asyn(Action3)
    , __optional(__not(IsAction2RunFirst), __asyn(Action3)) , __rsp(Action4));


异常处理
----------

一个事务是一个不可分割的操作，它或许会包括多个步骤，但这些步骤要么全部成功，要么全部失败。

所以，一个事务从开始到结束，中间发生任何错误，都会导致整个事务的失败。一旦一个事务失败，
就会执行回滚(rollback)操作，以将系统恢复到事务开始前的状态。

当整个事务成功执行后，需要执行提交(commit)操作，自此，整个事务对于系统的改动才算真正生效。在提交后，整个系统无法再通过事务的回滚操作恢复系统的状态。

Transaction DSL 提供了同样的机制：使用 Transaction DSL 定义的任何事务， 在运行时，如果中间某个操作发生了错误，则整个事务就进入失败的状态。

但不幸的是，对于一个具体的，由用户自己定义的事务而言，Transaction DSL 无从得知，当失败时，应该执行的具体回滚机制是什么。
所以 Transaction DSL 无法提供自动的回滚策略。或许对于某些系统，确实存在统一的模式，但另外一些系统则不然。

而在 Transaction DSL 的层面，则只能提供相应的机制；如果某些系统确实存在统一的回滚策略，
则可以利用这些机制在 Transaction DSL 之上层面进行统一定义。

如果没有统一的策略，同样可以利用 Transaction DSL 所提供的机制定义差异化的回滚操作。

**__procedure**
+++++++++++++++++++++

Transaction DSL 提供了 ``__procedure`` 来定义一个过程，无论这个过程中的所有操作全部成功，
还是执行到某一步时发生了失败，都会进入结束模式。用户可以自己定义结束模式里应该执行的操作是什么。
如果按照之前对于事务的描述，则用户可以在结束模式里根据过程进入结束模式时的状态，进行提交或回滚操作。

所以，``__procedure`` 包含了两个参数：第一个参数是此过程应该执行的正常操作，
第二个的参数则是以 ``__finally`` 修饰的结束模式中应该执行的操作。

比如，对于 ``__optional`` 中的例子，如果系统要求此事务无论成败最终都应该执行 ``Action5`` ，
但如果失败的话则需要对之前的操作进行回滚。我们就可以将其描述为：

.. code-block:: c++

  __procedure
    ( __sequential
        ( __req(Action1)
        , __call(Action2)
        , __concurrent(__asyn(Action3), __asyn(Action4)))
    , __finally
        ( __sequential
            ( __rsp(Action5)
            , __on_fail(__syn(Rollback)))));

