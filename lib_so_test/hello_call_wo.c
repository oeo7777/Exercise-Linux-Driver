#include <stdio.h>  

extern void world(void);  
void hello(void) {  
    printf("hello\n");  
    world();  
}  
