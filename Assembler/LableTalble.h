#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
class LableTable{
    std::unordered_map<std::string,unsigned int> Lable;
    std::unordered_map<std::string,unsigned int> Symbol;
    std::unordered_map<std::string,unsigned int> Comp;
    std::unordered_map<std::string,unsigned int> Dest;
    std::unordered_map<std::string,unsigned int> Jmp;
    public:
    LableTable();
    bool isDefined(const std::string key)const;
    void insert_Lable(const std::string key, unsigned int jmploc);
    void insert_Symbol(const std::string key, unsigned int oper);
    unsigned int mapping_Lab(const std::string key)const;
    unsigned int mapping_sym(const std::string key)const;
    unsigned int mapping_comp(const std::string key)const;
    unsigned int mapping_dest(const std::string key)const;
    unsigned int mapping_jmp(const std::string key)const;
};
LableTable::LableTable(){
    std::ifstream table("Table.txt");
    std::string buffer;
    unsigned int mapping;
    std::getline(table,buffer);
//    std::cout<<"inserting Symbol\n";
    while(table){
        table>>buffer;
        if(buffer[0]=='/' && buffer[1]=='/') break;
        table>>std::hex>>mapping;
        Symbol.insert(std::make_pair(buffer,mapping));
//        std::cout<<buffer<<' '<<std::hex<<mapping<<std::endl;
    }
//    std::getline(table,buffer);
//    std::cout<<"inserting jump\n";
    while(table){
        table>>buffer;
        if(buffer[0]=='/' && buffer[1]=='/') break;
        table>>std::hex>>mapping;
        Jmp.insert(std::make_pair(buffer,mapping));
//        std::cout<<buffer<<' '<<std::hex<<mapping<<std::endl;
    }
//    std::getline(table,buffer);
//    std::cout<<"inserting Dest\n";
    while(table){
        table>>buffer;
        if(buffer[0]=='/' && buffer[1]=='/') break;
        table>>std::hex>>mapping;
        Dest.insert(std::make_pair(buffer,mapping));
//        std::cout<<buffer<<' '<<std::hex<<mapping<<std::endl;
    }
//    std::getline(table,buffer);
//    std::cout<<"inserting Comp\n";
    while(table){
        table>>buffer;
        if(buffer.size()==0 || !table) break;
        table>>std::hex>>mapping;
        Comp.insert(std::make_pair(buffer,mapping));
//        std::cout<<buffer<<' '<<std::hex<<mapping<<std::endl;
    }
}
bool LableTable::isDefined(const std::string key)const{
    if(Lable.find(key) == Lable.end() && Symbol.find(key) == Symbol.end()) return false;
    else return true;
}
void LableTable::insert_Lable(const std::string key, unsigned int jmploc){
    Lable.insert(std::make_pair(key,jmploc));
}
void LableTable::insert_Symbol(const std::string key, unsigned int oper){
    Symbol.insert(std::make_pair(key,oper));
}
unsigned int LableTable::mapping_Lab(const std::string key)const{
    return Lable.find(key)->second;
}
unsigned int LableTable::mapping_sym(const std::string key)const{
    return Symbol.find(key)->second;
}
unsigned int LableTable::mapping_comp(const std::string key)const{
    return Comp.find(key)->second;
}
unsigned int LableTable::mapping_dest(const std::string key)const{
    return Dest.find(key)->second;
}
unsigned int LableTable::mapping_jmp(const std::string key)const{
    if(Jmp.find(key) == Jmp.end()){
        std::cout<<key<<' '<<"is not defined in Jmp\n";
        return 0xffff;
    }
    return Jmp.find(key)->second;
}