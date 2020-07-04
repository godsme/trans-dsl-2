
from pygments.lexers.c_cpp import CppLexer
from pygments.token import Name, Keyword
from pygments.lexers import get_lexer_by_name  # refer LEXERS
from pygments.lexers._mapping import LEXERS

class CppTransDslLexer(CppLexer):
    EXTRA_KEYWORDS = set(('__sync', '__asyn', '__sequential', '__concurrent', '__time_guard',
                          '__transaction', '__fork', '__join', '__optional', '__switch', '__case',
                          '__otherwise', '__wait', '__peek', '__with_id', '__void', '__safe',
                          '__procedure', '__prot_procedure', '__throw', '__is_status', '__loop',
                          '__break_if', '__redo_if', '__while', '__until', '__loop_max',
                          '__forever', '__multi_thread', '__on_fail', '__req', '__rsp',
                          '__params', '__def', '__as', '__thread_id', '__timer_id'))
    FUNC_KEYWORDS  = set(('exec', 'handleEvent', 'kill', 'WAIT_ON'))
    TYPE_KEYWORDS  = set(('ActionThreadId', 'TimerId', 'Status', 'TransactionInfo', 'Event'))
    CONST_KEYWORDS = set(('SUCCESS', 'CONTINUE', 'UNKNOWN_EVENT', 'FAILED', 
                          'DEF_SIMPLE_ASYNC_ACTION'))

    def get_tokens_unprocessed(self, text):
        for index, token, value in CppLexer.get_tokens_unprocessed(self, text):
            if token is Name and value in self.EXTRA_KEYWORDS:
                yield index, Keyword, value
            elif token is Name and value in self.FUNC_KEYWORDS:
                yield index, Name.Function, value
            elif token is Name and value in self.TYPE_KEYWORDS:
                yield index, Name.Class, value
            elif token is Name and value in self.CONST_KEYWORDS:
                yield index, Name.Constant, value
            else:
                yield index, token, value


def setup(app):
    app.add_lexer('c++', CppTransDslLexer)
