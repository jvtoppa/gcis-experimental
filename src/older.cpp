#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "../include/bitvector.h"
#include "../include/utils.h"
#include <limits>
#include <algorithm>
#include <sstream>
using namespace std;

struct Rule
{
    long level;
    long long name;
    vector<size_t> right_side_production;
};


int main(int argc, char* argv[]) {
    
	std::string T;
	
	if (argc < 2)
	{
		std::ostringstream buffer;
		buffer << std::cin.rdbuf();
		T = buffer.str();
		cout << "Read " << T.size() << " charaters. \n";
	}
    else
	{
		unsigned long max_chars = std::stoul(argv[1]);
		
		
		T.resize(max_chars);
	
		std::cin.read(&T[0], max_chars);
		
		T.resize(std::cin.gcount());

	}
    vector<long long> input;
    
    input.reserve(T.size());
    for (char c : T)
    {
        input.push_back(static_cast<unsigned char>(c));
    }
    input.push_back(0); 
    
    bitVector LMS = classify(input);
    vector<size_t> positions = findLMSchars(LMS);
    vector<Rule> rules;
    long level = 1;
    size_t alphabet_size = 256;
    vector<long long> CFG = main_loop(input, positions, LMS, rules, level, alphabet_size);
    cout << "[VERBOSE] Starting loop...\n";
    while(CFG.size() > 2)
    {
        cout << "[VERBOSE] loop...\n";
        LMS = classify(CFG);
        positions = findLMSchars(LMS);
        CFG = main_loop(CFG, positions, LMS, rules, level, alphabet_size);
    }
    cout << CFG << "\n";
    /*
    for (const auto& r : rules)
    {
        cout << "r(" << r.name << ") -> " << r.right_side_production << " @ lvl. " << r.level << "\n";
    } 
    */
    cout << "No. of rules: " << rules.size() << "\n";
    cout << "CFG size: " << CFG.size() << "\n";
    //vector<long long> CFG2 = main_loop(CFG, positions, LMS, rules, level, alphabet_size);
    return 0;
}