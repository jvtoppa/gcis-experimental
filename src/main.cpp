
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>  
#include "gcis.h"
#include <functional>

using namespace std;


int main(int argc, char* argv[])
{
    
	string T;
	
	if (argc < 2)
	{
		ostringstream buffer;
		buffer << cin.rdbuf();
		T = buffer.str();
		cout << "Read " << T.size() << " charaters. \n";
	}
    else
	{
		unsigned long max_chars = stoul(argv[1]);
			
		T.resize(max_chars);
	
		cin.read(&T[0], max_chars);
		
		T.resize(cin.gcount());

	}
    
    GCIS compressor(T);
    
    compressor.compress
    (
        [&compressor]() -> bool
        {
            return compressor.getLevel() <= 3; 
        }
    );

    compressor.output();

}