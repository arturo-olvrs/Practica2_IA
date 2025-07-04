#include "../Comportamientos_Jugador/auxiliar.hpp"
#include <iostream>
#include "motorlib/util.h"
#include "math.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <unordered_set>
#include <set>
#include <queue>

const unordered_set<char> ComportamientoAuxiliar::CASILLAS_NO_TRANSITABLES = {'P', 'M', 'B'};


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
		accion = ComportamientoAuxiliarNivel_2 (sensores);
		break;
	case 3:
		accion = ComportamientoAuxiliarNivel_3 (sensores);
		break;
	case 4:
		accion = ComportamientoAuxiliarNivel_4 (sensores);
		break;
	}

	return accion;
}

int ComportamientoAuxiliar::interact(Action accion, int valor)
{
	return 0;
}


pair<int, int> ComportamientoAuxiliar::aCoordenadas(int filAgente, int colAgente, Orientacion orientacion, int casillaRelativa)
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

	#ifdef DEBUG2
	cout << "PosActual " << filAgente << "," << sensores.posC << endl;
	cout << "PosRelativa " << casillaRelativa << endl;
	cout << "Rumbo " << sensores.rumbo << endl;
	cout << "DiffDelante " << diffDelante << endl;
	cout << "DiffDcha " << diffDcha << endl;
	#endif



	switch (orientacion)
	{
	case norte:
	case sur:
	case este:
	case oeste:
		varDelante 	= numDelante * diffDelante;	
		varDcha 	= numDcha * diffDcha;				
		if (rumboHorizontal){
			fil = filAgente + varDcha;
			col = colAgente + varDelante;
		}else{
			fil = filAgente + varDelante;
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
				fil = filAgente + varDcha;
				col = colAgente + varDelante;
			}else{
				varDelante 	= numDelante * diffDelante;
				varDcha 	= numDcha * (diffDcha+diffDelante);
				if (diffDelante == 0)
					varDelante = varDcha;
				fil = filAgente + varDelante;
				col = colAgente + varDcha;
			}
		}else{
			if (rumboHorizontal){
				varDelante 	= numDelante * diffDelante;
				varDcha 	= numDcha * (diffDelante-diffDcha);
				if (diffDelante == 0)
					varDelante = -varDcha;
				fil = filAgente + varDelante;
				col = colAgente + varDcha;
			}else{
				varDcha		= numDelante * (diffDelante-diffDcha);
				varDelante 	= numDcha * diffDelante;
				if (diffDelante == 0)
					varDelante = varDcha;
				fil = filAgente + varDcha;
				col = colAgente + varDelante;
			}
		}
		break;
	}

	#ifdef DEBUG2
	cout << "PosFinal " << fil << "," << col << endl;
	cout << "------------------------" << endl;
	#endif

	return make_pair(fil, col);
}



void ComportamientoAuxiliar::situarSensorEnMapa(
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

void ComportamientoAuxiliar::reinicializarVeces_VistaVisitada()
{
	// Reinicializamos el mapa de veces visitadas
	for (int i=0; i<mapaResultado.size(); ++i){
		for (int j=0; j<mapaResultado.at(i).size(); ++j){
			numVecesVisitada.at(i).at(j) = 0;
			numVecesVista.at(i).at(j) = 0;
		}
	}
}

void ComportamientoAuxiliar::actualizarMatrices_VistasVisitadas(const Sensores& sensores){
	numVecesVisitada.at(sensores.posF).at(sensores.posC)++;
	for (int i=0; i<NUM_CASILLAS; ++i){
		int fil, col;
		tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, i);
		numVecesVista.at(fil).at(col)++;
	}
}

bool ComportamientoAuxiliar::casillaAccesible(const Sensores& sensores, int casilla)
{
	return casillaAccesible(sensores.posF, sensores.posC, sensores.rumbo, tieneZapatillas, true, casilla);
}

bool ComportamientoAuxiliar::casillaAccesible(const Estado& estado, int casilla)
{
	return casillaAccesible(estado.fil, estado.col, estado.orientacion, estado.tieneZapatillas, false, casilla);
}

