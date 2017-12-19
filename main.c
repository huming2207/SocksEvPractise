#include "common.h"

int main(int argc, char ** argv) {

    bool verbose_opt = false;
    int option_count;
    enum working_mode mode;

    while((option_count = getopt(argc, argv, "scv")) != -1) {
        switch(option_count){
            case 's': mode = SERVER_MODE; break;
            case 'c': mode = CLIENT_MODE; break;
            case 'v': verbose_opt = false;
            default:{
                printf("Usage: SocksEvPractise [-sc] [-v]\n");
                exit(EXIT_FAILURE);
            }
        }
    }

}