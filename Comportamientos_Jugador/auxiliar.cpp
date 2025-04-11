#include "../Comportamientos_Jugador/auxiliar.hpp"
#include <iostream>
#include "motorlib/util.h"
#include "math.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <map>

Action ComportamientoAuxiliar::think(Sensores sensores)
{
	Action accion = IDLE;

	switch (sensores.nivel)
	{
	case 0:
		accion = ComportamientoAuxiliarNivel_0 (sensores);
		break;
	case 1:
		accion = ComportamientoAuxiliarNivel_1 (sensores);
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


pair<int, int> ComportamientoAuxiliar::aCoordenadas(const Sensores& sensores, int casillaRelativa)
{
  	// Relativo al agente.
	int numDelante;				// 1 casilla hacia delante del agente indica la variación de fila/columna
	int numDcha;				// 1 casilla hacia la derecha del agente indica la variación de fila/columna
	bool rumboHorizontal; 		// True si la rumbo es horizontal

	// Exclusivo para las casillas de los lateras
	// De mi izquierda a mi derecha: -1,-2,-3=YO, -4,-5
	// -1->-2, -2->-1, -3=0, -4->1, -5->2
	map<int, int> mapaCasillasLaterales = {
		{-1, -2},
		{-2, -1},
		{-3, 0},
		{-4, 1},
		{-5, 2}
	};


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

	int fila;		// Fila de la casilla. Relativa al mapa
	int columna;	// Columna de la casilla. Relativa al mapa
	int varDelante;	// Variación hacia delante. Relativa al agente y su rumbo
	int varDcha;	// Variación hacia la derecha. Relativa al agente su rumbo

	int diffDelante;	// Diferencia de la casilla hacia delante. Con respecto al agente
	int diffDcha;		// Diferencia de la casilla hacia la derecha. Con respecto al agente
	if (mapaCasillasLaterales.find(casillaRelativa) != mapaCasillasLaterales.end()){
		diffDelante = 0;
		diffDcha = mapaCasillasLaterales[casillaRelativa];
	}else{
		diffDelante = floor(sqrt(casillaRelativa));
		int casillaCentralFila = diffDelante*diffDelante + diffDelante;
		diffDcha = casillaRelativa - casillaCentralFila;
	}


	switch (sensores.rumbo)
	{
	case norte:
	case sur:
	case este:
	case oeste:
		varDelante 	= numDelante * diffDelante;	
		varDcha 	= numDcha * diffDcha;				
		if (rumboHorizontal){
			fila 	= sensores.posF + varDcha;
			columna = sensores.posC + varDelante;
		}else{
			fila 	= sensores.posF + varDelante;
			columna = sensores.posC + varDcha;
		}
		break;
	
	case noreste:
	case sureste:
	case suroeste:
	case noroeste:

		if (diffDcha <= 0){
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
		}else{
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
		}
		break;
	}

	return make_pair(fila, columna);
}



void ComportamientoAuxiliar::situarSensorEnMapa(
	vector<vector<unsigned char>> &mResultado, 
	vector<vector<unsigned char>> &mCotas,
	vector<vector<unsigned char>> &mEntidades,
	Sensores sensores)
{	
	// Número de casillas hacia delante que ve el agente (incluyendo la suya misma)
	const int PROFUNDIDAD_SENSOR = 4;
	
	int num_casillas = PROFUNDIDAD_SENSOR*PROFUNDIDAD_SENSOR;
	for (int i=0; i<num_casillas; ++i){
		int fila, columna;
		tie(fila, columna) = aCoordenadas(sensores, i);
		
		mResultado	[fila][columna] = sensores.superficie[i];
		mCotas		[fila][columna] = sensores.cota[i];
		mEntidades	[fila][columna] = sensores.agentes[i];
	}
}

bool ComportamientoAuxiliar::casillaAccesible(const Sensores & sensores, int casilla)
{
	// Comprobamos si la casilla es accesible o no. Será accesible si:
	// 1. No es un precipicio
	// 2. La diferencia de altura entre la casilla y la cota del rescatador es <= 1
	int fil, col;
	tie(fil, col) = aCoordenadas(sensores, casilla);

	bool accesible = mapaEntidades[fil][col] == '_'; // No hay agentes en la casilla

	
	int dif = abs(mapaCotas[sensores.posF][sensores.posC] - mapaCotas[fil][col]);
	accesible &= mapaResultado[fil][col] != 'P' && (dif<=1);

	return accesible;
}


int ComportamientoAuxiliar::veoCasillaInteresante(const Sensores & sensores){

	// Tipos de casillas permitidas
	vector<char> tiposCasillasPermitidas = {'X', 'D', 'C'}; // X: puesto base, D: zapatillas, C: camino
	if (sensores.nivel == 1)
		tiposCasillasPermitidas.push_back('S');

	// Obtenemos las casillas accesibles ***SOLO POR CAMINOS***
	// Casilla Alcanzable: Casilla que puedo alcanzar (posiblemente no pueda acceder)
	// Casilla Accesible: Casilla que puedo alcanzar y es accesible

	// El orden de las casillas es relevante para hacer el mínimo número de pasos. Lo buscado: 6 8 4 2 3 1 -4 -5 -2 -4
	const vector<int> casillasAlcanzables	= {2,3,1, -4, -2};
	vector<int> casillasAccesibles;
	int fil,col;


	for (auto i = casillasAlcanzables.begin(); i != casillasAlcanzables.end(); ++i){
		if (casillaAccesible(sensores, *i)){
			tie(fil,col)=aCoordenadas(sensores, *i);
			bool permitida = find(tiposCasillasPermitidas.begin(), tiposCasillasPermitidas.end(), mapaResultado[fil][col]) != tiposCasillasPermitidas.end();
			if (permitida)
				casillasAccesibles.push_back(*i);
		}
	}

	#ifdef DEBUG
	cout << "Casillas accesibles: " << endl;
	for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end(); ++i){
		tie(fil,col)=aCoordenadas(sensores, *i);
		cout << "(" << fil << "," << col << ") " << mapaResultado[fil][col] << " " << *i << endl;
	}
	#endif



	// Buscamos la casilla más interesante a la que ir
	int res = 0;	// Por defecto no veo nada interesante

	// Buscamos primero el puesto base
	if (sensores.nivel == 0){
		for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end() && res == 0; ++i){
			tie(fil, col) = aCoordenadas(sensores, *i);
			if (mapaResultado[fil][col] == 'X')
				res = *i;
		}
	}
	
	// Si no hay puesto base, miro si hay zapatillas
	if (!tieneZapatillas){
		for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end() && res == 0; ++i){
			tie(fil, col) = aCoordenadas(sensores, *i);
			if (mapaResultado[fil][col] == 'D')
				res = *i;
		}
	}
			
	
	// Ahora, buscamos caminos. Pero tenemos que ver cuál es el más interesante
	// Criterio: menor número de veces visitada. A iguadad, nos quedamos con la primera
	if (res==0 && !casillasAccesibles.empty()){
		int fil, col;
		int minVecesVisitada; 	// Número mínimo (hasta ahora) de veces visitada

		// Minimo por el momento. La primera casilla accesible que sea CAMINO.
		// No debo comprobar que sea accesible, ya lo he hecho antes
		// No debo comprobar que sea un camino, porque no hay ni zapatillas ni puesto base
		tie(fil, col) = aCoordenadas(sensores, casillasAccesibles[0]);
		minVecesVisitada = numVecesVisitada[fil][col];
		res = casillasAccesibles[0];

		// Buscamos la casilla con menor número de veces visitada
		for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end(); ++i){
			tie(fil, col) = aCoordenadas(sensores, *i);
			if (numVecesVisitada[fil][col] < minVecesVisitada){
				minVecesVisitada = numVecesVisitada[fil][col];
				res = *i;
			}
		}
	}

	#ifdef DEBUG
	cout << "Casilla más interesante: " << res << endl;
	#endif
	
	return res;
}


Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_0(Sensores sensores)
{
	// El comportamiento de seguir un camino hasta encontrar un puesto base.
	Action action = IDLE;	// Acción por defecto.

	// Actualización de variables de estado.
	situarSensorEnMapa(mapaResultado, mapaCotas, mapaEntidades, sensores);
	if (sensores.superficie[0] == 'D')
		tieneZapatillas = true;
	numVecesVisitada[sensores.posF][sensores.posC]++;
	

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
			case -2: // TURN_L
				action = Action::TURN_SR;
				num_TURN_SR_Restantes = 5; // 6 a realizar. -1 pq ya realiza uno en esta iteracion
				break;
			case -4: // TURN_R
				action = Action::TURN_SR;
				num_TURN_SR_Restantes = 1; // 2 a realizar. -1 pq ya realiza uno en esta iteracion
				break;
			default: // No veo nada interesante, TURN_R
				action = Action::TURN_SR;
				num_TURN_SR_Restantes = 1; // 2 a realizar. -1 pq ya realiza uno en esta iteracion
				break;
		}

	}

	lastAction = action;	// Actualizamos la última acción realizada
	return action;
}

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_1(Sensores sensores)
{
	// El comportamiento de seguir un camino hasta encontrar un puesto base.
	Action action = IDLE;	// Acción por defecto.

	// Actualización de variables de estado.
	situarSensorEnMapa(mapaResultado, mapaCotas, mapaEntidades, sensores);
	if (sensores.superficie[0] == 'D')
		tieneZapatillas = true;
	numVecesVisitada[sensores.posF][sensores.posC]++;
	

	// Definición del comportamiento
	if (num_TURN_SR_Restantes > 0){	// Está en su giro SL
		action = Action::TURN_SR;
		--num_TURN_SR_Restantes;
	}
	else{ // Vamos a realizar el movimiento determinado por lo que veo

		int res = veoCasillaInteresante(sensores);
		switch (res){
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
			case -2: // TURN_L
				action = Action::TURN_SR;
				num_TURN_SR_Restantes = 5; // 6 a realizar. -1 pq ya realiza uno en esta iteracion
				break;
			case -4: // TURN_R
				action = Action::TURN_SR;
				num_TURN_SR_Restantes = 1; // 2 a realizar. -1 pq ya realiza uno en esta iteracion
				break;
			default: // No veo nada interesante, TURN_R
				action = Action::TURN_SR;
				num_TURN_SR_Restantes = 1; // 2 a realizar. -1 pq ya realiza uno en esta iteracion
				break;
		}

	}

	lastAction = action;	// Actualizamos la última acción realizada
	return action;
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