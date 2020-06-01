//
// Created by sylwester on 8/8/19.
//

#include <graphs/generators/GraphGenerator.h>
#include <combinatorics/CombinatoricUtils.h>
#include <unordered_set>
#include <Constants.h>
#include <utils/RandomNumberGenerators.h>

#include "graphs/generators/GraphGenerator.h"

VVI GraphGenerator::getRandomGraph(int N, int M, bool directed) {
    VVI V(N);
    if( (LL)N*(N-1)/2 > 5*M ){ // in case of large graphs, i do by rand rather than storing the whole structure
        unordered_set<LL> edges;

//        random_device rd;
//        std::mt19937_64 rng;
//        rng.seed(171);
//        std::uniform_int_distribution<unsigned long long> unif;

        UniformIntGenerator gen( 0, (LL)N*(LL)N-1 );

        while( edges.size() < M ){
            long long e = gen.rand();
            long long a = e % N;
            long long b = e / N;
            if(a == b) continue;
            if( !directed && a > b ) swap(a,b);

//            assert( a < N && b < N );

            edges.insert( b*N+a );
//            edges.insert( e );
//            cerr << "Adding edge " << WRP( MP(a,b) ) << endl;
        }

        for( LL p : edges ){
            int a = p % N;
            int b = p / N;
            V[a].push_back(b);
            if( !directed ) V[b].push_back(a);
        }

//        DEBUG(edges);

    }else{
        VPII edges;
        for(int i=0; i<N; i++){
            for(int k=i+1; k<N; k++){
                edges.push_back({i,k});
                if(directed) edges.push_back({k,i});
            }
        }

        random_shuffle(ALL(edges));
        for(int i=0; i<min( M, (int)edges.size() ); i++){
            int a = edges[i].first;
            int b = edges[i].second;
            V[a].push_back(b);
            if( !directed ) V[b].push_back(a);
        }
    }

    return V;
}

VVI GraphGenerator::getRandomGraph(int N, double p, bool directed) {
    VVI V(N);

    std::mt19937_64 rng;
    rng.seed(171);
    std::uniform_real_distribution<double> unif(0, 1);

    for( int i=0; i<N; i++ ){
        for(int k= ( directed ? 0 : i+1 ) ; k<N; k++){
            double currentRandomNumber = unif(rng);
            if( currentRandomNumber < p ){
                V[i].push_back(k);
                if(!directed) V[k].push_back(i);
            }
        }
    }

    return V;
}


VVI GraphGenerator::getRandomBipartiteGraph(int L, int R, int M ) {
    int N = L+R;
    VVI V(N);
    if( N*(N-1)/2 > 5*M ){ // in case of large graphs, i do by rand rather than storing the whole structure


        set<PII> edges;



        UniformIntGenerator g( 0, N*N-1 );

        while( edges.size() < M ){
            int a = g.rand() % L;
            int b = L + g.rand() % R;

            edges.insert( {a,b} );
        }

        for( PII p : edges ){
            int a = p.first;
            int b = p.second;
            V[a].push_back(b);
            V[b].push_back(a);
        }

    }else{
        VPII edges;
        for(int i=0; i<L; i++){
            for(int k=L; k<N; k++){
                edges.push_back({i,k});
            }
        }

        random_shuffle(ALL(edges));
        for(int i=0; i< min(M,(int)edges.size()) ; i++){
            int a = edges[i].first;
            int b = edges[i].second;
            V[a].push_back(b);
            V[b].push_back(a);
        }
    }

    return V;
}


VVI GraphGenerator::getRandomBipartiteGraph(int L, int R, double p ) {
    int N = L+R;

    VVI V(N);

    std::mt19937_64 rng;
    rng.seed(171);
    std::uniform_real_distribution<double> unif(0, 1);

    for( int i=0; i<L; i++ ){
        for(int k= L; k<N; k++){
            double currentRandomNumber = unif(rng);
            if( currentRandomNumber < p ){
                V[i].push_back(k);
                V[k].push_back(i);
            }
        }
    }

    return V;
}

VVI GraphGenerator::getRandomGrid(int rows, int columns) {
    VVI V( rows*columns );

    for( int i=0; i<columns; i++ ){
        if( i < columns-1 ){

            for( int j=0; j<rows; j++ ){
                int a = j*columns+i;
                int b = a+1;

                V[a].push_back(b);
                V[b].push_back(a);
            }

        }
    }


    for( int i=0; i<rows; i++ ){
        if( i < rows-1 ){

            for( int j=0; j<columns; j++ ){
                int a = i*columns+j;
                int b = a+columns;

                V[a].push_back(b);
                V[b].push_back(a);
            }

        }
    }

    return V;
}

VVI GraphGenerator::getRandomTreePrufer(int N) {
    if( N <= 1 ) return VVI(1);
    if( N == 2 ){ VVI V(2); V[0].PB(1); V[1].PB(0); return V;	}

    int INF = Constants::INF;

    VI last(N,-1); // ostatnie wystapienie liczby i w kodzie prufera jest na miejscu last[i]
    VI seq = CombinatoricUtils::getRandomSequence( N-1, N-2 ); // generuje kod prufera
    FORD( i,SIZE(seq)-1,0 ) if( last[ seq[i] ] == -1 ) last[ seq[i] ] = i;
    int p = 0,t = INF;
    VVI V(N);
    REP(i,SIZE(seq)){
        if( t == INF ) while( ( t = p ) < SIZE(last) && last[p++] != -1 );

        V[t].PB( seq[i] );
        V[seq[i]].PB(t);
        last[t] = INF;

        t = INF;
        if( i == last[ seq[i] ] ){
            last[ seq[i] ] = -1;
            if( seq[i] < p ){
                last[seq[i]] = INF;
                t = seq[i];
            }
        }
    }
    REP( i,SIZE(last) ) if( last[i] == -1 && i != seq[SIZE(seq)-1] ){ p = i; break;	}
    V[p].PB( seq[ SIZE(seq)-1 ] );
    V[ seq[ SIZE(seq)-1 ] ].PB(p);
    return V;
}

VVI GraphGenerator::getPath(int N, bool randomOrder) {
    VI perm = CombinatoricUtils::getRandomPermutation(N);
    if( randomOrder == false ) iota(ALL(perm),0);
    VVI V(N);
    for( int i=1;  i<N; i++ ){
        int a = perm[i-1];
        int b = perm[i];
        V[a].push_back(b);
        V[b].push_back(a);
    }
    return V;
}

