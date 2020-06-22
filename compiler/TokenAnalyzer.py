from xml.etree.ElementTree import Element, SubElement, ElementTree
# linking은 나중에 할 것 즉 각 파일마다 독립적으로 처리되야 함 다른 클래스를 블러오는 것은 무조건 그 클래스가 있다는 가정하에 진행
# 실제로 그 클래스가 있는지는 linking 때 확인
# 예약어도 재귀함수 안에서 처리해야 함
import re, sys, os

# { }, ( ) [ ] 괄호는 함수 밖에서 처리할 것


class Tokenizer:
    multiline = re.compile('/\*\*.*?\*/')
    oneline = re.compile('//.*\n')
    validName = re.compile('[A-Z a-z]\w*')
 #   stringConstant = re.compile()
    table_symbol = set("""+-*/<>&|=~{}()[];.':,""")
    table_classVar = {"field", "static"}
    table_classRoutine_type = {"function", "constructor", "method"}
    table_builtinType = {"int", "boolean", "char"}
    table_state = {"do", "while", "if", "let", "return"}
    table_op = {'+', '-', '*', '&', '|', '<', '>', '=', '/'}
    table_unary = {'-', '~'}
    keywordConstant = {"true", "false", "null", "this"}

    def ifStatement(self, parent):
        print("ifStatement ", self.parsed[self.cursor])
        IfSt = SubElement(parent, "ifStatement")
        self.keyword(IfSt)
        self.symbol(IfSt, '(')
        self.expression(IfSt)
        self.symbol(IfSt, ')')
        self.symbol(IfSt, '{')
        self.statements(IfSt)
        self.symbol(IfSt, '}')
        if self.parsed[self.cursor] == "else":
            self.keyword(IfSt)
            self.symbol(IfSt, '{')
            self.statements(IfSt)
            self.symbol(IfSt, '}')

    def doStatement(self, parent):
        print("doStatement ", self.parsed[self.cursor])
        DoSt = SubElement(parent, "doStatement")
        self.keyword(DoSt)
        self.subRoutineCall(DoSt)
        self.symbol(DoSt, ';')

    def letStatement(self, parent):
        print("LetStatement ", self.parsed[self.cursor])
        LtSt = SubElement(parent, "letStatement")
        self.keyword(LtSt)
        self.varName(LtSt)
        self.symbol(LtSt, '=')
        self.expression(LtSt)
        self.symbol(LtSt, ';')

    def whileStatement(self, parent):
        print("whileStatement ", self.parsed[self.cursor])
        WhSt = SubElement(parent, "whileStatement")
        self.keyword(WhSt)
        self.symbol(WhSt, '(')
        self.expression(WhSt)
        self.symbol(WhSt, ')')
        self.symbol(WhSt, '{')
        self.statements(WhSt)
        self.symbol(WhSt, '}')

    def returnStatement(self, parent):
        print("returnStatement ", self.parsed[self.cursor])
        RtSt = SubElement(parent, "returnStatement")
        self.keyword(RtSt)
        if self.parsed[self.cursor] != ';':
            self.expression(RtSt)
        self.symbol(RtSt, ';')

    dict_state = {
        "if": ifStatement,
        "do": doStatement,
        "let": letStatement,
        "while": whileStatement,
        "return": returnStatement
    }

    def __init__(self, fileName):
        self.fileName = fileName[0:len(fileName)-5]
        reader = open(fileName, 'r')
        file = reader.read()
        reader.close()
        erased = Tokenizer.multiline.sub('\n', Tokenizer.oneline.sub('\n', file))
        self.stringList = re.findall('".*?"', erased)
        erased = re.sub('".*?"', ' " ', erased)
        self.parsed = erased.split()
        self.divideBySymbol()
        self.cursor = 0
        if self.parsed[self.cursor] != "class":
            print("error class keyword needed")
            exit(-1)

    def divideBySymbol(self):
        i = 0
        while i < len(self.parsed):
            #    print(parse[i], end=' ')
            if self.parsed[i] == '"':
                middle: str = self.stringList[0][1:len(self.stringList[0])-1]
                self.stringList.pop(0)
                self.parsed.insert(i+1, middle)
                self.parsed.insert(i+2, '"')
                i += 3
                continue
            if len(self.parsed[i]) == 1:
                #        print('')
                i += 1
            elif self.parsed[i][0] in Tokenizer.table_symbol:
                #        print("case1")
                c = self.parsed[i][0]
                residue = self.parsed[i][1:]
                self.parsed.pop(i)
                self.parsed.insert(i, c)
                self.parsed.insert(i + 1, residue)
                i += 1
            else:
                det = False
                for j in range(1, len(self.parsed[i]) - 1):
                    if self.parsed[i][j] in Tokenizer.table_symbol:
                        #            print("case2")
                        (left, c, right) = self.parsed[i].partition(self.parsed[i][j])
                        self.parsed.pop(i)
                        self.parsed.insert(i, left)
                        self.parsed.insert(i + 1, c)
                        self.parsed.insert(i + 2, right)
                        det = True
                        i += 2
                        break
                if det:
                    continue
                back = len(self.parsed[i]) - 1
                if self.parsed[i][back] in Tokenizer.table_symbol:
                    #        print("case3")
                    c = self.parsed[i][back]
                    residue = self.parsed[i][:back]
                    self.parsed.pop(i)
                    self.parsed.insert(i, residue)
                    self.parsed.insert(i + 1, c)
                    i += 2
                    continue
                i += 1

    def getClass(self):
        root = Element("class")
        self.keyword(root)
        if Tokenizer.validName.fullmatch(self.parsed[self.cursor]):
            self.identifier(root)
        else:
            print("error invalid name")
            exit(-1)
        self.symbol(root, '{')
        while self.parsed[self.cursor] == "field" or self.parsed[self.cursor] == "static":
            self.classVar(root)

        while self.parsed[self.cursor] in Tokenizer.table_classRoutine_type:
            self.classRoutine(root)

        self.symbol(root, '}')
        tree = ElementTree(root)
        tree.write(self.fileName + ".xml")

    def classVar(self, parent):
        print("classVar ", self.parsed[self.cursor])
        CVD = SubElement(parent, "classVarDec")
        self.keyword(CVD)
        self.type(CVD)
        self.varName(CVD)

        while self.parsed[self.cursor] == ',':
            SubElement(CVD, "symbol").text = ','
            self.cursor += 1
            self.varName(CVD)

        self.symbol(CVD, ';')

    def classRoutine(self, parent):
        print("classRoutine ", self.parsed[self.cursor])
        CSR = SubElement(parent, "subRoutineDec")
        self.keyword(CSR)
        self.type(CSR)
        self.varName(CSR)
        self.symbol(CSR, '(')
        self.parameterList(CSR)
        self.symbol(CSR, ')')
        self.routine_body(CSR)

    def parameterList(self, parent):
        print("parameterList ", self.parsed[self.cursor])
        parameters = SubElement(parent, "parameterList")
        if self.parsed[self.cursor] == ')':  # empty list
            return
        self.type(parameters)
        self.varName(parameters)

        while self.parsed[self.cursor] == ',':
            self.keyword(parameters)
            self.type(parameters)
            self.varName(parameters)

    def routine_body(self, parent):
        print("routine_body ", self.parsed[self.cursor])
        SRB = SubElement(parent, "subRoutineBody")
        self.symbol(SRB, '{')
        while self.parsed[self.cursor] == "var":
            self.varDec(SRB)
        self.statements(SRB)
        self.symbol(SRB, '}')

    def varDec(self, parent):
        print("varDec ", self.parsed[self.cursor])
        VD = SubElement(parent, "varDec")
        self.keyword(VD)
        self.type(VD)
        self.varName(VD)
        while self.parsed[self.cursor] == ',':
            SubElement(VD, "symbol").text = ','
            self.cursor += 1
            self.varName(VD)
        self.symbol(VD, ';')

    def statements(self, parent):
        print("statements ", self.parsed[self.cursor])
        STS = SubElement(parent, "statements")
        while self.parsed[self.cursor] in Tokenizer.dict_state.keys():
            if self.parsed[self.cursor] == "if":
                self.ifStatement(STS)
            elif self.parsed[self.cursor] == "do":
                self.doStatement(STS)
            elif self.parsed[self.cursor] == "let":
                self.letStatement(STS)
            elif self.parsed[self.cursor] == "while":
                self.whileStatement(STS)
            else:
                self.returnStatement(STS)

    def expression(self, parent):
        print("expression ",self.parsed[self.cursor])
        Exp = SubElement(parent, "expression")
        self.term(Exp)
        while self.parsed[self.cursor] in Tokenizer.table_op:
            self.op(Exp)
            self.term(Exp)

    def term(self, parent):  # unary op는 여기서 자체 처리
        # unaryOp는 여기서 check
        child_term = SubElement(parent, "term")
        if self.parsed[self.cursor].isdecimal():  # integerConstant
            self.integerConstant(child_term)
        elif self.parsed[self.cursor] == '"':  # stringConstant
            self.cursor += 1
            self.stringConstant(child_term)
        elif self.parsed[self.cursor] == '(':  # ( expression )
            SubElement(child_term, "symbol").text = '('
            self.cursor += 1
            self.expression(child_term)
            self.symbol(child_term, ')')
        elif self.parsed[self.cursor] in Tokenizer.keywordConstant:  # true, false, null 등
            self.keyword(child_term)
        elif self.parsed[self.cursor] in Tokenizer.table_unary:  # -"string" 같은 건 일단 무시
            _unary = SubElement(child_term, "unaryOp")
            SubElement(_unary, "symbol").text = self.parsed[self.cursor]
            self.cursor += 1
            self.term(child_term)
        else:  # identifier, identifier[expression], instance method, subroutine
            # check whether subRoutine or variable
            # cursor doesnt changes before checking
            tempCur = self.cursor + 1
            if self.parsed[tempCur] == '[':  # need to jump expression
                tempCur += 1
                stack = 1
                isStringConst = 1
                # jump expression
                while stack > 0 and tempCur < len(self.parsed):
                    if self.parsed[tempCur] == '[':
                        stack += 1 * isStringConst
                    elif self.parsed[tempCur] == ']':
                        stack -= 1 * isStringConst
                    elif self.parsed[tempCur] == '"':
                        if isStringConst == 1:
                            isStringConst = 0
                        else:
                            isStringConst = 1
                    else:
                        tempCur += 1
                        continue
                    tempCur += 1
                if tempCur == len(self.parsed):
                    print("error")
                    exit(-1)
                tempCur += 1

            if self.parsed[tempCur] == '.':  # subroutineCall
                self.subRoutineCall(child_term)
            else:  # just variable
                self.varName(child_term)

    def op(self, parent):
        child_op = SubElement(parent, "op")
        self.symbol(child_op, self.parsed[self.cursor])

    def subRoutineCall(self, parent):
        SRC = SubElement(parent, "subRoutineCall")
        self.varName(SRC)
        if self.parsed[self.cursor] == '.':  # className|varName . routine(expression list)
            SubElement(SRC, "symbol").text = '.'
            self.cursor += 1
            self.varName(SRC)
        self.symbol(SRC, '(')
        self.expressionList(SRC)
        self.symbol(SRC, ')')

    def expressionList(self, parent):
        ExpL = SubElement(parent, "expressionList")
        if self.parsed[self.cursor] == ')':
            return
        self.expression(ExpL)
        while self.parsed[self.cursor] == ',':
            self.symbol(ExpL, ',')
            self.expression(ExpL)

    # terminal
    def symbol(self, parent, c):
        if self.parsed[self.cursor] != c:
            print("error %s missing" % c)
            exit(-1)
        else:
            SubElement(parent, "symbol").text = c
            self.cursor += 1

    def identifier(self, parent):
        SubElement(parent, "identifier").text = self.parsed[self.cursor]
        self.cursor += 1

    def type(self, parent):  # type itself is not subNode of tree
        if self.parsed[self.cursor] in Tokenizer.table_builtinType or self.parsed[self.cursor] != "void":
            self.keyword(parent)
        elif Tokenizer.validName.fullmatch(self.parsed[self.cursor]):
            self.identifier(parent)
        else:
            print("error invalid typeName ", self.parsed[self.cursor])
            exit(-1)

    def varName(self, parent):
        if Tokenizer.validName.fullmatch(self.parsed[self.cursor]):
            self.identifier(parent)
        else:
            print("error invalid variable name ", self.parsed[self.cursor])
            exit(-1)
        if self.parsed[self.cursor] == '[':
            SubElement(parent, "symbol").text = '['
            self.cursor += 1
            self.expression(parent)
            self.symbol(parent, ']')

    def keyword(self, parent):
        SubElement(parent, "keyword").text = self.parsed[self.cursor]
        self.cursor += 1

    def integerConstant(self, parent):
        SubElement(parent, "integerConstant").text = self.parsed[self.cursor]
        self.cursor += 1

    def stringConstant(self, parent):
        buffer = self.parsed[self.cursor]
        self.cursor += 1
        if self.parsed[self.cursor] != '"':
            print('error " for closing string needed')
            exit(-1)
        SubElement(parent, "stringConstant").text = buffer
        self.cursor += 1


if len(sys.argv) < 2:
    print(
        """write option plz
        option:
            -a: read file list in argv ex: TokenAnalyzer.py -a Fraction.jack Main.jack
            -v: read files in folder ex: TokenAnalyzer.py -v ./Square
    """)
    exit(-1)
if sys.argv[1] == '-a':
    for i in range(2, len(sys.argv)):
        Tokenizer(sys.argv[i]).getClass()

elif sys.argv[1] == '-v':
    path = sys.argv[2]
    file_list = os.listdir(path)
    for file in file_list:
        if file.endswith(".jack"):
            Tokenizer(path+'/'+file).getClass()
else:
    print(
        """write option plz
        option:
            -a: read file list in argv ex: TokenAnalyzer.py Fraction.jack Main.jack
            -v: read files in folder ex: TokenAnalyzer.py ./Square
    """)