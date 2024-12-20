#define fast_cin() ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL)
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const char nl = '\n';
#define pb push_back
#define all(x) x.begin(), x.end()
#define allr(x) x.rbegin(), x.rend()
#define ff first
#define ss second
#define vint vector<int>
//#define int ll

namespace fs = std::filesystem;

struct Node {
    int index;
    double x, y;
    int demanda;
    int e, l;  // Ventanas de tiempo
    int s;     // Tiempo de servicio
};

class VRPTWInstancia {
public:
    int n; // Número de nodos de demanda
    int Q; // Capacidad de los vehículos
    vector<Node> nodos; // Lista de nodos con su información

    VRPTWInstancia(int n, int Q, vector<Node>& nodos) : n(n), Q(Q), nodos(nodos) {}
};

struct Ruta {
    vector<int> nodos;
    vector<double> tiempos_llegada;   
    int carga = 0;
    double tiempo = 0.0;
    double distancia = 0.0;
};

VRPTWInstancia leer_instancia(const string& file_path) {
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo" << "\n";
        exit(1);
    }

    int n, Q; file >> n >> Q;

    vector<Node> nodos;

    for (int i = 0; i <= n; ++i) {
        Node node;
        file >> node.index >> node.x >> node.y >> node.demanda >> node.e >> node.l >> node.s;
        nodos.push_back(node);
    }

    file.close();
    return VRPTWInstancia(n, Q, nodos);
}

double calcular_distancia(const Node& node1, const Node& node2) {
    double dx = node1.x - node2.x;
    double dy = node1.y - node2.y;
    double distancia = sqrt(dx * dx + dy * dy);
    return round(distancia * 1000.0) / 1000.0;
}

double calcularDistanciaTotal(const vector<Ruta>& solucion) {
    return accumulate(all(solucion), 0.0,
                        [](double sum, const Ruta& ruta) { return sum + ruta.distancia; });
}

class CONSTRUCTIVOSolver {
public:
    VRPTWInstancia instancia;

    CONSTRUCTIVOSolver(const VRPTWInstancia& instancia) : instancia(instancia) {}

    vector<Ruta> metodo_constructivo() {
        vector<bool> visitados(instancia.n + 1, false);
        vector<Ruta> rutas;

        visitados[0] = true; // Depósito

        while (true) {
            bool all_visitados = true;
            for (int i = 1; i <= instancia.n; ++i) {
                if (!visitados[i]) {
                    all_visitados = false;
                    break;
                }
            }
            if (all_visitados) break;

            Ruta ruta;
            ruta.nodos.push_back(0);
            ruta.tiempos_llegada.push_back(0.0);
            ruta.carga = 0;
            ruta.tiempo = 0.0;

            int nodo_actual = 0;

            while (true) {
                int nodo_siguiente = -1;
                double min_distancia = numeric_limits<double>::max();

                // Entontrar el nodo factible más cercano
                for (int j = 1; j <= instancia.n; ++j) {
                    if (visitados[j]) continue;

                    double distancia = calcular_distancia(instancia.nodos[nodo_actual], instancia.nodos[j]);
                    double distancia0 = calcular_distancia(instancia.nodos[0], instancia.nodos[j]);
                    double tiempo_llegada = ruta.tiempo + distancia;

                    if (instancia.nodos[j].demanda + ruta.carga <= instancia.Q &&
                        tiempo_llegada <= instancia.nodos[j].l &&
                        max(tiempo_llegada, (double) instancia.nodos[j].e) + instancia.nodos[j].s + distancia0 <= instancia.nodos[0].l) {
                        if (distancia < min_distancia) {
                            min_distancia = distancia;
                            nodo_siguiente = j;
                        }
                    }
                    
                }

                if (nodo_siguiente == -1) break; // No más nodos factibles a añadir

                visitados[nodo_siguiente] = true;
                ruta.nodos.push_back(nodo_siguiente);
                ruta.carga += instancia.nodos[nodo_siguiente].demanda;
                ruta.tiempo = max(ruta.tiempo + min_distancia, (double)instancia.nodos[nodo_siguiente].e);
                ruta.tiempos_llegada.push_back(ruta.tiempo);
                ruta.tiempo += instancia.nodos[nodo_siguiente].s;
                ruta.distancia += min_distancia;

                nodo_actual = nodo_siguiente;
            }

            // Retorno al depósito
            ruta.nodos.push_back(0);
            ruta.tiempos_llegada.push_back(ruta.tiempo + calcular_distancia(instancia.nodos[nodo_actual], instancia.nodos[0]));
            ruta.distancia += calcular_distancia(instancia.nodos[nodo_actual], instancia.nodos[0]);
            rutas.push_back(ruta);
        }

        return rutas;
    }
};

