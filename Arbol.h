#pragma once
#include <stack>
#include <queue>
#include <vector>
#include <windows.h>
#include <iomanip>
#include <iostream>
using namespace std;
class Arbol
{
public:
    class Nodo {
    public:
        Nodo* izq;
        Nodo* der;
        int idProceso;  //Identificador de proceso
        int tamProceso; //tamaño que necesita el proceso
        int particion;  //Particion minima/necesaria que ocupa en la memoria
        int burstTime;  //Tiempo que necesita
        Nodo() {}
        Nodo(int particion) {
            this->idProceso = NULL;
            this->tamProceso = NULL;
            this->particion = particion;
            this->burstTime = NULL;
            this->der = NULL;
            this->izq = NULL;
        }
    };
private:
    Nodo* raiz;
    int procesosTerminados;
    int velocidad;  //Para la impresion del procesos en ejecucion
public:
    //Constructores, Gets y Sets
    Arbol() {
        this->raiz = new Nodo;
        this->procesosTerminados = 0;
        this->velocidad = 0;
    }
    void setRaiz(int memTotal) {  //Asiganar la memoria total
        this->raiz->particion = memTotal;
        //Para que la raiz no tenga un solo proceso
        this->raiz->idProceso = 0001;
        this->raiz->burstTime = 0;
        raiz->der = new Nodo(memTotal / 2);
        raiz->izq = new Nodo(memTotal / 2);
    }
    Nodo* getRaiz() { return this->raiz; }

    void setVelocidad(int v) {  //Ajustar la velocidad para mostrar proceso en ejecucion
        this->velocidad = v;
    }
    //Métodos

    void particionar(int tamProceso) {
        queue<Nodo*>fila;
        Nodo* aux = this->raiz;
        fila.push(aux);
        while (!fila.empty()) {  //Mientras la pila no esté vacia
            aux = fila.front();
            if (aux->der == NULL && aux->izq == NULL) {
                if (tamProceso > raiz->particion / 2) {  //Para que no tome a la raiz, caso en postorden
                    return ;
                }
                if (tamProceso <= aux->particion) {
                    
                    if (tamProceso <= aux->particion / 2) {
                        if (aux->particion / 2 >= 32 && aux->idProceso==NULL) {  //Particionar cuando se pueda reducir mas y no esté ocupado ese segmento
                            aux->izq = new Nodo(aux->particion / 2);
                            aux->der = new Nodo(aux->particion / 2);
                            return;
                        }
                    }
                }
            }
            fila.pop();
            if (aux->izq)
                fila.push(aux->izq);
            if (aux->der)
                fila.push(aux->der);
        }
        return ;
    }

    //CREAR FUNCION QUE REVICE Y ELIMINE PARTICIONES NO USADAS

    bool insercion(Nodo* raiz,int idProceso,int tamProceso,int burstTime) {  //Recorrido por amplitud para evaluar
        queue<Nodo*>fila;
        Nodo* aux = raiz;
        fila.push(aux);
        while (!fila.empty()) { 
            aux = fila.front();

            if (aux->der == NULL && aux->izq == NULL) {
                if (tamProceso > raiz->particion / 2) {  //Para que no tome a la raiz, caso en postorden
                    return false;
                }
                if (tamProceso <= aux->particion) {
                    if (tamProceso >= aux->particion / 2) {
                        if (aux->idProceso == NULL) {
                            aux->idProceso = idProceso;
                            aux->tamProceso = tamProceso;
                            aux->burstTime = burstTime;
                            return true;
                        }
                    }
                }
            }
            fila.pop();
            if (aux->izq)
                fila.push(aux->izq);
            if (aux->der)
                fila.push(aux->der);
        }
        return false;
    }

