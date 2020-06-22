#include "CodeWriter.h"

CodeWriter::CodeWriter(std::ofstream &writer){
    _seg.insert(std::make_pair("this","THIS\n"));
    _seg.insert(std::make_pair("that","THAT\n"));
    _seg.insert(std::make_pair("argument","ARG\n"));
    _seg.insert(std::make_pair("local","LCL\n"));
    writerInit(writer);
    pWriter = &writer;
    _filename = "main";
}

void CodeWriter::writerInit(std::ofstream &writer){
    /*
    goto sys.init
    builtin functions (ex:neg, gt, lt)
    */
   std::ifstream reader("BootStrap.txt");
   std::string buffer;
   while (reader){ //bootstrap.txt 복붙
       std::getline(reader,buffer);
       if(!reader) break;
       writer<<buffer<<'\n';
   }
   
}

void CodeWriter::writeCode(const std::string &command, const std::string &arg1, int arg2){
    if(command == "_C_PUSH"){
        code_push(arg1,arg2);
    }
    else if(command == "_C_POP"){
        code_pop(arg1,arg2);
    }
    else if(command == "_C_ARITHMETIC"){
        code_arithmetic(arg1);
    }
    else if(command == "_C_LABEL"){
        code_label(arg1);
    }
    else if(command == "_C_GOTO"){
        code_goto(arg1);
    }
    else if(command == "_C_IF"){
        code_if(arg1);
    }
    else if(command == "_C_CALL"){
        code_call(arg1, arg2);
    }
    else if(command == "_C_RETURN"){
        code_return();
    }
    else if(command == "_C_FUNCTION"){
        code_function(arg1, arg2);
    }
    else{
        //error!!!
    }
}
void CodeWriter::code_push(const std::string &memSeg, int idx){
    *pWriter<<"//push "<<memSeg<<' '<<idx<<'\n';
  if(_seg.find(memSeg) != _seg.end()){ //*(seg+i)
        if(!idx){
            *pWriter<<'@'<<_seg.find(memSeg)->second<<"A=M\nD=M\n";
        }
        else *pWriter<<'@'<<_seg.find(memSeg)->second<<"D=M\n@"<<idx<<"\nA=A+D\nD=M\n";
    }
    else if(memSeg == "pointer"){
        if(idx == 0){
            (*pWriter)<<"@THIS\nD=M\n";
        }
        else if(idx == 1){
             (*pWriter)<<"@THAT\nD=M\n";
        }
        else{
            //return error!!!
        }
    }
    else if(memSeg=="static"){
        std::string Var = _filename+".";
        *pWriter<<'@'<<Var<<idx<<"\nD=M\n";
    }
    else if(memSeg == "constant"){
        *pWriter<<'@'<<idx<<"\nD=A\n";
    }
    else if(memSeg == "temp"){
        *pWriter<<"@R5\nD=A\n@"<<idx<<"\nA=A+D\nD=M\n";
    }
    else{
        //error
    }
    *pWriter<<"@SP\nA=M\nM=D\n@SP\nM=M+1\n";//D->M[sp], sp++  PUSH_D
};

void CodeWriter::code_pop(const std::string &memSeg, int idx){
    *pWriter<<"//pop "<<memSeg<<' '<<idx<<'\n';
    if(_seg.find(memSeg)!=_seg.end()){
        *pWriter<<'@'<< _seg.find(memSeg)->second <<"D=M\n@"<<idx<<"\nA=A+D\nD=A\n"<<'@'<<TEMP<<"\nM=D\n"<<"@SP\nM=M-1\nA=M\nD=M\n"<<'@'<<TEMP<<"\nA=M\nM=D\n";
    }
    else if(memSeg == "pointer"){
        if(idx == 0){
            *pWriter<<"@SP\nM=M-1\nA=M\nD=M\n@THIS\nM=D\n";
        }
        else if(idx == 1){
            *pWriter<<"@SP\nM=M-1\nA=M\nD=M\n@THAT\nM=D\n";
        }
        else{
            //return error!!!
        }
    }
    else if(memSeg=="static"){
        std::string Var = _filename+".";
        *pWriter<<"@SP\nM=M-1\nA=M\nD=M\n"<<"@"<<Var<<idx<<"\nM=D\n";
    }
    //pop dosen't go with constant
    else if(memSeg == "temp"){
        *pWriter<<"@R5\nD=A\n@"<<idx<<"\nA=A+D\nD=A\n@"<<TEMP<<"\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@"<<TEMP<<"\nA=M\nM=D\n";
    }
    else{
        //error
    }
}