class GRASPSolver {
public:
    VRPTWInstancia& instancia;
    double alpha; // Parámetro de aleatoriedad (0 <= alpha <= 1)
    int max_iteraciones;

    GRASPSolver(VRPTWInstancia& inst, double a, int max_iter) 
        : instancia(inst), alpha(a), max_iteraciones(max_iter) {}

    vector<Ruta> solve() {
        vector<Ruta> mejor_solucion;
        double mejor_distancia = numeric_limits<double>::max();

        for (int iter = 0; iter < max_iteraciones; ++iter) {
            vector<Ruta> solucion = solucion_GRASP();

            double distancia = calcularDistanciaTotal(solucion);
            if (distancia < mejor_distancia) {
                mejor_distancia = distancia;
                mejor_solucion = solucion;
            }
        }

        return mejor_solucion;
    }

    vector<Ruta> solucion_GRASP() {
        vector<bool> visitados(instancia.n + 1, false);
        vector<Ruta> rutas;
        visitados[0] = true; // Depósito

        while (true) {
            if (all_of(visitados.begin() + 1, visitados.end(), [](bool v) { return v; })) break;

            Ruta ruta;
            ruta.nodos.push_back(0);
            ruta.tiempos_llegada.push_back(0.0);
            ruta.carga = 0;
            ruta.tiempo = 0.0;

            int nodo_actual = 0;

            while (true) {
                vector<pair<int, double>> candidatos;
                for (int j = 1; j <= instancia.n; ++j) {
                    if (visitados[j]) continue;

                    double distancia = calcular_distancia(instancia.nodos[nodo_actual], instancia.nodos[j]);
                    double tiempo_llegada = ruta.tiempo + distancia;
                    double distancia0 = calcular_distancia(instancia.nodos[0], instancia.nodos[j]);

                    if (instancia.nodos[j].demanda + ruta.carga <= instancia.Q &&
                        tiempo_llegada <= instancia.nodos[j].l &&
                        max(tiempo_llegada, (double) instancia.nodos[j].e) + instancia.nodos[j].s + distancia0 <= instancia.nodos[0].l) {
                        candidatos.push_back({j, distancia});
                    }
                }

                if (candidatos.empty()) break;
                
                sort(all(candidatos), [](const auto& a, const auto& b) { 
                    return a.second < b.second; });

                int rcl_size = max(1, min(int(candidatos.size()), 
                                          int(alpha * candidatos.size())));
                int index_seleccionado = rand() % rcl_size;
                int siguiente_nodo = candidatos[index_seleccionado].first;

                visitados[siguiente_nodo] = true;
                ruta.nodos.push_back(siguiente_nodo);
                ruta.carga += instancia.nodos[siguiente_nodo].demanda;
                ruta.tiempo = max(ruta.tiempo + candidatos[index_seleccionado].second, 
                                 (double)instancia.nodos[siguiente_nodo].e);
                ruta.tiempos_llegada.push_back(ruta.tiempo);
                ruta.tiempo += instancia.nodos[siguiente_nodo].s;
                ruta.distancia += candidatos[index_seleccionado].second;

                nodo_actual = siguiente_nodo;
            }

            ruta.nodos.push_back(0);
            ruta.tiempos_llegada.push_back(ruta.tiempo + calcular_distancia(instancia.nodos[nodo_actual], instancia.nodos[0]));
            ruta.distancia += calcular_distancia(instancia.nodos[nodo_actual], instancia.nodos[0]);
            rutas.push_back(ruta);
        }

        return rutas;
    }
};

