#include "Arbol.h"
#include <ctime>
#include <queue>
#include <cstdio>
#include <iomanip>
#include <cstdlib>
#include <conio.h>  //Para Kbhit - Detectar teclas durante ejecucion
#include <windows.h>
#include <iostream>
using namespace std;

struct proceso  //Datos de un proceso
{
    int idProceso = 0;
    int tamProceso = 0;
    int burstTime = 0;
    int tempEjecucion = 0;
};

void mostrarCola(queue<proceso>);
void encolar(int, int);
void estadisticas(float, int, float, float, float);
void menu();
void tablaProcesosCola(queue<proceso>,int,int);
void estadisticasDurante(int, float, float);
void estadisticasDurante(float, float);
void dibujarCuadro(int, int, int, int);
void portada();
void menuPrincipal();

void gotoxy(int, int);

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    system("mode con: cols=52 lines=27");
    
    menuPrincipal();
    int memTotal = 0;
    int quantum = 0;
    gotoxy(8, 20); cout << "Memoria Total: ";
    gotoxy(8, 23); cout << "Cuanto: ";
    
    
    
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    int opc;
    do {  //<--------------------- Eleccion de tamaёo de 
        gotoxy(23, 20); cout << " ";
        gotoxy(23, 20); cin >> opc;
        if (opc == 1) {
            memTotal = 1024;
        }
        else {
            if (opc == 2) {
                memTotal = 2048;
            }
            else {
                if (opc == 3) {
                    memTotal = 4096;
                }
                else {
                    memTotal = 8192;
                }
            }
        }
    } while (opc<1 || opc>4);
    gotoxy(16, 23); cin >> quantum;
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    system("mode con: cols=170 lines=300");
    
    encolar(memTotal,quantum);
    
    cout << endl << endl;
    return 0;
}

void encolar(int memTotal,int quantum) {
    srand(time(NULL));
    Arbol* memoria = new Arbol;
    Arbol::Nodo* raiz;
    memoria->setRaiz(memTotal);
    cout << endl << endl;

    int burstTime = 0, idProceso = 0, tamProceso = 0, cuanto = 1;
    float tEj = 0, porcentajeMemUsada = 0.0, totalProcesos = 0;
    queue<proceso>cola;  //Cola de Procesos tipo struct
    proceso pp;  //Auximiar strcut para almacenar en la cola
    int id = 0, tamP = 0, bT = 0;

    float promAtencion = 0;
    float promedioTamProcesos = 0;
    float promTempEjecucion = 0;
    
    int velocidad = 1000;
    memoria->setVelocidad(velocidad);
    char key;
    int idaux = 0;
    while(1){
        if (_kbhit()) {
            key = _getch();
            if ((int)key == 27) {  //<----------------------- Esc para salir (mostrar estadisticas)
                system("cls");
                estadisticas(promTempEjecucion, totalProcesos, porcentajeMemUsada, promedioTamProcesos, promAtencion);
                break;
            }       
            if ((int)key == 43) {//<------------------------- Configurar para aumentar velocidad con '+'
                velocidad -= 200;
                
                if (velocidad <= 0) {  //<------------------- Configurar para tener la mayor velocidad, pensar si dejarlo en 0 o mсs
                    velocidad = 50;
                    memoria->setVelocidad(velocidad);
                }
                else {
                    memoria->setVelocidad(velocidad);
                }
            }
            if ((int)key == 45) {//<------------------------- Configurar para disminuir velocidad con '-'
                velocidad += 200;
                memoria->setVelocidad(velocidad);
            }
            if ((int)key == 80 || (int)key == 112) {//<------------------ Letra 'p' o 'P' para detener la ejecucion
                estadisticasDurante(promTempEjecucion, promAtencion);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
                gotoxy(80, 11);
                system("pause");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
            }  
        }
        system("cls");
        menu();
        idaux++;
        pp.idProceso = idaux;
        pp.tamProceso = 50 + rand() % 200; //<----------------------------- Rango entre 50 - 200
        pp.burstTime = 1 + rand() % 10;  //<------------------------------- Rango entre 1 - 10

        pp.tempEjecucion = pp.burstTime;
        cola.push(pp);
        
        //Auximiliares para la asignacion en la memoria
        pp = cola.front();
        id = pp.idProceso;
        tamP = pp.tamProceso;
        bT = pp.burstTime;
        tEj = pp.tempEjecucion;

        //***************************
                //MOSTRAR ESTADISTICAS TODO EL TIEMPO - Las necesarias
        totalProcesos = memoria->estadisticas(3);
        porcentajeMemUsada = memoria->estadisticas(1);
        estadisticasDurante(totalProcesos, porcentajeMemUsada, promedioTamProcesos);
        tablaProcesosCola(cola, cuanto, quantum);  //<-------------------------------------- Muestra la tabla de procesos durante la ejecucion
        cout << endl << endl << "\n     Memoria Total: " << memTotal << "    Cuanto designado: " << quantum << endl;
        cout << endl << endl << "    -Representacion de la memoria-" << endl << endl;
        memoria->imprimirMemoria();
        cout << endl << endl << endl << "    -Representacion de la memoria con un arbol binario-" << endl << endl;
        memoria->printBT(memoria->getRaiz());
        //**************************+

        if (memoria->asignacion(id, tamP, bT) == true) {
            if (id > 1) { 
                promTempEjecucion = (promTempEjecucion + tEj) / 2;
                promedioTamProcesos = (promedioTamProcesos + pp.tamProceso) / 2;
                promAtencion = (promAtencion + cuanto) / 2;
            }
            else { //Cuando es el primer proceso, el promedio es el valor inicial
                promTempEjecucion = tEj;
                promedioTamProcesos = pp.tamProceso;
                promAtencion = cuanto;
            }
            cola.pop();
            //Cuanto general pero que funciona de manera particular para cada proceso
            cuanto = 0;  //<---- Se reinicia en caso de que el proceso ya se haya insertado en la memoria
        }
        else {  //Sigue aumentando solo si el proceso aun no puede ingresar a la memoria (El cuanto aumenta hasta el limite)
            cuanto++;
            pp.tempEjecucion++;
        }
        if (cuanto == quantum) {  //<------------- Si ya pasѓ el cuanto asignado, volver a formar el proceso en la cola
            cola.push(cola.front());
            cola.pop();
            cuanto = 0;  //Reinicia el cuanto
        }
        
        Sleep(velocidad);
    }
}

