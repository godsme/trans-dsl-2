错误处理
=========

.. tip::
   - 所有的stop只有当出错时才会返回错误；
   - stop/kill的cause，不再传递。如果用户想知道cause，可以通过 ``TransactionInfo.getStatus()``从当前运行上下文获取。
   - 这就意味着，当一个带有上下文的 ``SchedAction`` ，
     比如 ``Procedure`` , ``Void`` , ``Concurrent`` , ``Multi-Thread`` 等等，在初次被调用stop/kill时，
     应该从parent-runtime-env里读出状态值，并更新到自己的runtime-env里。
   - 一个下层runtime-env，在正常运行时出错时，如果当前上下文并没有运行结束，则需要将错误直接report给上层上下文，由此层层递归，
     直到上报到最顶层，或者被 **免疫上下文** 阻断。
   - **免疫上下文** 指 ``Void`` 或者 ``Protected Procedure`` ，它们要么完全将错误控制在自己的runtime-env内（ ``Void`` ），
     要么有修复机会（ ``Protected Procedure`` ），只有等最终无法修复时，才会将最后的错误返回。
   - 一个下层runtime-env，在正常运行时出错时，如果当前runtime-env已经运行结束，则无需report给上层上下文，而是直接将错误返回，
     这个错误会一直返回到上一层runtime-env，由其根据自己当时的状态（结束与否），决定是直接返回，还是记录并上报。