#include "tools.h"
#include "tsp_brute_force.h"

//
//  TSP - HEURISTIQUES
//

void reverse(int *T, int p, int q) {
  // Renverse la partie T[p]...T[q] du tableau T avec p<q si
  // T={0,1,2,3,4,5,6} et p=2 et q=5, alors le nouveau tableau T sera
  // {0,1, 5,4,3,2, 6}.
  while(p<q){
    int tmpP, tmpQ;
    tmpP = T[p];
    tmpQ = T[q];
    T[p] = tmpQ;
    T[q] = tmpP;

    p++; q--;
  }
}

double first_flip(point *V, int n, int *P) {
  // Renvoie le gain>0 du premier flip réalisable, tout en réalisant
  // le flip, et 0 s'il n'y en a pas.
  int gain = 0;
  for(int i=0; i<= n-3; i++){
    int M = (i==0)? n-2 : n-1;
    for(int j=i+2; j<=M; j++){
      gain = dist(V[P[i]], V[P[i+1]]) + dist(V[P[j]], V[P[(j+1)%n]]);
      gain -= (dist(V[P[i]], V[P[j]]) + dist(V[P[i+1]], V[P[(j+1)%n]]));
      if(gain > 0){
        reverse(P, i+1, j);
        return gain;
      }  
    }
  }
  return 0;
}

double tsp_flip(point *V, int n, int *P) {
  // La fonction doit renvoyer la valeur de la tournée obtenue. Pensez
  // à initialiser P, par exemple à P[i]=i. Pensez aussi faire
  // drawTour() pour visualiser chaque flip.
  for(int i=0; i<n; i++){
    P[i] = i;
  }
  while(first_flip(V, n, P)>0 && running) drawTour(V, n, P);
  return value(V, n, P);
}

double tsp_greedy(point *V, int n, int *P) {
  // La fonction doit renvoyer la valeur de la tournée obtenue. Pensez
  // à initialiser P, par exemple à P[i]=i.
  for(int i=0; i<n; i++){
    P[i] = i;
  }
  for(int i=1; i<n; i++){
    double minDist = DBL_MAX;
    int minP = n;
    for(int j=i; j<n; j++){
      double tmpDist = dist(V[P[i-1]], V[P[j]]);
      if(tmpDist < minDist){
        minDist = tmpDist;
        minP = j;
      }
    }
    reverse(P, i, minP);
    SDL_Delay(100);
    drawPath(V, n, P, i);
    
  }

  return value(V, n, P);
}
