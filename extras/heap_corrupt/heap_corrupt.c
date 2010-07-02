#include "sa7_win.h"
void ghetto_breakpoint(){
    ((void (*)())"\xcc\xc3")();
};

int main() {
    HANDLE newheap_h;
    LPVOID *block_x;
    LPVOID *block_y;
    LPVOID *block_z;
    const int ONE_MEG = 1024*1024*1024;
    int i;
    printf("\nI am the Heap Corruptor.\n\tI alloc a BIG (1024) block of X's");
    printf("\n\tUnder that, I alloc a LITTLE (256) block of Y's");
    printf("\n\tUnder that I alloc a LITTLE (256) block of Z's");
    printf("\n\tThen I copy LOTS (512) of the X's into the block of the Y's.");
    printf("\n\tattach debugger if you want, hit <ENTER>");
    getch();
    
    newheap_h = HeapCreate(NULL, 0, ONE_MEG*16); //1 MEG!?
    if(newheap_h <= 0){
        perror("HEAPCREATE"); 
    };
    printf("\n\n\t[+] Created fresh Heap @ 0x%.8x.", newheap_h);
    //_asm int 3;
    //getch(); 
    //--------------------------------- 
    block_x = HeapAlloc(newheap_h, NULL, 1024);
    if(block_x == NULL){
        perror("HEAPALLOC X's");
    } else {
        //printf("\nGot 0x%.8x!", block_x);
    };
    for(i=1; i <= 900; i++){
        //printf("\n0%.8x",((char *)block_x)+i);
        memcpy(((char *)block_x)+i, "X", sizeof("X"));
    };
    printf("\n\t[+] Alloc'd place for 'X's in fresh Heap @: 0x%.8x.", block_x);
    printf("\n\t\t'%c': is first character of the block of X's", (char *)block_x[1]);
    getch();
    _asm int 3;

    //---------------------------------

    block_y = HeapAlloc(newheap_h, NULL, 256);
    if(block_y == NULL){
        perror("HEAPALLOC Y's");
    } else {
        //printf("\nGot 0x%.8x!", block_y);
    };
    for(i=1; i <= 200; i++){
        //printf("\n0%.8x",((char *)block_y)+i);
        memcpy(((char *)block_y)+i, "Y", sizeof("Y"));
    };
    printf("\n\t[+] Alloc'd place for 'Y's in fresh Heap @: 0x%.8x.", block_y);
    printf("\n\t\t'%c': is first character of the block of Y's", (char *)block_y[1]);
    getch();
    _asm int 3;

    //---------------------------------
    block_z = HeapAlloc(newheap_h, NULL, 256);
    if(block_z == NULL){
        perror("HEAPALLOC Z's");
    } else {
        //printf("\nGot 0x%.8x!", block_z);
    };
    for(i=1; i <= 200; i++){
        //printf("\n0%.8x",((char *)block_z)+i);
        memcpy(((char *)block_z)+i, "Z", sizeof("Z"));
    };
    printf("\n\t[+] Alloc'd place for 'Z's in fresh Heap @: 0x%.8x.", block_z);
    printf("\n\t\t'%c': is first character of the block of Z's", (char *)block_z[1]);
    getch();
    _asm int 3;

    //---------------------------------
    printf("\n\t[+] Ok tryin to cause corruption place in place for Z's @0x%.8x.", block_z);
    memcpy((char *)block_y, (char *)block_x, 512); //too much for block_y
    printf("\n\t\t'%c': is first character of the block of Z's", (char *)block_z[1]);
    //_asm int 3;
};