bool ComportamientoAuxiliar::casillaAccesible(int filAgente, int colAgente, Orientacion orientacion, bool conZapatillas, bool comprobarAgentes, int casilla)
{
	// Comprobamos si la casilla es accesible o no. Será accesible si:
	// 1. No es un precipicio
	// 2. La diferencia de altura entre la casilla y la cota del rescatador es <= 1
	int fil, col;
	tie(fil, col) = aCoordenadas(filAgente, colAgente, orientacion, casilla);


	bool accesible = 0<= fil && fil < mapaResultado.size() && 0<=col && col<mapaResultado.at(0).size();
	if (accesible && comprobarAgentes)
		// Esta línea se asegura de que alguna vez la ha visto
		accesible &= mapaEntidades.at(fil).at(col) == '_'; // No hay agentes en la casilla
	accesible = accesible && (CASILLAS_NO_TRANSITABLES.find(mapaResultado.at(fil).at(col)) == CASILLAS_NO_TRANSITABLES.end() || (conZapatillas && mapaResultado.at(fil).at(col) == 'B'));
	if (accesible && mapaCotas.at(fil).at(col) != 0 && mapaCotas.at(filAgente).at(colAgente) != 0){
		int dif = abs(mapaCotas.at(filAgente).at(colAgente) - mapaCotas.at(fil).at(col));
		accesible &= dif<=1;
	}
	

	return accesible;
}


int ComportamientoAuxiliar::determinaEmpatadas(vector<int> &casillasEmpatadas, const Sensores & sensores){
	int distancia=0;
	int res=0;

	// Para cada distancia, hemos de almacenar la primera en borrarse por si todas están a la misma distancia
	int primera_borrada = 0;

	// Si hay alguna que es positiva, elimino las negativas (no queremos ir atrás)
	bool hayPositiva=false;
	for (auto i = casillasEmpatadas.begin(); i != casillasEmpatadas.end(); ++i){
		hayPositiva |= *i > 0;

		// Haciendo uso de que las negativas estarán detrás, las elimino
		if (hayPositiva && *i < 0){
			i = casillasEmpatadas.erase(i);
			-- i;
		}
	}

	// Si hay más de una casilla empatada, se elige la que esté más cerca de un ?
	while (res == 0 && casillasEmpatadas.size() > 1){
		primera_borrada = 0;
		distancia++;

		for (auto i = casillasEmpatadas.begin(); i != casillasEmpatadas.end()
													&& res==0; ++i){
			int fil_accion, col_accion;
			tie(fil_accion, col_accion) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);

			int nueva_fil = sensores.posF + (fil_accion - sensores.posF)/((double) abs(fil_accion - sensores.posF)) * distancia;
			int nueva_col = sensores.posC + (col_accion - sensores.posC)/((double) abs(col_accion - sensores.posC)) * distancia;

			// Si ya me salgo del mapa, borro la casilla
			if (nueva_fil < 0 || nueva_col < 0 || nueva_fil >= mapaResultado.size() || nueva_col >= mapaResultado.at(0).size()){
				
				if (primera_borrada == 0)
					primera_borrada = *i;
				i = casillasEmpatadas.erase(i);
				-- i;
			}
			// Si encuentro un ?, me quedo con esa casilla
			else if (mapaResultado.at(nueva_fil).at(nueva_col) == '?')
				res = *i;
			
		} // for
	} // while 

	// Si no he encontrado un ?, y ya no quedan, todos estaban a la misma distancia
	if (res == 0 && casillasEmpatadas.size() == 0)
		res = primera_borrada;

	// Si no he encontrado y aún quedan empatados, esa es la única opción
	else if (res == 0 && casillasEmpatadas.size() == 1)
		res = casillasEmpatadas.at(0);
	
	return res;
}


