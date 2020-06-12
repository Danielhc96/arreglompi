#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

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

void insertar(int listaOrdenada[],int N, int numelem,int numero){
     while(numero != 0 && numelem < N){
        int i = 0;
        //ubica la posición para insertar el número
        while(i<numelem && listaOrdenada[i] < numero) i++;
        //en el caso que se inserte en la última posición: i == numelem
        if(i==numelem) {
           listaOrdenada[i]=numero;
           numelem++;
        }
        else{
           //tiene que desplazar a todos los elementos posteriores
           int j = numelem;
           numelem++;
           //recorre la lista de forma inversa para desplazar los valores
           while(j>i){
             listaOrdenada[j] = listaOrdenada[j-1];
             j--;
           }
           listaOrdenada[i] = numero;
        }
    }
}
int main (int argc, char *argv[]){
    /*********
    *
    crear areglo y llenar con numeros aleatoreos
    *
    *********/
    int i, j;
    int n = 10;
    /*printf("Introduzca tamaño del arreglo: ");
    scanf("%d",&n);*/
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
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    /*********
    *
    Dividir en partes iguales y obtener el resto
    *
    *********/
    int p = n/npr;          /*parte a repartir*/
    int r = n%npr;          /*numeros sobrantes (resto)*/
    /*int nm = p+r;           /*numeros que trabajara el maestro*/
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
           /* for(j=0; j<p; j++){     /*llena el arreglo en funcion de las maquinas*/
                /*arreglo3[j]=arreglo1[j+p*(i-1)];
                }*/
            /*MPI_Send(&arreglo3[0],p,MPI_INT,i,100,MPI_COMM_WORLD); /*envio de arreglo*/
            MPI_Send(&nm, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
            MPI_Send(&arreglo[nm], p, MPI_INT, dest, tag2, MPI_COMM_WORLD);
            printf("\n\nArreglo enviado");
            nm = nm + p;
        }
        /*********
        *
        Trabajo parte maestro
        *
        *********/
        printf("\n\nArreglo maestro");
        /*for (j=0; j<nm; j++){       llena la ultima parte
            arreglo2[j]=arreglo[j+p*(npr-1)];
        }*/
        
        SortArray(arreglo,0,nm);   /*ordena*/
        printf("\n\nArreglo maestro ordenado");
        for (j=0;j<nm;j++){              /*bandera*/
            printf("\nNumero %d = %d", j+1, arreglo[j]);
        }
        /*********
        *
        crea arreglo y llena con datos ordenados del maestro
        *
        *********/
        int arreglo2[n]; /*arreglo donde se agregaran datos ordenados*/
        for (i=0; i<n; i++){  /*inicializa en 0*/
            arreglo2[i] = 0;
        }
        for (i=0; i<nm; i++){  /*agrega datos ordenados de maestro*/
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
            /*insertar un arreglo en otro*/
            for (j=nm; j< nm+p; j++){
                insertar(arreglo2,n,nm,arreglo[j]);
            }

        }
        /*********
        *
        muestra arreglo ordenado
         *
        *********/
        printf("\n\nArreglo ordenado final");
        for (i=0;i<n;i++){
        printf("\nNumero %d = %d", i+1, arreglo[i]);
        }
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
        printf("\n\nArreglo nodo");
        for (j=nm ; j<nm+p; j++){              /*bandera*/
            printf("\nNumero %d = %d", j+1, arreglo[j]);
        }
      
        SortArray(arreglo,nm,nm+p);   /*ordena*/
      
        printf("\n\nArreglo ordenado");
        for (j=nm; j<nm+p; j++){              /*bandera*/
            printf("\nNumero %d = %d", j+1, arreglo[j]);
        }
      
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
    return 0;
}
