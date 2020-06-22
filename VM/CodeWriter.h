#include<fstream>
#include<string>
#include<map>
#define TEMP 16383
#define POP_Y "@SP\nM=M-1\nA=M\nD=M\n"//pop -> D
#define POP_X "@SP\nM=M-1\nA=M\n"//pop -> M
#define PUSH_D "@SP\nA=M\nM=D\n@SP\nM=M+1\n"
class CodeWriter{
    std::string code;//append to bootstrap code
    void writerInit(std::ofstream &writer);//write bootstrap code
    std::map<std::string,std::string> _seg;
    std::ofstream *pWriter;
    std::string _filename;
    int labelNum;
    std::string curFunc;
    int _argNum;

    void code_push(const std::string &memSeg, int idx);
    void code_pop(const std::string &memSeg, int idx);
    void code_arithmetic(const std::string &oper);
    void code_label(const std::string &label);
    void code_goto(const std::string &dest);
    void code_if(const std::string &dest);
    void code_call(const std::string &func, int argNum);
    void code_return();
    void code_function(const std::string &func, int argNum);//reset leabelNum

    public:
    CodeWriter(std::ofstream &writer);
    void currentFile(const std::string &filename){_filename = filename;};
    void currentFunc(const std::string &funcname);//code_func에서 호출, 생성자에서 main혹은 Sys.init으로 초기화
    void writeCode(const std::string &command, const std::string &arg1, int arg2);
    
};