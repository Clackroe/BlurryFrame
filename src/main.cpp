#include <stdio.h>
#include <iostream>
#include "core.h"

using namespace std;

int main (){

    cout << "Hello World!" << endl;

    // sample path for testing
    const char* path = "test_content/RentalMeme.png";

    // call the function from core.h
    display_image(path);

    return 0;
}
