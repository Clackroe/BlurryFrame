#include <stdio.h>
#include <iostream>
#include "core.h"

using namespace std;

int main (){

    cout << "Hello World!" << endl;

    // display an image pwease ;)
    const char* path = "/my/path";
    display_image(path);

    return 0;
}
