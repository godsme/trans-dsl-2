快速开始
============

我们先通过一系列简单的例子，来对 `Transaction DSL` 有一个直观的认识。

如图所示，站在 `US` 的角度，一个完整的事务操作，共分为 5 个步骤:

  1. 从 `Other System 1` 收到一条请求消息;
  2. 对 `Other System 2` 进行了一个函数调用;
  3. 对 `Other System 3` 发送了一条异步消息，并等待其应答消息;
  4. 对 `Other System 4` 发送了一条异步消息，并等待其应答消息;
  5. 给 `Other System 1` 发送了一条应答消息。

.. image:: images/ch-2/seq-1.png
   :align: center
   :scale: 50 %

定义事务
--------------

使用 `Transaction DSL` ，我们将上面序列图中的过程定义为:

.. code-block:: c++

  __transaction (
    __sequential
      ( __asyn(Action1)
      , __sync(Action2)
      , __asyn(Action3)
      , __asyn(Action4)
      , __sync(Action5))
  );

这段代码定义了一个名为 ``Transaction`` 的事务。它由一个包含了 5 个步骤的 **顺序操作** （Sequential Action）构成。
每个步骤都是一个 **基本操作** （`Atom Action`）。

**基本操作** 共分为两类： **同步操作** ( `Synchronous Action` )和 **异步操作** ( `Asynchronous Action` )。

任何无需进一步等待后续消息的 `Action` ，都称作 **同步操作** 。这包括：函数调用，以及只发消息、不等回应的操作。
它们都需要通过 ``__sync`` 来指明。但为了更加清晰的区分究竟一个同步操作属于哪种具体类型，你可以使用 ``__call`` 来说明一个操作是函数调用，
用 ``__ind`` 来说明这是一个发送指示消息的操作，用 ``__rsp`` 来说明一个操作是对之前某个请求的一个应答。

任何需要等待异步消息的操作，都称作 **异步操作** 。这包括：典型的 **请求——应答** 操作，消息触发的操作等。
而异步操作则需要通过 ``__asyn`` 来说明。同样的，你可以使用其它一些更加具体的修饰来明确一个异步操作的类型。
比如：你可以使用 ``__req`` 来说明这是一个消息触发型的请求操作。

所以，之前的例子可以修改为：

.. code-block:: c++

  __transaction (
    __sequential
      ( __req(Action1)
      , __sync(Action2)
      , __asyn(Action3)
      , __asyn(Action4)
      , __rsp(Action5))
  );

现在，我们可以看出，这段代码是对上述序列图简单而直接的描述。

定义基本操作
----------------------------

为了让上述代码可以工作，你需要实现其中的基本操作。事实上，无论一个事务有多么复杂，最终总是由基本操作组成，
你需要做的事情就是定义这些基本操作，然后使用 `Transaction DSL` 来描述它们运行的方式和顺序。

我们之前已经提到，基本操作分为两种： **同步操作** 和 **异步操作** 。定义它们时需要实现带有如下方法的类。

.. code-block:: c++

    Status exec(const TransactionInfo&);

.. code-block:: c++

    Status exec(const TransactionInfo&);
    Status handleEvent(const TransactionInfo&, const Event&);
    void kill(const TransactionInfo&, Status cause);

对于 **同步操作** ，你需要做的就是定一个原型为 ``Status (const TransactionInfo&)`` 的函数。

如果你通过类来定一个同步操作，你需要做的就是在你的类中定义一个名为 ``exec`` 的方法。

对于函数调用型的同步操作，其实现非常简单，其返回值为 ``SUCCESS`` 代表此操作成功，如果返回错误值则表示此操作失败。

.. code-block:: c++

   Status Action2(const TransactionInfo&) {
     return OtherSystem::func();
   }

甚至可以是一个 `lambda` ：

.. code-block:: c++

   auto Action2 = [](const TransactionInfo&) -> Status {
     return OtherSystem::func();
   }

对于 ``Action5`` ，尽管它发送了消息，却无需等待任何消息，所以它也是一个 **同步操作** 。
虽然也可以直接用函数直接定义，但基于举例的目的，这次我们用类来定义它：

.. code-block:: c++

  struct Action5 {
    Status exec(const TransactionInfo&) {
      // 构建并发送消息
      Response1 response;
      response.build();
      return sendResponseTo(OTHER_SYSTEM1_PID, response);
    }
  };

