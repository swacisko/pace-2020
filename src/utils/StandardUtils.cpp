//
// Created by sylwester on 9/6/19.
//

#include "utils/StandardUtils.h"
#include "utils/RandomNumberGenerators.h"


namespace StandardUtils{

    int binarySearch( int p, int q, function<bool (int)> objComp ){
        int P = p;
        int Q = q+1;
        int d;

        while( P < Q ){
            d = ( (P+Q) >> 1 );
            if( objComp(d) ) Q = d;
            else P = d+1;
        }

        if(Q > q) return -1;
        else return Q;
    }

    VI extractIndices(VB &tab) {
        return toVI(tab);
    }

    VI toVI(VB &v) {
        VI res; res.reserve( max( 5, (int)v.size() / 2 ) );
        for(int i=0; i <v.size(); i++) if( v[i] ) res.push_back(i);
        return res;
    }

    VB toVB(int N, VI &v) {
        VB res(N,false);
        for(int d : v) res[d] = true;
        return res;
    }

    vector<string> split(string &s, string pat) {
        while( !s.empty() && s.back() == ' ' ) s.pop_back();
        if(s.empty()) return {};

        if( pat.size() == 1 ){
            vector<string> res;
            int p = 0, q = 1;
            while( p < s.size() ){
                while( q < s.size() && s[q] != pat[0] ) q++;
                res.push_back(s.substr(p, q-p));
                p = q+1;
                q++;
            }
            return res;
        }else{
            clog << "Split not implemented yet for patterns with length > 1" << endl;
            exit(18);
        }
    }

    VVI partitionToLayers(VI &partition) {
        int M = *max_element(ALL(partition));
        VVI v(M+1);
        for( int i=0; i<partition.size(); i++ ) if( partition[i] != -1 ) v[partition[i]].push_back(i);
        return v;
    }

    VI layersToPartition(VVI &layers) {
        int M = -1;
        for( int i=0; i<layers.size(); i++ ) M = max( M, *max_element( ALL(layers[i]) ) );

        VI part(M+1,-1);
        for( int i=0; i<layers.size(); i++ ) for(int t : layers[i]) part[t] = i;
        return part;
    }

    VI removeFromArrayPreserveOrder(VI &A, VI &B, VB &in_B) {
        for(int d : B) in_B[d] = true;
        VI new_order;
        new_order.reserve( A.size() );
        for( int d : A ) if( !in_B[d] ) new_order.push_back(d);
        for(int d : B) in_B[d] = false;
        return new_order;
    }

    void removeFromArrayPreserveOrderInplace(VI &A, VI &B, VB &helper) {
        for(int d : B) helper[d] = true;
        int p = 0;
        for( int i=0; i<A.size(); i++ ){
            if( !helper[A[i]] ){
                A[p] = A[i];
                p++;
            }
        }
        A.resize(p);
        for(int d : B) helper[d] = false;
    }

    void removeFromArrayInplace(VI &A, VI B) {
        if(A.empty()) return;

        sort(ALL(A));
        sort(ALL(B));
        int p = (int)A.size()-1, q = (int)B.size()-1;

        while( p >= 0 && q >= 0 ){
            if( A[p] == B[q] ){
                swap(A[p], A.back());
                A.pop_back();
                p--;
                q--;
            }
            else if( A[p] > B[q] ) p--;
            else q--;
        }
    }



    string toBinary(LL val, int bits) {
        string s = "";
        while( val > 0 ){
            if( val&1 ) s += "1";
            else s += "0";
            val >>= 1;
        }

        while( (int)s.size() < bits ) s += "0";
        reverse(ALL(s));
        return s;
    }

    string getRandomString(int length){
        UniformIntGenerator rnd(0,25);
        string s = "";
        for(int i=0; i<length; i++) s += 'a' + rnd.rand();
        return s;
    }

    string convertIntegerToString(LL val, vector<char> &possible_characters) {
        if(possible_characters.empty()){
            for( int i=0; i <= 'z' - 'a'; i++ ) possible_characters.push_back( char('a' + i) );
            for( int i=0; i <= 'Z' - 'A'; i++ ) possible_characters.push_back( char('A' + i) );
            for( int i=0; i <= '9' - '0'; i++ ) possible_characters.push_back( char('0' + i) );
        }
        int S = possible_characters.size();
        string s;
        while( val > 0 ){
            s += possible_characters[ val % S ];
            val /= S;
        }
        return s;
    }

    string convertIntegerToString(LL val, bool use_small_letters, bool use_capital_letters, bool use_digits) {
        assert( use_small_letters || use_capital_letters || use_digits );

        auto getNext = [&](int v){
            if( use_small_letters ){
                if( v <= 'z'-'a' ) return char('a' + v);
                v -= 'z'-'a'+1;

                if( use_capital_letters ){
                    if( v <= 'Z'-'A' ) return char('A' + v);
                    v -= 'Z' - 'A' + 1;

                    return char('0'+v);
                } else{
                    return char( '0' + v );
                }
            }else {
                if (use_capital_letters) {
                    if (v <= 'Z' - 'A') return char('A' + v);
                    v -= 'Z' - 'A' + 1;

                    return char('0' + v);
                }
                else{
                    return char('0' + v);
                }
            }
        };

        int S = 0;
        if( use_small_letters ) S += 'z'-'a'+1;
        if( use_capital_letters ) S += 'Z'-'A'+1;
        if( use_digits ) S += '9'-'0'+1;

        string s;
//        while( val > 0 ){
//            s += getNext(val%S);
//            val /= S;
//        }
        do{
            s += getNext(val%S);
            val /= S;
        }while( val > 0 );
        return s;
    }

}
