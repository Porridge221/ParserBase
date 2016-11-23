#include <iostream>
#include "ParserBase.h"

using namespace std;

int main() {
    ParserBase Base("-(-2)+y*3+15* 2+4*3 + (5+7) * sin(-1+1) / 4- 2");
    //ParserBase Base = ParserBase("-(-3)");
    //ParserBase Base = ParserBase("3");
    //ParserBase Base("y + sin(0)");
    Base.Assign("y", 2);
    Base.Assign("sina", 5);
    Base.Execute();


    return 0;
}