而对于 **异步操作** ，存在一些常用的模式。大多数情况下，你可以根据模式从已存在的基类中继承。
比如：如果一个异步操作属于简单的 *请求-应答* 模式，你只需要从 ``SimpleAsyncAction`` 继承即可。

.. code-block:: c++

   DEF_SIMPLE_ASYNC_ACTION(Action3) {
     Status exec(const TransactionInfo&) {
       // 构建并发送请求消息
       Request3 request;
       request.build();
       Status status = sendRequestTo(OTHER_SYSTEM3_PID, request);
       if(status != SUCCESS) return status;

       // 声明自己要等待的应答消息类型，以及对应的处理函数；WAIT_ON会返回CONTINUE
       return WAIT_ON(EV_ACTION3_RSP, handleAction3Rsp);
     }

   private:
     // 定义事件处理函数
     Status handleAction3Rsp(const TransactionInfo&, const Event& event) {
       // 处理应答消息
       handleRsp(event);
       // 返回成功，代表此 Action 成功处理结束
       return SUCCESS;
     }
  };

而 ``Action1`` 则属于一个事件触发的操作，所以它不发送消息，只等待那么触发消息。但它仍然可以继承自 ``SimpleAsyncAction`` 。

.. code-block:: c++

   DEF_SIMPLE_ASYNC_ACTION(Action1) {
     Status exec(const TransactionInfo&) {
       // 声明自己要等待的消息类型，以及对应的处理函数
       return WAIT_ON(EV_ACTION1_REQ, handleAction1Req);
     }

   private:
     // 定义事件处理函数
     Status handleAction1Req(const TransactionInfo&, const Event& event) {
       // 处理触发消息
       handleReq(event);
       // 返回成功，代表此 Action 成功处理结束
       return SUCCESS;
     }
  };

对于 **异步操作** 的所有函数，其返回值有三种: ``SUCCESS`` 表示此操作成功结束; ``CONTINUE`` 表示此操作尚未结束，需要进一步的处理；
**错误值** 则表示此操作已经失败。

而函数 ``handleEvent`` 则存在一种额外的返回值： ``UNKNOWN_EVENT`` ，说明当前消息不是自己期待的消息。

.. list-table::
   :widths: 25  50
   :header-rows: 1

   * - 返回值
     - 语意
   * - **SUCCESS**
     - Action 成功结束
   * - **CONTINUE**
     - Action 仍然在工作
   * - **UNKNOWN_EVENT**
     - Action 收到一个未期待的消息
   * - 错误码
     - Action 失败，并已经中止

约束
-----

用户自定义的基本操作，如果通过类来定义， `Transaction DSL` 要求它们必须是自满足的。
即，它们不需要外部通过 **构造函数** 或 ``set`` 函数设置任何外部依赖。所有的依赖，都需要靠类自身到环境中亲自寻找，或亲自创建。
所以，这些类必须存在 **默认构造函数** 。至于其它带参数的 **构造函数** 或 ``set`` 接口，虽然其存在并不会妨害 `Transaction DSL`
的编译和运行，但它们永远也不会得到调用。

这样的约束，并不会对设计造成任何妨害。因为这些类本来就靠近系统的边界。而边界的代码本身就应该承担寻找或创建目标对象的职责。


运行
------

现在我们有了 `Transaction`，有了基本操作，一个事务就完整了。由于 `Transaction` 是一个事件驱动的组件。它的基本接口定义如下:

.. code-block:: c++

   Status start();
   Status start(const Event& event);

   Status handleEvent(const Event& event);


所以，你可以选择任何一个 ``start`` 接口来启动一个事务。像一个异步操作一样， 如果其返回值是 ``SUCCESS`` ，
说明此事务已经成功的执行；如果其返回值是一个 **错误值** ，则说明此事务已经失败；
而如果其返回了 ``CONTINUE`` ，则说明此事务正在工作状态中，尚未结束，仍然需要进一步的消息激励。

在 ``start`` 接口返回 ``CONTINUE`` 的情况下，随后每次系统收到一个消息，都需要
调用其 ``handleEvent`` 接口，直到其返回 ``SUCCESS`` 或一个 **错误值** 为止。

一个可能的实现如下所示:

