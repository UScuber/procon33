#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

enum{EXIT, DOWNLOAD, ANALYZE, SUBMIT, CHANGE, SETSEP};

int sep_num = 1;

int main(){
    int n;
    int last_op = -1;

    while(true){
        puts("DOWNLOAD(1) ANALYZE(2) SUBMIT(3) CHANGE TEMP(4) SETSETP(5) EXIT(0)");
        std::cin >> n;
        switch (n){
            case DOWNLOAD:
                char buf[64];
                #if defined(_WIN32) || defined(_WIN64)
                    sprintf(buf, "download.cmd %d", sep_num);
                    system(buf);
                #else
                    sprintf(buf, "bash download.sh %d", sep_num);
                    system(buf);
                #endif
            break;
            case ANALYZE:
                /*
                if(ANALYZE == last_op){
                    std::cout << "Already Analyzed\n";
                    break;
                }
                */
                #if defined(_WIN32) || defined(_WIN64)
                    system("analyze.cmd");
                #else
                    system("bash analyze.sh");
                #endif
                break;
            case SUBMIT:
                #if defined(_WIN32) || defined(_WIN64)
                    system("submit.cmd");
                #else
                    system("bash submit.sh");
                #endif
                break;
            case CHANGE:
                #if defined(_WIN32) || defined(_WIN64)
                    system("change_temp.exe");
                #else
                    system("./change_temp");
                #endif
                break;
            case SETSEP:
                printf("separate num: ");
                std::cin >> sep_num;
                break;
            case EXIT:
                return 0;
        }
        last_op = n;
    }
    return 0;
}