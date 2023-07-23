#include "render.h"
#include <iostream>

int main(){
    try{
        Renderer r;
        r.Launch();
    }catch(const std::runtime_error& e){
        std::cout << e.what() << std::endl; 
    }
}