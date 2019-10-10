import enum
from functools import partial


class Token():
    def __init__(self, line_num, token_type, value=""):
        self.line_num = line_num
        self.type = token_type
        self.value = value









class Lexer:
    def __init__(self, text):
        self.string = text
        self.currentState = State.START
        self.currentLexeme = ""
        self.line_num = 1
        self.token_list = list()
        self.offset=0

    def completeToken(self, value="", dec_offset=False):
        newToken = Token(self.line_num, self.currentState._value_, value=value)
        self.token_list.append(newToken)
        self.currentState = State.START
        self.currentLexeme = ""
        if dec_offset:
            self.offset -= 1

    def lexAll(self):
        while self.offset < len(self.string):
            char = self.string[self.offset]
            print(char)
            if self.currentState == State.START:
                self.lexStart(char)
            elif self.currentState == State.IDENT:
                self.lexIdent(char)
            if char == "\n":
                self.line_num += 1
            self.offset+=1

        self.completeToken(value=self.currentLexeme)

    def lexIdent(self, char):
        if char >= "a" and char <= "z":
            self.currentLexeme += char
            return
        else:
            self.completeToken(value=self.currentLexeme, dec_offset=True)
            return
    
    def lexStart(self, char):
        if char == "+":
            self.currentState = State.OP_PLUS
            self.completeToken()
            return
        elif char >= "a" and char <= "z":
            self.currentLexeme += char
            self.currentState = State.IDENT
            return
        else:
            return
    
    

class State(enum.Enum):
    START = "UNCOMPLETED_TOKEN"
    IDENT = "IDENT"
    OP_PLUS = "OP_PLUS"




string = 'aaa++++   qweqwrt basqwe'

lex = Lexer(string)
lex.lexAll()


for token in lex.token_list:
    print(token.type, ":", token.value)


