//
// Created by sylwester on 8/14/19.
//

#ifndef ALGORITHMSPROJECT_FORBIDDENSTATES_H
#define ALGORITHMSPROJECT_FORBIDDENSTATES_H

#include "Makros.h"

/**
 * ForbiddenStates is just a class that can be used to store states, that were recently visited.
 * It can be used to prevent cycles in local search algorithms.
 */
class ForbiddenStates {

public:

    ForbiddenStates();

    /**
     *
     * @param stateHash
     * @return true if the state with given hash is forbidden, false it it is not forbidden
     */
    bool isForbidden( LL stateHash ){ return forbiddenStates.count(stateHash ) > 0; }

    /**
     * Function clears forbidden states. After this function o state is forbidden.
     */
    void clearForbiddenStates(){ forbiddenStates.clear(); forbiddenStatesQueue.clear(); }

    /**
     * Function calculates a new hash for given stateHash, based on which elements were added and removed
     * @param stateHash
     * @param addedElements
     * @return hash after modification
     */
    template<typename _T>
    long long modifyHash( long long stateHash, vector<_T> addedElements, vector<_T> removedElements ){
        long long hashDiff = 0;
//        cerr << endl << "modifying hạsh" << endl;
//        cerr << "stateHash = " << stateHash << endl;
        for( auto d : addedElements ){
            hashDiff += getHash( d );
//            cerr << "adding " << d << " with hash " << getHash( d ) << endl;
            if( hashDiff >= hashBound ) hashDiff -= hashBound; // i can take - instead of % (it is the same for both values less than hashBound
        }



        for( auto d : removedElements ){
            hashDiff -= getHash( d );
//            cerr << "removing " << d << " with hash " << getHash( d ) << endl;
            if( hashDiff < 0 ) hashDiff += hashBound;
        }

//        cerr << "hashDiff: " << hashDiff << endl;

        stateHash += hashDiff;
        if( stateHash < 0 ){
            cerr << "state hash = " << stateHash << endl;
            exit(1);
        }
        if( stateHash >= hashBound ) stateHash -= hashBound;
        return stateHash;

    }

    /**
    * Function calculates the hash of added element. It usually may be (2^d) % hashBound.
    * Such values can be stored in a table to quickly access it.
    * @param d
    * @return hash of en element d that is added to the state.
    */
    long long getHash(long long int d);


    /**
     * Function adds given hash to forbidden states and removes the last state if @{forbiddenStatesQueue} is overfilled;
     * If the state was already forbidden, this function does nothing at all.
     * @param stateHash
     */
    void addToForbiddenStates( long long stateHash );

    /**
     * maximum number of forbidden states that is kept
     * @return
     */
    int getMaximumForbiddenStatesSize() const;


    /**
     *
     * @return value P such that all hashes are taken modulo P
     */
    long long getHashBound() const;

    /**
     *
     * @return size of the universe (e.g. for graphs it is usually N - number of nodes).
     */
    long long int getUniverseSize() const;

    /**
     * Sets universe size to other value. Note, that @{elementHashes} array will be reset (and all hashes will have to be recalculated) if it is used.
     * @param universeSize
     */
    void setUniverseSize(long long int universeSize);

    /**
     * If true, then hashes will be stored in an array of isze universeSize.
     * @param storeElementHashes
     */
    void setStoreElementHashes(bool storeElementHashes);

private:



    /**
     * Hash set containing all hashes of forbidden sets
     */
    unordered_set<long long> forbiddenStates;

    /**
     * Queue in qhichc there is a history of forbidden states
     */
    deque<long long> forbiddenStatesQueue;

    /**
     * Maximal number of states that will be kept in history (in the @{forbiddenStatesQueue})
     * A forbidden state is a hash of current solution
     */
    int maximumForbiddenStatesSize;
public:
    void setMaximumForbiddenStatesSize(int maximumForbiddenStatesSize);

private:


    /**
     * All hashes will be taken modulo hashBound
     */
    long long hashBound;

    /**
     * If all elements forming a state are bounded (as is e.g. in subsets of a graph, where states contain nodes - all elements are bounded by N - the number of nodes in the graph), then i can
     * store and quickly access hash values of elements from that universe.
     */
    long long universeSize;

    /**
     * If true, then all hashes of elements will be stored in an array of size universeSize.
     * Default value is true
     */
    bool storeElementHashes;



    /**
     * Array of hashes used if storeElementHashes is set to true. It is much faster to access element hash from array than calculate it perpetually.
     */
    VLL elementHashes;

};


#endif //ALGORITHMSPROJECT_FORBIDDENSTATES_H
