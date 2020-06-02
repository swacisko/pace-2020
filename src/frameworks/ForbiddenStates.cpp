/*
 * Copyright (c) 2020, Sylwester Swat
*/
#include <frameworks/ForbiddenStates.h>
#include <numbertheory/NumberTheoryUtils.h>



ForbiddenStates::ForbiddenStates() {
    hashBound = 100'000'000;
    hashBound *= hashBound;
    hashBound++; // 10^16+1

    maximumForbiddenStatesSize = 0;
    universeSize = 0;
    storeElementHashes = false;
    elementHashes = VLL();
}

int ForbiddenStates::getMaximumForbiddenStatesSize() const {
    return maximumForbiddenStatesSize;
}



long long ForbiddenStates::getHashBound() const {
    return hashBound;
}

long long ForbiddenStates::getHash(long long int d) {
    if( storeElementHashes ){
//        cerr << "storing Element Hash d = " << d << endl;
//        exit(1);
//cerr << "elementHashes: " << elementHashes << endl;
//cerr << "elementHashse[" << 2 << "] = " << elementHashes[d] << endl;
        if( elementHashes[d] == -1){
            long long hash = NumberTheoryUtils::expMod(2, d, hashBound);
            elementHashes[d] = hash;
        }

        return elementHashes[d];
    }else{
        return NumberTheoryUtils::expMod(2, d, hashBound);
    }
}

//template<typename _T>
//long long ForbiddenStates::modifyHash(long long stateHash, vector<_T> addedElements, vector<_T> removedElements) {
//    long long hashDiff = 0;
//    for( auto d : addedElements ){
//        hashDiff += getHash( d );
//        if( hashDiff >= hashBound ) hashDiff -= hashBound; // i can take - instead of % (it is the same for both values less than hashBound
//    }
//
//    for( auto d : removedElements ){
//        hashDiff -= getHash( d );
//        if( hashDiff < 0 ) hashDiff += hashBound;
//    }
//
//    stateHash += hashDiff;
//    if( stateHash >= hashBound ) stateHash -= hashBound;
//    return stateHash;
//
//}

void ForbiddenStates::addToForbiddenStates(long long stateHash) {
    if( !isForbidden(stateHash) ){ // i do something only if the hash is not fobidden yet.
        forbiddenStates.insert(stateHash);
        forbiddenStatesQueue.push_back(stateHash);
        if( forbiddenStatesQueue.size() > maximumForbiddenStatesSize ){
            forbiddenStates.erase( forbiddenStatesQueue.front() );
            forbiddenStatesQueue.pop_front();
        }
    }

}

long long int ForbiddenStates::getUniverseSize() const {
    return universeSize;
}

void ForbiddenStates::setUniverseSize(long long int universeSize) {
    this->universeSize = universeSize;
    if( storeElementHashes ) elementHashes = VLL( universeSize,-1 );
}

void ForbiddenStates::setStoreElementHashes(bool storeElementHashes) {
//    cerr << "setting value to " << storeElementHashes << endl; exit(1);
    this->storeElementHashes = storeElementHashes;
    if( storeElementHashes ) elementHashes = VLL( universeSize,-1 );
}

void ForbiddenStates::setMaximumForbiddenStatesSize(int maximumForbiddenStatesSize) {
    ForbiddenStates::maximumForbiddenStatesSize = maximumForbiddenStatesSize;
}

