#include "tools.h"
#include "heap.h" // il faut aussi votre code pour heap.c

//Version 0: simple, 1: double


// Une fonction de type "heuristic" est une fonction h() qui renvoie
// une distance (double) entre une position de départ et une position
// de fin de la grille. La fonction pourrait aussi dépendre de la
// grille (comme par exemple du nombre de murs rencontrés par le
// segment départ-fin), mais on n'utilisera pas forcément ce dernier
// paramètre. Vous pouvez définir votre propre heuristique.
typedef double (*heuristic)(position,position,grid*);


// Heuristique "nulle" pour Dijkstra.
double h0(position s, position t, grid *G){
  return 0.0;
}


// Heuristique "vol d'oiseau" pour A*.
double hvo(position s, position t, grid *G){
  return fmax(abs(t.x-s.x),abs(t.y-s.y));
}


// Heuristique "alpha x vol d'oiseau" pour A*.
static double alpha=0; // 0 = h0, 1 = hvo, 2 = approximation ...
double halpha(position s, position t, grid *G) {
  return alpha*hvo(s,t,G);
}


// Structure "noeud" pour le tas min Q.
typedef struct node {
  int source;          //  1 a partir de s, 0 a partir de t
  position pos;        // position (.x,.y) d'un noeud u
  double cost;         // coût[u]
  double score;        // score[u] = coût[u] + h(u,end)
  struct node* parent; // parent[u] = pointeur vers le père, NULL pour start
} *node;

static int fcmp_node(const void *p, const void *q) {
  const double a = ((node)p)->score;
  const double b = ((node)q)->score;
  return (a<b)? -1 : (a>b); // ou encore return (a>b) - (a<b);
}

static node createNode(int src, position pos, double c, double s, node p){
  node n = malloc(sizeof(*n));
  n->source = src;
  n->pos = pos;
  n->cost = c;
  n->score = s;
  n->parent = p;

  return n;
}

void markPath(grid G, node x){
  if(x){
    G.mark[x->pos.x][x->pos.y] = M_PATH;
    markPath(G, x->parent);
  }
}

bool isDiagonal(position source, position target){
  if(target.x == source.x || target.y == source.y){
    return false;
  }
  return true;
}




// Les arêtes, connectant les 8 cases voisines de la grille, sont
// valuées seulement par certaines valeurs. Le poids de l'arête u->v,
// noté w(u,v) dans le cours, entre deux cases u et v voisines est
// déterminé par la valeur de la case finale v. Plus précisément, si
// la case v de la grille contient la valeur C, le poids de u->v
// vaudra w(u,v) = weight[C] dont les valeurs numériques exactes sont
// définies ci-après. La liste des valeurs possibles d'une case est
// donnée dans "tools.h": V_FREE, V_WALL, V_WATER, ... Remarquer que
// weight[V_WALL]<0 ce qui n'est pas a priori une valuation correcte.
// En effet A* ne marche qu'avec des poids positifs! Mais ce n'est pas
// un problème, puisqu'en position (x,y), si G.value[x][y] = V_WALL,
// alors c'est que le sommet à cette position n'existe pas! Et donc
// aucune arête ne peut donc être incidente à (x,y).

double weight[]={
    1.0,  // V_FREE
  -99.9,  // V_WALL
    3.0,  // V_SAND
    9.0,  // V_WATER
    2.3,  // V_MUD
    1.5,  // V_GRASS
    0.1,  // V_TUNNEL
};


