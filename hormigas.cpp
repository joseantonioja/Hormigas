#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
using namespace std;

#define TE_INICIO 0.1
#define P 0.01

class Trayecto{
public:
	double costo;
	double ne;
	double te;
	Trayecto(double, double);
	Trayecto();
	void imprime();
};

Trayecto::Trayecto(double c, double t){
	costo = c;
	ne = 1.0/costo;
	te = t;
}

Trayecto::Trayecto(){
	costo = ne = te= 0.0;
}

void Trayecto::imprime(){
	cout << " costo= "<<costo << " ne= "<<ne << " te= "<<te;
}

class Grafo{
public:
	Grafo();
	Grafo(int );
	void agregarTrayecto(int , int , double, double );
	map <int, Trayecto> obtieneTrayectos(int);
	void imprime();
	void resetTau();
	void actualizaTau(int, int, double);
private:
	int n;
	map <int, map< int, Trayecto > > aristas;
	map <int, map<int, Trayecto> >::iterator i;
	map <int, Trayecto>::iterator j;
};

Grafo::Grafo(){
	n = 0;
}

Grafo::Grafo(int numVertices){
	n = numVertices;
	for(int i=1; i <= n; i++){  
		map<int, Trayecto> tmp;        
		aristas[i] = tmp;
	}
}

void Grafo::agregarTrayecto(int inicio, int fin, double costo, double tau){
	aristas[inicio][fin] = Trayecto(costo, tau);
	aristas[fin][inicio] = Trayecto(costo, tau);
}

map<int, Trayecto> Grafo::obtieneTrayectos(int vertice){
	return aristas[vertice];
}
void Grafo::imprime(){
	for(i = aristas.begin(); i != aristas.end(); i++){
		for( j = i->second.begin(); j != i->second.end(); j++ ){
			cout << i->first << " -> "<<j->first; j->second.imprime(); cout << endl;
		}
	}
}

void Grafo::resetTau(){
	for( i = aristas.begin(); i!=aristas.end(); i++){
		for( j = i->second.begin(); j!=i->second.end(); j++){
			j->second.te*=(1 - P);
		}
	}
}

void Grafo::actualizaTau(int inicio, int fin, double costo){
	cout << inicio << " -> " << fin <<": t Valor previo: "<< aristas[inicio][fin].te << " Sumando "<< 1.0/costo<<endl;
	aristas[inicio][fin].te += 1.0/costo;
	aristas[fin][inicio].te += 1.0/costo;
}

class Hormiga{
public:
	Hormiga(int , int );
	void recorreGrafo(Grafo);
	vector<int> ruta;
	double costo;
private:
	int inicio;
	int destino;
	vector<double> prob_acum, nt;
	map< int, Trayecto> trayectosPosibles;
	map <int, Trayecto>::iterator it;
};

Hormiga::Hormiga(int s, int d){
	inicio = s;
	destino = d;
	costo = 0.0;
}

void Hormiga::recorreGrafo(Grafo g){
	ruta.clear();
	costo = 0.0;
	ruta.push_back(inicio);
	int vActual = inicio;
	int vPrevio = 0;
	int i;
	int numTrayectos;
	double aleatorio;
	double total;
	while(vActual!=destino){
		cout << "Puede irse de "<<vActual << " a "<<endl;
		trayectosPosibles = map<int, Trayecto>(g.obtieneTrayectos(vActual));
		trayectosPosibles.erase(vPrevio);
		numTrayectos = trayectosPosibles.size();
		prob_acum.clear(); nt.clear();
		total = 0.0;
		for(it=trayectosPosibles.begin(); it != trayectosPosibles.end(); it++){
			nt.push_back(it->second.ne*it->second.te);
			total+=it->second.ne*it->second.te;
		}
		it = trayectosPosibles.begin();
		prob_acum.push_back(nt[0]/total);
		cout << "\t"<< it->first << " p= "<<prob_acum[0]<<endl;
		for(i=1; i<numTrayectos; i++){
			it++;
			prob_acum.push_back(prob_acum[i-1]+nt[i]/total);
			cout << "\t"<< it->first << " p= "<<prob_acum[i]<<endl;
		}
		aleatorio = (double)rand() / (double)RAND_MAX ;
		cout << "#Aleat = "<<aleatorio<<endl;
		it = trayectosPosibles.begin();
		i = 0;
		vPrevio = vActual;
		while(i<numTrayectos && it!=trayectosPosibles.end()){
			if(prob_acum[i]>aleatorio){
				ruta.push_back(it->first);
				vActual = it->first;
				costo += it->second.costo;
				break;
			}
			else{
				it++;
				i++;
			}
		}
		cout << "Se elige "<<vActual<<endl;
	}
	cout << "Costo = "<<costo<<endl;
	cout << "Ruta = ";
	for(i = 0; i<ruta.size(); i++)
		cout << ruta[i]<<" - ";
	cout << endl;
}

class Colonia{
public:
	Colonia(int);
	void agregarHormiga(int, int);
	void iteracion();
	Grafo caminos;
private:
	int numIndividuos;
	vector<Hormiga> hormigas;
};

Colonia::Colonia(int numVertices){
	caminos = Grafo(numVertices);
	numIndividuos = 0;
}

void Colonia::agregarHormiga(int i, int e){
	hormigas.push_back(Hormiga(i, e));
	numIndividuos++;
}

void Colonia::iteracion(){
	caminos.imprime();
	int i, j;
	for(i=0; i<numIndividuos; i++){
		cout << "Hormiga "<<i+1<<endl;
		hormigas[i].recorreGrafo(caminos);
	}
	caminos.resetTau();
	for(i=0; i<numIndividuos; i++){
		for(j=0; j<hormigas[i].ruta.size()-1; j++){
			caminos.actualizaTau(hormigas[i].ruta[j], hormigas[i].ruta[j+1], hormigas[i].costo);
		}
	}
}

int main(){
	int numVertices, inicio, final, it;
	double costo;
	cin >> numVertices;
	Colonia col(numVertices);
	while(cin >> inicio >> final >> costo){
		col.caminos.agregarTrayecto(inicio, final, costo, TE_INICIO);
	}
	col.agregarHormiga(1, 6);
	col.agregarHormiga(1, 6);
	//Para la ruta de 1 a 5
	//col.agregarHormiga(1, 5);
	//col.agregarHormiga(1, 5);
	//Para la ruta de 1 a 7
	//col.agregarHormiga(1, 7);
	//col.agregarHormiga(1, 7);
	srand(time(NULL));
	for(it=0; it<2; it++){
		cout << "******************* ITERACION "<<it+1<<"*********************"<<endl;
		col.iteracion();
	}

	return 0;
}