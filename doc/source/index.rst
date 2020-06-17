========================
Transaction DSL简介
========================

.. note:: Transaction DSL 是一套使用C++编写的领域专用语言，通过它，可以简单直观的描述任意复杂的异步通信过程。

2.0 版本相对于1.x版本的重大改进有：

1. 极小的内存占用

   比如下面的例子，做为1.x的对象和2.x的对象，在64位机器上的大小。

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
         ( __wait(7)
         , __wait(8)
         , __wait(9)
         , __wait(1)))
   ) a;

1.0:
  ``sizeof(a) = 496``

2.0:
  ``sizeof(a) = 48``

2. 更快的性能

   对于上面的action，在我同样的设备上的运行耗时为：

1.0:
  ``2573 ns``

2.0:
  ``1124 ns``

3. 新的循环设计

   参见 :ref:`loop`

4. 清晰一致的错误处理策略

   参见 :ref:`error-handling`

5. 简化了用户的定义方式:

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




