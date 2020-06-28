.. _memory_usage:

内存占用
=============

如下所有测试均基于 ``64-bit`` 主机。

.. code-block:: c++

   __procedure
   ( __sequential
     ( __wait(1)
     , __wait(2)
     , __wait(3)
     , __wait(4)
     , __wait(5)
     , __wait(6))
     , __finally
       ( __sequential
         ( __wait(7)
         , __wait(8)
         , __wait(9)))
   ) a;

1.x:
  `sizeof(a) = 432`

2.0:
  `sizeof(a) = 48`

如果我们增加两个 ``__wait``:

.. code-block:: c++

   __procedure
   ( __sequential
     ( __wait(1)
     , __wait(2)
     , __wait(3)
     , __wait(4)
     , __wait(5)
     , __wait(6))
     , __wait(7))
     , __finally
       ( __sequential
         ( __wait(7)
         , __wait(8)
         , __wait(9)
         , __wait(1)))
   ) a;

1.x:
  ``sizeof(a) = 496``

2.0:
  ``sizeof(a) = 48``


.. code-block:: c++

   using ProcedureAction1 =
     __procedure(
       __wait(1),
       __finally(__asyn(AsyncAction2)));

   using ProcedureAction2 =
     __procedure(
       __wait(2),
       __finally(__asyn(AsyncAction1)));

   using Concurrent = __concurrent(ProcedureAction1, ProcedureAction2);

1.x:
  ``sizeof(a) = 480``

2.0:
  ``sizeof(a) = 160``


如果我们再增加一个并发。

.. code-block:: c++

   using ProcedureAction3 =
      __procedure(
         __wait(3),
         __finally(__asyn(AsyncAction4)));

   using Concurrent = __concurrent(ProcedureAction1, ProcedureAction2, ProcedureAction3);

1.x:
  ``sizeof(a) = 688``

2.0:
  ``sizeof(a) = 224``