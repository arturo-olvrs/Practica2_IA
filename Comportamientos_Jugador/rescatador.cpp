#include "../Comportamientos_Jugador/rescatador.hpp"
#include "motorlib/util.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_set>
#include <list>
#include <set>
#include <queue>

#include <iostream>

// Inicializamos el conjunto de casillas no transitables
const unordered_set<char> ComportamientoRescatador::CASILLAS_NO_TRANSITABLES = {'P', 'M', 'B'};




Action ComportamientoRescatador::think(Sensores sensores)
{
	Action accion = IDLE;

	switch (sensores.nivel)
	{
	case 0:
		accion = ComportamientoRescatadorNivel_0 (sensores);
		break;
	case 1:
		accion = ComportamientoRescatadorNivel_1 (sensores);
		break;
	case 2:
		accion = ComportamientoRescatadorNivel_2 (sensores);
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




pair<int, int> ComportamientoRescatador::aCoordenadas(int filAgente, int colAgente, Orientacion orientacion, int casillaRelativa)
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


	switch (orientacion)
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

	int fil;		// Fila de la casilla. Relativa al mapa
	int col;	// Columna de la casilla. Relativa al mapa
	int varDelante;	// Variación hacia delante. Relativa al agente y su rumbo
	int varDcha;	// Variación hacia la derecha. Relativa al agente su rumbo

	int diffDelante;	// Diferencia de la casilla hacia delante. Con respecto al agente
	int diffDcha;		// Diferencia de la casilla hacia la derecha. Con respecto al agente
	if (mapaCasillasLaterales.find(casillaRelativa) != mapaCasillasLaterales.end()){
		diffDelante = 0;
		diffDcha = mapaCasillasLaterales.at(casillaRelativa);
	}else{
		diffDelante = floor(sqrt(casillaRelativa));
		int casillaCentralFila = diffDelante*diffDelante + diffDelante;
		diffDcha = casillaRelativa - casillaCentralFila;
	}


	switch (orientacion)
	{
	case norte:
	case sur:
	case este:
	case oeste:
		varDelante 	= numDelante * diffDelante;	
		varDcha 	= numDcha * diffDcha;				
		if (rumboHorizontal){
			fil 	= filAgente+ varDcha;
			col = colAgente + varDelante;
		}else{
			fil 	= filAgente + varDelante;
			col = colAgente + varDcha;
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
				if (diffDelante == 0)
					varDelante = -varDcha;
				fil 		= filAgente + varDcha;
				col = colAgente + varDelante;
			}else{
				varDelante 	= numDelante * diffDelante;
				varDcha 	= numDcha * (diffDcha+diffDelante);
				if (diffDelante == 0)
					varDelante = varDcha;
				fil 		= filAgente + varDelante;
				col = colAgente + varDcha;
			}
		}else{
			if (rumboHorizontal){
				varDelante 	= numDelante * diffDelante;
				varDcha 	= numDcha * (diffDelante-diffDcha);
				if (diffDelante == 0)
					varDelante = -varDcha;
				fil 		= filAgente + varDelante;
				col = colAgente + varDcha;
			}else{
				varDcha		= numDelante * (diffDelante-diffDcha);
				varDelante 	= numDcha * diffDelante;
				if (diffDelante == 0)
					varDelante = varDcha;
				fil 		= filAgente + varDcha;
				col = colAgente + varDelante;
			}
		}
		break;
	}

	return make_pair(fil, col);
}



void ComportamientoRescatador::situarSensorEnMapa(
	vector<vector<unsigned char>> &mResultado, 
	vector<vector<unsigned char>> &mCotas,
	vector<vector<unsigned char>> &mEntidades,
	Sensores sensores)
{	
	for (int i=0; i<NUM_CASILLAS; ++i){
		int fil, col;
		tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, i);
		
		mResultado.at(fil).at(col) = sensores.superficie.at(i);
		mCotas.at(fil).at(col) = sensores.cota.at(i);
		mEntidades.at(fil).at(col) = sensores.agentes.at(i);
	}
}

