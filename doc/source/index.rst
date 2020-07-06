========================
Transaction DSL简介
========================

.. note:: Transaction DSL 是一套使用C++编写的领域专用语言，通过它，可以简单直观的描述任意复杂的异步通信过程。

主要改进
------------

``2.0`` 版本相对于 ``1.x`` 版本的主要改进有：

- :ref:`简化用户定义方式<user-friendly>`
- :ref:`极小的内存占用<memory_usage>`
- :ref:`极快的性能<performance>`
- :ref:`高度灵活的循环<loop>`
- :ref:`清晰一致的错误处理策略<error-handling>`


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