void mostrarCola(queue<proceso> cola) {
    proceso aux;
    for (int i = 0; i < cola.size(); i++) {
        aux = cola.front();
        cout << " " << aux.idProceso;
        cola.push(aux);
        cola.pop();
    }
    cout << endl << endl;
}

void estadisticas(float promTempEjecucion, int totalProcesos, float porcentajeUsado, float promTamProcesos,float promAtencion) {
    system("mode con: cols=55 lines=50");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
    cout << endl << endl << endl << "\t\tMODULO DE ESTADISTICAS" << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << endl << "        Promedio Tiempo de Atencion: " << fixed << setprecision(6) << promAtencion << " ms";
    cout << endl << "        Pomedio de Ejecucion: " << fixed << setprecision(6) << promTempEjecucion << " ms";
    cout << endl << "        Total de Procesos ejecutados: " << totalProcesos;
    cout << endl << "        Tamanio promedio de Procesos: " << fixed << setprecision(0) << promTamProcesos << " kb";
    cout << endl << "        Porcentaje de uso en la memoria: " << fixed << setprecision(2) << porcentajeUsado << " %";
    cout << endl << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    portada();
    dibujarCuadro(4,1,51,40);
}

void menu() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    cout << endl << "   лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл";
    cout << endl << "   лллллллллллл Aumentar Velocidad [+] ллллллллллллллллллл Disminuir Velocidad [-] лллллллллллллллллллллл Pausar [p] ллллллллллллллллллллллллл Salir [Esc] лллллллллллл";
    cout << endl << "   лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл";
    //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void tablaProcesosCola(queue<proceso>cola,int cuanto,int quantum) {

    //ARREGLAR TIEMPO DE ESPERA - AUMENTAR DESDE QUE SE CREA EL PROCESO
    gotoxy(6, 7); cout << "Lista de Procesos en Cola";
    cout << endl << "    ----------------------------";
    cout << endl << "        ID     TAM     BT    Q";
    cout << endl << "    ----------------------------";
    proceso aux;
    int i = 0;
    for (i = 0; i < cola.size(); i++) {
        aux = cola.front();
        gotoxy(6,i+11); cout << fixed << setprecision(6) << aux.idProceso;
        gotoxy(15, i + 11); cout << aux.tamProceso;
        gotoxy(23, i + 11); cout << aux.burstTime;
        gotoxy(28, 11); cout << cuanto << "/" << quantum;
        cola.push(aux);
        cola.pop();
    }
    gotoxy(6, i + 10);
}

