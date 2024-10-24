#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void camera() {
    char cmd[] = "sudo libcamera-jpeg --width 800 --height 800 -t 1 -o test.jpeg";
    system(cmd);
    printf("Take a picture \n");
}