#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdint>
#include "gcis.h"
#include "../include/bitvector.h"
using namespace std;


bitVector GCIS::setTypes()
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

vector<size_t> GCIS::setPositions()
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


void GCIS::setLMS_distance()
{
    if (this->positions.empty()) return;

    size_t input_size = types.size();
    this->lms_distance.resize(input_size, 0);

    size_t len = 0;
    long long j = this->positions.size() - 1;
    for (long long i = input_size; i > 0; i--)
    {
        len++;
        if(this->positions[j] == static_cast<size_t>(i - 1))
        {
            this->lms_distance[i - 1] = len;
            len = 1;
            if(j > 0) j--;
            continue;
        } 
        this->lms_distance[i - 1] = len;
    }
    return;
}

bool GCIS::LMSequal(size_t pos1, size_t pos2)
{
    if (pos1 == pos2) return true;
    
    size_t len1 = this->lms_distance[pos1];
    size_t len2 = this->lms_distance[pos2];

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

bool GCIS::isLMS(size_t pos)
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

void GCIS::naming()
{
    vector<size_t> tails = bucket_tails; 
    for (long long i = positions.size() - 1; i >= 0; i--)
    {
        size_t pos = positions[i];
        SA[--tails[input[pos]]] = pos;
    }
    return;
}

void GCIS::induce_l()
{

    vector<size_t> heads = bucket_heads;    
    for (size_t i = 0; i < SA.size(); i++)
    {
        if (SA[i] != -1 && SA[i] > 0)
        { 
            int prev_pos = SA[i] - 1;
            if (types[prev_pos])
            {
                SA[heads[input[prev_pos]]++] = prev_pos;
            }
        }
    }

}

void GCIS::induce_s()
{

    vector<size_t> tails = bucket_tails;
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
}


void GCIS::setBuckets()
{
    vector<size_t> count(alphabet_size, 0);
    
    for(auto c : input)
    {
        count[c]++;
    }
    
    this->bucket_heads.assign(alphabet_size, 0);
    this->bucket_tails.assign(alphabet_size, 0);

    size_t sum = 0;
    
    for (size_t i = 0; i < alphabet_size; i++)
    {
        bucket_heads[i] = sum;
        sum += count[i];        
        bucket_tails[i] = sum;
    }
}

void GCIS::findCFGandRules()
{
    long long prev_lms = -1;
   
    size_t first_lms_pos = positions.empty() ? input.size() : positions[0];
    
    long long current_name = 1;
    
    size_t cfg_size = positions.size() + (first_lms_pos > 0 ? 1 : 0);

    this->CFG.assign(cfg_size, -1);

    RuleLevel current_level;
    current_level.rule_pointers.push_back(0);
    current_level.right_sides.push_back(0); 
    current_level.rule_pointers.push_back(current_level.right_sides.size());
    
    /*
    Takes care of the leftmost rule in the case that the first rule is not an LMS.
    If it is not an LMS, does not enter the branch.
    */
    
    if(first_lms_pos > 0)
    {
        for(size_t k = 0; k < first_lms_pos; ++k)
        {
            current_level.right_sides.push_back(input[k]);
        }
        current_level.rule_pointers.push_back(current_level.right_sides.size());
        
        CFG[0] = 1;
        current_name = 2;
    }

    /*
    This is the inverse index of the positions vector (Which positions are LMS?). Uses quite a bit of memory.
    Could be improved further!  
    */
    vector<int> lms_index_map(types.size(), -1);

    for (size_t i = 0; i < positions.size(); i++)
    {
        size_t strpos = positions[i];
        lms_index_map[strpos] = i;
    }
    
    for (size_t i = 0; i < SA.size(); i++)
    {
        int pos = SA[i];
        if (pos < 0 || !isLMS(pos)) continue;        
        size_t cfg_index = lms_index_map[pos] + (first_lms_pos > 0 ? 1 : 0);

        /*
        Takes care of the rightmost rule. 
        It should always be rule 0 (or the lowest rule, maybe change?).
        */
        if (prev_lms == -1)
        {
            CFG[cfg_index] = 0;
        }
        /*
        If the LMS are equal, they have the same name. 
        */
        else if (LMSequal(prev_lms, pos)) 
        {
            CFG[cfg_index] = current_name - 1;
        }
        else
        {
            CFG[cfg_index] = current_name;
            
            for (size_t k = 0; k < lms_distance[pos] - 1; ++k)
            {
                current_level.right_sides.push_back(input[pos + k]);
            }
            
            current_level.rule_pointers.push_back(current_level.right_sides.size());
            
            current_name++;
        }

        prev_lms = pos;
    }
    this->rules.push_back(current_level);
    this->level++;
    this->alphabet_size = current_name + 1; 
 
}

void GCIS::gen()
{
    this->types = setTypes();
    this->positions = setPositions();
    this->setLMS_distance();   
    this->setBuckets();

    this->SA.assign(input.size(), -1);
    
    this->naming();
    this->induce_l();
    this->induce_s();
    
    bucket_heads.clear();
    bucket_heads.shrink_to_fit();
    bucket_tails.clear();
    bucket_tails.shrink_to_fit();
    
    this->CFG.assign(positions.size(), -1); 
    this->findCFGandRules();
}

vector<uint64_t> GCIS::decompress(const vector<RuleLevel>& rules_dummy,const vector<int64_t>& CFG_dummy) const
{
    vector<uint64_t> decompressed_text;
    for(auto symbol : CFG_dummy)
    {
        decompressed_text.push_back(symbol);
    }

    for(long long lvl = rules_dummy.size() - 1; lvl >= 0; lvl--)
    {
        vector<uint64_t> expanded;
        const auto& current_level = rules_dummy[lvl];
        for(auto symbol : decompressed_text)
        {
            size_t start = current_level.rule_pointers[symbol];
            size_t end = current_level.rule_pointers[symbol + 1];
            for(size_t k = start; k < end; k++)
            {
                expanded.push_back(current_level.right_sides[k]);
            }
        }
        decompressed_text = move(expanded);
    }
    
    return decompressed_text;
}

vector<uint64_t> GCIS::decompress() const
{
    return decompress(this->rules, this->CFG);
}
