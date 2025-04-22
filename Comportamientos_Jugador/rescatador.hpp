#ifndef COMPORTAMIENTORESCATADOR_H
#define COMPORTAMIENTORESCATADOR_H

#include <chrono>
#include <time.h>
#include <thread>
#include <unordered_set>
#include <list>
#include <iostream>

#include "comportamientos/comportamiento.hpp"






class ComportamientoRescatador : public Comportamiento
{

public:
  ComportamientoRescatador(unsigned int size = 0) : Comportamiento(size)
  {
    // Inicializar Variables de Estado Niveles 0,1,4
    lastAction = Action::IDLE;
    tieneZapatillas = false;
    inTURN_SL = false;
    inTURN_R = false;

    // Inicializar el mapa de veces visitadas
    vector<int> fila(size, 0);
    numVecesVisitada.resize(size, fila);
    numVecesVista.resize(size, fila);
  }
  ComportamientoRescatador(std::vector<std::vector<unsigned char>> mapaR, std::vector<std::vector<unsigned char>> mapaC) : Comportamiento(mapaR,mapaC)
  {
    // Inicializar Variables de Estado Niveles 2,3
    tieneZapatillas = false;
    plan= list<Action>();
  }
  ComportamientoRescatador(const ComportamientoRescatador &comport) : Comportamiento(comport) {}
  ~ComportamientoRescatador() {}

  Action think(Sensores sensores);

  int interact(Action accion, int valor);

  Action ComportamientoRescatadorNivel_0(Sensores sensores);
  Action ComportamientoRescatadorNivel_1(Sensores sensores);
  Action ComportamientoRescatadorNivel_2(Sensores sensores);
  Action ComportamientoRescatadorNivel_3(Sensores sensores);
  Action ComportamientoRescatadorNivel_4(Sensores sensores);

private:
  // Variables de Estado
  Action lastAction;      // Almacena la última acción realizada por el rescatador
  bool tieneZapatillas;   // Indica si el rescatador tiene zapatillas
  bool inTURN_SL;          // Indica si el rescatador está en su turno de SL
  bool inTURN_R;          // Indica si el rescatador está en su turno de SR
  vector<vector<int>> numVecesVisitada;
  vector<vector<int>> numVecesVista;
  static const int PROFUNDIDAD_SENSOR = 4; // Número de casillas hacia delante que ve el agente (incluyendo la suya misma)
  static const int NUM_CASILLAS = PROFUNDIDAD_SENSOR * PROFUNDIDAD_SENSOR; // Número de casillas que ve el agente

  static const unordered_set<char> CASILLAS_NO_TRANSITABLES; // Conjunto de casillas no transitables

  list<Action> plan; // Lista de acciones que forman el camino a seguir



  /**
   * @brief Struct que representa el estado del agente.
   */
  struct Estado{
    int fil;
    int col;
    Orientacion orientacion;
    bool tieneZapatillas;

    // Operador de Igualdad
    bool operator==(const Estado & otro) const {
      return fil == otro.fil
          && col == otro.col
          && orientacion == otro.orientacion
          && tieneZapatillas == otro.tieneZapatillas;
    }

    // Operador de < para poder usarlo en un set
    bool operator<(const Estado & otro) const {
      if (fil < otro.fil)
        return true;
      else if (fil == otro.fil && col < otro.col)
        return true;
      else if (fil == otro.fil && col == otro.col && orientacion < otro.orientacion)
        return true;
      else if (fil == otro.fil && col == otro.col && orientacion == otro.orientacion && tieneZapatillas < otro.tieneZapatillas)
        return true;
      else
        return false;
    }
  };


  /**
   * @brief Struct que representa un nodo en el algoritmo de Dijkstra.
   */
  struct Nodo{
    Estado estado;	// Estado del nodo
    int gastoEnergia;	// Gasto de energía al nodo
    list<Action> acciones;	// Acciones que se han realizado para llegar al nodo

    bool operator<(const Nodo & otro) const {
      return estado < otro.estado;
    }

    bool operator==(const Nodo & otro) const {
      return estado == otro.estado;
    }
  };


  // Métodos Privados

  /**
   * @brief Método que determina la casilla más interesante a la que se puede mover el rescatador.
   * @param sensores  Estructura de datos que contiene la información de los sensores.
   * 
   * @return  1-15: Debe moverse a la casilla correspondiente
   *          -1,-2,-4,-5: Debe moverse a la casilla correspondiente del lateral
   *          0 si no ve ninguna casilla interesante. En ese caso, hará TURN_L
   */
  int veoCasillaInteresante(const Sensores & sensores);

