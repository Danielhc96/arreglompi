#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Devuelve un número aleatorio en un rango
int aleatorio(int minimo, int maximo) {
  return minimo + rand() / (RAND_MAX / (maximo - minimo + 1) + 1);
}

void SortArray (int array[],int first,int last){
    int i,j,p,t;
    // i se hace igual al índice del primer elemento
    i= first;
    // y j igual al índice del último elemento
    j= last;
    // p se hace igual al elemento pivote del arreglo
    p= array[(first+last)/2];
    do {
        // se hace la partición del arreglo
        while (array[i]<p) i++;
        while (p<array[j]) j--;
        if (i<=j) {
                // se intercambian los elementos i-esimo y j-esimo del arreglo
                t= array[i];
                array[i]= array[j];
                array[j]= t;
                i++; j--;
            }
    } while (i<=j);
    if (first<j) SortArray(array,first,j);
    if (i<last) SortArray(array,i,last);
}
void merge(int a[], int m, int b[], int n, int sorted[]) {
  int i, j, k;

  j = k = 0;

  for (i = 0; i < m + n;) {
    if (j < m && k < n) {
      if (a[j] < b[k]) {
        sorted[i] = a[j];
        j++;
      }
      else {
        sorted[i] = b[k];
        k++;
      }
      i++;
    }
    else if (j == m) {
      for (; i < m + n;) {
        sorted[i] = b[k];
        k++;
        i++;
      }
    }
    else {
      for (; i < m + n;) {
        sorted[i] = a[j];
        j++;
        i++;
      }
    }
  }
}

int main (int argc, char *argv[]){
  
    /*********
    *
    crear areglo y llenar con numeros aleatoreos
    *
    *********/
  
    int i, j, n;
    n = atoi(argv[1]);
    int arreglo[n];
    srand(time(NULL));
    for (i=0; i<n; i++){
        arreglo[i] = aleatorio(0, 10000);
    }
  
    /*********
    *
    Inicio MPI
    npr = numero de procesos
    pid = indentificador de proceso
    *
    *********/
  
    int npr, pid;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &npr);
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
  
    /*********
    *
    Dividir en partes iguales y obtener el resto
    *
    *********/
  
    int p = n/npr;          /*parte a repartir*/
    int r = n%npr;          /*numeros sobrantes (resto)*/
    int nm, dest, source;  /*parte de maestro, destino, fuente, contador*/
    int tag1 = 2;
    int tag2 = 1;
  
    /*********
    *
    Parte del Maestro
    - llenar arreglo a enviar
    - Envio de arreglos a procesos
    - trabajar parte
    - recibir arreglos
    - juntar arreglos
    *
    *********/
  
    if (pid == 0){
        /*********
        *
        Envio de tareas a procesos
        *
        *********/
        
        nm = p+r;  
        for(dest=1; dest<npr; dest++){
              MPI_Send(&nm, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
              MPI_Send(&arreglo[nm], p, MPI_INT, dest, tag2, MPI_COMM_WORLD);
              nm = nm + p;
        }
      
        /*********
        *
        Trabajo parte maestro
        *
        *********/
        
        SortArray(arreglo,0,p+r);   /*ordena*/

        /*********
        *
        crea arreglo y llena con datos ordenados del maestro
        *
        *********/
      
        int arreglo2[n]; /*arreglo donde se agregaran datos ordenados*/
        
        for (i=0; i<(p+r); i++){  /*agrega datos ordenados de maestro*/
            arreglo2[i] = arreglo[i];
        }
        
        /*********
        *
        Recibe los arreglos desde esclavos e inserta en arreglo
        *
        *********/
      
        for (i=1; i<npr; i++){
            source = i;
            MPI_Recv(&nm, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
            MPI_Recv(&arreglo[nm], p, MPI_INT, source, tag2, MPI_COMM_WORLD, &status); 
        }
      
         /*********
        *
        une y muestra arreglo ordenado
         *
        *********/
        
        int arreglo3[p];
        int arreglo4[n];

        nm = p+r;
        for (i=1; i<npr; i++){
            for(j=0; j<p; j++){
                arreglo3[j] = arreglo[j+nm];
            }           
            merge(arreglo2, nm, arreglo3, p, arreglo4);
            for(j=0; j<nm+p; j++){
              arreglo2[j]=arreglo4[j];
            }
            nm = nm + p;
        }
        printf("\nArreglo ordenado\n");
        for (i=0; i<n; i++){
            printf("%d ", arreglo4[i]);
        }
        printf("\n");
    }
  
    /*********
    *
    Parte del Esclavo
    - recibe arreglo
    - trabaja arreglo
    - envia arreglo
    *
    *********/
  
    if (pid > 0){
        source = 0;
        MPI_Recv(&nm, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
        MPI_Recv(&arreglo[nm], p, MPI_INT, source, tag2, MPI_COMM_WORLD, &status);
        
        SortArray(arreglo,nm,nm+p);   /*ordena*/

        dest = 0;
        MPI_Send(&nm, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
        MPI_Send(&arreglo[nm], p, MPI_INT, dest, tag2, MPI_COMM_WORLD);
    }
    /*********
    *
    termina
    *
    *********/
  
    MPI_Finalize();
}
