#include "heap.h"
#include <stdlib.h>
#include <stdio.h>

heap heap_create(int (*f)(const void *, const void *))
{
  heap h = malloc(sizeof(*h));
  h->array = malloc((4 + 1) * sizeof(void *));
  h->n = 0;
  h->nmax = 4;
  h->f = *f;

  return h;
}

void heap_destroy(heap h)
{
  free(h->array);
  free(h);
  h = NULL;
}

bool heap_empty(heap h)
{
  if (h->n > 0)
  {
    return false;
  }
  return true;
}

bool heap_add(heap h, void *object)
{
  if (h->n + 1 > h->nmax)
  {
    h->nmax *= 2;
    h->array = (void*) realloc(h->array, sizeof(void*) * (1+h->nmax));
  }
  h->array[h->n + 1] = object;
  if (!heap_empty(h))
  {
    int indice = h->n + 1;
    int indicePere = (indice / 2 == 0) ? 1 : indice / 2;
    while (h->f(h->array[indice], h->array[indicePere]) < 0)
    {
      void *tmp = h->array[indice];
      h->array[indice] = h->array[indicePere];
      h->array[indicePere] = tmp;
      indice = indicePere;
      indicePere = (indice / 2 == 0) ? 1 : indice / 2;
    }
  }

  h->n++;
  return false;
}

void *heap_top(heap h)
{
  if (h->n > 0)
  {
    return h->array[1];
  }
  return NULL;
}

void *heap_pop(heap h)
{
  void *poped = heap_top(h);

  if (poped)
  {
    h->array[1] = h->array[h->n--];
    int i = 1;
    while (true)
    {
      int j = 2 * i;
      if (j > h->n)
        break;
      // ici i a un fils gauche

      if (j + 1 <= h->n && h->f(h->array[j], h->array[j + 1]) > 0)
        j++;
      // ici j est le plus petit des fils

      if (h->f(h->array[i], h->array[j]) < 0)
        break;
      // maintenant il faut échanger i et j

      void *tmp = h->array[i];
      h->array[i] = h->array[j];
      h->array[j] = tmp;
      i = j;
    }
  }
  return poped;

  /*
    int indice = 1;
    int indiceGauche = 2 * indice;
    int indiceDroit = 2 * indice + 1;
    
    while(true)
    {
      if (h->array[indiceGauche] != NULL && h->array[indiceDroit] != NULL)
      {
        if (h->f(h->array[indiceGauche], h->array[indiceDroit]) <= 0)
        {
          if (h->f(h->array[indice], h->array[indiceGauche]) > 0)
          {
            void *tmp = h->array[indice];
            h->array[indice] = h->array[indiceGauche];
            h->array[indiceGauche] = tmp;
            indice = indiceGauche;
            indiceGauche = indice * 2;
            indiceDroit = indice * 2 + 1;
            if(indiceGauche > h->n || indiceDroit > h->n){
              break;
            }
          }
          else
          {
            break;
          }
        }
        else
        {
          if (h->f(h->array[indice], h->array[indiceDroit]) > 0)
          {
            void *tmp = h->array[indice];
            h->array[indice] = h->array[indiceDroit];
            h->array[indiceDroit] = tmp;
            indice = indiceDroit;
            indiceGauche = indice * 2;
            indiceDroit = indice * 2 + 1;
            if(indiceGauche > h->n || indiceDroit > h->n){
              break;
            }
          }
          else
          {
            break;
          }
        }
      }
      else if (h->array[indiceDroit] == NULL)
      {
        if (h->f(h->array[indice], h->array[indiceGauche]) > 0)
        {
          void *tmp = h->array[indice];
          h->array[indice] = h->array[indiceGauche];
          h->array[indiceGauche] = tmp;
          indice = indiceGauche;
          indiceGauche = indice * 2;
          indiceDroit = indice * 2 + 1;
          if(indiceGauche > h->n || indiceDroit > h->n){
              break;
            }
        }
        else
        {
          break;
        }
      }
      else
      {
        break;
      }
    }
    h->n--;
    return poped;
  }
  return NULL;*/
}
