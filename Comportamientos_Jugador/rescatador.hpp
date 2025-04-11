#ifndef COMPORTAMIENTORESCATADOR_H
#define COMPORTAMIENTORESCATADOR_H

#include <chrono>
#include <time.h>
#include <thread>

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
   * @param sensores  Estructura de datos que contiene la información de los sensores.
   * @param casilla   Número de la casilla a comprobar.
   * 
   * @return  true si la casilla es accesible.
   *          false si la casilla no es accesible.
   */
  bool casillaAccesible(const Sensores & sensores, int casilla);

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
   * @param casillaRelativa  Número de la casilla relativa al agente.
   * @param sensores      Sensores del agente
   * 
   * @return  Un par de enteros que representan las coordenadas de la casilla en el mapa.
   *          El primer entero es la fila y el segundo entero es la columna.
   */
  pair<int, int> aCoordenadas(const Sensores& sensores, int casillaRelativa);


  /**
   * @brief Método que actualiza las matrices de veces visitadas y vistas.
   */
  void actualizarMatrices_VistasVisitadas(const Sensores& sensores);

};

#endif
