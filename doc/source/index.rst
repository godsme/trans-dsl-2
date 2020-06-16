========================
Transaction DSL简介
========================

.. note:: Transaction DSL 是一套使用C++编写的领域专用语言，通过它，可以简单直观的描述任意复杂的异步通信过程。

2.0 版本相对于1.x版本的重大改进有：

极小的内存占用:

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
         ( __wait(3)
         , __wait(4)
         , __wait(5)))
   ) a;

1.0:
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
         ( __wait(3)
         , __wait(4)
         , __wait(5)
         , __wait(6)))
   ) a;

1.0:
  `sizeof(a) = 496`

2.0:
  `sizeof(a) = 48`

更快的性能:

新的循环设计:
    允许任意复杂度的循环控制

清晰一致的错误处理策略:

简化了用户的定义方式:
   - 对于 ``sync action`` 和 ``predicate`` ，可以直接使用函数和lambda，同时也允许使用 ``仿函数`` ；
   - 对于 ``atom action`` ， 无需从任何接口类继承，而是直接定义相关函数即可。


.. toctree::
   :maxdepth: 2
   :numbered:
   :caption: 目录

   motivation
   quick-start
   proc-control
   fragment
   loop
   multi-thread
   stop
   event
   error-handling
   monitor
   appendix




