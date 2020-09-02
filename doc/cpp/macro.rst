MACRO
==========

一个 ``类函数宏`` 的展开，分为两个阶段：

1. 首先对宏的参数进行 **完全展开** ;
2. 所有  **完全展开** 后的参数，在宏定义里进行替换之后，结合宏定义之后的其它字符，进行第二次 **完全展开** 。

在这个过程中，正在被展开的宏的名字，会被标记为不可再展开，以避免 ``自引用`` 所导致的 ``无穷递归`` 问题；

对于 ``类函数宏`` 的参数展开，其处理边界为其参数自身所包含（或展开后包含）的所有字符。做为参数分割的 ``逗号`` 、
或者宏的 ``右括号`` 之后的字符，都不会参与参数的 **完全展开** 过程。

因而，一个参数被完全展开的过程，要避免出现 ``自引用`` 。一旦一个符号被标记为自引用，则此符号在随后的所有扩展过程中
都不可能再被展开。

一个参数被完全展开之后，没有被标记为 ``自引用`` 的符号， 在宏的第二次展开过程中，如果被发现可以和后面的符号（即圆括号）
形成一个 ``类函数宏`` "调用" ，则会按照 ``类函数宏`` 的方式进一步展开。

.. code-block:: c++

   #define 

.. C99: 6.10.3.1 Argument substitution
..
   After the arguments for the invocation of a function-like
   macro have been identified, argument substitution takes place.
   A parameter in the replacement list, unless preceded by a # or ## preprocessing
   token or followed by a ## preprocessing token (see below), is replaced by the
   corresponding argument after all macros contained therein have been expanded.
   Before being substituted, each argument’s preprocessing tokens are completely
   macro replaced as if they formed the rest of the preprocessing file; no other
   preprocessing tokens are available.

.. C99: 6.10.3.4 Rescanning and further replacement
..
   After all parameters in the replacement list have been substituted and # and ## processing
   has taken place, all placemarker preprocessing tokens are removed. Then, the resulting
   preprocessing token sequence is rescanned, along with all subsequent preprocessing tokens
   of the source file, for more macro names to replace.


   If the name of the macro being replaced is found during this scan of the replacement list
   (not including the rest of the source file’s preprocessing tokens), it is not replaced. Furthermore,
   if any nested replacements encounter the name of the macro being replaced, it is not replaced. These
   nonreplaced macro name preprocessing tokens are no longer available for further replacement
   even if they are later (re)examined in contexts in which that macro name preprocessing token
   would otherwise have been replaced.


   The resulting completely macro-replaced preprocessing token sequence is not processed as a preprocessing
   directive even if it resembles one.