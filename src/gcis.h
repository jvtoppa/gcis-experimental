#ifndef GCIS_H
#define GCIS_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdint>
#include "../include/bitvector.h"

using namespace std;

struct Rule
{
    long level;
    long long name;
    vector<size_t> right_side_production;
};

class GCIS
{
private:
    vector<Rule> rules;
    vector<long long> CFG;
    long level = 1;
    size_t alphabet_size = 256;
    bitVector types;
    vector<size_t> positions;
    vector<size_t> SA;
    /**
    * Classifies the string in L or S.
    * @param input A vector of uint64_t for classification.
    * @return a bitvector where S = 0 and L = 1.
    */
    bitVector classify(vector<uint64_t>& input);
    
    /**
    * Finds all characters that are LMS. Needs types to be properly filled.
    * @return A vector of size_t where each element of the vector is a position in types that is LMS.
    */
    vector<size_t> findLMSchars();
    
    /**
    * Sets all positions on the SA vector to the distance to the closest LMS to the right for any given pos.
    */
    void buildLMSLength();
    
    /** Finds if any two given positions represent the same LMS (have the same name).
    * @param pos1 The first position.
    * @param pos2 The second position.
    * @return A boolean whether pos1 and pos2 represent the same LMS.
    */
    bool LMSequal(size_t pos1, size_t pos2);
    

    /** Finds if any given position is an LMS character.
    * @param pos The position that is being tested.
    * @return A boolean whether pos is an LMS character.
    */
    bool isLMS(size_t pos);
public:
    GCIS(){};
    ~GCIS(){};

    vector<long long> gen(const vector<long long>& input, const vector<size_t>& positions, const bitVector& types, vector<Rule>& rules, long& level, size_t& alphabet_size);
    

};


#endif 