class ACOSolver {
public:
    VRPTWInstancia& instancia;
    int num_ants;
    int max_iteraciones;
    double alpha; // Importancia de las feromonas
    double beta;  // Importancia de la información heurística
    double rho;   // Tasa de evaporación de feromonas
    double tasa_Q;     // Constante de depósito de feromonas

    vector<vector<double>> feromonas;
    vector<vector<double>> info_heuristica;

    ACOSolver(VRPTWInstancia& inst, int ants, int max_iter, double a, double b, double r, double q)
        : instancia(inst), num_ants(ants), max_iteraciones(max_iter), alpha(a), beta(b), rho(r), tasa_Q(q) {
        initializarFeromonas();
        calcularInfoHeuristica();
    }

    vector<Ruta> solve() {
        vector<Ruta> mejor_solucion;
        double mejor_distancia = numeric_limits<double>::max();

        for (int iter = 0; iter < max_iteraciones; ++iter) {
            vector<vector<Ruta>> ant_soluciones(num_ants);

            for (int ant = 0; ant < num_ants; ++ant) {
                ant_soluciones[ant] = solucion_aco();
            }

            actualizarFeromonas(ant_soluciones);

            for (const auto& solucion : ant_soluciones) {
                double distancia = calcularDistanciaTotal(solucion);
                if (distancia < mejor_distancia) {
                    mejor_distancia = distancia;
                    mejor_solucion = solucion;
                }
            }
        }

        return mejor_solucion;
    }

    void initializarFeromonas() {
        feromonas.resize(instancia.n + 1, vector<double>(instancia.n + 1, 1.0));
    }

    void calcularInfoHeuristica() {
        info_heuristica.resize(instancia.n + 1, vector<double>(instancia.n + 1, 0.0));
        for (int i = 0; i <= instancia.n; ++i) {
            for (int j = 0; j <= instancia.n; ++j) {
                if (i != j) {
                    info_heuristica[i][j] = 1.0 / calcular_distancia(instancia.nodos[i], instancia.nodos[j]);
                }
            }
        }
    }

    vector<Ruta> solucion_aco() {
        vector<bool> visitados(instancia.n + 1, false);
        vector<Ruta> rutas;
        visitados[0] = true; // Depósito

        while (true) {
            if (all_of(visitados.begin() + 1, visitados.end(), [](bool v) { return v; })) break;

            Ruta ruta;
            ruta.nodos.push_back(0);
            ruta.tiempos_llegada.push_back(0.0);
            ruta.carga = 0;
            ruta.tiempo = 0.0;

            int nodo_actual = 0;

            while (true) {
                vector<pair<int, double>> candidatos;
                double probabilidad_total = 0.0;

                for (int j = 1; j <= instancia.n; ++j) {
                    if (visitados[j]) continue;

                    double distancia = calcular_distancia(instancia.nodos[nodo_actual], instancia.nodos[j]);
                    double tiempo_llegada = ruta.tiempo + distancia;
                    double distancia0 = calcular_distancia(instancia.nodos[0], instancia.nodos[j]);

                    if (instancia.nodos[j].demanda + ruta.carga <= instancia.Q &&
                        tiempo_llegada <= instancia.nodos[j].l &&
                        max(tiempo_llegada, (double) instancia.nodos[j].e) + instancia.nodos[j].s + distancia0 <= instancia.nodos[0].l) {
                        double probabilidad = pow(feromonas[nodo_actual][j], alpha) * 
                                             pow(info_heuristica[nodo_actual][j], beta);
                        candidatos.push_back({j, probabilidad});
                        probabilidad_total += probabilidad;
                    }
                }

                if (candidatos.empty()) break;

                // Valor entre [0, probabilidad_total]
                double valor_random = (double)rand() / RAND_MAX * probabilidad_total;
                int nodo_siguiente = -1;

                for (const auto& candidato : candidatos) {
                    valor_random -= candidato.second;
                    if (valor_random <= 0) {
                        nodo_siguiente = candidato.first;
                        break;
                    }
                }

                if (nodo_siguiente == -1) nodo_siguiente = candidatos.back().first;

                visitados[nodo_siguiente] = true;
                ruta.nodos.push_back(nodo_siguiente);
                ruta.carga += instancia.nodos[nodo_siguiente].demanda;
                double tiempo_viaje = calcular_distancia(instancia.nodos[nodo_actual], instancia.nodos[nodo_siguiente]);
                ruta.tiempo = max(ruta.tiempo + tiempo_viaje, (double)instancia.nodos[nodo_siguiente].e);
                ruta.tiempos_llegada.push_back(ruta.tiempo);
                ruta.tiempo += instancia.nodos[nodo_siguiente].s;
                ruta.distancia += tiempo_viaje;

                nodo_actual = nodo_siguiente;
            }

            ruta.nodos.push_back(0);
            ruta.tiempos_llegada.push_back(ruta.tiempo + calcular_distancia(instancia.nodos[nodo_actual], instancia.nodos[0]));
            ruta.distancia += calcular_distancia(instancia.nodos[nodo_actual], instancia.nodos[0]);
            rutas.push_back(ruta);
        }

        return rutas;
    }

