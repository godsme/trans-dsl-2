错误处理
=========


Action外部行为规范
--------------------

Action从外部看，总共有4个状态，它们的状态转换关系如下图所示：

.. image:: images/ch-4/action-ext-state.png
   :align: center
   :scale: 50 %

下面我们对其进行详细说明。

.. _IDLE:

IDLE
++++++++++

任何一个Action ，单纯从外部看，在没有发生任何调用之前，Action必然处于 :ref:`IDLE` 状态。

而 :ref:`IDLE` 状态下，唯一合法的调用是``exec``，如果 ``exec`` 返回 ``CONTINUE`` 代表Action进入 :ref:`WORKING` 状态。
而 :ref:`WORKING` 的含义是，此Action需要进一步的异步消息激励。


.. _WORKING:

WORKING
+++++++++++++++

在 :ref:`WORKING` 状态下，

- ``exec`` 不可再被调用，否则应返回 ``FATAL_BUG`` ；
- 如果有事件到达，可以调用 ``handleEvent`` 进行处理；其可能结果如下：

  - ``SUCCESS`` 代表Action进入 :ref:`DONE` 状态；
  - 任何错误值，也代表Action进入 :ref:`DONE` 状态；
  - ``CONTINUE`` 代表Action依然处于 :ref:`WORKING` 状态；并且这条消息被Action成功的 :ref:`accepted <accepted>` 并处理，
    只是还需要进一步的消息激励；
  - ``UNKNOWN_EVENT`` 表示消息并未被 :ref:`accepted <accepted>` ；

- 如果调用 ``stop`` ，其可能结果如下：

  - 如果返回 ``CONTINUE`` ，表示Action进入 :ref:`STOPPING` 状态；
  - 如果返回 ``SUCCESS`` ，表示Action进入 :ref:`DONE` 状态；
  - 如果返回错误值，表示Action进入 :ref:`DONE` 状态；

- 如果调用 ``kill`` ，Action立即应进入 :ref:`DONE` 状态。

.. _STOPPING:

STOPPING
+++++++++++++

在 `STOPPING` 状态下，

- ``exec`` 不可再被调用，否则应返回 ``FATAL_BUG`` ；
- 如果调用 ``stop``，不应对Action产生任何影响，而直接返回 ``CONTINUE`` ;
- 如果调用 ``kill`` ，应立即进入 :ref:`DONE` 状态
- 如果调用 ``handleEvent`` , 其可能结果如下：

  - ``SUCCESS`` 代表Action进入 :ref:`DONE` 状态；
  - 任何错误值，也代表Action进入 :ref:`DONE` 状态；
  - ``CONTINUE`` 代表Action依然处于 :ref:`STOPPING` 状态；
  - ``UNKNOWN_EVENT`` 表示消息并未被 :ref:`accepted <accepted>` ；

.. _DONE:

DONE
+++++++++++++

在 *DONE* 状态下，

- ``exec`` ， ``stop`` , ``handleEvent`` 都不可再被调用，否则应返回 ``FATAL_BUG`` ；
- 如果调用 ``kill`` ，应该对Action状态无任何影响，依然处于 :ref:`DONE` 状态。


.. _accepted:
.. Hint::
   - 一个Action的 ``handleEvent`` ，只要返回 ``SUCCESS`` ，
     ``CONTINUE`` ，包括大部分错误（某些错误，比如 ``FATAL_BUG`` ，
     表示在此Action已经处于不应该再被调用 ``handleEvent`` 的状态），
     都代表这条消息被 **accepted** ；

   - 而返回 ``UNKNOWN_EVENT`` 则明确代表此消息没有被此Action **accepted** 。

   - 一个消息被 **accepted** ，并不代表一个消息被 **consumed** 。如果没有被 **consumed** ，
     代表此消息依然可以被其它Action处理。


错误传播方式
-----------------

错误的传播，主要有三种方式：

1. 最直接，也是最典型的，通过 **返回值** 。这发生于一个Action运行结束，进入 ``DONE`` 状态时；这属于一个从内层上下文，向外层上下文
   传播错误的方式。
2. 但一个Action内部发生错误后，并没有直接进入 ``DONE`` 模式，而是需要进一步的消息激励，因而会处于 ``WORKING`` 或 ``STOPPING``
   状态。但此错误需要立即为外界所感知，从而尽快对此错误作出响应。此时，可以通过 **运行时上下文** 的嵌套父子关系，有内层上下文直接逐级上报，
   向外传播；
3. 外层上下文由于任何原因，最典型的原因是，通过内层Action的返回值，或者内层上下文的上报，得到了一个错误，需要将错误传递给其它下层上下文。
   此时，可以通过 ``stop`` 调用，带着cause值，将错误有外向内传播。

简单的说就是：
- 由内向外传播

  - 内层Action的返回值（此时Action进入 :ref:`DONE` 状态）
  - 内层上下问向外层上下文的直接传递（此时调用返回值是 ``CONTINUE``，因而Action处于 :ref:`WORKING` 或 :ref:`STOPPING` 状态 ）

- 由外向内传播：
  - `stop(cause)`


错误处理模式
----------------

每一个可嵌套Action都有4种模式：

正常模式：Normal Mode
   错误既可以向内传播，也可以向内传播；

沙箱模式：Sandbox Mode
   - 错误不可通过 *运行时上下文* 向外传播
   - 可能允许通过 *返回值* 返回最终的错误；
   - 允许外部的错误通过 ``stop`` 传播进来；

