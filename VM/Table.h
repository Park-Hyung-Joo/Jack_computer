#include<string>
#include<algorithm>
#include<map>
#define TEMP "16383"
#define POP_Y "@SP\nM=M-1\nA=M\nM=D\n"//pop -> D
#define POP_X "@SP\nM=M-1\nA=M\n"//pop -> A
#define PUSH_D "@SP\nA=M\nM=D\n@SP\nM=M+1\n"
class MemTable{
    std::map<std::string,std::string> table;
    std::map<std::string,std::string> comp;
    int curLabel;
    public:
    MemTable();
    bool isStackSeg(std::string str);
    bool isComp(std::string str);
    std::string mem(std::string str);
    std::string printComp(std::string str);
};
MemTable::MemTable(){
    table.insert(std::make_pair("this","THIS\n"));
    table.insert(std::make_pair("that","THAT\n"));
    table.insert(std::make_pair("argument","ARG\n"));
    table.insert(std::make_pair("local","LCL\n"));
    table.insert(std::make_pair("add",static_cast<std::string>(POP_Y)+POP_X+"D=M+D\n"+PUSH_D));
    table.insert(std::make_pair("sub",static_cast<std::string>(POP_Y)+POP_X+"D=M-D\n"+PUSH_D));
    table.insert(std::make_pair("and",static_cast<std::string>(POP_Y)+POP_X+"D=M&D\n"+PUSH_D));
    table.insert(std::make_pair("or",static_cast<std::string>(POP_Y)+POP_X+"D=M|D\n"+PUSH_D));
    table.insert(std::make_pair("neg",static_cast<std::string>(POP_Y)+POP_X+"D=-D\n"+PUSH_D));
    table.insert(std::make_pair("not",static_cast<std::string>(POP_Y)+POP_X+"D=!D\n"+PUSH_D));
    //$$EQZ에서 POP_D를 사용할 것 
    comp.insert(std::make_pair("eq",static_cast<std::string>(POP_Y)+POP_X+"D=M-D\n@"+TEMP+"\nM=D\n@$$RIP:$n$\nD=A\n"+PUSH_D+"@$$EQZ\n0;JMP\n($$RIP:$n$)\n"+PUSH_D));
    comp.insert(std::make_pair("gt",static_cast<std::string>(POP_Y)+POP_X+"D=M-D\n@"+TEMP+"\nM=D\n@$$RIP:$n$\nD=A\n"+PUSH_D+"@$$GTZ\n0;JMP\n($$RIP:$n$)\n"+PUSH_D));
    comp.insert(std::make_pair("lt",static_cast<std::string>(POP_Y)+POP_X+"D=M-D\n@"+TEMP+"\nM=D\n@$$RIP:$n$\nD=A\n"+PUSH_D+"@$$LTZ\n0;JMP\n($$RIP:$n$)\n"+PUSH_D));
    curLabel = 0;
}
bool MemTable::isStackSeg(std::string str){
    if ( table.find(str)==table.end() ) return false;
    else return true;
};
bool MemTable::isComp(std::string str){
    if ( comp.find(str)==comp.end() ) return false;
    else return true;
};
std::string MemTable::mem(std::string str){
    return table.find(str)->second;
}
std::string MemTable::printComp(std::string str){
    std::string tempBuf = comp.find(str)->second;
    std::string num = std::to_string(curLabel);
    int cursor = tempBuf.find("$n$");
    tempBuf.replace(cursor,3,num);
    cursor = tempBuf.find("$n$",cursor);
    tempBuf.replace(cursor,3,num);
    curLabel++;
    return tempBuf;
}