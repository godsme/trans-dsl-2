.. _loop:

循环
=====

循环的控制可以非常灵活。比如，在 `C/C++` 语言中，循环的典型语意有：

.. code-block:: c++

   /////////////////////////////
   do {
     // ...
   } while(cond);

   /////////////////////////////
   while(cond) {
     // ...
   }

   /////////////////////////////
   for(init_list; cond; op) {
     //
   }

同时，在任意循环里，可以随时随地进行两种循环控制：

``continue`` ：
   即重新回到循环的起点开始运行。
``break`` :
   中断当前的循环，或从当前的循环跳出。

程序员可以通过上述的语意，组合出来任意复杂的循环控制。

.. attention::
   一个可以控制任意复杂循环控制的基本要素包括：

   1. 提供一种或多种循环定义方式：比如，``while(1){}`` 或者 ``do{}while(1)`` ；
   2. 用户可以在循环里的任何位置，根据当时的条件决定终止循环；即 ``if(cond) break`` 。其实 ``while(cond)`` 本身
      的语意等价于 ``if(!cond) break``。因而 ``while(cond){}`` 和 ``do{}while(cond)`` 都是语法糖。
   3. 用户可以在循环里的任何位置，根据当时的条件决定重新循环；即 ``if(cond) continue`` 。
   4. 用于循环控制的状态信息：最典型的例子是计数器。这些状态信息，在整个循环进行期间，必须要保证其生命周期的连续性。


**__loop**
---------------

当程序员需要在一个Transaction里构建一个循环子过程时，可以使用 ``__loop(...)`` 来进行定义。这也是 `Transaction DSL` 所提供
的唯一一种循环定义方式，它相当于 `C/C++` 中的 ``while(1){}`` 。

.. code-block:: c++
   :linenos:

   __loop
   ( __sync(Action1)
   , __async(Action2)
   , __time_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   );

这是一个死循环，它自身永远也不会终止，即便内部的某些 `Action` 出现运行时错误，它也不会停止循环。
要想终止它，只能依靠 ``stop`` 或 ``kill``。

因而，我们必须提供其它机制，让循环可以自然的终止。

**__break_if**
--------------

``__break_if`` 大致相当于 `C/C++` 的 ``if(cond) break`` ，即当 ``cond`` 所设置的条件得到满足时，即可立即跳出循环。

.. code-block:: c++
   :linenos:

   __loop
   ( __sync(Action1)
   , __async(Action2)
   , __time_guard(TIMER_2, Action3)
   , __break_if(__is_timeout)
   , __concurrent(__async(Action3), __async(Action5))
   );

这个例子中，如果在 ``__break_if`` 之前，发生了 ``TIMEDOUT`` 错误，则循环终止，而整个 ``__loop`` 的运行结果也是 ``TIMEDOUT`` 。

当然，如果用户想在结束循环的同时，让 ``__loop`` 的运行结果为另一个错误值，则可以明确进行指定：

.. code-block:: c++
   :linenos:

   __loop(
   , __sync(Action1)
   , __async(Action2)
   , __time_guard(TIMER_2, Action3)
   , __break_if(__is_timeout, SUCCESS)
   , __concurrent(__async(Action3), __async(Action5))
   );

这样， ``__loop`` 的运行结果将是 ``SUCCESS`` 。

**__while**
+++++++++++++++++

