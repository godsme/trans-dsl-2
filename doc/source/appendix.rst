语言及框架
==============

.. code-blocks::

   __def(sub_name [, __params(para [, ...])]) __as(actions...);

   para := __action(action_name)  |
           __thread_id(thread_id) |
           __action_id(action_id) |
           __timer_id(timer_id);


   action := __sync(synchronous_action)  |
             __asyn(asynchronous_action) |
             __sequential(action, action [, ...]) |
             __concurrent(action, action [, ...]) |
             __optional(pred, action) | __timer_prot(timer_id, action) |
             __sleep(timer_id) |
             __fork(thread_id, action) |
             __join(thread_id , ...)   |
             __procedure(action, __finally(action)) |
             __procedure(action, __recover(action)) |
             __with_id(action_id, action) |
             __apply(sub_name [, __with(param [, ...])]) |
             __wait(event_id) |
             __peek(event_id) |
             __loop(...) |
             __time_guard(timer_id, action) |
             __safe(action) |
             __void(action);

   timer_id  := BYTE
   thread_id := BYTE
   signal_id := BYTE
   action_id := BYTE



