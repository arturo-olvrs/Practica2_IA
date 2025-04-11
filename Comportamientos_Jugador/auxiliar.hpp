#ifndef COMPORTAMIENTOAUXILIAR_H
#define COMPORTAMIENTOAUXILIAR_H

#include <chrono>
#include <time.h>
#include <thread>

#include "comportamientos/comportamiento.hpp"

class ComportamientoAuxiliar : public Comportamiento
{

public:
  ComportamientoAuxiliar(unsigned int size = 0) : Comportamiento(size)
  {
    // Inicializar Variables de Estado Niveles 0,1,4
    lastAction = Action::IDLE;
    tieneZapatillas = false;
    num_TURN_SR_Restantes = 0;

    // Inicializar el mapa de veces visitadas
    vector<int> fila(size, 0);
    numVecesVisitada.resize(size, fila);
  }
  ComportamientoAuxiliar(std::vector<std::vector<unsigned char>> mapaR, std::vector<std::vector<unsigned char>> mapaC) : Comportamiento(mapaR,mapaC)
  {
    // Inicializar Variables de Estado Niveles 2,3
  }
  ComportamientoAuxiliar(const ComportamientoAuxiliar &comport) : Comportamiento(comport) {}
  ~ComportamientoAuxiliar() {}

  Action think(Sensores sensores);

  int interact(Action accion, int valor);

  Action ComportamientoAuxiliarNivel_0(Sensores sensores);
  Action ComportamientoAuxiliarNivel_1(Sensores sensores);
  Action ComportamientoAuxiliarNivel_2(Sensores sensores);
  Action ComportamientoAuxiliarNivel_3(Sensores sensores);
  Action ComportamientoAuxiliarNivel_4(Sensores sensores);

private:
  // Definir Variables de Estado
  Action lastAction;      // Almacena la última acción realizada por el auxiliar
  bool tieneZapatillas;   // Indica si el auxiliar tiene zapatillas
  int num_TURN_SR_Restantes;  // Indica el número de TURN_SR restantes para lograr TURN_SL
  vector<vector<int>> numVecesVisitada;


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
   * @brief Método que devuelve las coordenadas en el mapa de la casilla relativa a la posición del agente.
   * 
   * @param casillaRelativa  Número de la casilla relativa al agente.
   * @param sensores      Sensores del agente
   * 
   * @return  Un par de enteros que representan las coordenadas de la casilla en el mapa.
   *          El primer entero es la fila y el segundo entero es la columna.
   */
  pair<int, int> aCoordenadas(const Sensores& sensores, int casillaRelativa);
};

#endif