事实上，正如我们之前所讨论的，我们可以使用 ``__loop`` 和 ``__break_if`` 描述 ``while(cond) {}`` 和 ``do{} while(cond`` ：

.. code-block:: c++
   :linenos:

   __loop( __break_if(__not(CondSatisfied))
   , __sync(Action1)
   , __async(Action2)
   , __time_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   );

   __loop
   ( __sync(Action1)
   , __async(Action2)
   , __time_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   , __break_if(__not(CondSatisfied))
   );

也就是说，我们只需要将 ``__break_if(__not(cond))`` 放在 ``__loop`` 的最前面和最后面，
即等价于 ``while(cond){...}`` 和 ``do{...}while(cond)`` 。

为了表达的更加直观， `Transaction DSL` 提供了一个语法糖： ``__while(cond)`` ，其等价于 ``__break_if(__not(cond))`` 。

.. code-block:: c++
   :linenos:

   __loop( __while(CondSatisfied)
   , __sync(Action1)
   , __async(Action2)
   , __time_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   );

   __loop
   ( __sync(Action1)
   , __async(Action2)
   , __time_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   , __while(CondSatisfied)
   );

当然， ``__while`` 也可以指定循环结束时的返回值： ``__while(cond, FAILED)`` ，
如果不指定，循环结束时，则会返回循环所处的 **运行时环境** 的状态。

**__until**
++++++++++++++++

`Transaction DSL` 所提供的另外一个语法糖是 ``__until`` ，它完全等价于 ``__break_if`` 。但对于某些程序员来讲，
这在循环尾部决定循环是否终止时，更加符合语意理解习惯。

.. code-block:: c++
   :linenos:

   __loop
   ( __sync(Action1)
   , __async(Action2)
   , __time_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   , __until(CondSatisfied)
   );

.. attention::
   ``do ... until(cond)`` 的语意，与 ``do ... while(cond}`` 正好相反。


**__redo_if**
-------------------

`Transaction DSL` 所提供的 ``continue`` 语意的关键字是 ``__redo_if`` ，相当于 ``if(cond) continue`` 。

.. code-block:: c++
   :linenos:

   __loop
   ( __sync(Action1)
   , __async(Action2)
   , __time_guard(TIMER_2, Action3)
   , __redo_if(__is_timeout)
   , __concurrent(__async(Action3), __async(Action5))
   );

在这个例子中，如果发生了timeout，则不再执行后续的其它Action，而是重新开始循环。


用户状态
---------

用户的状态不应该保存在用户定义的Action中，每一个Action运行结束后，其所保存的状态信息也会立即失效。
用户唯一可以保存信息的地方是那些用在 ``__break_if`` ， ``__redo_if`` 及其语法糖里的 **谓词** 。

`Transaction DSL` 保证，所有这些谓词里所持有的状态信息，和循环的生命周期一致。
即只要一个循环没有运行结束，无论其在内部循环了多少次，在循环内对于这些状态的修改，始终保持连续有效。

因而，我们就可以定义这样的谓词：

.. code-block:: c++

   struct ShouldRetry {
      bool operator()(const TransactionInfo& trans) {
         return IsFailed(trans) && retryTime++ < 5;
      }
    private:
      int retryTimes = 0;
   }

.. code-block:: c++
   :linenos:

   __loop(
   , __sync(Action1)
   , __async(Action2)
   , __time_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   , __while(ShouldRetry)
   );

这样，整个循环内部的操作在连续失败5次之前，不会结束。

.. attention::
   在 ``__loop`` 里，只有与循环控制有关的谓词，其状态的连续性才会得到保证。在正常Action的普通谓词，
   比如：``__optional(__not(ShouldRetry), __sync(Action1))`` ，
   其中的谓词 ``ShouldRetry`` 的状态连续性无法得到保证。

错误处理
---------

首先，整个 ``__loop`` 有一个自己的 **运行时环境** ，而这个运行时环境是一个 `Sandbox` ，即它内部所发生的任何错误，
在整个 ``__loop`` 没有结束之前，外界无从感知，因而对外界并无任何影响。

动作段与谓词段
+++++++++++++++

在进一步描述 ``__loop`` 的错误处理之前，我们先来看两个概念：

动作段：Action Segment
  一个或多个 *连续* 的 **动作** （Action）
谓词段：Predicate Segment
  一个或多个 *连续* 的 **谓词** （Predicate）

比如下面的代码里， ``__loop`` 一共可以划分为5个段：3个 **动作段** ，2个 **谓词段** ：

.. code-block:: c++
   :linenos:

   __loop(
   // Action Segment 1
     __sync(Action1)
   , __async(Action2)

   // Predicate Segment 1
   , __break_if(__is_status(FATAL_BUG))
   , __redo_if(__is_failed)

   // Action Segment 2
   , __async(Action3)
   , __async(Action4)
   , __time_guard(TIMER_2, Action5)

   // Predicate Segment 2
   , __break_if(__is_timeout)
   , __redo_if(__is_failed)

   // Action Segment 3
   , __concurrent(__async(Action6), __async(Action7))
   );

对于任何一个 **动作段** ，如果执行到某个Action，出了错，则此段后续的所有Action将都会被跳过。比如，本例子中
的 `Action Segment 2` 一共包含了3个Action，如果 ``Action3`` 的执行出了错，则后续的 ``Action4`` ， ``Action5`` 都会被跳过。

当然，如果没有任何错误，一个 **动作段** 里的所有Action会依次全部执行。

无论一个 **动作段** 出没出错，都会进入紧随其后的 **谓词段** （如果存在的话）。如果之前出了错，在进入 **谓词段** 之后，
此错误总是可以被此 **谓词段** 中的所有 **谓词** 读取，以做为谓词判断的依据之一。

一个之前发生的错误，一旦离开最近的 **谓词段** 之后，便会马上清理。比如，本例中的 **动作段2** 如果出了错， **谓词段2** 中的所有谓词
均可读取此错误。但是，一旦离开 **谓词段2** ，进入 **动作段3** ，此错误将会被清理。在 **动作段3** 里的任意地方读取运行时上下文状态，
总是会得到 ``SUCCESS`` 。

除非， **动作段3** 里又发生了一个新错误，这样， **动作段3** 将会终止其执行， **谓词段3** 将可以读到新的错误。

对于最后一个 **动作段** 的状态，如果重新回到循环的起始位置，而循环的起始位置是一个 **谓词段** ，则此 **谓词段** 可以读取最后一个
**动作段** 的状态；如果循环的起始位置是一个 **动作段** ，则最后一个 **动作段** 的错误会首先被清理，以保证起始位置的 **动作段** 可以
从正确状态开始。比如，本例子中， ``Action Segment 3`` 的错误状态，会在 ``Action Segment 1`` 开始之前被清理。

但是，对于下面的例子， ``Action Segment 2`` 中的错误，在重新回到 ``Predicate Segment 1`` 时，依然可以被读取，直到 ``Predicate Segment 1``
运行结束，错误才会被清理，以保证 ``Action Segment1`` 可以以正确状态开始。

.. code-block:: c++
   :linenos:

   __loop(
   // Predicate Segment 1
     __break_if(__is_status(FATAL_BUG))
   , __redo_if(__is_failed)

   // Action Segment 1
   , __async(Action3)
   , __async(Action4)
   , __time_guard(TIMER_2, Action5)

   // Predicate Segment 2
   , __break_if(__is_timeout)
   , __redo_if(__is_failed)

   // Action Segment 2
   , __concurrent(__async(Action6), __async(Action7))
   );

**stop**
+++++++++++++

当一个 ``__loop`` 被stop后，当前正在执行的Action会被stop，此Action被彻底stop后（有可能不能马上结束，
需要进一步的消息激励后才能结束），返回的状态，则是整个 ``__loop`` 的返回状态。

死循环
++++++++++

如果一个 ``__loop`` ，运行一次完整的循环，其间却没有任何消息激励，那么很可能这个循环进入了死循环状态，这种情况下 ``__loop`` 会被
强制终止，并返回 ``USER_FATAL_BUG`` 错误。比如下面的循环隐性的包含死循环状态，但 ``__loop`` 会在一个完整的循环之后，将其终止。

.. code-block:: c++

   __loop
   ( __sync(Action1)
   , __sync(Action2)
   , __sync(Action3));

如果用户想避免这样的检查，则可以使用： ``__loop_max`` 或者 ``__forever`` 以特别说明这的确是用户有意为之，而不是一个无意中犯下的错误。

比如：

.. code-block:: c++

   __loop_max(1000
   , __sync(Action1)
   , __sync(Action2)
   , __sync(Action3));

或者：

.. code-block:: c++

   __forever
   ( __sync(Action1)
   , __sync(Action2)
   , __sync(Action3));

.. attention::
   ``__loop_max`` 与 ``__forever`` 并不意味着循环一定要永远循环下去，或者要循环到最大次数。循环里仍然可以设置谓词，
   当谓词条件满足时，``__break_if`` 及其语法糖，将可能更早的终止循环。

   另外，``__loop_max`` 所指定的次数，指的是无消息激励的情况下，最大的循环次数。在有消息激励的情况下，指定的次数不起作用，
   因而即便超过指定次数也没有任何关系。当 ``无消息激励`` 循环的次数达到最大时，会返回 ``USER_FATAL_BUG`` 错误。
   因而，在使用中，必须将次数设置的比所需的更大，才能避免这种错误。

