#include <bits/stdc++.h>
using namespace std;

// Definiciones iniciales
typedef int Arista;
typedef vector<Arista> Nodo;
typedef pair<int, int> AristaPeso;
typedef vector<AristaPeso> NodoPeso;

const int INF (1 << 30);
const int MAXN = 100000;
const int MAXM = 100000;

Nodo grafo[MAXN];
Nodo grafo_peso[MAXN];
int in_degree[MAXN];

void AgregarArista(int u, int v) {
    grafo[u].push_back(v);
    ++in_degree[v];
}

//Emparejamiento Bipartito
struct Bipartito {
    int n; Lista pareja;
    vector<Lista> aristas;
    vector<bool> lado, visitado;

    Bipartito(int N) : lado(N), pareja(N),
        visitado(N), aristas(N), n(N) {}

    void AgregarArista(int u, int v) {
        aristas[u].push_back(v);
        aristas[v].push_back(u);
    }

    void AgregarIzq(int u) { lado[u] = true; }
    void AgregarDer(int u) { lado[u] = false; }

    int CaminoIncremental(int u) {
        visitado[u] = true;
        for (int i = 0; i < aristas[u].size(); ++i)
            if (pareja[aristas[u][i]] == -1) 
                return pareja[aristas[u][i]] = u;
        for (int i = 0; i < aristas[u].size(); ++i) {
            int v = aristas[u][i];
            if (visitado[pareja[v]]) continue;
            if (CaminoIncremental(pareja[v]) != -1)
                return pareja[v] = u;
        }
        return -1;
    }

    vector<Par> MaxEmparejamiento() {
        fill(pareja.begin(), pareja.end(), -1);
        for (int i = 0; i < n; ++i) {
            if (!lado[i]) continue; CaminoIncremental(i);
            fill(visitado.begin(), visitado.end(), false);
        }
        vector<Par> pares;
        for (int i = 0; i < n; ++i)
            if (!lado[i] && pareja[i] != -1)
                pares.push_back(Par(pareja[i], i));
        return pares; // Cardinalidad = pares.size()
    }
};

// Detecta ciclos en una componente conexa bidireccional.
// Devuelve -1 si no existen ciclos. En caso de existir,
// los nodos del ciclo se guardan en el arreglo ciclo.

int top_ciclo;
int ciclo[MAXN];
bool ciclo_activo;
bool visitado[MAXN];

int EncontrarCiclo_(int u, int p) {
    visitado[u] = true;
    for (int i = 0; i < grafo[u].size(); ++i) {
        if (grafo[u][i] == p) continue;
        int v = grafo[u][i], cic;
        if (visitado[v]) return ciclo[top_ciclo++] = v; 
        if ((cic = EncontrarCiclo_(v, u)) >= 0) {
            if (ciclo_activo) ciclo[top_ciclo++] = v;
            if (cic == u) ciclo_activo = false;
            return cic;
        }
    }
    return -1;
}

int EncontrarCiclo(int u) {
    ciclo_activo = true, top_ciclo = 0;
    return EncontrarCiclo_(u, -1);
}

// Orden topologico de un grafo dirigido aciclico.
// Los indices de los nodos se asumen de 0 a n - 1.

vector<int> orden_topo; // Resultado

void OrdenTopologico_(int u) {
    visitado[u] = true;
    for (int i = 0; i < grafo[u].size(); ++i)
        if (!visitado[grafo[u][i]])
            OrdenTopologico_(grafo[u][i]);
    orden_topo.push_back(u);
}

void OrdenTopologico(int n) {
    orden_topo.clear();
    fill(visitado, visitado + n, false);
    for (int i = 0; i < n; ++i)
        if (!visitado[i]) OrdenTopologico_(i);
}

// Obtener componentes fuertemente conexas en un grafo
// dirigido. Asume los nodos indexados de 0 a n - 1.

int numeracion, top_activo;
int low[MAXN], num[MAXN], activo[MAXN];
vector< vector<int> > CFCs; // Resultado

void ObtenerCFCs_(int u) {
    activo[top_activo++] = u;
    low[u] = num[u] = ++numeracion;
    for (int i = 0; i < grafo[u].size(); ++i) {
        int v = grafo[u][i];
        if (!num[v]) ObtenerCFCs_(v);
        low[u] = min(low[u], low[v]);
    }
    if (low[u] == num[u]) {
        vector<int> CFC;
        while (activo[top_activo - 1] != u) {
            CFC.push_back(activo[--top_activo]);
            low[activo[top_activo]] = MAXN;
        }
        CFC.push_back(activo[--top_activo]);
        low[u] = INF; CFCs.push_back(CFC);
    }
}

void ObtenerCFCs(int n) {
    CFCs.clear();
    numeracion = 0;
    fill(num, num + n, 0);
    fill(low, low + n, 0);
    for (int i = 0; i < n; ++i)
        if (!num[i]) ObtenerCFCs_(i);
}

// Detecta los puentes y puntos de articulacion en
// un grafo bidireccional. Indices de 0 a n - 1.

Nodo puentes[MAXN]; // Resultado
bool punto_art[MAXN]; // Resultado

void PuntosArtPuentes_(int u, int p) {
    low[u] = num[u] = ++numeracion;
    for (int i = 0; i < grafo[u].size(); ++i) {
        if (grafo[u][i] == p) continue;
        int v = grafo[u][i];
        if (!num[v]) PuntosArtPuentes_(v, u);
        if (low[v] > num[u]) {
            puentes[u].push_back(v);
            puentes[v].push_back(u);
        }
        punto_art[u] |= low[v] >= num[u];
        low[u] = min(low[u], low[v]);
    }
}

void PuntosArtPuentes(int n) {
    numeracion = 0;
    fill(num, num + n, 0);
    fill(low, low + n, 0);
    fill(punto_art, punto_art + n, false);
    for(int i = 0; i < n; ++i)
        puentes[i].clear();
    for(int i = 0; i < n; ++i) {
        if(num[i]) continue;
        PuntosArtPuentes_(i, -1);
        punto_art[i] = grafo[i].size() > 1;
    }
}


// Estructura de conjuntos disjuntos.
// Conjuntos indexados de 0 a n - 1.

struct UnionFind {
    int nconjuntos;
    vector<int> padre;
    vector<int> tamano;

    UnionFind(int n) : nconjuntos(n),
        padre(n), tamano(n, 1) {
        for(int i = 0; i < n; ++i)
            padre[i] = i;
    }

    int Encontrar(int u) {
        if (padre[u] == u) return u;
        return padre[u] = Encontrar(padre[u]);
    }

    void Unir(int u, int v) {
        int Ru = Encontrar(u);
        int Rv = Encontrar(v);
        if (Ru == Rv) return;
        -- nconjuntos, padre[Ru] = Rv;
        tamano[Rv] += tamano[Ru];
    }

    bool MismoConjunto(int u, int v) {
        return Encontrar(u) == Encontrar(v);
    }

    int TamanoConjunto(int u) {
        return tamano[Encontrar(u)];
    }
};

int main() {
    return 0;
}
