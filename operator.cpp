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
                #if defined(_WIN32) || defined(_WIN64)
                    system("download.cmd");
                #else
                    system("./download.sh");
                #endif
            break;
            case ANALYZE:
                #if defined(_WIN32) || defined(_WIN64)
                    system("analyze.cmd");
                #else
                    system("./analyze.sh");
                #endif
                break;
            case SUBMIT:
                #if defined(_WIN32) || defined(_WIN64)
                    system("submit.cmd");
                #else
                    system("./submit.sh");
                #endif
                break;
            case EXIT:
                return 0;
        }
    }
    return 0;
}