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

			tie(fil,col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, casillaFinal);
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

			tie(fil,col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, casillaFinal);
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
		tie(fil,col)=aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
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



list<Action> ComportamientoRescatador::Dijkstra(
	const Estado &origen,
	int filDestino,
	int colDestino)
{

	// Acciones posibles del agente
	const vector<Action> acciones = {Action::RUN, Action::WALK, Action::TURN_SR, Action::TURN_L};

	const int INVALID = -1;	// Como no hay un valor máximo, ponemos un valor imposible
	

	// Cola con prioridad formada por Nodos. Ordenada por el gasto de Energia al nodo Origen
	struct Comparador {
		bool operator()(const Nodo& a, const Nodo& b) {
			// Retorna true si a tiene más energía que b,
			// así el de menor energía queda al tope
			return a.gastoEnergia > b.gastoEnergia;
		}
	};


	priority_queue<Nodo, vector<Nodo>, Comparador> frontera;

	// Nodos ya visitados, para evitar repeticiones
	set<Nodo> visitados;


	Nodo nodoActual = {origen, 0, {}};	// Nodo origen
	frontera.push(nodoActual);

	#ifdef DEBUG_DIJKSTRA
		int interaciones = 0;
	#endif




	bool caminoOptimoEncontrado = false;
	while(!frontera.empty() && !caminoOptimoEncontrado){

		nodoActual = frontera.top();
		frontera.pop();

		// Comprobamos si el nodo actual es el destino
		caminoOptimoEncontrado = (nodoActual.estado.fil == filDestino && nodoActual.estado.col == colDestino);
		
		if (!caminoOptimoEncontrado && visitados.insert(nodoActual).second){

			#ifdef DEBUG_DIJKSTRA
				interaciones++;
			#endif

			// Comprobamos si hemos conseguido Zapatillas
			if (mapaResultado.at(nodoActual.estado.fil).at(nodoActual.estado.col) == 'D')
				nodoActual.estado.tieneZapatillas = true;

			// Exploramos los nodos resultantes de aplicar cada una de las acciones
			Nodo nuevoNodo;


			for (auto it = acciones.begin(); it != acciones.end(); ++it){
				nuevoNodo.estado = ejecutarAccion(*it, nodoActual.estado);

				// Comprobamos que se ha generado un nodo nuevo
				if (visitados.find(nuevoNodo) == visitados.end()){

					// Modificamos el gasto de energía del nuevo nodo
					nuevoNodo.gastoEnergia = nodoActual.gastoEnergia + gastoAccion(*it, nodoActual.estado.fil, nodoActual.estado.col, nuevoNodo.estado.fil, nuevoNodo.estado.col);
					nuevoNodo.acciones = nodoActual.acciones;
					nuevoNodo.acciones.push_back(*it);
					

					// Añadimos el nuevo nodo a la frontera (podrá estar ya, pero podemos haber encontrado un camino mejor)
					frontera.push(nuevoNodo);
				} 
			} // for accion
		} // if visitados.insert
	} // while frontera

	// Si hemos encontrado el camino óptimo, lo devolvemos
	list<Action> plan = caminoOptimoEncontrado ? nodoActual.acciones : list<Action>();

	#ifdef DEBUG_DIJKSTRA
		cout << "Iteraciones: " << interaciones << endl;
		cout << "Nodos abiertos: " << frontera.size() << endl;
		cout << "Nodos cerrados: " << visitados.size() << endl;
	#endif
		
	return plan;

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


int ComportamientoRescatador::gastoAccion(Action action, int filInicio, int colInicio, int filDestino, int colDestino){
	int gasto=0;
	int difAltura = mapaCotas.at(filDestino).at(colDestino) - mapaCotas.at(filInicio).at(colInicio);
	if (difAltura > 0)
		difAltura = 1;
	else if (difAltura < 0)
		difAltura = -1;
	// difAltura == 0 ya está correcto

	switch (action){
		case Action::WALK:
			switch (mapaResultado.at(filInicio).at(colInicio)){
				case 'A':
					gasto = 100;
					gasto += difAltura * 10;
					break;
				case 'T':
					gasto = 20;
					gasto += difAltura * 5;
					break;
				case 'S':
					gasto = 2;
					gasto += difAltura * 1;
					break;
				default:
					gasto = 1;
					gasto += difAltura * 0;
					break;
			}
			break;
		case Action::RUN:
			switch (mapaResultado.at(filInicio).at(colInicio)){
				case 'A':
					gasto = 150;
					gasto += difAltura * 15;
					break;
				case 'T':
					gasto = 35;
					gasto += difAltura * 5;
					break;
				case 'S':
					gasto = 3;
					gasto += difAltura * 2;
					break;
				default:
					gasto = 1;
					gasto += difAltura * 0;
					break;
			}
			break;
		case Action::TURN_L:
			switch (mapaResultado.at(filInicio).at(colInicio)){
				case 'A':
					gasto = 30;
					break;
				case 'T':
					gasto = 5;
					break;
				case 'S':
					gasto = 1;
					break;
				default:
					gasto = 1;
					break;
			}
			break;
		case Action::TURN_SR:
			switch (mapaResultado.at(filInicio).at(colInicio)){
				case 'A':
					gasto = 16;
					break;
				case 'T':
					gasto = 3;
					break;
				case 'S':
					gasto = 1;
					break;
				default:
					gasto = 1;
					break;
			}
			break;
	}	
	
	return gasto;
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

		
		plan = Dijkstra(origen, sensores.destinoF, sensores.destinoC);

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
	Action accion= IDLE;
	return accion;
}

Action ComportamientoRescatador::ComportamientoRescatadorNivel_4(Sensores sensores)
{
	Action accion= IDLE;
	return accion;
}