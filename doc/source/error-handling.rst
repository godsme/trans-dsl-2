错误处理
=========


Action外部行为规范
--------------------

``IDLE``
++++++++++

任何一个 ``Action`` ，单纯从外部看，在没有发生任何调用之前，Action必然处于 ``IDLE`` 状态。

而 ``IDLE`` 状态下，唯一合法的调用是``exec``，如果 ``exec`` 返回 ``CONTINUE`` 代表Action进入 ``WORKING`` 状态。
而 ``WORKING`` 的含义是，此Action需要进一步的异步消息激励。

``WORKING``
++++++++++

在 ``WORKING`` 状态下，

- ``exec`` 不可再被调用，否则应返回 ``FATAL_BUG`` ；
- 如果有事件到达，可以调用 ``handleEvent`` 进行处理；其可能结果如下：
  - ``SUCCESS`` 代表Action进入 ``DONE`` 状态；
  - 任何错误值，也代表Action进入 ``DONE`` 状态；
  - ``CONTINUE`` 代表Action依然处于 ``WORKING`` 状态；并且这条消息被Action成功的 ``accepted`` 并处理，只是还需要进一步的消息激励；
  - ``UNKNOWN_EVENT`` 表示消息并未被 ``accepted`` ；
- 如果调用 ``stop`` ，其可能结果如下：
  - 如果返回 ``CONTINUE`` ，表示Action进入 ``STOPPING`` 状态；
  - 如果返回 ``SUCCESS`` ，表示Action进入 ``DONE`` 状态；
  - 如果返回错误值，表示Action进入 ``DONE`` 状态；
- 如果调用 ``kill`` ，Action立即应进入 ``DONE`` 状态。


``STOPPING``
+++++++++++++

在 ``STOPPING`` 状态下，

- ``exec`` 与 ``stop`` 不可再被调用，否则应返回 ``FATAL_BUG`` ；
- 如果调用 ``kill`` ，应立即进入 ``DONE`` 状态
- 如果调用 ``handleEvent`` , 其可能结果如下：
  - ``SUCCESS`` 代表Action进入 ``DONE`` 状态；
  - 任何错误值，也代表Action进入 ``DONE`` 状态；
  - ``CONTINUE`` 代表Action依然处于 ``STOPPING`` 状态；
  - ``UNKNOWN_EVENT`` 表示消息并未被 ``accepted`` ；

``DONE``
+++++++++++++

在 ``DONE`` 状态下，

- ``exec`` ， ``stop`` , ``handleEvent`` 都不可再被调用，否则应返回 ``FATAL_BUG`` ；
- 如果调用 ``kill`` ，应该对Action状态无任何影响，依然处于 ``DONE``状态。


.. Hint::

   - 一个Action的 ``handleEvent`` ，只要返回 ``SUCCESS`` ，
   ``CONTINUE`` ，包括大部分错误（某些错误，比如 ``FATAL_BUG`` ，
   表示在此Action已经处于不应该再被调用 ``handleEvent`` 的状态），
   都代表这条消息被 ``accepted`` ；

   - 而返回 ``UNKNOWN_EVENT`` 则明确代表此消息没有被此Action ``accepted`` 。

   - 一个消息被 ``accepted`` ，并不代表一个消息被 ``consumed``。如果没有被 ``consumed`` ，代表此消息依然可以被其它Action处理。


Action内部行为规范
---------------------

.. tip:: 所有的 ``stop`` 只有当出错时才会返回错误。

.. note:: ``stop/kill`` 的cause，不再传递。如果用户想知道cause，可以通过 ``TransactionInfo.getStatus()`` 从
   当前运行上下文获取。

.. hint::
   这就意味着，当一个带有上下文的 ``SchedAction`` ，
   比如 ``Procedure`` , ``Void`` , ``Concurrent`` , ``Multi-Thread`` 等等，在初次被调用 ``stop/kill`` 时，
   应该从parent-runtime-env里读出状态值，并更新到自己的runtime-env里。

.. important::
   一个下层runtime-env，在正常运行时出错时，如果当前上下文并没有运行结束，则需要将错误直接report给上层runtime-env，由此层层递归，
   直到上报到最顶层，或者被 **免疫上下文** 阻断。

.. attention::
   一个下层runtime-env，在正常运行时出错时，如果当前runtime-env已经运行结束，则无需report给上层上下文，而是直接将错误返回，
   这个错误会一直返回到上一层runtime-env，由其根据自己当时的状态（结束与否），决定是直接返回，还是记录并上报。



免疫上下文: immune-runtime-context
  指 ``Void`` 或者 ``Protected Procedure`` ，它们要么完全将错误控制在自己的runtime-env内（ ``Void`` ），
  要么有修复机会（ ``Protected Procedure`` ），只有等最终无法修复时，才会将最后的错误返回。

