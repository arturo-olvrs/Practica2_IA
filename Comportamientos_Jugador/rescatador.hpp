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


  // Métodos Privados

  /**
   * @brief Método que determina la casilla más interesante a la que se puede mover el rescatador.
   * @param sensores  Estructura de datos que contiene la información de los sensores.
   * 
   * @return  1 si debe moverse a la casilla 1 (TURN_SL)
   *          2 si debe moverse a la casilla 2 (WALK)
   *          3 si debe moverse a la casilla 3 (TURN_SR)
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
   * @brief Método que busca entre las tres casillas del frente del rescatador una casilla de un tipo determinado.
   * 
   * @param sensores  Estructura de datos que contiene la información de los sensores.
   * @param tipo      Tipo de casilla a buscar.
   * 
   * @return  Número de la casilla encontrada.
   *          0 si no se encuentra ninguna casilla del tipo indicado.
   *          1 si se encuentra una casilla del tipo indicado en la casilla 1.
   *          2 si se encuentra una casilla del tipo indicado en la casilla 2.
   *          3 si se encuentra una casilla del tipo indicado en la casilla 3.
   */
  int buscaCasilla(const Sensores & sensores, char tipo);

  /**
   * @brief Método que actualiza la información de los sensores en los mapas del agente.
   * 
   * @param mResultado  Mapa de resultados del agente.
   * @param mCotas     Mapa de cotas del agente.
   * @param sensores   Estructura de datos que contiene la información de los sensores.
   */
  void situarSensorEnMapa(vector<vector<unsigned char>> &mResultado, 
    vector<vector<unsigned char>> &mCotas,
    Sensores sensores);

};

#endif