// Que doit renvoyer la fonction A_star(G,h) ?
//---------------------------------------------
//
// Votre fonction A_star(G,h) doit construire un chemin dans la grille
// G, entre la position G.start et G.end, selon l'heuristique h() et
// renvoyer son coût. Le chemin doit être calculé selon l'algorithme
// A* vu en cours (utilisez les notes de cours !). L'heuristique h()
// est une fonction à choisir lors de l'appel parmi celles ci-dessus
// ou que vous pouvez vous-même définir.
//
// Si le chemin n'est pas trouvé (par exemple si la destination est
// enfermée entre 4 murs ou si G.end est sur un mur), il faut renvoyer
// une valeur < 0.
//
// Sinon, il faut renvoyer le coût du chemin trouvé et remplir le
// champs .mark de G pour que le chemin trouvé puisse être visualisé
// par drawGrid(G) (plutard dans le main). Il faut, par convention,
// avoir G.mark[x][y] = M_PATH ssi la case (x,y) appartient au chemin
// trouvé. Utilisez les touches a,z,+,-,p,c pour gérer la vitesse
// d'affichage et de progression de l'algorithme par exemple.
// Repportez-vous à "tools.h" pour avoir la liste des differentes
// touches et leurs actions, ainsi que les différentes valeurs
// possibles pour G.mark[][].
//
//
// Comment gérer les ensembles P et Q ?
//--------------------------------------
//
// Pour gérer l'ensemble P, servez-vous du champs G.mark[x][y] (=
// M_USED ssi (x,y) est dans P). Par défaut, ce champs est initialisé
// partout à M_NULL par toute fonction initGridXXX().
//
// Pour gérer l'ensemble Q, vous devez utiliser un tas min de noeuds
// (type node) avec une fonction de comparaison (à créer) qui dépend
// du champs .score des noeuds. Pour la fonction de comparaison faites
// attention au fait que l'expression "(int)(8.1 - 8.2)" n'est pas
// négative, mais nulle! Vous devez utilisez la gestion paresseuse du
// tas (cf. le paragraphe du cours à ce sujet, dans l'implémentation
// de Dijkstra). Pensez qu'avec cette gestion paresseuse, la taille de
// Q est au plus la somme des degrés des sommets dans la grille. Pour
// visualiser un noeud de coordonnées (x,y) qui passe dans le tas Q
// vous pourrez mettre G.mark[x][y] = M_FRONT au moment où vous
// l'ajoutez. Même si cela est tentant, il ne faut pas utiliser la
// marque M_FRONT pour savoir si un sommet est dans Q. Le champs .mark
// ne doit servir que pour l'ensemble P et l'affichage de la grille.
// Pas pour le tas Q !

double A_star(grid G, heuristic h){

  // Pensez à dessiner la grille avec drawGrid(G) à chaque fois que
  // possible, par exemple, lorsque vous ajoutez un sommet à P mais
  // aussi lorsque vous reconstruisez le chemin à la fin de la
  // fonction. Lorsqu'un sommet passe dans Q vous pourrez le marquer
  // M_FRONT (dans son champs .mark) pour le distinguer à l'affichage
  // des sommets de P (couleur différente).

  // Après avoir extrait un noeud de Q, il ne faut pas le détruire,
  // sous peine de ne plus pouvoir reconstruire le chemin trouvé !
  // Une fonction createNode() peut simplifier votre code.

  // Les bords de la grille sont toujours constitués de murs (V_WALL)
  // ce qui évite d'avoir à tester la validité des indices des
  // positions (sentinelle). Dit autrement, un chemin ne peut pas
  // s'échapper de la grille.
  heap Q = heap_create(fcmp_node);
  node start = createNode(1, G.start, 0, h(G.start, G.end, &G), NULL);
  node end = createNode(0, G.end, 0, h(G.end, G.start, &G), NULL);
  heap_add(Q, start);
  heap_add(Q, end);

  while(!heap_empty(Q)){
    node current = heap_pop(Q);

    if(G.mark[current->pos.x][current->pos.y] == M_USED && current->source == 0){
      markPath(G, current);

      node w;
      bool found = false;
      while((w=heap_pop(Q)) && !found){
          position p = w->parent->pos;
          if(p.x == current->pos.x && p.y == current->pos.y){
              found = true;
              markPath(G, w->parent);
          }
      }

      return current->cost + w->parent->cost; 
    } 
    if(G.mark[current->pos.x][current->pos.y] == M_USED2 && current->source == 1){
      markPath(G, current);
      node w;
      bool found = false;
      while((w=heap_pop(Q)) && !found){
          position p = w->parent->pos;
          if(p.x == current->pos.x && p.y == current->pos.y){
              found = true;
              markPath(G, w->parent);
          }
      }

      return current->cost + w->parent->cost; 
    } 

    if(G.mark[current->pos.x][current->pos.y] == M_USED ) continue;
    if(G.mark[current->pos.x][current->pos.y] == M_USED2 ) continue;

    if(current->source == 1){
        G.mark[current->pos.x][current->pos.y] = M_USED;
    } else if(current->source == 0){
        G.mark[current->pos.x][current->pos.y] = M_USED2;
    }
    
    for(int x=current->pos.x-1; x<= current->pos.x+1; x++){
      for(int y=current->pos.y-1; y<= current->pos.y+1; y++){
        if(G.value[x][y] == V_WALL) continue;
        if(G.mark[x][y] == M_USED && current->source == 1) continue;
        if(G.mark[x][y] == M_USED2 && current->source == 0) continue;
        double c = current->cost + weight[G.value[x][y]];
        position p = (position){ .x = x, .y = y };
        if(isDiagonal(current->pos, p)){
          c+=0.0000000001;
        }
        position final=(current->source==1)? G.end : G.start;
        node v = createNode(current->source,p, c, c + h(p, final, &G), current);
        if(G.mark[x][y] == M_NULL){
            G.mark[x][y] = M_FRONT;
        }
        heap_add(Q, v);
        drawGrid(G);
      }
    }

  }
  
  return -1;
}




