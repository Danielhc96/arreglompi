#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int  main ( int argc, char * argv [])
{
  int i;
  int arreglo[10];
  for (i=0; i<10; i++){
    arreglo[i] = i;
  }
  int pid,npr;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &npr);
  MPI_Comm_rank(MPI_COMM_WORLD,&pid);
  
  int p = 10/npr;          /*parte a repartir*/
  int r = 10%npr;          /*numeros sobrantes (resto)*/
  int nm;
  int tag1 = 2;
  int tag2 = 1;
  int j;
  int dest, source;
  
  if(pid == 0){
     nm = p+r;           /*numeros que trabajara el maestro*/
     for(dest=1; dest<npr; dest++) {
     MPI_Send(&nm, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
     MPI_Send(&arreglo[nm], p, MPI_INT, dest, tag2, MPI_COMM_WORLD);
     printf("\n\nArreglo enviado");
     nm = nm + p;
     }
     
     printf("\n\nArreglo maestro");
     for (j=0;j<p;j++){
      printf("\nNumero %d = %d", j+1, arreglo[j]);
     }
     for (i=1; i<npr; i++) {
      source = i;
      MPI_Recv(&nm, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
      MPI_Recv(&arreglo[nm], p, MPI_INT, source, tag2, MPI_COMM_WORLD, &status);
    }
    
    if (pid > 0){
      source = 0;
      MPI_Recv(&nm, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
      MPI_Recv(&arreglo[nm], p, MPI_INT, source, tag2, MPI_COMM_WORLD, &status);
      
      printf("\n\nArreglo nodo");
      for (j=nm;j<nm+p;j++){
        printf("\nNumero %d = %d", j+1, arreglo[j]);
      }

      dest = 0;
      MPI_Send(&nm, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
      MPI_Send(&arreglo[nm], p, MPI_INT, dest, tag2, MPI_COMM_WORLD);
    }
  MPI_Finalize();
} 