void ComportamientoRescatador::reinicializarVeces_VistaVisitada(){
	// Reinicializamos el mapa de veces visitadas
	for (int i=0; i<mapaResultado.size(); ++i){
		for (int j=0; j<mapaResultado.at(i).size(); ++j){
			numVecesVisitada.at(i).at(j) = 0;
			numVecesVista.at(i).at(j) = 0;
		}
	}
}

void ComportamientoRescatador::actualizarMatrices_VistasVisitadas(const Sensores& sensores){
	numVecesVisitada.at(sensores.posF).at(sensores.posC)++;
	for (int i=0; i<NUM_CASILLAS; ++i){
		int fil, col;
		tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, i);
		numVecesVista.at(fil).at(col)++;
	}
}

bool ComportamientoRescatador::casillaAccesible(const Estado& estado, int casilla)
{
	return casillaAccesible(estado.fil, estado.col, estado.orientacion, estado.tieneZapatillas, false, casilla);
}

bool ComportamientoRescatador::casillaAccesible(const Sensores& sensores, int casilla)
{
	return casillaAccesible(sensores.posF, sensores.posC, sensores.rumbo, tieneZapatillas, true, casilla);
}

bool ComportamientoRescatador::casillaAccesible(int filAgente, int colAgente, Orientacion orientacion, bool conZapatillas, bool comprobarAgentes, int casilla)
{
	// Comprobamos si la casilla es accesible o no. Será accesible si:
	// 1. No es un precipicio
	// 2. La diferencia de altura entre la casilla y la cota del rescatador es <= 1
	// 3. Si el rescatador tiene zapatillas, la diferencia de altura puede ser <= 2
	int fil, col;
	tie(fil, col) = aCoordenadas(filAgente, colAgente, orientacion, casilla);

	bool accesible = true;
	
	if (comprobarAgentes)
		// Esta línea se asegura de que alguna vez la ha visto
		accesible &= mapaEntidades.at(fil).at(col) == '_'; // No hay agentes en la casilla
	
	// Comprobamos que es transitable
	accesible &= CASILLAS_NO_TRANSITABLES.find(mapaResultado.at(fil).at(col)) == CASILLAS_NO_TRANSITABLES.end();
	
	int dif = abs(mapaCotas.at(filAgente).at(colAgente) - mapaCotas.at(fil).at(col));
	accesible &= (dif<=1 || (conZapatillas && dif <=2));

	return accesible;
}