免疫模式：Immune Mode
   - 错误不可向内传播
   - 但允许内部的错误通过 *运行时上下文* 或者 *返回值* 向外传播

孤岛模式：Island Mode
   - 同时处于 *沙箱模式* 和 *免疫模式*


每一个可嵌套Action的设计，必须遵从如下原则：

- 如果本来处于 *正常模式* ，一旦被调用 `stop` ，如果`stop` 没有导致
  其进入 :ref:`DONE` 状态，则必然进入 *免疫模式* ; 随后再次调用其 `stop` 将会被阻断，
  直接返回 ``CONTINUE`` ，而不会对其产生任何影响；
- 如果处于 *正常模式* 或 *免疫模式* ，在内部发生错误后，如果随后不能立即结束，则必须通过 *运行时上下文* 及时上报错误；
- 一旦通过 *运行时上下文* 上报过一次错误，则随后再发生的错误，禁止再通过 *运行时上下文* 上报。这就意味着，
  进入了 *沙箱模式* （从 *正常模式* ）或 *孤岛模式* （从 *免疫模式* ）。


stop的设计原则
----------------------------------------

``stop`` 的处理原则如下：

- 如果 ``stop`` 并没有导致一个Action处理失败，即Action依然完成了它本来的职责，则依然返回 ``SUCCESS`` ；
- 如果 ``stop`` 本身没有失败，但Action并没有完成它本来应该完成的任务，则返回 ``FORCE_STOPPED`` 。
- 如果 ``stop`` 导致了的其它失败，则返回其它错误；


如果一个action被stop，内部处于：
- 可以stop状态
- 不可stop状态

stop有可能并不会引起任何后果：
比如，自己的运行时环境已经出了错。或者，正在处于不可stop状态。

对于可以stop的状态，如果内部的stop过程：
- 没有出错，则返回 ``FORCE_STOPPED``
- 除了错，则返回对应的错误

对于action内部，应该可以查询两个状态：

- 是否被外界 ``FORCE_STOPPED`` （以及Stop Cause？) （上层runtime-context state?)
- 是否有内部错误。

User Defined Async Action
++++++++++++++++++++++++++++++++

stop 可能返回如下：
1. ``FORCE_STOPPED``
2. failure
3. ``SUCCESS`` (stop might not produce an error?)

Sequential
++++++++++++++++++++++++++++++++

当前序列中的Action被stop之后，可能返回3中结果:

1. ``SUCCESS`` -> ``FORCE_STOPPED`` (what about last one return SUCCESS?)
2. failure -> failure
3. ``FORCE_STOPPED`` -> ``FORCE_STOPPED``

Procedure
++++++++++++++++++++++++++++++++

- 运行在normal part

  - stop, wait until it finishes, record its return-value to its own runtime-context.
    so that final part could read it.
  - stop状态下的错误，无需马上report to upper runtime-context，要看最后的final结果。

- 运行在final part

  - 不可能接受stop，直接返回 ``CONTINUE`` ；
  - 因而最后可能会返回 ``SUCCESS`` 。

Concurrent
++++++++++++++++++++++++++++++++

如果内部某个线程失败，会导致对所有其它正在运行的线程发起stop。自身转入 :ref:`STOPPING` 状态。此时，如果外界再对其进行 ``stop`` ，
将立即返回 ``continue`` 而不会将 ``stop`` 继续传播。

对于每一个线程，如果结束时返回的是 ``FORCE_STOPPED`` 。 而 ``concurrent`` 记录的last error已经是个错误值，
则 ``FORCE_STOPPED`` 不应该替换之前的错误值 。

在正常运行下，某个线程内部是否会返回 ``FORCE_STOPPED`` ? 此时应该当作正确还是错误？

如果一个 ``concurrent`` 的确是因为外部发起的stop，而导致进入 :ref:`STOPPING` 状态，而此 `stop` 继续向下传播，
但最终的结果所有的线程都返回 ``SUCCESS``， 是否，也应该返回 ``SUCCESS`` 。
如果其中有一个或多个返回 ``FORCE_STOPPED`` ， 但其它的都返回 ``SUCCESS`` ， 应该返回 ``FORCE_STOPPED`` 。

Loop
++++++++++++++++++++++++++++++++

Loop接收到 ``stop`` ，将会返回当前Action的 ``stop`` 之后，结束时的返回值，如果为 ``SUCCESS`` ，则返回 ``FORCE_STOPPED`` 。


错误向上传播
----------

一个运行时环境内，如果发生了一个错误，这个错误导致运行时环境所对应的 Action 进入 :ref:`STOPPING` 状态
（也可能:ref:`WORKING`状态？），如果此环境不是一个 沙箱环境 ，则此错误应及时向上传播。

``FORCE_STOPPED`` 不应向上传播？

在 :ref:``STOPPING`` 状态下的错误不应向上传播？

这个传播，主要会影响到的行为是 `concurrent` 与  `multi-thread` 。会导致它们马上发起对其它处于 :ref:`WORKING` 状态
线程发起 `stop` 。

对于 `procedure` 无影响。因为它仍然会按照其运行逻辑继续进行。


Procedure
+++++++++++++

错误发生在：

- Normal Action
- Final Action
  - 仍然应该向上传播，既然这个错误最终也会为外界所知。
