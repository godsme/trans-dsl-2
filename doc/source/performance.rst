.. _performance:

性能对比
==============

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
  ``2573 ns``

2.0:
  ``1124 ns``

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
  ``4246 ns``

2.0:
  ``1249 ns``


如果我们再增加一个并发。

.. code-block:: c++

   using ProcedureAction3 =
      __procedure(
         __wait(3),
         __finally(__asyn(AsyncAction4)));

   using Concurrent = __concurrent(ProcedureAction1, ProcedureAction2, ProcedureAction3);

1.x:
  ``6798 ns``

2.0:
  ``1871 ns``

