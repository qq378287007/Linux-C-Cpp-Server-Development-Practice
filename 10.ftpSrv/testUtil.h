#pragma once

#ifdef TEST
#include <iostream>
using namespace std;
#define testout(msg) cout << msg << endl << flush
#else 
#define testout(msg) 
#endif