.. code-block:: c++

   Status runTransaction() {
     // 将之前定义的 Transaction 实例化 Transaction trans;
     // 启动
     Status status = trans.start();
     if(status != CONTINUE) return status;

     // 消息处理循环
     while(recvEvent(event) == SUCCESS) {
       status = trans.handleEvent(event);
       if(status != CONTINUE && status != UNKNOWN_EVENT) {
          return status;
       }
     }

     return FAILED;
   }

这个实现是一种简单的处理，主要为了说明一个事务的运行方式。事实上，事务往往不是一个系统的顶层框架，
一个事务仅仅是对一个处理过程的描述。在复杂系统中，事务与事务之间可以并发，可以抢占。
但那是事务框架之外的事情。在这里我们就不在详细讨论。


并发
------

一旦系统因为性能要求，需要同时给不同其它系统/子系统发出请求消息，并同时等待它们的应答，如图所示。

.. image:: images/ch-2/concurrent.png
   :align: center
   :scale: 50 %

在这个例子中， ``Action3`` 和 ``Action4`` 同时给各自的目标系统发出请求消息，并各自等待应答。
这种情况下，简单的策略已经无法处理，实现者仍然不得不回到状态机模型中。

使用 `Transaction DSL` ，一个并发过程的定义非常简单，如下所示：

.. code-block:: c++

   __transaction
   ( __sequential
      ( __req(Action1)
      , __sync(Action2)
      , __concurrent(__asyn(Action3), __asyn(Action4))
      , __rsp(Action5))
   );


我们只需要将 ``Action3`` 和 ``Action4`` 放入一个叫做 ``__concurrent`` 的盒子里即可。
它会保证两者可以得到并发的执行，并发的等待应答，并确保，只有在 ``Action3`` 和 ``Action4`` 都执行结束后，才会执行 ``Action5`` 。

哦...事实上，最后这条不是它单独保证的，因为 ``__concurrent`` 这个盒子放在了更大的盒子 ``__sequential`` 里面，
在 ``__sequential`` 看来，它里面有四个操作：分别是 ``Action1`` , ``Action2`` , ``__concurrent`` 和 ``Action4`` ，
它会来保证这四个操作严格的按照顺序来执行。


时间约束
------------

既然是异步系统，那么发出去的消息就有可能由于各种原因而一去无回；或至少，很晚才回来。
为了避免系统被这样的情况挂死，或者不满足实时性要求，设计者往往会对异步过程有着时间的约束；即在一定时间之内，如果一个操作无法完成，
则当前操作就失败。

下图所示的过程就属于这样的情况。其中，存在两个时间约束：首先，整个操作必须在 ``250ms`` 之内完成，而其中的并发异步过程则必须
在 ``200ms`` 之内完成。

.. image:: images/ch-2/timer-seq.png
   :align: center
   :scale: 50 %

我们将这个带有时间约束的事务描述如下：

.. code-block:: c++

    const TimerId TIMER_1 = 1;
    const TimerId TIMER_2 = 2;

    __transaction
    ( __time_guard(TIMER_1, __sequential
        ( __req(Action1)
        , __sync(Action2)
        , __time_guard(TIMER_2, __concurrent(__asyn(Action3), __asyn(Action4)))
        , __rsp(Action5)))
    );


母语言
```````

在这段代码里，我们首先定义了两个 ``TimerId`` : ``TIMER_1`` 和 ``TIMER_2`` 。其定义的方式是 `C++` 的常量。
为什么这里可以使用 `C++` 的语法?

是的，`Transaction DSL` 本身是就是 `C++` 的代码，它可以被任何成熟的 `C++` 编译器编译。
它是 `C++` 代码这个事实，让它可以在需要时，使用任何 `C++` 的元素。

母语法
```````

它采取的语法形式非常接近于 `Lisp` ，除了两点较大的差别:

1. `Lisp` 将一个 `list` 的名字放在括号内;
2. `Lisp` 一个 `list` 内的各个元素之间无需逗号分割。

如果你以前没有接触过 `Lisp` 语言，可通过下面一段 `Lisp` 的例子代码获取直观的印象。

.. code-block:: lisp

  (defun find-books (towns)
     (if (null towns) nil
       (let ((shops (bookshops (car towns))))
       (if shops (values (car towns) shops)
                 (find-books (cdr towns))))))


无论你是否喜欢 `Lisp` 语言的语法形式，但对于我们的问题，这已经是我所能找 到的 `C++` 元编程的最好表现形式。
但两者相似的地方不仅仅是语法形式，更重要的在于其背后的 **元模型** 。对于 `Lisp` 语言而言，一切都是 `List` ，
所以，它本身就在表述一颗 **语法树** 。

