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

bitVector classify(vector<long long>& input)
{     
    
    
    size_t n = input.size();
    
    bitVector types(n, 0); 
    
    // 0 = S
    //1 = L

    types.set0(n - 1);

    for (long long i = n - 2; i >= 0; i--)
    {
        if (input[i] < input[i + 1])
        {
            types.set0(i);
        }
        else if (input[i] > input[i + 1])
        {
            types.set1(i);
        }
        else
        {
            (types[i + 1] == 1) ? types.set1(i) : types.set0(i);
        }
    }
    return types;
}

vector<size_t> findLMSchars(const bitVector& types)
{
    vector<size_t> positions;
    if (types.size() < 2) return positions;
    if(types[0] == 0)
    {
        positions.push_back(0);
    }
    for (size_t i = 1; i < types.size(); i++)
    {
        if (types[i] == 0 && types[i - 1] == 1)
        {
            positions.push_back(i);
        }
    }
    return positions;
}


size_t getLMSLength(size_t pos, const bitVector& types)
{
    size_t input_size = types.size();
    if (pos >= input_size - 1) return 1;
    
    size_t len = 1;
    while (pos + len < input_size)
    {
        if (!types[pos + len] && types[pos + len - 1])
        {
            len++;
            break;
        }
        len++;
    }
    return len;
}

bool LMSequal(size_t pos1, size_t pos2, const vector<long long>& input, const bitVector& types)
{
    if (pos1 == pos2) return true;
    
    size_t len1 = getLMSLength(pos1, types, input.size());
    size_t len2 = getLMSLength(pos2, types, input.size());

    if (len1 != len2) return false;

    for (size_t i = 0; i < len1; i++)
    {
        if (input[pos1 + i] != input[pos2 + i] || types[pos1 + i] != types[pos2 + i])
        {
            return false;
        }
    }
    return true;
}

bool isLMS(size_t pos, const bitVector& types)
{

    if (pos == 0 && types[pos] == 0)
    {
        return true;
    }
    if(pos == 0)
    {
        return false;
    }
    
    return types[pos] == 0 && types[pos - 1] == 1;
}

vector<long long> main_loop(const vector<long long>& input, const vector<size_t>& positions, const bitVector& types, vector<Rule>& rules, long& level, size_t& alphabet_size)
{

    vector<size_t> count(alphabet_size, 0);
    
    for(auto c : input)
    {
        count[c]++;
    }
    
    vector<size_t> bucket_heads(alphabet_size, 0);
    vector<size_t> bucket_tails(alphabet_size, 0);
    
    size_t sum = 0;
    
    for (size_t i = 0; i < alphabet_size; i++)
    {
        bucket_heads[i] = sum;
        sum += count[i];        
        bucket_tails[i] = sum;
    }

    //Naming
    vector<int> SA(input.size(), -1);
    vector<size_t> tails = bucket_tails; 
    for (long long i = positions.size() - 1; i >= 0; i--)
    {
        size_t pos = positions[i];
        SA[--tails[input[pos]]] = pos;
    }

    //Induce L
    vector<size_t> heads = bucket_heads;
    for (size_t i = 0; i < SA.size(); i++)
    {
        if (SA[i] > 0)
        { 
            int prev_pos = SA[i] - 1;
            if (types[prev_pos])
            {
                SA[heads[input[prev_pos]]++] = prev_pos;
            }
        }
    }

    //Induce S
    tails = bucket_tails;
    for (long long i = SA.size() - 1; i >= 0; i--)
    {
        if (SA[i] > 0)
        {
            int prev_pos = SA[i] - 1;
            if (!types[prev_pos])
            {
                SA[--tails[input[prev_pos]]] = prev_pos;
            }
        }
    } 
    
    long long prev_lms = -1;
    size_t first_lms_pos = positions.empty() ? input.size() : positions[0];
    
    long long current_name = 1;
    vector<long long> CFG(positions.size(), -1); 
    if(first_lms_pos > 0)
    {
        CFG.resize(positions.size() + 1, -1);
        vector<size_t> prefix_text(input.begin(), input.begin() + first_lms_pos);
        rules.push_back({level, 1, prefix_text});
        CFG[0] = 1;
        current_name = 2;
    }

    for (size_t i = 0; i < SA.size(); i++)
    {
        int pos = SA[i];
        if (pos >= 0 && isLMS(pos, types))
        {
            auto it = lower_bound(positions.begin(), positions.end(), pos);
            int cfg_index = distance(positions.begin(), it);
            if (first_lms_pos > 0)
            {
                cfg_index += 1;
            }
            if (prev_lms == -1)
            {
                CFG[cfg_index] = 0;
                rules.push_back({level, 0, {0}}); 
            }
            else
            {
                if (LMSequal(prev_lms, pos, input, types))
                {
                    CFG[cfg_index] = rules.back().name;
                }
                else
                {
                    CFG[cfg_index] = current_name;
                    size_t len = getLMSLength(pos, types, types.size()) - 1;
                    vector<size_t> text(input.begin() + pos, input.begin() + pos + len);
                    rules.push_back({level, current_name, text});
                    
                    current_name++;
                }
            }
            prev_lms = pos;
        }
    }

    level++;
    
    alphabet_size = current_name + 1; 
    
    return CFG;
}

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