    bool asignacion(int idProceso,int tamProceso,int burstTime) {
        int n=0;
        //Calcular numero max de veces en donde pueden haber particiones (Necesario)
        if (raiz->particion > 2048) {  //<----------------------- Si el tamaño de memoria es 4096
            n = 128;
        }
        else {
            if (raiz->particion > 1024) {  //<------------------- Si el tamaño de memoria es 2048
                n = 64;
            }
            else {  //<------------------------------------------ Si el tamaño de memoria es 1024
                n = 32;
            }
        }
        int cuanto = 27;  //El tiempo que da el CPU
        int bandera = false;
        for (int i = 0; i < n; i++) {  //Ciclo hasta que se inserte, o no
            if (insercion(getRaiz(), idProceso, tamProceso, burstTime) == true) {
                bandera = true;
                break;
            }
            else {
                particionar(tamProceso);
            }
        }
        //Para eliminar problema de que particiona demás
        //Funcion para que cheque si hay particiones que se puedan juntar, y que las junte
        for (int i = 0; i < n; i++) { 
            concatenar();
        }
        //Funcion para sumar restar cuanto a los procesos
        restaCuanto();//<----------------------------------------------- Se muestra proceso en ejecucion
        //Funcion para checar si algun proceso ya puede ser descargado
        eliminar();
        if (bandera == true) {
            //Se insertó adecuadamente el proceso en la memoria
            //cout << endl << "Correcto" << endl;
            return true;
        }
        else {
            //No se insertó adecuadamente el proceso en la memoria
            //cout << endl << "No Correcto" << endl;
            return false;
        }
    }

    void concatenar() {  //<------- Junta espacios de memoria que esten libres
        queue<Nodo*>fila;
        Nodo* aux = this->raiz;
        fila.push(aux);
        while (!fila.empty()) {
            aux = fila.front();
            if (aux != this->raiz) {
                if (aux->der != NULL && aux->izq != NULL) {
                    //eliminar particiones que no se estan utilizando
                    if (aux->izq->idProceso == NULL && aux->der->idProceso == NULL) {
                        if (aux->izq->izq == NULL && aux->izq->der == NULL && aux->der->izq == NULL && aux->der->der == NULL) {
                        aux->izq = NULL;
                        aux->der = NULL;
                        return;
                        }
                    }
                }
            }
            fila.pop();
            if (aux->izq)
                fila.push(aux->izq);
            if (aux->der)
                fila.push(aux->der);
        }
    }

    void restaCuanto() {  //Resta 1 cuanto del sistema al burst time de los procesos, se muestra individual para cada proceso
        queue<Nodo*>fila;
        Nodo* aux = this->raiz;
        fila.push(aux);
        while (!fila.empty()) {
            aux = fila.front();
            if (aux->der == NULL && aux->izq == NULL && aux->idProceso != NULL) {
                gotoxy(60, 14); cout << "                                                 ";
                gotoxy(60, 14); cout <<"Proceso en ejecucion : " << "[id:" << aux->idProceso << ", tam: " << aux->tamProceso << ", bT: " << aux->burstTime << "]";
                aux->burstTime = aux->burstTime - 1;  //BurstTime -1;
                gotoxy(112, 14); cout << "                                      ";
                gotoxy(112, 14);cout<<" ----->  "<< "[id:" << aux->idProceso << ", tam:" << aux->tamProceso << ", bT:" << aux->burstTime << "]";
                Sleep(this->velocidad);
            }
            fila.pop();
            if (aux->izq)
                fila.push(aux->izq);
            if (aux->der)
                fila.push(aux->der);
            gotoxy(112, 30);
        }
    }

    void eliminar() {
        int n = raiz->particion;
        int n2 = 0;
        while (n >= 32) {  //Saber cuantos niveles existen para su recorrido
            n = n / 2;
            n2++;
        }
        eliminarAux();
        
        for (int i = 0; i < 45; i++) {
            concatenar();  //<------------- Para concatenar una posible particion desocupada
        } 
    }