void estadisticasDurante(int totalProcesos, float porcentajeUsado, float promTamProcesos) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    gotoxy(35, 7); cout << "Total de Procesos ejecutados: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << totalProcesos;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    gotoxy(80, 7); cout << "Tamanio promedio de Procesos: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << fixed << setprecision(0) << promTamProcesos << " kb";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    gotoxy(124, 7); cout << "Porcentaje de uso en la memoria: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << fixed << setprecision(2) << porcentajeUsado << " %";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
}

void estadisticasDurante(float promTempEjecucion,float promAtencion) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    gotoxy(55, 9); cout << "Promedio Tiempo de Atencion: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << fixed << setprecision(6) << promAtencion << " ms";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    gotoxy(106, 9); cout << "Pomedio de Ejecucion ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << fixed << setprecision(6) << promTempEjecucion << " ms";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
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

void menuPrincipal() {

    gotoxy(2, 2);
    cout << "\t     ллЛ   ллЛлллллллЛлллллллЛ" << endl;
    cout << "\t     ллК   ллКллЩЭЭллКллЩЭЭллК" << endl;
    cout << "\t     ллК   ллКлллллллКлллллллК" << endl;
    cout << "\t     ллК   ллКллЩЭЭллКллЩЭЭллК" << endl;
    cout << "\t     ллллллллКллК  ллКллК  ллК" << endl;
    cout << "\t     ШЭЭЭЭЭЭЭМШЭМ  ШЭМШЭМ  ШЭМ" << endl;
    cout << endl << "   SIMULADOR DE TECNICAS DE COLOCACION DE MEMORIA";
    cout << endl << "       Y PLANIFICADOR DE PROCESOS BASADO EN";
    cout << endl << "            BUDDY SYSTEM Y ROUND ROBIN";
    gotoxy(19, 15); cout << "Opciones-kb";
    gotoxy(8,16); cout << "["; SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << "1024"; SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    cout << "]   ";
    cout << "["; SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << "2048"; SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    cout << "]   ";
    cout << "["; SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << "4096"; SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    cout << "]   ";
    cout << "["; SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << "8192"; SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    cout << "]   ";
    gotoxy(8, 17); cout << "  1        2        3        4";
    dibujarCuadro(1, 1, 50, 25); //SE DIBUJA EL CUADRO DEL TITULO
}

//FUNCION QUE DIBUJA EL CUADRO
void dibujarCuadro(int x1, int y1, int x2, int y2) {
    int i;
    for (i = x1; i < x2; i++) {
        gotoxy(i, y1); cout << "\304"; //linea horizontal superior
        gotoxy(i, y2); cout << "\304"; //linea horizontal inferior
    }

    for (i = y1; i < y2; i++) {
        gotoxy(x1, i); cout << "\263"; //linea vertical izquierda
        gotoxy(x2, i); cout << "\263"; //linea vertical derecha
    }

    gotoxy(x1, y1); cout << "\332";
    gotoxy(x1, y2); cout << "\300";
    gotoxy(x2, y1); cout << "\277";
    gotoxy(x2, y2); cout << "\331";
}

void portada() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
    cout << endl << endl << endl << endl << endl << endl;
    cout << "\t\tллЛ   ллЛлллллллЛлллллллЛ" << endl;
    cout << "\t\tллК   ллКллЩЭЭллКллЩЭЭллК" << endl;
    cout << "\t\tллК   ллКлллллллКлллллллК" << endl;
    cout << "\t\tллК   ллКллЩЭЭллКллЩЭЭллК" << endl;
    cout << "\t\tллллллллКллК  ллКллК  ллК" << endl;
    cout << "\t\tШЭЭЭЭЭЭЭМШЭМ  ШЭМШЭМ  ШЭМ" << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    //setlocale(LC_ALL, "spanish");
    cout << endl << "\t UNIVERSIDAD AUTONOMA DE AGUASCALIENTES";
    cout << endl << "\t INGENIERIA EN SISTEMAS COMPUTACIONALES";
    cout << endl << "\t\t      3er semestre" << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << endl << "\t\t   Sistemas Operativos";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << endl << "\t Profesor Javier Santiago Cortes Lopez" << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << endl << "\t\t      Integrantes:";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << endl << "\t\t   Cesar Reyes Torres\n\t       Estefeen Sandoval Rodriguez\n\t      Frida Ximena Escamilla Ramirez";
    cout << endl << "\t\t   Rubщn Davila Flores" << endl;
    cout << endl << "\t\t     Proyecto Final";
    cout << endl << "\t     15/12/2022 - Aguascalientes,Ags"; 
}

void procesosEnMemoria(int memTotal,int quantum,queue<proceso> cola) {
    cout << endl << endl << "   Memoria Total: " << memTotal << "    Cuanto designado: " << quantum << endl;
}