int ComportamientoRescatador::veoCasillaInteresante(const Sensores & sensores){

	// Tipos de casillas permitidas
	vector<char> tiposCasillasPermitidas = {'X', 'D', 'C'}; // X: puesto base, D: zapatillas, C: camino
	if (sensores.nivel == 1)
		tiposCasillasPermitidas.push_back('S');

	// Obtenemos las casillas accesibles ***SOLO POR CAMINOS***
	// Casilla Alcanzable: Casilla que puedo alcanzar (posiblemente no pueda acceder)
	// Casilla Accesible: Casilla que puedo alcanzar y es accesible
	// Casilla Alcanzable Directa: Casilla que puedo alcanzar directamente
	// Casilla Alcanzable Indirecta: Casilla que puedo alcanzar indirectamente, he de comprobar que la casilla intermedia es accesible

	// El orden de las casillas es relevante para hacer el mínimo número de pasos. Lo buscado: 6 8 4 2 3 1 -4 -5 -2 -4
	const vector<int> casillasAlcanzablesDirectas 	= {2,3,1};
	const vector<int> casillasAlcanzablesIndirectas = {6,8,4};
	const vector<int> casillasAlcanzablesDirectas_Atras 	= {-2,-4};
	const vector<int> casillasAlcanzablesIndirectas_Atras 	= {-1,-5};
	vector<int> casillasAccesibles;
	int fil,col;


	// ------------ Casillas de Delante ----------------
	for (int i=0; i<casillasAlcanzablesIndirectas.size(); i++){
		// Para llegar a casillasAlcanzablesIndirectas.at(i), ha de pasar por casillasAccesibleDirectas.at(i)
		int casillaIntermedia 	= casillasAlcanzablesDirectas.at(i);
		int casillaFinal 		= casillasAlcanzablesIndirectas.at(i);
		if (casillaAccesible(sensores, casillaIntermedia) && casillaAccesible(sensores, casillaFinal)){

			tie(fil,col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, i);
			bool finalPermitida =	find(tiposCasillasPermitidas.begin(), tiposCasillasPermitidas.end(), mapaResultado.at(fil).at(col)) != tiposCasillasPermitidas.end();

			// La intermedia no se pisa.
			if (finalPermitida)
				casillasAccesibles.push_back(casillaFinal);
		}
	}
	for (auto i = casillasAlcanzablesDirectas.begin(); i != casillasAlcanzablesDirectas.end(); ++i){
		if (casillaAccesible(sensores, *i)){
			tie(fil,col)=aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
			bool permitida = find(tiposCasillasPermitidas.begin(), tiposCasillasPermitidas.end(), mapaResultado.at(fil).at(col)) != tiposCasillasPermitidas.end();
			if (permitida)
				casillasAccesibles.push_back(*i);
		}
	}



	// ------------ Casillas de Detrás ----------------
	for (int i=0; i<casillasAlcanzablesIndirectas_Atras.size(); i++){
		// Para llegar a casillasAlcanzablesIndirectas.at(i), ha de pasar por casillasAccesibleDirectas.at(i)
		int casillaIntermedia 	= casillasAlcanzablesDirectas_Atras.at(i);
		int casillaFinal 		= casillasAlcanzablesIndirectas_Atras.at(i);
		if (casillaAccesible(sensores, casillaIntermedia) && casillaAccesible(sensores, casillaFinal)){

			tie(fil,col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, i);
			bool finalPermitida =	find(tiposCasillasPermitidas.begin(), tiposCasillasPermitidas.end(), mapaResultado.at(fil).at(col)) != tiposCasillasPermitidas.end();

			// La intermedia no se pisa.
			if (finalPermitida)
				casillasAccesibles.push_back(casillaFinal);
		}
	}
	for (auto i = casillasAlcanzablesDirectas_Atras.begin(); i != casillasAlcanzablesDirectas_Atras.end(); ++i){
		if (casillaAccesible(sensores, *i)){
			tie(fil,col)=aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
			bool permitida = find(tiposCasillasPermitidas.begin(), tiposCasillasPermitidas.end(), mapaResultado.at(fil).at(col)) != tiposCasillasPermitidas.end();
			if (permitida)
				casillasAccesibles.push_back(*i);
		}
	}

	#ifdef DEBUG_RESC
	cout << "Casillas accesibles: " << endl;
	for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end(); ++i){
		tie(fil,col)=aCoordenadas(sensores, *i);
		cout << "(" << fil << "," << col << ") " << mapaResultado.at(fil).at(col) << " " << *i << endl;
	}
	#endif



	// Buscamos la casilla más interesante a la que ir
	int res = 0;	// Por defecto no veo nada interesante

	// Buscamos primero el puesto base
	if (sensores.nivel == 0){
		for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end() && res == 0; ++i){
			tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
			if (mapaResultado.at(fil).at(col) == 'X')
				res = *i;
		}
	}
	
	// Si no hay puesto base, miro si hay zapatillas
	if (!tieneZapatillas){
		for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end() && res == 0; ++i){
			tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
			if (mapaResultado.at(fil).at(col) == 'D')
				res = *i;
		}
	}
			
	
	// Ahora, buscamos caminos. Pero tenemos que ver cuál es el más interesante
	// Criterio: menor valor de: PESO_VISTA * numVecesVista + PESO_VISITADA * numVecesVisitada
	if (res==0 && !casillasAccesibles.empty()){
		int fil, col;
		double minPuntuacion;

		// Minimo por el momento. La primera casilla accesible que sea CAMINO.
		// No debo comprobar que sea accesible, ya lo he hecho antes
		// No debo comprobar que sea un camino, porque no hay ni zapatillas ni puesto base
		const double PESO_VISTA = 0.6; 		// Peso de la vista.
		const double PESO_VISITADA = 1; 	// Peso de la visitada


		tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, casillasAccesibles.at(0));
		minPuntuacion = PESO_VISTA * numVecesVista.at(fil).at(col) + PESO_VISITADA * numVecesVisitada.at(fil).at(col);
		res = casillasAccesibles.at(0);

		// Buscamos la casilla con menor número de veces visitada
		for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end(); ++i){
			tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
			double puntuacion = PESO_VISTA * numVecesVista.at(fil).at(col) + PESO_VISITADA * numVecesVisitada.at(fil).at(col);
			#ifdef DEBUG_RESC
			cout << "Casilla: " << *i << endl;
			cout << "\t - fila: " << fil << endl;
			cout << "\t - col: " << col << endl;
			cout << "\t - numVecesVista: " << numVecesVista.at(fil).at(col) << endl;
			cout << "\t - numVecesVisitada: " << numVecesVisitada.at(fil).at(col) << endl;
			cout << "\t - puntuacion: " << puntuacion << endl;
			#endif
			if (puntuacion < minPuntuacion){
				minPuntuacion = puntuacion;
				res = *i;
			}
		}
	}

	#ifdef DEBUG_RESC
	cout << "Casilla más interesante: " << res << endl;
	#endif

	return res;
}


