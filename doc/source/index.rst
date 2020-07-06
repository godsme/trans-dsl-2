========================
Transaction DSL简介
========================

.. note:: Transaction DSL 是一套使用C++编写的领域专用语言，通过它，可以简单直观的描述任意复杂的异步通信过程。

主要改进
------------

2.0 版本相对于1.x版本的主要改进有：

- :ref:`极小的内存占用<memory_usage>`
- :ref:`更快的性能<performance>`
- :ref:`新的循环设计<loop>`
- :ref:`清晰一致的错误处理策略<error-handling>`
- :ref:`简化用户定义方式<user-friendly>`


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




