#include "compresse.h"





int main(int argc, char const *argv[])
{
    
    if(argc == 3)
    {
        FILE* f = fopen(argv[1],"rb");
        FILE* f1 = fopen(argv[2],"wb");
        compress(f,f1,8);
        fclose(f);
        fclose(f1);
    }
    if(argc == 4)
    {
        FILE* f = fopen(argv[1],"rb");
        FILE* f1 = fopen(argv[2],"wb");
        compress(f,f1,*argv[3]-'0');
        fclose(f);
        fclose(f1);
    }
    else
    {
        printf("Warning : \n<usage> : cmd src_file dest_file\n");
        return 1;
    }
    
   
    
    

    return 0;
}