Action ComportamientoRescatador::ComportamientoRescatadorNivel_0(Sensores sensores)
{

	// El comportamiento de seguir un camino hasta encontrar un puesto base.

	Action action = IDLE;	// Acción por defecto.

	// Actualización de variables de estado.
	situarSensorEnMapa(mapaResultado, mapaCotas, mapaEntidades, sensores);
	if (!tieneZapatillas && sensores.superficie.at(0) == 'D'){
		tieneZapatillas = true;
		reinicializarVeces_VistaVisitada();
	}
	if (lastAction == Action::RUN || lastAction == Action::WALK)
		actualizarMatrices_VistasVisitadas(sensores);
	

	// Definición del comportamiento
	if (sensores.superficie.at(0) == 'X')	// Llegó al puesto base
		action = Action::IDLE;
	else if (inTURN_SL){	// Está en su giro SL
		action = Action::TURN_SR;
		inTURN_SL = false;
	}
	else if (inTURN_R){		// Está en su giro R
		action = Action::TURN_SR;
		inTURN_R = false;
	}
	else{ // Vamos a realizar el movimiento determinado por lo que veo
		int res = veoCasillaInteresante(sensores);
		switch (res){
			case 1: // TURN_SL
			case 4:
				inTURN_SL = true;
				action = Action::TURN_L;
				break;
			case 2: // WALK
				action = Action::WALK;
				break;
			case 3: // TURN_SR
			case 8:
				action = Action::TURN_SR;
				break;
			case 6: // RUN
				action = Action::RUN;
				break;
			case -1: // TURN_L
			case -2:
				action = Action::TURN_L;
				break;
			case -4:
			case -5:
				inTURN_R = true;
				action = Action::TURN_SR;
				break;
			default: // No veo nada interesante
				action = Action::TURN_L;
				break;
		}

	}

	lastAction = action;	// Actualizamos la última acción realizada
	return action;
}

Action ComportamientoRescatador::ComportamientoRescatadorNivel_1(Sensores sensores)
{
	// El comportamiento de seguir un camino hasta encontrar un puesto base.

	Action action = IDLE;	// Acción por defecto.

	// Actualización de variables de estado.
	situarSensorEnMapa(mapaResultado, mapaCotas, mapaEntidades, sensores);
	if (!tieneZapatillas && sensores.superficie.at(0) == 'D'){
		tieneZapatillas = true;
		reinicializarVeces_VistaVisitada();
	}
	if (lastAction == Action::RUN || lastAction == Action::WALK)
		actualizarMatrices_VistasVisitadas(sensores);
	

	// Definición del comportamiento
	if (inTURN_SL){	// Está en su giro SL
		action = Action::TURN_SR;
		inTURN_SL = false;
	}
	else if (inTURN_R){		// Está en su giro R
		action = Action::TURN_SR;
		inTURN_R = false;
	}
	else{ // Vamos a realizar el movimiento determinado por lo que veo
		int res = veoCasillaInteresante(sensores);
		switch (res){
			case 1: // TURN_SL
			case 4:
				inTURN_SL = true;
				action = Action::TURN_L;
				break;
			case 2: // WALK
				action = Action::WALK;
				break;
			case 3: // TURN_SR
			case 8:
				action = Action::TURN_SR;
				break;
			case 6: // RUN
				action = Action::RUN;
				break;
			case -1: // TURN_L
			case -2:
				action = Action::TURN_L;
				break;
			case -4:
			case -5:
				inTURN_R = true;
				action = Action::TURN_SR;
				break;
			default: // No veo nada interesante
				action = Action::TURN_L;
				break;
		}

	}

	lastAction = action;	// Actualizamos la última acción realizada
	return action;
}

