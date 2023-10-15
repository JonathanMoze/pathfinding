#include "tools.h"
#include <math.h>

//
//  TSP - BRUTE-FORCE
//
// -> la structure "point" est définie dans "tools.h"
// -> tsp_main peut être testé dès les 3 premières fonctions codées
//

double dist(point A, point B) {
  return sqrt(pow(B.x - A.x, 2) + pow(B.y - A.y, 2));
}

double value(point *V, int n, int *P) {
  double val = 0.0;
  for(int i=0; i< n; i++){
    if(i==n-1){
      val += dist(V[P[i]], V[P[0]]);
    }else{
      val += dist(V[P[i]], V[P[i+1]]);
    }
  }
  return val;
}

double tsp_brute_force(point *V, int n, int *Q) {
  double longueur = DBL_MAX;
  int perm [n];
  for(int i=0; i<n ; i++){
    perm[i] = i;
  }

  do{
    double l = value(V, n, perm);
    if(l<longueur){
      longueur = l;
      for(int i=0; i<n;i++){
        Q[i] = perm[i];
      }
    }
    //drawTour(V,n,perm);
    //SDL_Delay(100);
  }while(NextPermutation(perm,n) /*&& running*/);
  return longueur;
}

void MaxPermutation(int *P, int n, int k) {
  int perm[n];
  for(int i=0; i<n ; i++){
    perm[i] = P[i];
  }

  for(int i=0; i<n-k;i++){
    P[k+i] = perm[n-i-1];
  }
}

double value_opt(point *V, int n, int *P, double w) {
  double val = 0.0;
  for(int i=0; i< n-1; i++){
    val += dist(V[P[i]], V[P[i+1]]);
    if(val + dist(V[P[i+1]], V[P[0]]) > w){
      return -(i+2);
    }
  }
  val += dist(V[P[n-1]], V[P[0]]);
  return val;
}

double tsp_brute_force_opt(point *V, int n, int *Q) {
  double longueur = DBL_MAX;
  int perm [n];
  for(int i=0; i<n ; i++){
    perm[i] = i;
  }

  do{
    double l = value_opt(V, n, perm, longueur);
    if(l <0){
      MaxPermutation(perm, n, -l);
    }else{
        longueur = l;
        for(int i=0; i<n;i++){
          Q[i] = perm[i];
      }
    }
    //drawTour(V,n,perm);
    //SDL_Delay(100);
  }while(NextPermutation(perm,n) /*&& running*/);
  return longueur;
}