int ComportamientoAuxiliar::veoCasillaInteresante(const Sensores & sensores){

	// Tipos de casillas permitidas
	set<char> tiposCasillasPermitidas = {'X', 'D', 'C'}; // X: puesto base, D: zapatillas, C: camino
	if (sensores.nivel == 1)
		tiposCasillasPermitidas.insert('S');

	// Buscamos las casillas accesibles
	vector<int> casillasAccesibles = getCasillasAccesibles(sensores, true, tiposCasillasPermitidas);


	// Buscamos la casilla más interesante a la que ir
	int res = 0;	// Por defecto no veo nada interesante

	// Buscamos primero el puesto base
	if (sensores.nivel == 0)
		res = buscaCasilla(sensores, 'X', true, tiposCasillasPermitidas);
	
	
	// Si no hay puesto base, miro si hay zapatillas
	if (!tieneZapatillas){
		res = buscaCasilla(sensores, 'D', true, tiposCasillasPermitidas);
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
		vector<int> casillasEmpatadas;		// Casillas empatadas con la mínima puntuación
		// No se hace el push back, puesto que en algún momento se llegará ahí

		// Buscamos la casilla con menor número de veces visitada
		for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end(); ++i){
			tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
			double puntuacion = PESO_VISTA * numVecesVista.at(fil).at(col) + PESO_VISITADA * numVecesVisitada.at(fil).at(col);
			if (*i==2) puntuacion -= PESO_VISTA;		// Ventaja a avanzar recto

			#ifdef DEBUG_AUX
			cout << "Casilla: " << *i << endl;
			cout << "\t - fila: " << fil << endl;
			cout << "\t - col: " << col << endl;
			cout << "\t - numVecesVista: " << numVecesVista.at(fil).at(col) << endl;
			cout << "\t - numVecesVisitada: " << numVecesVisitada.at(fil).at(col) << endl;
			cout << "\t - puntuacion: " << puntuacion << endl;
			#endif

			if (puntuacion < minPuntuacion){
				minPuntuacion = puntuacion;
				casillasEmpatadas.clear();
				casillasEmpatadas.push_back(*i);
			}
			else if (puntuacion == minPuntuacion){
				casillasEmpatadas.push_back(*i);
			}
		} // fin for

		#ifdef DEBUG_AUX
		cout << "Casillas empatadas: " << endl;
		for (auto i = casillasEmpatadas.begin(); i != casillasEmpatadas.end(); ++i){
			tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
			cout << "- (" << fil << "," << col << ") " << mapaResultado.at(fil).at(col) << " " << *i << endl;
		}
		#endif

		// De todas las casillas empatadas, elijo la más interesante
		res = determinaEmpatadas(casillasEmpatadas, sensores);
	}

	#ifdef DEBUG_AUX
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
	if (!tieneZapatillas && sensores.superficie.at(0) == 'D'){
		tieneZapatillas = true;
		reinicializarVeces_VistaVisitada();
	}
	if (lastAction == Action::WALK)
		actualizarMatrices_VistasVisitadas(sensores);
	

	// Definición del comportamiento
	if (sensores.superficie.at(0) == 'X')	// Llegó al puesto base
		action = Action::IDLE;
	else if (num_TURN_SR_Restantes > 0){	// Está en su giro SL
		action = Action::TURN_SR;
		--num_TURN_SR_Restantes;
	}
	else{ // Vamos a realizar el movimiento determinado por lo que veo
		int res = veoCasillaInteresante(sensores);
		action = comoLlegarA(res);
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
	if (!tieneZapatillas && sensores.superficie.at(0) == 'D'){
		tieneZapatillas = true;
		reinicializarVeces_VistaVisitada();
	}
	if (lastAction == Action::WALK)
		actualizarMatrices_VistasVisitadas(sensores);
	

	// Definición del comportamiento
	if (num_TURN_SR_Restantes > 0){	// Está en su giro SL
		action = Action::TURN_SR;
		--num_TURN_SR_Restantes;
	}
	else if (sensores.energia < MAYOR_COSTE){
		action = Action::IDLE;
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

// -----------------------------------------------------------------------------



Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_2(Sensores sensores)
{
	Action action = IDLE;	// Acción por defecto.
	lastAction = action;
	return action;
}


// -----------------------------------------------------------------------------

ComportamientoAuxiliar::Estado ComportamientoAuxiliar::ejecutarAccion(Action action, const Estado & inicio){
	Estado nuevoEstado = inicio;
	switch (action){
		case Action::TURN_SR:
			nuevoEstado.orientacion = (Orientacion)(((int)inicio.orientacion + 1) % 8);
			break;
		case Action::WALK:
			if (casillaAccesible(inicio, 2)){
				tie(nuevoEstado.fil, nuevoEstado.col) = aCoordenadas(inicio.fil, inicio.col, inicio.orientacion, 2);
			}
			break;
	}
	
	return nuevoEstado;
}


int ComportamientoAuxiliar::gastoAccion(Action action, int filInicio, int colInicio, int filDestino, int colDestino){
	int gasto=0;
	int difAltura=-1;	// Si no es reconocida, fomentamos que vaya ahí
	if (mapaResultado.at(filInicio).at(colInicio) != '?')
		difAltura = mapaCotas.at(filDestino).at(colDestino) - mapaCotas.at(filInicio).at(colInicio);
	
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

int ComportamientoAuxiliar::Heuristica(const Estado& estado, int filDestino, int colDestino)
{
	// Heurística de la Norma Infinito
	int difFil = abs(estado.fil - filDestino);
	int difCol = abs(estado.col - colDestino);
	return max(difFil, difCol);
}

vector<Action> ComportamientoAuxiliar::A_Estrella(
	const Estado &origen,
	int filDestino,
	int colDestino)
{
	set<pair<int, int>> setDestinos;
	setDestinos.insert(make_pair(filDestino, colDestino));
	return A_Estrella(origen, setDestinos, make_pair(filDestino, colDestino));
}



vector<Action> ComportamientoAuxiliar::A_Estrella(
	const Estado &origen,
	const set<pair<int, int>>& setDestinos,
	const pair<int, int>& destReferencia)
{

	// Acciones posibles del agente
	const vector<Action> acciones = {Action::WALK, Action::TURN_SR};

	const int INVALID = -1;	// Como no hay un valor máximo, ponemos un valor imposible
	

	// Cola con prioridad formada por Nodos. Ordenada por el gasto de Energia al nodo Origen
	struct Comparador {
		int filDestino;
		int colDestino;
	
		Comparador(int fil, int col) : filDestino(fil), colDestino(col) {}
	
		bool operator()(const Nodo& a, const Nodo& b) const {
			return a.gastoEnergia + Heuristica(a.estado, filDestino, colDestino)
				 > b.gastoEnergia + Heuristica(b.estado, filDestino, colDestino);
		}
	};


	priority_queue<Nodo, vector<Nodo>, Comparador> frontera(Comparador(destReferencia.first, destReferencia.second));

	// Nodos ya visitados, para evitar repeticiones
	set<Nodo> visitados;


	Nodo nodoActual = {origen, 0, {}};	// Nodo origen
	frontera.push(nodoActual);

	#ifdef DEBUG_DIJKSTRA_AUX
		int interaciones = 0;
	#endif



	bool caminoOptimoEncontrado = false;
	while(!frontera.empty() && !caminoOptimoEncontrado){

		nodoActual = frontera.top();
		frontera.pop();

		// Comprobamos si el nodo actual es el destino
		caminoOptimoEncontrado = setDestinos.find(make_pair(nodoActual.estado.fil, nodoActual.estado.col)) != setDestinos.end();
		
		if (!caminoOptimoEncontrado && visitados.insert(nodoActual).second){

			#ifdef DEBUG_DIJKSTRA_AUX
				interaciones++;
			#endif
			


			// Actualizamos el estado de las zapatillas
			if (!nodoActual.estado.tieneZapatillas &&
				(mapaResultado.at(nodoActual.estado.fil).at(nodoActual.estado.col) == 'D'
				|| mapaResultado.at(nodoActual.estado.fil).at(nodoActual.estado.col) == '?')
			)
				nodoActual.estado.tieneZapatillas = true;
			

			// Exploramos los nodos resultantes de aplicar cada una de las acciones
			Nodo nuevoNodo;

			for (auto it = acciones.begin(); it != acciones.end(); ++it){
				nuevoNodo.estado = ejecutarAccion(*it, nodoActual.estado);
				// Comprobamos que se ha generado un nodo nuevo
				if (visitados.find(nuevoNodo) == visitados.end()){

					// Modificamos el gasto de energía del nuevo nodo
					nuevoNodo.gastoEnergia = nodoActual.gastoEnergia
												+ gastoAccion(*it, nodoActual.estado.fil, nodoActual.estado.col, nuevoNodo.estado.fil, nuevoNodo.estado.col);
					nuevoNodo.acciones = nodoActual.acciones;
					nuevoNodo.acciones.push_back(*it);

					// Añadimos el nuevo nodo a la frontera (podrá estar ya, pero podemos haber encontrado un camino mejor)
					frontera.push(nuevoNodo);
				} 
			} // for accion
		} // if visitados.insert
	} // while frontera

	// Si hemos encontrado el camino óptimo, lo devolvemos
	vector<Action> plan = caminoOptimoEncontrado ? nodoActual.acciones : vector<Action>();

	#ifdef DEBUG_DIJKSTRA_AUX
		cout << "Iteraciones: " << interaciones << endl;
		cout << "Nodos abiertos: " << frontera.size() << endl;
		cout << "Nodos cerrados: " << visitados.size() << endl;
	#endif
		
	return plan;

}

void ComportamientoAuxiliar::VisualizaPlan(const Estado& origen, const vector<Action>& plan){
	
	mapaConPlan.clear();
	mapaConPlan.resize(mapaResultado.size(), vector<unsigned char>(mapaResultado.at(0).size(), 0));

	Estado estadoActual = origen;
	if (!estadoActual.tieneZapatillas && mapaResultado.at(estadoActual.fil).at(estadoActual.col) == 'D')
		estadoActual.tieneZapatillas = true;


	for(auto it = plan.begin(); it != plan.end(); ++it){

		estadoActual = ejecutarAccion(*it, estadoActual);

		if (!estadoActual.tieneZapatillas && mapaResultado.at(estadoActual.fil).at(estadoActual.col) == 'D')
			estadoActual.tieneZapatillas = true;
		switch (*it){
			case Action::WALK:
				mapaConPlan.at(estadoActual.fil).at(estadoActual.col) = 2;
				break;
		}
	}
}

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_3(Sensores sensores)
{
	Action accion= IDLE;
	// Actualización de variables de estado.
	situarSensorEnMapa(mapaResultado, mapaCotas, mapaEntidades, sensores);
	
	if (plan.empty() && (sensores.posF != sensores.destinoF || sensores.posC != sensores.destinoC)){


		// Inicializamos el nodo origen
		Estado origen;
		origen.fil = sensores.posF;
		origen.col = sensores.posC;
		origen.orientacion = sensores.rumbo;
		origen.tieneZapatillas = tieneZapatillas;

		
		plan = A_Estrella(origen, sensores.destinoF, sensores.destinoC);

		VisualizaPlan(origen, plan);
	}

	if (!plan.empty()){
		accion = plan.at(numEnPlan);
		++numEnPlan;

		// Si hemos terminado el plan, lo reinicializamos
		if (numEnPlan >= plan.size()){
			numEnPlan = 0;
			plan.clear();
		}
	}

	return accion;
}

void ComportamientoAuxiliar::actualizaPuestosBase(const Sensores& sensores)
{	
	for (int i=0; i<NUM_CASILLAS; ++i)
		if(sensores.superficie.at(i) == 'X'){
			int fil, col;
			tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, i);

			puestosBase.insert(make_pair(fil, col));
		}
}

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_4(Sensores sensores)
{
	Action accion= IDLE;


	// Actualización de variables de estado.
	situarSensorEnMapa(mapaResultado, mapaCotas, mapaEntidades, sensores);
	actualizaPuestosBase(sensores);
	if (!tieneZapatillas && sensores.superficie.at(0) == 'D')
		tieneZapatillas = true;


	// Buscamos zapatillas al alrededor del agente
	if (accion == Action::IDLE && !tieneZapatillas){
		int res = buscaCasilla(sensores, 'D', false);
		if (res != 0)
			accion = comoLlegarA(res);
	}

	if (accion == Action::IDLE){
		// Tan solo se moverá si tiene un destino válido
		if (0 <= sensores.destinoF && sensores.destinoF < mapaResultado.size() &&
			0 <= sensores.destinoC && sensores.destinoC < mapaResultado.at(0).size()){
				// Inicializamos el nodo origen
				Estado origen;
				origen.fil = sensores.posF;
				origen.col = sensores.posC;
				origen.orientacion = sensores.rumbo;
				origen.tieneZapatillas = tieneZapatillas;


				set<pair<int, int>> setDestinos;
				for(int difFil = -(PROFUNDIDAD_SENSOR-1); difFil <= PROFUNDIDAD_SENSOR-1; ++difFil){
					for(int difCol = -(PROFUNDIDAD_SENSOR-1); difCol <= PROFUNDIDAD_SENSOR-1; ++difCol){
						int filDestino = sensores.destinoF + difFil;
						int colDestino = sensores.destinoC + difCol;
						if (0 <= filDestino && filDestino < mapaResultado.size() &&
							0 <= colDestino && colDestino < mapaResultado.at(0).size()){
							setDestinos.insert(make_pair(filDestino, colDestino));
						}
					}
				}

				// Si no estamos en un Destino, buscamos el camino
				if (setDestinos.find(make_pair(sensores.posF, sensores.posC)) == setDestinos.end()){
					// Buscamos el camino
					plan = A_Estrella(origen, setDestinos, make_pair(sensores.destinoF, sensores.destinoC));
					VisualizaPlan(origen, plan);
					if (!plan.empty())
						accion = *plan.begin();
				}
				else{
					// Si estamos en un destino, tenemos que girar hasta ver el destino
					bool encontrado = false;
					for (int i=0; i<NUM_CASILLAS && !encontrado; ++i){
						int fil, col;
						tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, i);
						encontrado = fil==sensores.destinoF && col==sensores.destinoC;
					}
					if (!encontrado)
						accion = Action::TURN_SR;
				}				
			}
	}

	
	if (lastAction == Action::IDLE){
		mapaConPlan.clear();
		mapaConPlan.resize(mapaResultado.size(), vector<unsigned char>(mapaResultado.at(0).size(), 0));
	}
	


	lastAction = accion;	// Actualizamos la última acción realizada
	return accion;
}



int ComportamientoAuxiliar::buscaCasilla(const Sensores& sensores, char tipo, bool hayLimitacion, set<char> casillasPermitidas){

	int res=0;
	int fil, col;

	vector<int> casillasAccesibles = getCasillasAccesibles(sensores, hayLimitacion, casillasPermitidas);
	for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end() && res == 0; ++i){
		tie(fil, col) = aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
		if (mapaResultado.at(fil).at(col) == tipo)
			res = *i;
	}

	return res;
}