// -----------------------------------------------------------------------------

Estado ComportamientoRescatador::ejecutarAccion(Action action, const Estado & inicio){

	Estado nuevoEstado = inicio;
	switch (action){
		case Action::RUN:
			if (casillaAccesible(inicio, 2) && casillaAccesible(inicio, 6))
				tie(nuevoEstado.fil, nuevoEstado.col) = aCoordenadas(inicio.fil, inicio.col, inicio.orientacion, 6);
			break;
		case Action::TURN_SR:
			nuevoEstado.orientacion = (Orientacion)(((int)inicio.orientacion + 1) % 8);
			break;
		case Action::TURN_L:
			nuevoEstado.orientacion = (Orientacion)((8+(int)inicio.orientacion - 2) % 8);
			break;
		case Action::WALK:
			if (casillaAccesible(inicio, 2))
				tie(nuevoEstado.fil, nuevoEstado.col) = aCoordenadas(inicio.fil, inicio.col, inicio.orientacion, 2);
			break;
	}
	return nuevoEstado;
}



void ComportamientoRescatador::Dijkstra(const Estado& origen, 
	vector<vector<vector<int>>> &gastoEnergia, 
	vector<vector<vector<Predecesor>>> &predecesores)
{

	// Acciones posibles del agente
	const vector<Action> acciones = {Action::RUN, Action::WALK, Action::TURN_SR, Action::TURN_L};

	const int INVALID = -1;	// Como no hay un valor máximo, ponemos un valor imposible
	
	// Inicializamos las matrices de Dijkstra
	gastoEnergia.clear();
	gastoEnergia.resize(mapaResultado.size(), vector<vector<int>>(mapaResultado.at(0).size(), vector<int>(8, INVALID)));


	predecesores.clear();
	predecesores.resize(mapaResultado.size(), vector<vector<Predecesor>>(mapaResultado.at(0).size(), vector<Predecesor>(8, {INVALID, INVALID, INVALID, {}})));


	// Inicializamos el nodo origen
	gastoEnergia.at(origen.fil).at(origen.col).at(origen.orientacion) = 0;	// Gasto de energía al nodo origen
	predecesores.at(origen.fil).at(origen.col).at(origen.orientacion) = {origen.fil, origen.col, origen.orientacion, {}};


	// Cola con prioridad formada por Nodos. Ordenada por el gasto de Energia al nodo Origen
	priority_queue<Nodo, vector<Nodo>, greater<>> frontera;

	// Nodos ya visitados, para evitar repeticiones
	set<Estado> visitados;


	Nodo nodoActual = {origen, 0, {}};	// Nodo origen
	frontera.push(nodoActual);
	while(!frontera.empty()){
		nodoActual = frontera.top();
		frontera.pop();
		
		if (visitados.insert(nodoActual.estado).second){

			// Comprobamos si hemos conseguido Zapatillas
			if (mapaResultado.at(nodoActual.estado.fil).at(nodoActual.estado.col) == 'D')
				nodoActual.estado.tieneZapatillas = true;

			// Exploramos los nodos resultantes de aplicar cada una de las acciones
			Action accion;
			Nodo nuevoNodo;

			#ifdef DEBUG_RESC_DIJK
			cout << "-------------------------------------------------------"
				<< "\nEstado actual: " << nodoActual.estado.fil << "," << nodoActual.estado.col << " "
				<< "Orientacion: " << nodoActual.estado.orientacion << endl;
			#endif


			for (auto it = acciones.begin(); it != acciones.end(); ++it){
				accion = *it;
				nuevoNodo.estado = ejecutarAccion(accion, nodoActual.estado);
				if (visitados.find(nuevoNodo.estado) == visitados.end()){

					#ifdef DEBUG_RESC_DIJK
					cout << "Nodo Expandido " << nuevoNodo.estado.fil << "," << nuevoNodo.estado.col << " "
						<< "Orientacion: " << nuevoNodo.estado.orientacion << "(" << accion << ")" << endl;
					#endif
					// No se ha explorado, hay que explorarlo.


					// Modificamos el gasto de energía del nuevo nodo
					nuevoNodo.gastoEnergia = nodoActual.gastoEnergia + 1;	// TODO: Cambiar. Por ahora, cada acción gasta 1 de energía
					nuevoNodo.acciones = nodoActual.acciones;
					nuevoNodo.acciones.push_back(accion);
					
					if (gastoEnergia.at(nuevoNodo.estado.fil).at(nuevoNodo.estado.col).at(nuevoNodo.estado.orientacion) == INVALID
						|| nuevoNodo.gastoEnergia < gastoEnergia.at(nuevoNodo.estado.fil).at(nuevoNodo.estado.col).at(nuevoNodo.estado.orientacion)){

						// Actualizamos las matrices de Dijkstra
						gastoEnergia.at(nuevoNodo.estado.fil).at(nuevoNodo.estado.col).at(nuevoNodo.estado.orientacion) = nuevoNodo.gastoEnergia;
						predecesores.at(nuevoNodo.estado.fil).at(nuevoNodo.estado.col).at(nuevoNodo.estado.orientacion).fil = nodoActual.estado.fil;
						predecesores.at(nuevoNodo.estado.fil).at(nuevoNodo.estado.col).at(nuevoNodo.estado.orientacion).col = nodoActual.estado.col;
						predecesores.at(nuevoNodo.estado.fil).at(nuevoNodo.estado.col).at(nuevoNodo.estado.orientacion).orientacion = nodoActual.estado.orientacion;
						predecesores.at(nuevoNodo.estado.fil).at(nuevoNodo.estado.col).at(nuevoNodo.estado.orientacion).acciones = nuevoNodo.acciones;

						cout << "Actualizamos!!!" << endl;
						#ifdef DEBUG_RESC_DIJK
						cout << "Gasto energia: " << nuevoNodo.gastoEnergia << endl;
						cout << "Predecesor: " << nodoActual.estado.fil << "," << nodoActual.estado.col << " "
							<< "Orientacion: " << nodoActual.estado.orientacion << endl;
						cout << "Acciones: ";
						for (auto it = nuevoNodo.acciones.begin(); it != nuevoNodo.acciones.end(); ++it)
							cout << *it << " ";
						cout << endl;
						#endif

						// Reseteamos las acciones; ya han quedado grabadas
						nuevoNodo.acciones.clear();
					}



					// Añadimos el nuevo nodo a la frontera (podrá estar ya)
					frontera.push(nuevoNodo);
				} 
			} // for accion
		} // if visitados.insert
	} // while frontera

}

