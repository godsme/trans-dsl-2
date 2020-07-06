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

  参见 :ref:`user-friendly`



.. toctree::
   :maxdepth: 2
   :numbered:
   :caption: 目录

   motivation
   quick-start
   proc-control
   loop
   error-handling
   fragment
   multi-thread
   stop
   event

   monitor
   appendix
   performance
   memory
   user-friendly