// Améliorations à faire seulement quand vous aurez bien avancé:
//
// (1) Le chemin a tendance à zizaguer, c'est-à-dire à utiliser aussi
//     bien des arêtes horizontales que diagonales (qui peuvent avoir
//     le même coût), même pour des chemins en ligne droite. Essayez
//     de rectifier ce problème d'esthétique en modifiant le calcul de
//     score[v] de sorte qu'à coût[v] égale les arêtes (u,v)
//     horizontales ou verticales soient favorisées (un score plus
//     faible). Bien sûr, votre modification ne doit en rien changer
//     la distance (la somme des coût) entre .start et .end.
//
// (2) Modifier votre implémentation du tas dans heap.c de façon à
//     utiliser un tableau de taille variable, en utilisant realloc()
//     et une stratégie "doublante": lorsqu'il n'y a pas plus assez de
//     place dans le tableau, on double sa taille avec un realloc().
//     On peut imaginer que l'ancien paramètre 'nmax' devienne non pas
//     le nombre maximal d'éléments, mais sa taille maximale initiale
//     (comme par exemple nmax=4).
//
// (3) Gérer plus efficacement la mémoire en libérant les noeuds
//     devenus inutiles. Pour cela on ajoute un champs .nchild à la
//     structure node, permettant de gérer le nombre de fils qu'un
//     node de P ou Q possède. C'est relativement léger et facile à
//     gérer puisqu'on augmente .nchild de u chaque fois qu'on fait
//     parent[v]=p, soit juste après "node v = createNode(p,...)".
//     Pensez à faire .nchild=0 dans createNode(). Notez bien qu'ici
//     on parle de "node", donc de copie de sommet.
//
//     L'observation utile est que tous les nodes de Q sont des
//     feuilles. On va alors pouvoir se débarrasser de tous les nodes
//     ancêtres de ces feuilles simplement en extrayant les nodes de Q
//     dans n'importe quel ordre. (Si on veut être plus efficace que
//     |Q|*log|Q|, on peut vider le tableau .array[] directement sans
//     passer par heap_pop(). Pour être propre, il faudrait peut-être
//     ajouter une fonctions comme "void* heap_get(int i)" qui
//     permettrait d'extraire l'objet numéro i sans modifier le tas,
//     et renvoie NULL s'il est vide). On supprime alors chaque node,
//     en mettant à jour le nombre de fils de son père, puis en
//     supprimant le père s'il devient feuille (son .nchild passe 0)
//     et ainsi de suite. On élimine ainsi l'arbre par branches qui se
//     terminent toutes dans Q.
//
//     Ce processus peut ne pas fonctionner si P contient des nodes
//     qui sont des feuilles. L'observation est que de tels nodes ne
//     peuvent pas être sur le chemin s->t. On peut donc les supprimer
//     au fur et à mesure directement dans la boucle principale quand
//     on les détecte. On voit qu'un tel node apparaît si après avoir
//     parcouru tous les voisins de u aucun node v n'est créé (et
//     ajouté dans Q). Il suffit donc de savoir si on est passé par
//     heap_add() (ou encore de comparer la taille de Q avant et après
//     la boucle sur les voisins). Si u est une feuille, on peut alors
//     supprimer le node u, mettre à jour .nchild de son père et
//     remonter la branche jusqu'à trouver un node qui n'est plus une
//     feuille. C'est donc la même procédure d'élagage que précdemment
//     qu'on pourrait capturer par une fonction freeNode(node p).