void ComportamientoRescatador::VisualizaPlan(const Estado& origen, const list<Action>& plan){
	
	mapaConPlan.clear();
	mapaConPlan.resize(mapaResultado.size(), vector<unsigned char>(mapaResultado.at(0).size(), 0));

	Estado estadoActual = origen;
	for(auto it = plan.begin(); it != plan.end(); ++it){

		estadoActual = ejecutarAccion(*it, estadoActual);
		switch (*it){
			case Action::RUN:
				mapaConPlan.at(estadoActual.fil).at(estadoActual.col) = 3;
				break;
			case Action::WALK:
				mapaConPlan.at(estadoActual.fil).at(estadoActual.col) = 1;
				break;
		}
	}
}



Action ComportamientoRescatador::ComportamientoRescatadorNivel_2(Sensores sensores)
{
	Action accion= IDLE;
	if (plan.empty() && (sensores.posF != sensores.destinoF || sensores.posC != sensores.destinoC)){


		// Inicializamos el nodo origen
		Estado origen;
		origen.fil = sensores.posF;
		origen.col = sensores.posC;
		origen.orientacion = sensores.rumbo;
		origen.tieneZapatillas = tieneZapatillas;

		// Inicializamos las matrices de Dijkstra
		vector<vector<vector<int>>> gastoEnergia;
		vector<vector<vector<Predecesor>>> predecesores;
		Dijkstra(origen, gastoEnergia, predecesores);


		int filObjetivo = sensores.destinoF;
		int colObjetivo = sensores.destinoC;
		int orientacionObjetivo;

		// Para saber la direcciób objetivo, de entre las 8 posibles nos quedamos la que tenga menor gasto de energía
		int minEnergia = gastoEnergia.at(filObjetivo).at(colObjetivo).at(0);
		orientacionObjetivo = (Orientacion)0;
		for (int i=1; i<8; ++i){
			if (gastoEnergia.at(filObjetivo).at(colObjetivo).at(i) < minEnergia){
				minEnergia = gastoEnergia.at(filObjetivo).at(colObjetivo).at(i);
				orientacionObjetivo= (Orientacion)i;
			}
		}

		cout << "Estado origen: " << origen.fil << "," << origen.col << " "
			<< "Orientacion: " << origen.orientacion << endl;
		cout << "Estado objetivo: " << filObjetivo << "," << colObjetivo << " "
			<< "Orientacion: " << orientacionObjetivo << endl;
		cout << "Gasto energia: " << minEnergia << endl;
	


		// Vamos añadiendo hasta que encontremos la que solo tiene IDLE
		while (filObjetivo != origen.fil || colObjetivo != origen.col){
			plan.insert(plan.begin(),
						predecesores.at(filObjetivo).at(colObjetivo).at(orientacionObjetivo).acciones.begin(),
						predecesores.at(filObjetivo).at(colObjetivo).at(orientacionObjetivo).acciones.end());

			int filPredecesor = predecesores.at(filObjetivo).at(colObjetivo).at(orientacionObjetivo).fil;
			int colPredecesor = predecesores.at(filObjetivo).at(colObjetivo).at(orientacionObjetivo).col;
			int orientacionPredecesor = predecesores.at(filObjetivo).at(colObjetivo).at(orientacionObjetivo).orientacion;

			#define DEBUG_RESC_DIJKSTRA 0
			#ifdef DEBUG_RESC_DIJKSTRA
			cout << "Casilla " << filObjetivo << "," << colObjetivo << " " << orientacionObjetivo << " "
				<< "Gasto energia: " << gastoEnergia.at(filObjetivo).at(colObjetivo).at(orientacionObjetivo) << endl;
			cout << "Predecesor: " << filPredecesor << "," << colPredecesor << " "
				<< "Orientacion: " << orientacionPredecesor << endl;
			cout << "Acciones: ";
			for (auto it = predecesores.at(filObjetivo).at(colObjetivo).at(orientacionObjetivo).acciones.begin(); it != predecesores.at(filObjetivo).at(colObjetivo).at(orientacionObjetivo).acciones.end(); ++it){
				cout << *it << " ";
			}
			cout << endl;
			cout << "------------------------" << endl;
			#endif

			filObjetivo = filPredecesor;
			colObjetivo = colPredecesor;
			orientacionObjetivo = orientacionPredecesor;

		}
		cout << "Plan encontrado " << plan.size() << endl;
		// Imprimimos el plan
		for (auto it = plan.begin(); it != plan.end(); ++it){
			cout << *it << " ";
		}
		cout << endl;

		VisualizaPlan(origen, plan);
	}

	else if (!plan.empty()){
		accion = plan.front();
		plan.pop_front();
	}

	return accion;
}

Action ComportamientoRescatador::ComportamientoRescatadorNivel_3(Sensores sensores)
{
}

Action ComportamientoRescatador::ComportamientoRescatadorNivel_4(Sensores sensores)
{
}