#include <iostream>
#include<vector>
#include <string>
#include <sys/types.h>
#include <dirent.h>

typedef std::vector<std::string> strvec;
void read_directory(const std::string& name, strvec& v);
int main(int argc, char** argv){
    if(argc>2) return 0;
    strvec buffer;
    read_directory(argv[1],buffer);
    for(int i = 0; i<buffer.size(); i++){
        std::cout<<buffer[i]<<"\n";
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