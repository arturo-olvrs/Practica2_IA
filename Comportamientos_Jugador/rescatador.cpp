#include "../Comportamientos_Jugador/rescatador.hpp"
#include "motorlib/util.h"

Action ComportamientoRescatador::think(Sensores sensores)
{
	Action accion = IDLE;

	switch (sensores.nivel)
	{
	case 0:
		accion = ComportamientoRescatadorNivel_0 (sensores);
		break;
	case 1:
		// accion = ComportamientoRescatadorNivel_1 (sensores);
		break;
	case 2:
		// accion = ComportamientoRescatadorNivel_2 (sensores);
		break;
	case 3:
		// accion = ComportamientoRescatadorNivel_3 (sensores);
		break;
	case 4:
		// accion = ComportamientoRescatadorNivel_4 (sensores);
		break;
	}

	return accion;
}

int ComportamientoRescatador::interact(Action accion, int valor)
{
	return 0;
}


int ComportamientoRescatador::veoCasillaInteresante(Sensores sensores){

	int res = 0;

	// Prioridad:
	// 1. WALK		(costará un paso)
	// 2. TURN_SL	(costará un giro + un paso)
	// 3. TURN_SR 	(costará dos giros + un paso)

	// Iteramos por tanto 2,3,1

	// Comenzamos por identificar puesto base
	if (sensores.superficie[2] == 'X') res = 2;
	else if (sensores.superficie[3] == 'X') res = 3;
	else if (sensores.superficie[1] == 'X') res = 1;

	// Si no hay puesto base y no tengo zapatillas, miro si hay zapatillas
	if (res == 0 && !tieneZapatillas){
		if (sensores.superficie[2] == 'D') res = 2;
		else if (sensores.superficie[3] == 'D') res = 3;
		else if (sensores.superficie[1] == 'D') res = 1;
	}
	
	// Si no hay puesto base ni zapatillas, miro si hay una casilla de interés
	if (res == 0){
		if (sensores.superficie[2] == 'C') res = 2;
		else if (sensores.superficie[3] == 'C') res = 3;
		else if (sensores.superficie[1] == 'C') res = 1;
	}

	return res;
}


Action ComportamientoRescatador::ComportamientoRescatadorNivel_0(Sensores sensores)
{
	// El comportamiento de seguir un camino hasta encontrar un puesto base.
	
	Action action = IDLE;	// Acción por defecto.

	// Actualización de variables de estado.
	if (sensores.superficie[0] == 'D')
		tieneZapatillas = true;
	

	// Definición del comportamiento
	if (sensores.superficie[0] == 'X')	// Llegó al puesto base
		action = Action::IDLE;
	else if (inTURN_SL){	// Está en su giro SL
		action = Action::TURN_SR;
		inTURN_SL = false;
	}
	else{ // Vamos a realizar el movimiento determinado por lo que veo

		int res = veoCasillaInteresante(sensores);
		switch (res){
			case 0: // No veo nada interesante
				action = Action::TURN_L;
				break;
			case 1: // TURN_SL
				inTURN_SL = true;
				action = Action::TURN_L;
				break;
			case 2: // WALK
				action = Action::WALK;
				break;
			case 3: // TURN_SR
				action = Action::TURN_SR;
				break;
		}

	}

	lastAction = action;	// Actualizamos la última acción realizada
	return action;
}

Action ComportamientoRescatador::ComportamientoRescatadorNivel_1(Sensores sensores)
{
}

Action ComportamientoRescatador::ComportamientoRescatadorNivel_2(Sensores sensores)
{
}

Action ComportamientoRescatador::ComportamientoRescatadorNivel_3(Sensores sensores)
{
}

Action ComportamientoRescatador::ComportamientoRescatadorNivel_4(Sensores sensores)
{
}