#include <iostream>
#include<vector>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include "Parser.h"
#include "CodeWriter.h"
typedef std::vector<std::string> strvec;
void read_directory(const std::string& name, strvec& v);

int main(int argc, char** argv){
    if(argc>3) return -1;
    strvec buffer;
    read_directory(argv[1],buffer);
    for(const auto &str:buffer){
        std::cout<<str<<"\n";
    }
    if(argc!=3) return 0;

    std::ofstream writer(argv[2]);
    CodeWriter composer(writer);

    for(const auto &str:buffer){
        if(*(str.rbegin()+2)=='.'&&*(str.rbegin()+1)=='v'&&*(str.rbegin())=='m'){
            
            std::string fileDir = static_cast<std::string>(argv[1])+"/"+str;
            std::cout<<fileDir<<'\n';
            std::ifstream reader(fileDir);
            Parser analyzer(reader);
            composer.currentFile(str);
            Command temp;//{string, string, int} -> parser return용 struct 
            while(analyzer.isRemain()){
                temp = analyzer.getCommand();
                std::cout<<temp.type<<'\n';
                composer.writeCode(temp.type,temp.arg1,temp.arg2);
            }
        }
    }
    return 0;
}
void read_directory(const std::string& name, strvec& v){
    DIR* pdir = opendir(name.c_str());
    struct dirent *dp;
    while( (dp = readdir(pdir)) != NULL){
        v.push_back(dp->d_name);
    }
    closedir(pdir);    
}