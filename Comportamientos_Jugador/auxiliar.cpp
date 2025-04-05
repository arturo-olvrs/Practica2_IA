#include "../Comportamientos_Jugador/auxiliar.hpp"
#include <iostream>
#include "motorlib/util.h"
#include "math.h"

Action ComportamientoAuxiliar::think(Sensores sensores)
{
	Action accion = IDLE;

	switch (sensores.nivel)
	{
	case 0:
		accion = ComportamientoAuxiliarNivel_0 (sensores);
		break;
	case 1:
		//accion = ComportamientoAuxiliarNivel_1 (sensores);
		break;
	case 2:
		// accion = ComportamientoAuxiliarNivel_2 (sensores);
		break;
	case 3:
		// accion = ComportamientoAuxiliarNivel_3 (sensores);
		break;
	case 4:
		// accion = ComportamientoAuxiliarNivel_4 (sensores);
		break;
	}

	return accion;
}

int ComportamientoAuxiliar::interact(Action accion, int valor)
{
	return 0;
}



void ComportamientoAuxiliar::situarSensorEnMapa(
	vector<vector<unsigned char>> &mResultado, 
	vector<vector<unsigned char>> &mCotas,
	Sensores sensores)
{	
	// Número de casillas hacia delante que ve el agente
	const int PROFUNDIDAD_SENSOR = 3;

	// No debo preocuparme por salirme del mapa, puesto que las tres ultimas filas/columnas son precipicios
	// y no se puede acceder a ellas.

	mResultado	[sensores.posF][sensores.posC] = sensores.superficie[0];
	mCotas		[sensores.posF][sensores.posC] = sensores.cota[0];

	// Relativo al agente.
	int numDelante;				// 1 casilla hacia delante del agente indica la variación de fila/columna
	int numDcha;				// 1 casilla hacia la derecha del agente indica la variación de fila/columna
	bool rumboHorizontal; 		// True si la rumbo es horizontal
	

	switch (sensores.rumbo)
	{
	case norte:
	case noreste:
		numDelante = -1;
		numDcha = 1;
		rumboHorizontal = false;
		break;
	case sur:
	case suroeste:
		numDelante = 1;
		numDcha = -1;
		rumboHorizontal = false;
		break;
	case este:
	case sureste:
		numDelante = 1;
		numDcha = 1;
		rumboHorizontal = true;
		break;
	case oeste:
	case noroeste:
		numDelante = -1;
		numDcha = -1;
		rumboHorizontal = true;
		break;		
	}


	int fila;		// Fila de la casilla a comprobar. Relativa al mapa
	int columna;	// Columna de la casilla a comprobar. Relativa al mapa
	int varDelante;	// Variación hacia delante. Relativa al agente y su rumbo
	int varDcha;	// Variación hacia la derecha. Relativa al agente su rumbo
	int casilla;	// Numeración de la casilla a comprobar. Relativa al sensor del agente

	switch (sensores.rumbo)
	{
	case norte:
	case sur:
	case este:
	case oeste:
		// Iteramos en primer lugar por la diferencia de delante con el agente
		for (int diffDelante=1; diffDelante <= PROFUNDIDAD_SENSOR; diffDelante++){
			varDelante 	= numDelante * diffDelante;

			// Iteramos por la diferencia de derecha con el agente
			for (int diffDcha=-diffDelante; diffDcha <= diffDelante; diffDcha++)
			{
				casilla = diffDelante*(diffDelante+1) + diffDcha;

				
				varDcha 	= numDcha * diffDcha;				
				if (rumboHorizontal){
					fila 	= sensores.posF + varDcha;
					columna = sensores.posC + varDelante;
				}else{
					fila 	= sensores.posF + varDelante;
					columna = sensores.posC + varDcha;
				}
				
				mResultado	[fila][columna] = sensores.superficie	[casilla];
				mCotas		[fila][columna] = sensores.cota			[casilla];
			}
		}
		break;
	
	case noreste:
	case sureste:
	case suroeste:
	case noroeste:
		
		// Iteramos en primer lugar por la diferencia de delante con el agente
		for (int diffDelante=1; diffDelante <= PROFUNDIDAD_SENSOR; diffDelante++){

			// Iteramos por la diferencia de derecha con el agente

			// Primero hasta llegar a la diagonal. Debemos mantener fila fija
			for (int diffDcha=-diffDelante; diffDcha <= 0; diffDcha++){
				casilla = diffDelante*(diffDelante+1) + diffDcha;

				if (rumboHorizontal){
					varDcha		= numDelante * (diffDcha+diffDelante);
					varDelante 	= numDcha * diffDelante;
					fila 		= sensores.posF + varDcha;
					columna 	= sensores.posC + varDelante;
				}else{
					varDelante 	= numDelante * diffDelante;
					varDcha 	= numDcha * (diffDcha+diffDelante);
					fila 		= sensores.posF + varDelante;
					columna 	= sensores.posC + varDcha;
				}

				mResultado	[fila][columna] = sensores.superficie	[casilla];
				mCotas		[fila][columna] = sensores.cota			[casilla];
			}

			// Una vez pasada la diagonal
			for (int diffDcha=1; diffDcha <= diffDelante; diffDcha++){
				casilla = diffDelante*(diffDelante+1) + diffDcha;


				if (rumboHorizontal){
					varDelante 	= numDelante * diffDelante;
					varDcha 	= numDcha * (diffDelante-diffDcha);
					fila 		= sensores.posF + varDelante;
					columna 	= sensores.posC + varDcha;
				}else{
					varDcha		= numDelante * (diffDelante-diffDcha);
					varDelante 	= numDcha * diffDelante;
					fila 		= sensores.posF + varDcha;
					columna 	= sensores.posC + varDelante;
				}
				
				mResultado	[fila][columna] = sensores.superficie	[casilla];
				mCotas		[fila][columna] = sensores.cota			[casilla];
			}
		}
		break;
	}
}

