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
int main (int argc, char *argv[]){
    /*********
    *
    crear areglo y llenar con numeros aleatoreos
    *
    *********/
    int i, n;
    printf("Introduzca tamaño del arreglo: ");
    scanf("%d",&n);
    int arreglo1 [n];
    srand(time(NULL));
    for (i=0; i<n; i++){
        arreglo1[i] = aleatorio(0, 10000);
    }
    /*********
    *
    Inicio MPI
    npr = numero de procesos
    pid = indentificador de proceso
    *
    *********/
    int npr, pid;
    MPI_Status estado;
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
    int nm = p+r;           /*numeros que trabajara el maestro*/
    int arreglo2[nm];       /*arreglo para maestro*/
    int arreglo3[p];        /*arreglo para esclavos*/
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
    int j;
    if (pid == 0){
        /*********
        *
        Envio de tareas a procesos
        *
        *********/
        for(i=1; i<npr; i++){
            for(j=0; j<p; j++){     /*llena el arreglo en funcion de las maquinas*/
                arreglo3[j]=arreglo1[j+p*(i-1)];
                }
            MPI_Send(arreglo3,p,MPI_INT,i,2,MPI_COMM_WORLD); /*envio de arreglo*/
            MPI_Send(p,1,MPI_INT,i,3,MPI_COMM_WORLD); /*envio de tamaño*/
            printf("\n\nArreglo enviado");
            for (j=0;j<p;j++){              /*bandera*/
                printf("\nNumero %d = %d", j+1, arreglo3[j]);
            }
        }
        /*********
        *
        Trabajo parte maestro
        *
        *********/
        printf("\n\nArreglo maestro");
        for (j=0; j<nm; j++){       /*llena la ultima parte*/
            arreglo2[j]=arreglo1[j+p*(npr-1)];
        }
        for (j=0;j<nm;j++){              /*bandera*/
            printf("\nNumero %d = %d", j+1, arreglo2[j]);
        }
        SortArray(arreglo2,0,nm);   /*ordena*/
        printf("\n\nArreglo maestro ordenado");
        for (j=0;j<nm;j++){              /*bandera*/
            printf("\nNumero %d = %d", j+1, arreglo2[j]);
        }
        /*********
        *
        crea arreglo y llena con datos ordenados del maestro
        *
        *********/
        int arreglo4[n]; /*arreglo donde se agregaran datos ordenados*/
        for (i=0; i<n; i++){  /*inicializa en 0*/
            arreglo4[i] = 0;
        }
        for (i=0; i<nm; i++){ /*agrega datos ordenados de maestro*/
            arreglo4[i] = arreglo2[i];
        }
        /*********
        *
        Recibe los arreglos desde esclavos e inserta en arreglo
        *
        *********/
        for(i=1; i<npr; i++){
            MPI_Recv(arreglo3,p,MPI_INT,i,2,MPI_COMM_WORLD,&estado); /*recibe arreglo*/
            printf("\n\nArreglo recibido");
            for (j=0;j<p;j++){              /*bandera*/
                printf("\nNumero %d = %d", j+1, arreglo3[j]);
            }
            /*insertar un arreglo en otro*/
            for (j=0; j<p; j++){
                insertar(arreglo4,n,nm,arreglo3[j]);
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
    else if(pid > 0){
        MPI_Recv(arreglo3,p,MPI_INT,0,2,MPI_COMM_WORLD,&estado); /*recibe arreglo*/
        MPI_Recv(p,1,MPI_INT,0,3,MPI_COMM_WORLD,&estado); /*recibe tamaño*/
        printf("\n\nArreglo recibido");
        for (j=0;j<p;j++){              /*bandera*/
            printf("\nNumero %d = %d", j+1, arreglo3[j]);
        }
        SortArray(arreglo3,0,p);   /*ordena*/
        printf("\n\nArreglo ordenado");
        for (j=0;j<p;j++){              /*bandera*/
            printf("\nNumero %d = %d", j+1, arreglo3[j]);
        }
        MPI_Send(arreglo3,p,MPI_INT,0,2,MPI_COMM_WORLD);
    }
    /*********
    *
    termina
    *
    *********/
    MPI_Finalize();
    return 0;
}