而对于 `Transaction DSL` 而言，一切都是 `Action` ，
它本身就在表述一颗 `Action Tree` ，在这颗树上，每一个节点所代表的子树都是一个 `Action` 。
而在叶子节点，就是用户所定义的 **同步** 或 **异步** 的 `Action` (操作)。

Timer ID
```````````````

你已经看到，我们并没有在 `Transaction` 中直接写 ``200ms`` 或 ``250ms`` ，而是使用了 `Timer ID` 。

将时间约束的数值直接写在 `Transaction` 里，从实现上并无难度，但是，这将导致用户将失去运行时修改它的能力。
而用户很有可能希望某些时间约束是可配置的，从而在运行时是可修改的。

所以，我们需要定义一个标识来定义这个约束，然后，Timer 管理器会将这个 TimerID 和某项配置关联起来
（你当然也可以 hard coding 将其写死）。

另外，无论 `Transaction DSL` 被移植到什么平台上，都必须要满足:

- 在 `Timer` 过期时，必须发送一条消息出来;
- `Timer` 的实现必须能够将 ``Timer ID`` 和 `Timer` 过期消息之间建立起一一映射关系。
  而这一切，在我所见过的各种 `Timer` 机制中，都不难实现。

同步操作的时间约束
````````````````````````````````

虽然你可以在一段 `Transaction DSL` 代码中，对一个同步操作进行时间约束，但事实上，这个约束形同虚设。

这是因为，一旦进入一个 **同步操作** 的执行，事务框架就失去了控制权。所以，它无法在定时器过期时，抢占或打断一个同步操作的执行。

而 **异步操作** 则不同，由于它们需要等待消息激励，在等待期间，事务掌握着控制权，当收到其定时器过期消息时，事务可以马上中止其运行。

回到最初
----------------

现在，我们可以使用 Transaction DSL 来对我们最初的过程进行描述:

.. code-block:: c++

  __transaction (
    __sequential
      ( __asyn(ApplicationAcceptance)
      , __concurrent
          ( __asyn(BackgroundInvestigation)
          , __sequential
              ( __asyn(Exam)
              , __asyn(Interview)))
      , __asyn(OfferNegotiation)
      , __time_guard(TIMER_ONBOARD, __asyn(OnBoard)))
   );


剩下的事情，就是把每一个 **基本操作** 进行实现，而它们都是非常简单，原子级别的交互过程。

优势
--------

在本章中，我们初步了解了如何使用 `Transaction DSL` 来定义一个异步过程。 即，实现 **原子操作** ：分为 **同步** 和 **异步** 两种。
然后通过 `Transaction DSL` 来描述这些 **基本操作** 的执行方式。

通过这些例子，我们可以看到 ``Transaction DSL`` 的一些主要优势:

直观:
   `Transaction DSL` 的描述，和 `UML` 序列图的描述有着明确直观的映射关系。设计和实现之间的 gap 被大大缩小。

简单:
   没有复杂的事无巨细的状态机，没有重复繁琐的定时器操作，没有亲力亲为的并发过程控制——开发这些代码是一个无趣的，
   重复的，容易出错的，给开发和维护都带来大量成本过程。

   不过你仍然需要写一些代码——那就是——将序列图没有冗余的描述一次。 但...please...不要告诉我，
   你打算开发一个图形界面，将 `UML` 序列图自动转化为 `Transaction DSL` ，这样就可以连这些代码也不用写了。

   毫无疑问，这将是一笔极其糟糕的投资，你应该将你过剩的精力花在真正有价值的事情上。

事务与操作的分离:
   到目前为止，你可能已经注意到，虽然我们的例子从最初的简单顺序执行，到后来改变为并发执行，最后，我们又增加了时间约束，
   但我们却没有修改基本操作的一行代码。我们只是在修改事务的代码——以一种非常简单、快速的方式。

Native Language:
   `Transaction DSL` 本身就是 `C++` 代码，这就意味着它可以和 `C++` 的其它代码元
   素自由的结合使用，以和其它的 `C++` 代码进行无缝的配合。

图灵完备:
   随着后续章节的介绍，你就可以发现， `Transaction DSL` 是图灵完备的。也就是说它可以解决一切图灵可计算问题。
