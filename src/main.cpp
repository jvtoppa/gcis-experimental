
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>  
#include "gcis.h"
#include <functional>
#include <chrono>
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
    auto start = chrono::high_resolution_clock::now();    
    GCIS compressor(T);
    
    compressor.compress
    (
    [&compressor]() -> bool
    {
        if (compressor.getCFG().empty()) 
        {
            return true; 
        }

        return compressor.getCFG().size() != (compressor.getAlphabet_size() - 1); //Get pidgeonholed!
    }
    
    );
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> time = end - start;
    cout << "[CONSOLE] Compression time: "<< time.count() << "ms\n";
    //compressor.output();
    vector<uint64_t> ip;
    for (auto token : T) 
    {
        ip.push_back(static_cast<uint64_t>(token));
    }
    
    auto start2 = chrono::high_resolution_clock::now();
    auto decompressed = compressor.decompress();
    auto end2 = chrono::high_resolution_clock::now();
    
    chrono::duration<double, milli> time2 = end2 - start2;
    
    cout << "[CONSOLE] Decompression time: "<< time2.count() << "ms\n";
    
    bool fail = false;
    for (size_t i = 0; i < T.size(); i++)
    {
        unsigned char orig_byte = static_cast<unsigned char>(T[i]);
        [[unlikely]] if(orig_byte != decompressed[i])
        {fail = true; cout << i << ", T[i] = " << static_cast<int>(orig_byte) << " decompressed[i] = " << decompressed[i] << "\n";}
        
    }
    if(fail)
    cout << "Failed to decompress...\n";
    else
    cout << "[CONSOLE] Decompression worked!\n";
}