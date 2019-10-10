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
            elif self.currentState == State.LIT_INT:
                self.lexLitInt(char)
            elif self.currentState == State.LIT_FLOAT:
                self.lexLitFloat(char)
            elif self.currentState == State.LIT_STRING:
                self.lexLitString(char)
            if char == "\n":
                self.line_num += 1
            self.offset+=1

        self.completeToken(value=self.currentLexeme)

    #TODO: Make escape characters work
    def lexLitString(self, char):
        if char != "\n" and char != "\"":
            self.currentLexeme += char
        elif char == "\"":
            self.completeToken(value=self.currentLexeme)
        else:
            return
            #TODO: New line before string ends error

    #TODO: Make const floats with exponents work
    def lexLitFloat(self, char):
        if char.isdigit():
            self.currentLexeme += char
        elif char.isalpha() :
            #TODO: Grammar error
            return
        else:
            self.completeToken(value=self.currentLexeme, dec_offset=True)

    def lexLitInt(self, char):
        if char.isdigit():
            self.currentLexeme += char
        elif char.isalpha() :
            #TODO: Grammar error
            return
        elif char == ".":
            self.currentLexeme += char
            self.currentState = State.LIT_FLOAT
        else:
            self.completeToken(value=self.currentLexeme, dec_offset=True)

    def lexIdent(self, char):
        if char.isalpha() or char.isdigit() or char == "_":
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
        elif char.isalpha() or char == "_":
            self.currentLexeme += char
            self.currentState = State.IDENT
            return
        elif char.isdigit():
            self.currentLexeme += char
            self.currentState = State.LIT_INT
        elif char == ".":
            self.currentLexeme += char
            self.currentState = State.LIT_FLOAT
        elif char == "\"":
            self.currentState = State.LIT_STRING
        else:
            return
    
    

class State(enum.Enum):
    START = "UNCOMPLETED_TOKEN"
    IDENT = "IDENT"
    LIT_INT = "LIT_INT"
    LIT_FLOAT = "LIT_FLOAT"
    LIT_STRING = "LIT_STRING"
    OP_PLUS = "OP_PLUS"





string = '14894+aasf5+"jeeez"'

lex = Lexer(string)
lex.lexAll()


for token in lex.token_list:
    print(token.type, ":", token.value)