bool ComportamientoAuxiliar::casillaAccesible(const Sensores & sensores, int casilla)
{
	// Comprobamos si la casilla es accesible o no. Será accesible si:
	// 1. No es un precipicio
	// 2. La diferencia de altura entre la casilla y la cota del rescatador es <= 1
	// 3. Si el rescatador tiene zapatillas, la diferencia de altura puede ser <= 2
	bool accesible = sensores.agentes[casilla] == '_'; // No hay agentes en la casilla
	
	int dif = abs(sensores.cota[0] - sensores.cota[casilla]);
	accesible &= sensores.superficie[casilla] != 'P' && (dif<=1);

	return accesible;
}

int ComportamientoAuxiliar::buscaCasilla(const Sensores & sensores, char tipo)
{

	int res = 0;

	// Prioridad:
	// 1. WALK		(costará un paso)
	// 2. TURN_SL	(costará un giro + un paso)
	// 3. TURN_SR 	(costará dos giros + un paso)

	// Iteramos por tanto 2,3,1

	if (casillaAccesible(sensores, 2) && sensores.superficie[2] == tipo) res = 2;
	else if (casillaAccesible(sensores, 3) && sensores.superficie[3] == tipo) res = 3;
	else if (casillaAccesible(sensores, 1) && sensores.superficie[1] == tipo) res = 1;

	return res;
}


int ComportamientoAuxiliar::veoCasillaInteresante(const Sensores & sensores){

	int res = 0;

	// Comenzamos por identificar puesto base
	res = buscaCasilla(sensores, 'X');

	// Si no hay puesto base y no tengo zapatillas, miro si hay zapatillas
	if (res == 0 && !tieneZapatillas)
		res = buscaCasilla(sensores, 'D');
	
	
	// Si no hay puesto base ni zapatillas, miro si hay un camino
	if (res == 0)
		res = buscaCasilla(sensores, 'C');

	return res;
}


Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_0(Sensores sensores)
{
	// El comportamiento de seguir un camino hasta encontrar un puesto base.
	
	Action action = IDLE;	// Acción por defecto.

	// Actualización de variables de estado.
	situarSensorEnMapa(mapaResultado, mapaCotas, sensores);
	if (sensores.superficie[0] == 'D')
		tieneZapatillas = true;
	

	// Definición del comportamiento
	if (sensores.superficie[0] == 'X')	// Llegó al puesto base
		action = Action::IDLE;
	else if (num_TURN_SR_Restantes > 0){	// Está en su giro SL
		action = Action::TURN_SR;
		--num_TURN_SR_Restantes;
	}
	else{ // Vamos a realizar el movimiento determinado por lo que veo

		int res = veoCasillaInteresante(sensores);
		switch (res){
			case 0: // No veo nada interesante, TURN_R
				action = Action::TURN_SR;
				num_TURN_SR_Restantes = 1; // 2 a realizar. -1 pq ya realiza uno en esta iteracion
				break;
			case 1: // TURN_SL
				action = Action::TURN_SR;
				num_TURN_SR_Restantes = 6; // 8 giros es permanecer constante, 8-1 en total. -1 pq ya realiza uno en esta iteracion
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

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_1(Sensores sensores)
{
}

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_2(Sensores sensores)
{
}

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_3(Sensores sensores)
{
}

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_4(Sensores sensores)
{
}