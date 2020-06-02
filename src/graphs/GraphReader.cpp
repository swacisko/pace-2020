/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include "graphs/GraphReader.h"


namespace GraphReader{


    VVI readGraphStandardEdges(istream &cin, bool directed ) {
        int N,M;
        cin >> N >> M;
        VVI V(N);
        for(int i=0; i<M; i++){
            int a,b;
            cin >> a >> b;
            V[a-1].push_back(b-1);
            if( !directed ) V[b-1].push_back(a-1);
        }

        return V;
    }


    VVI readGraphDIMACSWunweighed(istream &cin, bool edgeFoolowE ) {
        string s;
        VVI V;

        int lineNumber = 1;

        int N,M;
        int edges_read = 0;


        while( true ){
            getline(cin,s);


//            if( s[0] == 'p' ){
//            if( s.Find("edge") != string::npos ){

            if( s[0] == 'c' ){
                // nothing to do here, this is a comment
            }else if( s[0] == 'p' && lineNumber++ == 1 ){
//                s = s.substr( 4,s.size()-4 );
                stringstream str(s);
                string nothingBox;
                str >> nothingBox >> nothingBox >> N >> M;

                V = VVI(N);

            }else{
                stringstream str(s);
                int a,b;
                char e;

                if(edgeFoolowE) str >> e >> a >> b;
                else str >> a >> b;

                edges_read++;



                // CAUTION - THIS SHOULD BE HERE, COMMENTED ONLY FOR TESTING
                a--;
                b--;





                V[a].push_back(b);
                V[b].push_back(a);
            }
            if( edges_read == M ) break;
        }

        for( int i=0; i<N; i++ ){
            sort(ALL(V[i]));
            V[i].resize( unique(ALL(V[i])) - V[i].begin() );
        }

        return V;
    }


}