#include<string>
#include<map>
#include<set>
#include<fstream>
struct Command{
    std::string type;
    std::string arg1;
    int arg2;    
};
class Parser{

    static bool isSpace(unsigned char c) { return (c == '\r' || c == '\t' || c == '\v' || c == '\f'); }//'\n',' ' 제외 모든 공백문자

    std::map<std::string,std::string> _command;
    std::set<std::string> _singleArg;
    std::string file;
    bool hasMoreCommand;
    
    std::string::size_type cursor;
    void TableInit();
    public:
    Parser(std::ifstream &reader);//main함수에서 전달받는 것
    const bool isRemain(){return hasMoreCommand;}
    const Command getCommand();
};