  /**
   * @brief Método que determina si la casilla es accesible o no.
   * 
   * @param sensores  Estructura de datos que contiene la información de los sensores.
   * @param comprobarAltura  Indica si se debe comprobar la altura de la casilla.
   * @param casilla  Número de la casilla.
   * 
   * @return  true si la casilla es accesible, false en caso contrario.
   */
  bool casillaAccesible(const Sensores& sensores, bool comprobarAltura, int casilla);

  /**
   * @brief Método que determina si la casilla es accesible o no.
   * 
   * @param estado  Estado del agente.
   * @param comprobarAltura  Indica si se debe comprobar la altura de la casilla.
   * @param casilla  Número de la casilla.
   * 
   * @pre No se comprueban agentes
   * 
   * @return  true si la casilla es accesible, false en caso contrario.
   */
  bool casillaAccesible(const Estado& estado, bool comprobarAltura, int casilla);

  /**
   * @brief Método que determina si la casilla es accesible o no.
   * 
   * @param filAgente  Fila del agente.
   * @param colAgente  Columna del agente.
   * @param orientacion  Orientación del agente.
   * @param casilla  Número de la casilla.
   * @param conZapatillas  Indica si el agente tiene zapatillas.
   * @param comprobarAgentes Indica si se debe comprobar que no hay agentes en la casilla.
   * @param comprobarAltura Indica si se debe comprobar la altura de la casilla.
   * 
   * @return  true si la casilla es accesible, false en caso contrario.
   */
  bool casillaAccesible(int filAgente, int colAgente, Orientacion orientacion, bool conZapatillas, bool comprobarAgentes, bool comprobarAltura, int casilla);

  /**
   * @brief Método que actualiza la información de los sensores en los mapas del agente.
   * 
   * @param mResultado  Mapa de resultados del agente.
   * @param mCotas     Mapa de cotas del agente.
   * @param mEntidades  Mapa de entidades del agente.
   * @param sensores   Estructura de datos que contiene la información de los sensores.
   */
  void situarSensorEnMapa(vector<vector<unsigned char>> &mResultado, 
                          vector<vector<unsigned char>> &mCotas,
                          vector<vector<unsigned char>> &mEntidades,
                          Sensores sensores);
  
  /**
   * @brief Método que reinicializa el mapa de veces visitadas y vistas.
   * 
   * @post  El mapa de veces visitadas y vistas se reinicializa a 0.
   */
  void reinicializarVeces_VistaVisitada();



  /**
   * @brief Método que devuelve las coordenadas en el mapa de la casilla relativa a la posición del agente.
   * 
   * @param filAgente  Fila de la casilla relativa.
   * @param colAgente  Columna de la casilla relativa.
   * @param orientacion  Orientación del agente.
   * @param casillaRelativa  Número de la casilla relativa.
   * 
   * @return  Un par de enteros que representan las coordenadas de la casilla en el mapa.
   *          El primer entero es la fila y el segundo entero es la columna.
   */
  pair<int, int> aCoordenadas(int filAgente, int colAgente, Orientacion orientacion, int casillaRelativa);


  /**
   * @brief Método que actualiza las matrices de veces visitadas y vistas.
   */
  void actualizarMatrices_VistasVisitadas(const Sensores& sensores);


  /**
   * @brief Método que devuelve el estado del agente después de ejecutar una acción.
   * 
   * @param action  Acción a ejecutar.
   * @param inicio  Estado inicial del agente.
   */
  Estado ejecutarAccion(Action action, const Estado & inicio);

  /**
   * @brief Método que implementa el algoritmo de Dijkstra para encontrar el camino más corto.
   * 
   * @param origen  Estado de origen.
   * @param filDestino  Fila de la casilla destino.
   * @param colDestino  Columna de la casilla destino.
   * 
   * @return  Lista de acciones a seguir para llegar al destino.
   */
  list<Action> Dijkstra(const Estado& origen, int filDestino, int colDestino);
  
  /**
   * @brief Método que calcula el gasto de energía al realizar una acción.
   * 
   * @param action  Acción a realizar.
   * @param filInicio  Fila de la casilla inicial.
   * @param colInicio  Columna de la casilla inicial.
   * @param filDestino  Fila de la casilla destino.
   * @param colDestino  Columna de la casilla destino.
   * 
   * @return  Gasto de energía al realizar la acción.
   * 
   * @pre La acción es válida
   */
  int gastoAccion(Action action, int filInicio, int colInicio, int filDestino, int colDestino);

  /**
   * @brief Método que visualiza el plan de acciones a seguir.
   * 
   * @param origen  Estado inicial del agente.
   * @param plan    Lista de acciones a seguir.
   */
  void VisualizaPlan(const Estado& origen, const list<Action>& plan);

};

#endif
