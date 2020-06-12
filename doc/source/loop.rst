循环
=====

循环的控制可以非常灵活。比如，在 `C/C++` 语言中，循环的典型语意有：

.. code-block:: c++

   do {
     // ...
   } while(cond);

   while(cond) {
     // ...
   }

   for(init_list; cond; op) {
     //
   }

同时，在任意循环里，可以随时随地进行两种循环控制：

``continue`` 或者 ``redo`` ：
   即重新回到循环的起点开始运行。
``break`` :
   中断当前的循环，或从当前的循环跳出。

程序员可以通过上述的语意，组合出来任意复杂的循环控制。

归纳下来，一个可以控制任意复杂循环控制的基本要素包括：

1. 提供一种或多种循环定义方式：比如，``while(1){}`` 或者 ``do{}while(1)`` ；
2. 用户可以在循环里的任何位置，根据当时的条件决定终止循环；即 ``if(cond) break`` 。其实 ``while(cond)`` 本身
   的语意等价于 ``if(!cond) break``。因而 ``while(cond){}`` 和 ``do{}while(cond)`` 都是语法糖。
3. 用户可以在循环里的任何位置，根据当时的条件决定重新循环；即 ``if(cond) continue`` 。
4. 用于循环控制的状态信息：最典型的例子是计数器。这些状态信息，在整个循环进行期间，必须要保证其生命周期的连续性。

`__loop`
---------

当程序员需要在一个Transaction里构建一个循环子过程时，可以使用 ``__loop(...)`` 来进行定义。这也是 `Transaction DSL` 所提供
的唯一一种循环定义方式，它相当于 `C/C++` 中的 ``while(1){}`` 。

.. code-block::

   __loop(
   , __sync(Action1)
   , __async(Action2)
   , __timer_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   );

这是一个死循环，它自身永远也不会终止，即便内部的某些 `Action` 出现运行时错误，它也不会停止循环。
要想终止它，只能依靠 ``stop`` 或 ``kill``。

因而，我们必须提供其它机制，让循环可以自然的终止。

`__break_if`
--------------

``__break_if`` 大致相当于 `C/C++` 的 ``if(cond) break`` ，即当 ``cond`` 所设置的条件得到满足时，即可立即跳出循环。

.. code-block:: c++

   __loop(
   , __sync(Action1)
   , __async(Action2)
   , __timer_guard(TIMER_2, Action3)
   , __break_if(__is_timeout)
   , __concurrent(__async(Action3), __async(Action5))
   );

这个例子中，如果在 ``__break_if`` 之前，发生了 ``TIMEDOUT`` 错误，则循环终止，而整个 ``__loop`` 的运行结果也是 ``TIMEDOUT`` 。

当然，如果用户想在结束循环的同时，让 ``__loop`` 的运行结果为另一个错误值，则可以明确进行指定：

.. code-block:: c++

   __loop(
   , __sync(Action1)
   , __async(Action2)
   , __timer_guard(TIMER_2, Action3)
   , __break_if(__is_timeout, SUCCESS)
   , __concurrent(__async(Action3), __async(Action5))
   );

这样， ``__loop`` 的运行结果将是 ``SUCCESS`` 。

`__while`
+++++++++++

事实上，正如我们之前所讨论的，我们可以使用 ``__loop`` 和 ``__break_if`` 描述 ``while(cond) {}`` 和 ``do{} while(cond`` ：

.. code-block:: c++

   __loop( __break_if(__not(CondSatisfied))
   , __sync(Action1)
   , __async(Action2)
   , __timer_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   );

   __loop(
   , __sync(Action1)
   , __async(Action2)
   , __timer_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   , __break_if(__not(CondSatisfied))
   );

也就是说，我们只需要将 ``__break_if(__not(cond))`` 放在 ``__loop`` 的最前面和最后面，
即等价于 ``while(cond){...}`` 和 ``do{...}while(cond)`` 。

为了表达的更加直观， `Transaction DSL` 提供了一个语法糖： ``__while(cond)`` ，其等价于 ``__break_if(__not(cond))`` 。

.. code-block:: c++

   __loop( __while(CondSatisfied)
   , __sync(Action1)
   , __async(Action2)
   , __timer_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   );

   __loop(
   , __sync(Action1)
   , __async(Action2)
   , __timer_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   , __while(CondSatisfied)
   );

当然，``__while`` 也可以指定循环结束时的返回值： ``__while(cond, FAILED)`` ，
如果不指定，循环结束时，则会返回循环所处的 **运行时环境** 的状态。

`__until`
+++++++++++

`Transaction DSL` 所提供的另外一个语法糖是 ``__until`` ，它完全等价于 ``__break_if`` 。但对于某些程序员来讲，
这在循环尾部决定循环是否终止时，更加符合语意理解习惯。

.. code-block:: c++

   __loop(
   , __sync(Action1)
   , __async(Action2)
   , __timer_guard(TIMER_2, Action3)
   , __concurrent(__async(Action3), __async(Action5))
   , __until(CondSatisfied)
   );

注意，其语意和 ``do{}while(cond}`` 正好相反。

`__redo_if`
-------------

`Transaction DSL` 所提供的 ``continue`` 语意的关键字是 ``__redo_if`` ，相当于 ``if(cond) continue`` 。

.. code-block:: c++
   __loop(
   , __sync(Action1)
   , __async(Action2)
   , __timer_guard(TIMER_2, Action3)
   , __redo_if(__is_timeout)
   , __concurrent(__async(Action3), __async(Action5))
   );

在这个例子中，如果发生了timeout，则不再执行后续的其它Action，而是重新开始循环。

