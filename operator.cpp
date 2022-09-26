#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

enum{EXIT, DOWNLOAD, ANALYZE, SUBMIT};



int main(){
    int n;
    
    while(true){
        puts("DOWNLOAD(1) ANALYZE(2) SUBMIT(3) EXIT(0)");
        std::cin >> n;
        switch (n){
            case DOWNLOAD:
                system("download.cmd");
                break;
            case ANALYZE:
                system("analyze.cmd");
                break;
            case SUBMIT:
                system("submit.cmd");
                break;
            case EXIT:
                return 0;
        }
    }
    return 0;
}