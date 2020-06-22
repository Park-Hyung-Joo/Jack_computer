#include<algorithm>
#include<iostream>
#include "Parser.h"


Parser::Parser(std::ifstream &reader){
    std::string buffer;
    std::string::size_type anno,size,emptyline;
    //파일 내용 복사
    while(reader){
       std::getline(reader,buffer);
       if(!reader) break;
       file = file + buffer + '\n';
   }
   //주석지우기
    anno=0;
    while(anno < file.size()){
        anno = file.find("//",anno);
        if(anno == file.npos) break;
        size = file.find('\n',anno) - anno;
        file.erase(anno,size);
    }
    file.erase(std::remove_if(file.begin(), file.end(), isSpace), file.end());// '\n',' ' 제외 모든 공백 삭제
    //빈 줄 없애기
    emptyline=0;
    while(emptyline < file.size()){
        emptyline = file.find("\n\n",emptyline);
        if(emptyline == file.npos) break;
        file.erase(emptyline,1);
    }
    if(file[0]=='\n') file.erase(file.begin());
    if(file.back()!='\n') file.push_back('\n');//반드시 마지막 줄에 '\n'이 있게 한다.
    cursor = 0;
    if(file.find('\n',cursor)==file.npos) hasMoreCommand = false;
    else hasMoreCommand = true;
    TableInit();
    std::cout<<file;
}

void Parser::TableInit(){
    _command.insert(std::make_pair("push","_C_PUSH"));
    _command.insert(std::make_pair("pop","_C_POP"));
    _command.insert(std::make_pair("add","_C_ARITHMETIC"));
    _command.insert(std::make_pair("sub","_C_ARITHMETIC"));
    _command.insert(std::make_pair("not","_C_ARITHMETIC"));
    _command.insert(std::make_pair("or","_C_ARITHMETIC"));
    _command.insert(std::make_pair("and","_C_ARITHMETIC"));
    _command.insert(std::make_pair("lt","_C_ARITHMETIC"));
    _command.insert(std::make_pair("eq","_C_ARITHMETIC"));
    _command.insert(std::make_pair("gt","_C_ARITHMETIC"));
    _command.insert(std::make_pair("neg","_C_ARITHMETIC"));
    _command.insert(std::make_pair("label","_C_LABEL"));
    _command.insert(std::make_pair("goto","_C_GOTO"));
    _command.insert(std::make_pair("if-goto","_C_IF"));
    _command.insert(std::make_pair("call","_C_CALL"));
    _command.insert(std::make_pair("return","_C_RETURN"));
    _command.insert(std::make_pair("function","_C_FUNCTION"));
    _singleArg.insert({"_C_LABEL","_C_GOTO","_C_IF"});
}

const Command Parser::getCommand(){
    Command temp;
    std::string buffer;
    std::string::size_type tempCur;
    while(file[cursor]==' ') cursor++;

    for(tempCur = cursor; file[tempCur]!=' ' && file[tempCur]!='\n'; tempCur++);

    buffer = file.substr(cursor,tempCur-cursor);
    if(_command.find(buffer) == _command.end()){
        //error
        std::cout<<"error_no commmand "<<buffer<<'\n';
        exit(-1);
    }
    temp.type = _command.find(buffer)->second;
    if(temp.type == "_C_RETURN");//do nothing
    else if(temp.type=="_C_ARITHMETIC"){
        temp.arg1 = buffer;
    }
    else if(_singleArg.find(temp.type) != _singleArg.end()){//그 외에도 명령이 한 단어인 문장 추가
        for(cursor = tempCur+1 ; cursor<file.size() && file[cursor]==' ';cursor++);
        for(tempCur = cursor; file[tempCur]!=' ' && file[tempCur]!='\n'; tempCur++);
        buffer = file.substr(cursor,tempCur-cursor);
        temp.arg1 = buffer;
    }
    else{
        for(cursor = tempCur+1 ; cursor<file.size() && file[cursor]==' ';cursor++);
        for(tempCur = cursor; tempCur<file.size() && file[tempCur]!=' '; tempCur++);
        buffer = file.substr(cursor,tempCur-cursor);
        temp.arg1 = buffer;
        for(cursor = tempCur+1 ; cursor<file.size() && file[cursor]==' ';cursor++);
        for(tempCur = cursor; file[tempCur] != ' ' && file[tempCur]!='\n'; tempCur++);
        buffer = file.substr(cursor,tempCur-cursor);
        temp.arg2 = std::stoi(buffer);
    }
    cursor = file.find('\n',tempCur) + 1;
    if(file.find('\n',cursor) == file.npos) hasMoreCommand = false;
    return temp;
};