    void eliminarAux() {
        queue<Nodo*>fila;
        Nodo* aux = this->raiz;
        fila.push(aux);
        while (!fila.empty()) {
            aux = fila.front();
            if (aux != this->raiz) {
                if (aux->der == NULL && aux->izq == NULL && aux->idProceso != NULL) {
                    if (aux->burstTime <= 0) {  //<-------------- Se resetea el nodo encontrado
                        aux->idProceso = NULL;
                        aux->tamProceso = NULL;
                        aux->burstTime = NULL;
                        this->procesosTerminados++;
                    }  
                }
            }
            fila.pop();
            if (aux->izq)
                fila.push(aux->izq);
            if (aux->der)
                fila.push(aux->der);
        }
    }

    float estadisticas(int opc) {
        if (opc == 3) {  //<----------------------- Retorna numero de procesos ejecutados
            return this->procesosTerminados;
        }
        int suma = 0;
        float auxiliarPromedios = 0.0;
        queue<Nodo*>fila;
        Nodo* aux = this->raiz;
        fila.push(aux);
        while (!fila.empty()) {
            aux = fila.front();
            if (aux != this->raiz) {
                if (aux->der == NULL && aux->izq == NULL && aux->idProceso != NULL) {
                    if (opc == 1) {       //<---------------------- Calcula promedio de uso de la memoria
                        auxiliarPromedios += aux->particion;
                    }
                    if (opc == 2) {
                        suma++;
                    }
                }
            }
            fila.pop();
            if (aux->izq)
                fila.push(aux->izq);
            if (aux->der)
                fila.push(aux->der);
        }
        if (opc == 1) {       //<---------------------- Retorna promedio de uso de la memoria
            return (auxiliarPromedios * 100) / raiz->particion;
        }
        else {
            return suma;
        }
    }

    void imprimirMemoria() {  //Muestra los procesos que se encuentran en la memoria
        queue<Nodo*>fila;
        Nodo* aux = this->raiz;
        fila.push(aux);
        while (!fila.empty()) {
            aux = fila.front();
            if (aux->der == NULL && aux->izq == NULL && aux->idProceso != NULL) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                cout << "[id:" << aux->idProceso << ", tam:" << aux->tamProceso << ", bT:" << aux->burstTime << "]";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
            }
            if (aux->der == NULL && aux->izq == NULL && aux->idProceso == NULL) { 
                cout << "[" << aux->idProceso << ", tam:" << aux->particion<< ", " << aux->burstTime << "]";
            }
            fila.pop();
            if (aux->izq)
                fila.push(aux->izq);
            if (aux->der)
                fila.push(aux->der);
        }
    }


    //*********************************************************************************
    //IMPRIMIR Arbol <------------------------------- Otra forma de imprimir la memoria
    void printBT(const std::string& prefix, const Nodo* node, bool isLeft)
    {
        if (node != NULL)
        {
            std::cout << prefix;

            std::cout << (isLeft ? "|-" : "-");

            // print the value of the node
            if (node->idProceso == 1 && node->burstTime == 0) {
                std::cout << node->particion << std::endl;
            }
            else {
                if (node->idProceso == 0) {
                    if (node->der == NULL && node->izq == NULL) {
                        std::cout << node->particion; SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                        cout << " -- " << "Free" << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                        
                    }
                    else {
                        std::cout << node->particion << std::endl;
                    }
                }
                else {
                    std::cout << node->particion;
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                    cout << "<--" << "[id: " << node->idProceso << ", tam:" << node->tamProceso << ", bT: " << node->burstTime << "]" << std::endl;
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                }
            }
            

            // enter the next tree level - left and right branch
            printBT(prefix + (isLeft ? "|   " : "    "), node->izq, true);
            printBT(prefix + (isLeft ? "|   " : "    "), node->der, false);
        }
    }
    void printBT(const Nodo* node)
    {
        printBT("", node, false);
    }
    //FUNCION GOTOXY
    void gotoxy(int x, int y) {
        HANDLE hcon;
        hcon = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD dwPos;
        dwPos.X = x;
        dwPos.Y = y;
        SetConsoleCursorPosition(hcon, dwPos);
    }
};