Action ComportamientoAuxiliar::comoLlegarA(int casilla){
	Action action;

	switch (casilla){
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
	
	return action;
}



vector<int> ComportamientoAuxiliar::getCasillasAccesibles(const Sensores & sensores, bool hayLimitacion, const set<char> &casillasPermitidas){

	// Obtenemos las casillas accesibles ***SOLO POR CAMINOS***
	// Casilla Alcanzable: Casilla que puedo alcanzar (posiblemente no pueda acceder)
	// Casilla Accesible: Casilla que puedo alcanzar y es accesible

	// El orden de las casillas es relevante para hacer el mínimo número de pasos. Lo buscado: 6 8 4 2 3 1 -4 -5 -2 -4
	const vector<int> casillasAlcanzables	= {2,3,1, -4, -2};
	vector<int> casillasAccesibles;
	int fil,col;


	for (auto i = casillasAlcanzables.begin(); i != casillasAlcanzables.end(); ++i){
		if (casillaAccesible(sensores, *i)){

			bool aniade = true;
			if (hayLimitacion){
				tie(fil,col)=aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
				aniade = find(casillasPermitidas.begin(), casillasPermitidas.end(), mapaResultado.at(fil).at(col)) != casillasPermitidas.end();
			}
			
			if (aniade)
				casillasAccesibles.push_back(*i);
		}
	}

	#ifdef DEBUG_AUX
	cout << "Casillas accesibles: " << endl;
	for (auto i = casillasAccesibles.begin(); i != casillasAccesibles.end(); ++i){
		tie(fil,col)=aCoordenadas(sensores.posF, sensores.posC, sensores.rumbo, *i);
		cout << "(" << fil << "," << col << ") " << mapaResultado.at(fil).at(col) << " " << *i << endl;
	}
	#endif

	return casillasAccesibles;
}
