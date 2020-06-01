//
// Created by sylwester on 3/20/20.
//

#include "graphs/components/BridgesAndArtPoints.h"


namespace BridgesAndArtPoints{

    pair< VI, VPII > getBridgesAndArtPoints(VVI& V){
//        VVI V; // wektor reprezentujacy graf
        VPII bridges; // wektor przechowujacy mosty(pary wierzcholkow)
        VI arts; // wektor przechowujacy numery wierzcholkow ktore sa punktami artykulacji
        VI preOrder; // numery preOrder wierzcholkow
        VI dfsSons; // ilosc synow podczas przeszukiwania grafu dfsem
        VI low; // wektor wyznaczajacy funkcje low

        // funkcja zwraca wszystkie mosty i punkty artykulacji w grafie V
        function<void(int&,int,int)> getAll = [&preOrder,&V,&bridges,&arts,&dfsSons,&low,&getAll]( int & dfsn, int num, int par ){
            preOrder[ num ] = dfsn++;
            low[ num ] = preOrder[ num ];
            dfsSons[ num ] = 0;

            bool isArt = false;
            REP(i,SIZE(V[num])){
                    if( preOrder[ V[num][i] ] == 0 ){
                            getAll( dfsn, V[num][i], num );
                            low[num] = min( low[num], low[ V[num][i] ] );
                            if( low[ V[num][i] ] >= preOrder[num] && par != -1  ) isArt = true;
                            if( low[ V[num][i] ] == preOrder[ V[num][i] ] ) bridges.PB( MP( min(num,V[num][i]), max(num,V[num][i]) ) );
                            dfsSons[num]++;
                    }
                    else if( V[num][i] != par ) low[ num ] = min( low[num], preOrder[ V[num][i] ] );
            }

            if( isArt ) arts.PB(num);
        };

        bridges.clear();
        arts.clear();
        preOrder = VI(SIZE(V),0);
        dfsSons = VI(SIZE(V),0);
        low = VI(SIZE(V),0);
        int dfsn = 1;
        REP( i,SIZE(V) ) if( !preOrder[i] ){
                        dfsn = dfsn; // dfsn = 1;   // uwaga, tutaj nie ma znaczenia co wpisze, to i tak sluzy tylko do powornywania kolejnosci wejsc do wierzcholkow
                        getAll( dfsn,i,-1 ); // dla kazdej skladowej spojnosci wywoluje ta funkcje
                        if( dfsSons[i] >= 2 ) arts.PB(i); // jesli korzen w drzewie przeszukiwania dfs ma conajmniej 2 synow, to jest artem
                }
        sort( ALL(arts) ); // sortuje wszystkie punkty artykulacji
        sort( ALL(bridges) ); // sortuje wszystkie mosty

        return { arts,bridges };

    }




}