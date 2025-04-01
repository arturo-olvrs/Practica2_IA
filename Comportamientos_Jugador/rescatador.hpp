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
   * @brief Método que determina si el rescatador ve una casilla interesante.
   * 
   * @return  1 si debe moverse a la casilla 1 (TURN_SL)
   *          2 si debe moverse a la casilla 2 (WALK)
   *          3 si debe moverse a la casilla 3 (TURN_SR)
   *          0 si no ve ninguna casilla interesante. En ese caso, hará TURN_L
   */
  int veoCasillaInteresante(Sensores sensores);
};

#endif
