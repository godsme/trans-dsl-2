MACRO
==========


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