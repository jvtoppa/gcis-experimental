/**
 * @mainpage GCIS Library Reference
 *
 * @section intro_sec Introduction
 * Welcome to the GCIS (Grammar Compressor with Induced Suffix Sorting) documentation. This program 
 * computes a Context-Free Grammar iteratively by finding and sorting 
 * LMS substrings.
 *
 * @section usage_sec Quick Usage
 * @code
 * GCIS gcis("abracadabra");
 * gcis.compress([]() { return false; }); // Run until baseline termination
 * gcis.output();
 * @endcode
 */

#ifndef GCIS_H
#define GCIS_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdint>
#include <functional>
#include "../include/bitvector.h"
#include "../include/utils.h"
using namespace std;

/**
 * @brief Represents a single substitution rule in the context-free grammar.
 */
struct RuleLevel
{
    vector<uint64_t> rule_pointers;
    vector<uint64_t> right_sides;
};

/**
 * @brief Core driver class for the Grammar-Compressed Suffix Array algorithm.
 */
class GCIS
{
private:
    vector<RuleLevel> rules;     /**< Global dictionary storing all parsed grammar rules across levels. */
    vector<uint64_t> input;      /**< Working text string, modified dynamically per compression level. */
    vector<int64_t> CFG;         /**< Generated grammar parse stream forming the compressed output. */
    long level = 1;              /**< Monotonically increasing counter tracks current recursion depth. */
    size_t alphabet_size = 256;  /**< Effective vocabulary size, shifts dynamically based on rule naming. */
    bitVector types;             /**< Binary mapping categorizing string indices into S-Type (0) or L-Type (1). */
    vector<size_t> positions;    /**< Coordinates tracking the location of every LMS item in the array. */
    vector<int64_t> SA;          /**< The Suffix Array storage block used during induced bucket passes. */
    vector<size_t> lms_distance; /**< Stores distance interval measurements between sequential LMS items. */
    vector<size_t> bucket_heads; /**< Pointers navigating the starting thresholds of suffix sorting buckets. */
    vector<size_t> bucket_tails; /**< Pointers navigating the ending thresholds of suffix sorting buckets. */ 
    
    /**
    * Classifies the string in L or S.
    * @return a bitvector where S = 0 and L = 1.
    */
    bitVector setTypes();
    
    /**
    * Finds all characters that are LMS. Needs types to be properly filled.
    * @return A vector of size_t where each element of the vector is a position in types that is LMS.
    */
    vector<size_t> setPositions();
    
    /**
    * Sets all positions on the SA vector to the distance to the closest LMS to the right for any given pos.
    */
    void setLMS_distance();
    

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

    /** Induces the first part of the LMS substrings, based on the tails.
    */
    void naming();
    
    /** Induces the L part of the LMS substrings, based on the head - from right to left.
    */
    void induce_l();
    

    /** Induces the S part of the LMS substrings, based on the head - from left to right.
    */
    void induce_s();
    
    /** Initializes bucket_head and bucket_tail.
    */
    void setBuckets();

    /** Finds CFG and Rules for the i-th level.
    */
    void findCFGandRules();


    /** Generates the SA, the rules and the CFG for the i-th level.
    */
    void gen();

public:
    
    
    /**
     * @brief Instantiates a GCIS object and loads initial target raw data.
     * @param in Input string to compress.
     * @param lvl Starting level counter offset, defaults to 1.
     */
    GCIS(string in, long lvl = 1) : level(lvl), types(in.size(), 0)
    {
        for (char c : in)
        {
            input.push_back(static_cast<unsigned char>(c));
        }

        input.push_back(0); //Terminator. 
        
        this->types = setTypes();
        this->positions = setPositions();
        setLMS_distance();   
        setBuckets();
    };
    
/**
     * @brief Simple destructor.
     */
    ~GCIS(){};    

     /** @brief Drives the execution of the recursive compression framework.
     * @param stop_condition Extensible lambda function for triggering of loop behavior.
     */
    void compress(function<bool()> stop_condition)
    {
        
        while(stop_condition())
        {
            gen();
            if (!CFG.empty()) 
            {
                input.clear();
                for (auto token : CFG) 
                {
                    input.push_back(static_cast<uint64_t>(token));
                }
            }
        }
    }

    /** @return Read-only access to the final compressed context-free grammar parsing list. */
    vector<int64_t> getCFG() const {return CFG;}
    
    /** @return Read-only access to the structural substitution rule mapping dictionary. */
    vector<RuleLevel> const getRules() {return rules;}
    
    /** @return Numeric indicator signifying current structural height level. */
    long getLevel() const {return level;}

    /** @return Current size of the i-th alphabet. */
    size_t getAlphabet_size() const {return alphabet_size;}

    /**
     * @brief Serializes structure to console.
     */
    void output() const
    {
        if (CFG.empty() || rules.empty()) return;

        for (size_t level = 0; level < rules.size(); ++level)
        {
            cout << "\n\nLevel " << level << " -------------------\n\n";
            
            const auto& current_level = rules[level];
            
            for (size_t rule_name = 0; rule_name + 1 < current_level.rule_pointers.size(); ++rule_name)
            {
                cout << rule_name << " (" << level << ") -> ";
                
                size_t start_idx = current_level.rule_pointers[rule_name];
                size_t end_idx = current_level.rule_pointers[rule_name + 1];
                
                for (size_t k = start_idx; k < end_idx; ++k)
                {
                    cout << current_level.right_sides[k] << " ";
                }
                cout << "\n";
            }
        }
        
        cout << "\n\n-------------------\n\nCFG: ";
        for(auto symbol : CFG)
        {
            cout << symbol << " ";
        }
        cout << "\n";
    }

    /**
     * @brief Decompresses a compressed text.
     */
    vector<uint64_t> decompress(const vector<RuleLevel>& rules_dummy,const vector<int64_t>& CFG_dummy) const;
    vector<uint64_t> decompress() const;

};

#endif