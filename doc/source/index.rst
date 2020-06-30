========================
Transaction DSL简介
========================

.. note:: Transaction DSL 是一套使用C++编写的领域专用语言，通过它，可以简单直观的描述任意复杂的异步通信过程。

主要改进
------------

2.0 版本相对于1.x版本的主要改进有：

**极小的内存占用** :

  参见 :ref:`memory_usage`

**更快的性能** :

  参见 :ref:`performance`


**新的循环设计** :

  参见 :ref:`loop`


**清晰一致的错误处理策略** :

  参见 :ref:`error-handling`

**简化用户定义方式** :

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
   performance
   memory




