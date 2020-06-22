#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cctype>
#include "LableTalble.h"
bool isSpace(unsigned char c) {//'\n'제외 모든 공백문자
	return (c == ' ' || c == '\r' ||
		   c == '\t' || c == '\v' || c == '\f');
}
bool isNum(std::string str){
    for(int i=0;i<str.size();i++)
        if(!isdigit(str[i])) return false;
    
    return true;
}
std::string binstr(unsigned int dec){
    std::string temp;
    temp.resize(16);
    for(int i=15;i>=0;i--){
        if(dec%2) temp[i] = '1';
        else temp[i] = '0';
        dec>>=1;
    }
    return temp;
}
int strToNum(std::string str){//isNum(str) must true
    int temp=0;
    for(int i=0; i<str.size();i++){
        temp*=10;
        temp+=str[i]-48;
    }
    return temp;
}
int main(int argc, char** argv){
    std::ifstream reader(argv[1]);
    std::ofstream writer(argv[2]);

    if(!reader.is_open()){
        std::cout<<"file dosen't exist\n";
        return -1;
    }
    int curLine,size,anno,jmploc,ramloc;
    char word;
    std::string file;
    std::string buffer;
    file.resize(0);
    // 위치 지정자를 파일 끝으로 옮긴다.
    reader.seekg(0, std::ios::end);
    // 그리고 그 위치를 읽는다. (파일의 크기)
    size = reader.tellg();
    // 그 크기의 문자열을 할당한다.
    file.reserve(size);

    // 위치 지정자를 다시 파일 맨 앞으로 옮긴다.
    reader.seekg(0, std::ios::beg);

   while(reader){
       std::getline(reader,buffer);
       if(!reader) break;
       file = file + buffer + '\n';
   }
//    std::cout << file << std::endl;
    //주석지우기
    anno=0;
    while(anno < file.size()){
        anno = file.find("//",anno);
        if(anno == file.npos) break;
        size = file.find('\n',anno) - anno;
        file.erase(anno,size);
    }
    file.erase(std::remove_if(file.begin(), file.end(), isSpace), file.end());// '\n'제외 모든 공백 삭제
//    std::cout<<file<<'\n';
    //빈 줄 없애기
    anno=0;
    while(anno < file.size()){
        anno = file.find("\n\n",anno);
        if(anno == file.npos) break;
        file.erase(anno,1);
    }
    if(file[0]=='\n') file.erase(file.begin());
//    std::cout<<file<<'\n';
    //(lable) 처리
    LableTable LT;

    jmploc=anno=0;
    while(anno<file.size()){
        if(file[anno]=='('){
            size = file.find(')',anno) - anno + 1;
            buffer = file.substr(anno,size);
            buffer.erase(buffer.end()-1);
            buffer.erase(buffer.begin());
            LT.insert_Symbol(buffer,jmploc);
            if(file.find('\n',anno) == file.npos) // last line without '\n'
                file.erase(anno,size);
            
            else file.erase(anno,size + 1);
        }
        else {
            jmploc++;
            anno = file.find('\n',anno);
            if(anno == file.npos) break;
            else anno++;
        }
    }
    //여기서부터 본 명령 처리
    anno = 0;//항상 줄의 첫번째를 가리킨다
    ramloc = 16;// 변수 위치
    long long int temp =0;
    while(anno < file.size()){
        int oper = 0;
        temp = file.find('\n',anno);
        if(temp == file.npos){
            temp = file.size();
            buffer = file.substr(anno);
        }
        else buffer = file.substr(anno,temp - anno);
        std::cout<<buffer<<'\n';
        if(buffer[0] == '@'){//A operation
            buffer.erase(buffer.begin());
   //         std::cout<<buffer<<'\n';
            if(buffer[0]=='0' && buffer[1]=='x'){
                oper += std::stoi(buffer.substr(2,4),0,16);
            }
            else if(isNum(buffer)){
    //            std::cout<<"number\n";
                oper += std::stoi(buffer,0,10);
            }
            else if(LT.isDefined(buffer)){
    //            std::cout<<"defined symbol\n";
                oper += LT.mapping_sym(buffer);
            }
            else{
     //           std::cout<<"undefined\n";
                LT.insert_Symbol(buffer,ramloc);
    //            std::cout<<"insert complete\n";
                oper += ramloc++;
            }
        }
        else{//C operation form: Dest=Comp;Jmp
            oper += 0xe000;
            unsigned int cur = buffer.find(';');
            if(cur != buffer.npos){//jump 있음
                std::cout<<buffer.substr(cur+1)<<'\n';
                oper+=LT.mapping_jmp(buffer.substr(cur+1));
                std::cout<<std::hex<<oper<<' ';
                buffer.erase(cur);
            }
            //comp는 반드시 있음
            cur = buffer.find('=');
            if(cur != buffer.npos){//dest 있음
                oper+=LT.mapping_comp(buffer.substr(cur+1));
                std::cout<<std::hex<<oper<<' ';
                buffer.erase(cur);
                oper += LT.mapping_dest(buffer);
                std::cout<<std::hex<<oper<<' ';
            }
            else{//dest 없음
                oper += LT.mapping_comp(buffer);
                std::cout<<std::hex<<oper<<' ';
            }
        }
        //write oper on output file
        std::string tempstr = binstr(oper);
        std::cout<<tempstr<<'\n';
        writer<<tempstr<<'\n';
        anno = temp+1;
        std::cout<<temp<<' '<<anno<<' '<<file.size()<<'\n';
    }
    return 0;
}