int main(int argc, char *argv[]){

  unsigned seed=time(NULL)%1000;
  printf("seed: %u\n",seed); // pour rejouer la même grille au cas où
  srandom(seed);

  // testez différentes grilles ...

  grid G = initGridPoints(80,60,V_FREE,1); // petite grille vide, sans mur
  //grid G = initGridPoints(width,height,V_FREE,1); // grande grille vide, sans mur
  //grid G = initGridPoints(32,24,V_WALL,0.2); // petite grille avec quelques murs
  //grid G = initGridLaby(12,9,8); // petit labyrinthe aléatoire
  //grid G = initGridLaby(width/8,height/8,3); // grand labyrinthe aléatoire
  //grid G = initGridFile("mygrid.txt"); // grille à partir d'un fichier

  // ajoutez à G une (ou plus) "région" de texture donnée ...
  // (déconseillé pour initGridLaby() et initGridFile())

  //addRandomBlob(G, V_WALL,   (G.X+G.Y)/20);
  //addRandomBlob(G, V_SAND,   (G.X+G.Y)/15);
  //addRandomBlob(G, V_WATER,  (G.X+G.Y)/6);
  //addRandomBlob(G, V_MUD,    (G.X+G.Y)/3);
  //addRandomBlob(G, V_GRASS,  (G.X+G.Y)/15);
  //addRandomBlob(G, V_TUNNEL, (G.X+G.Y)/4);
  //addRandomArc(G, V_WALL,    (G.X+G.Y)/25);

  // sélectionnez des positions s->t ...
  // (inutile pour initGridLaby() et initGridFile())

  G.start=(position){0.1*G.X,0.2*G.Y}, G.end=(position){0.8*G.X,0.9*G.Y};
  //G.start=randomPosition(G,V_FREE); G.end=randomPosition(G,V_FREE);

  // constantes à initialiser avant init_SDL_OpenGL()
  scale = fmin((double)width/G.X,(double)height/G.Y); // zoom courant
  hover = false; // interdire les déplacements de points
  init_SDL_OpenGL(); // à mettre avant le 1er "draw"
  drawGrid(G); // dessin de la grille avant l'algo
  update = false; // accélère les dessins répétitifs

  alpha=0;
  double d = A_star(G,hvo); // heuristique: h0, hvo, alpha*hvo

  // chemin trouvé ou pas ?
  if (d < 0) printf("path not found!\n");
  else printf("bingo!!! cost of the path: %g\n", d);

  // compte le nombre de sommets explorés pour comparer les
  // heuristiques
  int m = 0;
  for (int i=0; i<G.X; i++)
    for (int j=0; j<G.Y; j++)
      m += (G.mark[i][j] != M_NULL);
  printf("#nodes explored: %i\n", m);

  while (running) {    // affiche le résultat et attend
    update = true;     // force l'affichage de chaque dessin
    drawGrid(G);       // dessine la grille (/!\ passe update à false)
    handleEvent(true); // attend un évènement
  }

  freeGrid(G);
  cleaning_SDL_OpenGL();
  return 0;
}