    void actualizarFeromonas(const vector<vector<Ruta>>& ant_soluciones) {
        // Evaporación
        for (auto& row : feromonas) {
            for (auto& feromona : row) {
                feromona *= (1 - rho);
            }
        }

        // Depósito de feromonas
        for (const auto& solucion : ant_soluciones) {
            double calidad_solucion = 1.0 / calcularDistanciaTotal(solucion);
            for (const auto& ruta : solucion) {
                for (int i = 0; i < ruta.nodos.size() - 1; ++i) {
                    int de = ruta.nodos[i];
                    int a = ruta.nodos[i + 1];
                    feromonas[de][a] += tasa_Q * calidad_solucion;
                    feromonas[a][de] += tasa_Q * calidad_solucion;
                }
            }
        }
    }
};

pair<int, double> calcular_lower_bound(const VRPTWInstancia& instancia) {
    double distancia_total = 0.0;
    int demanda_total = 0;

    // Encontrar la mínima distancia del depósito a un nodo
    double min_dist_a_nodo = numeric_limits<double>::max();

    for (int i = 1; i <= instancia.n; i++) {
        double distancia_a_nodo = calcular_distancia(instancia.nodos[0], instancia.nodos[i]);
        min_dist_a_nodo = min(min_dist_a_nodo, distancia_a_nodo);
        demanda_total += instancia.nodos[i].demanda;
    }

    // Lower bound de el número de vehículos
    int min_vehiculos = ceil((double) demanda_total/instancia.Q);

    // Lower bound basado en el minimum spanning tree
    vector<double> min_distancias(instancia.n + 1, numeric_limits<double>::max());
    min_distancias[0] = 0;
    
    for (int i = 0; i < instancia.n + 1; i++) {
        int u = -1;
        for (int j = 0; j < instancia.n + 1; j++) {
            if (min_distancias[j] < numeric_limits<double>::max() && (u == -1 || min_distancias[j] < min_distancias[u])) {
                u = j;
            }
        }
        
        for (int v = 0; v < instancia.n + 1; v++) {
            if (v != u) {
                double dist = calcular_distancia(instancia.nodos[u], instancia.nodos[v]);
                min_distancias[v] = min(min_distancias[v], dist);
            }
        }
        
        distancia_total += min_distancias[u];
        min_distancias[u] = numeric_limits<double>::max();
    }

    // Lower bound de la distancia total
    double distancia_lower_bound = max(distancia_total, 2 * min_vehiculos * min_dist_a_nodo);

    return {min_vehiculos, distancia_lower_bound};
}