void CodeWriter::code_arithmetic(const std::string &oper){
    *pWriter<<"//"<<oper<<'\n';
    if(oper == "add"){
        *pWriter<<POP_Y<<POP_X<<"D=M+D\n"<<PUSH_D;
    }
    else if(oper == "sub"){
        *pWriter<<POP_Y<<POP_X<<"D=M-D\n"<<PUSH_D;
    }
    else if(oper == "and"){
        *pWriter<<POP_Y<<POP_X<<"D=M&D\n"<<PUSH_D;
    }
    else if(oper == "or"){
        *pWriter<<POP_Y<<POP_X<<"D=M|D\n"<<PUSH_D;
    }
    else if(oper == "not"){
        *pWriter<<POP_Y<<"D=!D\n"<<PUSH_D;
    }
    else if(oper == "neg"){
        *pWriter<<POP_Y<<"D=-D\n"<<PUSH_D;
    }
    else if(oper == "eq"){
        *pWriter<<"@$$"<<curFunc<<".RIP:"<<labelNum<<"\nD=A\n"<<'@'<<TEMP<<"\nM=D\n"<<"@$$BUILTIN.EQ\n0;JMP\n($$"<<curFunc<<".RIP:"<<labelNum<<")\n";
        labelNum++;
    }
    else if(oper == "gt"){
        *pWriter<<"@$$"<<curFunc<<".RIP:"<<labelNum<<"\nD=A\n"<<'@'<<TEMP<<"\nM=D\n"<<"@$$BUILTIN.GT\n0;JMP\n($$"<<curFunc<<".RIP:"<<labelNum<<")\n";
        labelNum++;
    }
    else if(oper == "lt"){
        *pWriter<<"@$$"<<curFunc<<".RIP:"<<labelNum<<"\nD=A\n"<<'@'<<TEMP<<"\nM=D\n"<<"@$$BUILTIN.LT\n0;JMP\n($$"<<curFunc<<".RIP:"<<labelNum<<")\n";
        labelNum++;
    }
    else{
        //error
    }
}
void CodeWriter::code_label(const std::string &label){
    *pWriter<<"//label "<<label<<'\n';
    *pWriter<<"("<<curFunc<<'.'<<label<<")\n";
}
void CodeWriter::code_goto(const std::string &dest){
    *pWriter<<"//goto "<<dest<<'\n';
    *pWriter<<"@"<<curFunc<<'.'<<dest<<"\n0;JMP\n";
}
void CodeWriter::code_if(const std::string &dest){
    *pWriter<<"//if_goto "<<dest<<'\n';
    *pWriter<<POP_Y<<'@'<<curFunc<<'.'<<dest<<"\nD;JNE\n";
}
void CodeWriter::code_call(const std::string &func, int argNum){//arguments는 사전에 쌓여있었다고 가정
    /*
    //push return address
    @$$curfunc.RIP:n
    D=A
    PUSH_D
    //push LCL
    //ARG<-SP-argNum-5

    ($$curfunc.RIP:n)
     */
    *pWriter<<"//call "<<func<<' '<<argNum<<'\n';
    *pWriter<<"@$$"<<curFunc<<".RIP:"<<labelNum<<"\nD=A\n"<<PUSH_D;
    *pWriter<<"@LCL\nD=M\n"<<PUSH_D;
    *pWriter<<"@ARG\nD=M\n"<<PUSH_D;
    *pWriter<<"@THIS\nD=M\n"<<PUSH_D;
    *pWriter<<"@THAT\nD=M\n"<<PUSH_D;
    *pWriter<<"@"<<argNum<<"\nD=A\n@5\nD=A+D\n@SP\nD=M-D\n@ARG\nM=D\n"; //ARG=SP-(5+argNum)
    *pWriter<<"@SP\nD=M\n@LCL\nM=D\n"; //LCL = SP
    *pWriter<<'@'<<func<<"\n0;JMP\n"<<"($$"<<curFunc<<".RIP:"<<labelNum<<")\n";
    labelNum++;
}
void CodeWriter::code_return(){
    *pWriter<<"//return\n";
    //return value는 스택 최상단에 저장돼있음
    *pWriter<<POP_Y<<"@ARG\nA=M\nM=D\n";//*ARG = return value
    *pWriter<<"@ARG\nD=M\n@SP\nM=D+1\n";//SP = ARG+1
    *pWriter<<"@LCL\nD=M\n@5\nA=D-A\nD=M\n@"<<TEMP<<"\nM=D\n";//RET=*(LCL-5)
    *pWriter<<"@LCL\nM=M-1\nA=M\nD=M\n@THAT\nM=D\n";//LCL--, THAT= *LCL
    *pWriter<<"@LCL\nM=M-1\nA=M\nD=M\n@THIS\nM=D\n";//LCL--, THIS= *LCL
    *pWriter<<"@LCL\nM=M-1\nA=M\nD=M\n@ARG\nM=D\n";//LCL--, ARG= *LCL
    *pWriter<<"@LCL\nM=M-1\nA=M\nD=M\n@LCL\nM=D\n";//LCL--, LCL= *LCL
    *pWriter<<"@"<<TEMP<<"\nA=M\n0;JMP\n";//goto RET
}
void CodeWriter::code_function(const std::string &func, int argNum){
    *pWriter<<"("<<func<<")\n";
    for(int i=0;i<argNum;i++){ //local var init
        *pWriter<<"@SP\nA=M\nM=0\n@SP\nM=M+1\n";
    }
    curFunc = func;
    labelNum = 0;
};//reset leabelNum