void print_solucion(const vector<Ruta>& rutas, double duration, const string& archivo_salida) {
    ofstream output_file(archivo_salida); // Abrir archivo para escribir
    if (!output_file.is_open()) {
        cerr << "Error al abrir el archivo de salida\n";
        return;
    }

    output_file << rutas.size() << "," << fixed << setprecision(3) <<
     calcularDistanciaTotal(rutas) << "," << (int) round(duration) << "\n";

    for (const Ruta& ruta : rutas) {
        output_file << (ruta.nodos.size() - 2) << ","; // Excluir depósito
        for (int i = 0; i < ruta.nodos.size(); ++i) {
            output_file << ruta.nodos[i];
            if (i < ruta.nodos.size() - 1) {
                output_file << ",";
            }
        }
        output_file << ",";
        for (int i = 0; i < ruta.tiempos_llegada.size(); ++i) {
            output_file << fixed << setprecision(3) << ruta.tiempos_llegada[i];
            if (i < ruta.tiempos_llegada.size() - 1) {
                output_file << ",";
            }
        }
        output_file << "," << fixed << setprecision(3) << ruta.carga << "\n";
    }

    output_file.close();
}

int main() {
    
    fast_cin();

    // Directorio donde los csv se guardan
    string output_dir = "Constructivo_sol";
    
    // Crea el directorio si no existe
    if (!fs::exists(output_dir)) fs::create_directory(output_dir);

    for (int i = 1; i <= 18; ++i) {

        // Crea el nombre de archivo para cada instancia
        string file_name = "VRPTW Instances/VRPTW" + to_string(i) + ".txt";
        string output_csv = output_dir + "/VRPTW" + to_string(i) + ".csv";
        
        VRPTWInstancia instancia = leer_instancia(file_name);
        
        CONSTRUCTIVOSolver solver(instancia);
        auto inicio_const = chrono::high_resolution_clock::now();
        vector<Ruta> sol_constructivo = solver.metodo_constructivo();
        auto fin_const = chrono::high_resolution_clock::now();
        double duracion_constructivo = chrono::duration_cast<chrono::milliseconds>(fin_const - inicio_const).count();

        // Guarda los resultados
        print_solucion(sol_constructivo, duracion_constructivo, output_csv);
    
    }

    output_dir = "Grasp_sol";

    if (!fs::exists(output_dir)) fs::create_directory(output_dir);

    for (int i = 1; i <= 18; ++i) {

        string file_name = "VRPTW Instances/VRPTW" + to_string(i) + ".txt";
        string output_csv = output_dir + "/VRPTW" + to_string(i) + ".csv";
        
        VRPTWInstancia instancia = leer_instancia(file_name);
        
        // GRASP
        double alpha = 0.1;
        int max_iter_grasp = 100;
        GRASPSolver grasp_solver(instancia, alpha, max_iter_grasp);

        auto inicio_grasp = chrono::high_resolution_clock::now();
        vector<Ruta> solucion_grasp = grasp_solver.solve();
        auto fin_grasp = chrono::high_resolution_clock::now();

        double duracion_grasp = chrono::duration_cast<chrono::milliseconds>(fin_grasp - inicio_grasp).count();

        print_solucion(solucion_grasp, duracion_grasp, output_csv);

    }
    
    output_dir = "Aco_sol";

    if (!fs::exists(output_dir)) fs::create_directory(output_dir);

    for (int i = 1; i <= 18; ++i) {

        string file_name = "VRPTW Instances/VRPTW" + to_string(i) + ".txt";
        string output_csv = output_dir + "/VRPTW" + to_string(i) + ".csv";
        
        VRPTWInstancia instancia = leer_instancia(file_name);
        
        // ACO
        int num_ants = 10;
        int max_iters_aco = 100;
        double alpha_aco = 1.0;
        double beta_aco = 2.0;
        double rho = 0.1;
        double tasa_Q = 100.0;
        ACOSolver aco_solver(instancia, num_ants, max_iters_aco, alpha_aco, beta_aco, rho, tasa_Q);
        
        auto inicio_aco = chrono::high_resolution_clock::now();
        vector<Ruta> aco_solucion = aco_solver.solve();
        auto fin_aco = chrono::high_resolution_clock::now();
        double duracion_aco = chrono::duration_cast<chrono::milliseconds>(fin_aco - inicio_aco).count();

        print_solucion(aco_solucion, duracion_aco, output_csv);

    }

    //pair<int, double> lower_bd = calcular_lower_bound(instancia);
    //cout << "Lower bound vehiculos: " << lower_bd.ff << "\n";
    //cout << "Lower bound distancia: " << fixed << setprecision(3) << lower_bd.ss << "\n";

    return 0;
}