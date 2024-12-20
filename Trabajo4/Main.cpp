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

// Función para verificar factibilidad
bool is_route_feasible(const Ruta& ruta, const VRPTWInstancia& instancia) {
    double tiempo = 0;
    int carga = 0;
    
    for (int i = 0; i < ruta.nodos.size(); ++i) {
        int nodo_actual = ruta.nodos[i];
        const Node& node = instancia.nodos[nodo_actual];
        
        if (i > 0) {
            int nodo_anterior = ruta.nodos[i-1];
            tiempo += calcular_distancia(instancia.nodos[nodo_anterior], node);
        }
        
        tiempo = max(tiempo, (double)node.e);
        if (tiempo > node.l) return false;
        
        tiempo += node.s;
        carga += node.demanda;
        
        if (carga > instancia.Q) return false;
    }
    
    return true;
}

// Función para actualizar las propiedades de una ruta
void update_route_properties(Ruta& ruta, const VRPTWInstancia& instancia) {
    ruta.distancia = 0;
    ruta.tiempo = 0;
    ruta.carga = 0;
    ruta.tiempos_llegada.clear();
    
    for (int i = 0; i < ruta.nodos.size(); ++i) {
        int nodo_actual = ruta.nodos[i];
        const Node& node = instancia.nodos[nodo_actual];
        
        if (i > 0) {
            int nodo_anterior = ruta.nodos[i-1];
            double dist = calcular_distancia(instancia.nodos[nodo_anterior], node);
            ruta.distancia += dist;
            ruta.tiempo += dist;
        }
        
        ruta.tiempo = max(ruta.tiempo, (double)node.e);
        ruta.tiempos_llegada.push_back(ruta.tiempo);
        ruta.tiempo += node.s;
        ruta.carga += node.demanda;
    }
}

// Función para aplicar el movimiento 2-opt clásico en una ruta
bool apply_2opt(Ruta& ruta, int i, int j, const VRPTWInstancia& instancia) {
    if (i >= j) return false;
    
    // Guardar ruta original
    vector<int> original_ruta = ruta.nodos;
    double original_distance = ruta.distancia;
    
    // Hacer el movimiento 2-opt
    reverse(ruta.nodos.begin() + i + 1, ruta.nodos.begin() + j + 1);
    
    // Revisar factibilidad y actualizar propiedades de la ruta
    if (is_route_feasible(ruta, instancia)) {
        update_route_properties(ruta, instancia);
        if (ruta.distancia < original_distance) {
            return true;
        }
    }
    
    // Si no se mejoró la solución, revertir los cambios
    ruta.nodos = original_ruta;
    update_route_properties(ruta, instancia);
    return false;
}

// Estrategia 1 (2-opt sencillo) best improvement 
vector<Ruta> local_search_2opt_best(vector<Ruta>& initial_solution, const VRPTWInstancia& instancia) {
    vector<Ruta> best_solution = initial_solution;
    bool improved = true;
    
    while (improved) {
        improved = false;
        double best_improvement = 0;
        int best_r = -1, best_i = -1, best_j = -1;
        
        for (int r = 0; r < best_solution.size(); ++r) {
            for (int i = 0; i < best_solution[r].nodos.size() - 2; ++i) {
                for (int j = i + 2; j < best_solution[r].nodos.size() - 1; ++j) {
                    Ruta temp_ruta = best_solution[r];
                    double original_distance = temp_ruta.distancia;
                    
                    if (apply_2opt(temp_ruta, i, j, instancia)) {
                        double improvement = original_distance - temp_ruta.distancia;
                        if (improvement > best_improvement) {
                            best_improvement = improvement;
                            best_r = r;
                            best_i = i;
                            best_j = j;
                        }
                    }
                }
            }
        }
        
        if (best_improvement > 0) {
            apply_2opt(best_solution[best_r], best_i, best_j, instancia);
            improved = true;
        }
    }
    
    return best_solution;
}

// Estrategia 1 (2-opt sencillo) first improvement
vector<Ruta> local_search_2opt_first(vector<Ruta>& initial_solution, const VRPTWInstancia& instancia) {
    vector<Ruta> solution = initial_solution;
    bool improved = true;
    
    while (improved) {
        improved = false;
        
        for (int r = 0; r < solution.size(); ++r) {
            for (int i = 0; i < solution[r].nodos.size() - 2; ++i) {
                for (int j = i + 2; j < solution[r].nodos.size() - 1; ++j) {
                    if (apply_2opt(solution[r], i, j, instancia)) {
                        improved = true;
                        break;
                    }
                }
                if (improved) break;
            }
            if (improved) break;
        }
    }
    
    return solution;
}

// Función para aplicar el movimiento 2-opt* entre 2 rutas
bool apply_2opt_star(Ruta& ruta1, Ruta& ruta2, int i, int j, const VRPTWInstancia& instancia) {
    // Guardar rutas originales
    vector<int> original_ruta1 = ruta1.nodos;
    vector<int> original_ruta2 = ruta2.nodos;
    double original_distance1 = ruta1.distancia;
    double original_distance2 = ruta2.distancia;
    
    // Realizar el cambio
    vector<int> new_ruta1(ruta1.nodos.begin(), ruta1.nodos.begin() + i + 1);
    new_ruta1.insert(new_ruta1.end(), ruta2.nodos.begin() + j + 1, ruta2.nodos.end());
    
    vector<int> new_ruta2(ruta2.nodos.begin(), ruta2.nodos.begin() + j + 1);
    new_ruta2.insert(new_ruta2.end(), ruta1.nodos.begin() + i + 1, ruta1.nodos.end());
    
    ruta1.nodos = new_ruta1;
    ruta2.nodos = new_ruta2;
    
    // Revisar factibilidad y actualizar propiedades de las rutas
    if (is_route_feasible(ruta1, instancia) && is_route_feasible(ruta2, instancia)) {
        update_route_properties(ruta1, instancia);
        update_route_properties(ruta2, instancia);
        
        if (ruta1.distancia + ruta2.distancia < original_distance1 + original_distance2) {
            return true;
        }
    }
    
    // Si no se mejoró la solución, revertir los cambios
    ruta1.nodos = original_ruta1;
    ruta2.nodos = original_ruta2;
    ruta1.distancia = original_distance1;
    ruta2.distancia = original_distance2;
    return false;
}

// Estrategia 2 (2-opt*) best improvement 
vector<Ruta> local_search_2opt_star_best(vector<Ruta>& initial_solution, const VRPTWInstancia& instancia) {
    vector<Ruta> best_solution = initial_solution;
    bool improved = true;
    
    while (improved) {
        improved = false;
        double best_improvement = 0;
        int best_r1 = -1, best_r2 = -1, best_i = -1, best_j = -1;
        
        for (int r1 = 0; r1 < best_solution.size(); ++r1) {
            for (int r2 = r1 + 1; r2 < best_solution.size(); ++r2) {
                for (int i = 0; i < best_solution[r1].nodos.size() - 1; ++i) {
                    for (int j = 0; j < best_solution[r2].nodos.size() - 1; ++j) {
                        Ruta temp_ruta1 = best_solution[r1];
                        Ruta temp_ruta2 = best_solution[r2];
                        double original_distance = temp_ruta1.distancia + temp_ruta2.distancia;
                        
                        if (apply_2opt_star(temp_ruta1, temp_ruta2, i, j, instancia)) {
                            double improvement = original_distance - (temp_ruta1.distancia + temp_ruta2.distancia);
                            if (improvement > best_improvement) {
                                best_improvement = improvement;
                                best_r1 = r1; best_r2 = r2;
                                best_i = i; best_j = j;
                            }
                        }
                    }
                }
            }
        }
        
        if (best_improvement > 0) {
            apply_2opt_star(best_solution[best_r1], best_solution[best_r2], best_i, best_j, instancia);
            improved = true;
        }
    }
    
    return best_solution;
}

// Estrategia 2 (2-opt*) first improvement 
vector<Ruta> local_search_2opt_star_first(vector<Ruta>& initial_solution, const VRPTWInstancia& instancia) {
    vector<Ruta> solution = initial_solution;
    bool improved = true;
    
    while (improved) {
        improved = false;
        
        for (int r1 = 0; r1 < solution.size() && !improved; ++r1) {
            for (int r2 = r1 + 1; r2 < solution.size() && !improved; ++r2) {
                for (int i = 0; i < solution[r1].nodos.size() - 1 && !improved; ++i) {
                    for (int j = 0; j < solution[r2].nodos.size() - 1 && !improved; ++j) {
                        if (apply_2opt_star(solution[r1], solution[r2], i, j, instancia)) {
                            improved = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return solution;
}

// Función para aplicar el movimiento de relocate entre dos rutas
bool apply_relocate_between_routes(Ruta& ruta_origen, Ruta& ruta_destino, int nodo_pos, int insert_pos, const VRPTWInstancia& instancia) {
    // Guardamos las rutas originales por si necesitamos revertir
    vector<int> original_ruta_origen = ruta_origen.nodos;
    vector<int> original_ruta_destino = ruta_destino.nodos;
    
    // Realizamos el movimiento
    int nodo = ruta_origen.nodos[nodo_pos];
    ruta_origen.nodos.erase(ruta_origen.nodos.begin() + nodo_pos);
    ruta_destino.nodos.insert(ruta_destino.nodos.begin() + insert_pos, nodo);
    
    // Verificamos la factibilidad y actualizamos las propiedades de las rutas
    if (is_route_feasible(ruta_origen, instancia) && is_route_feasible(ruta_destino, instancia)) {
        // Actualizamos las propiedades de las rutas
        update_route_properties(ruta_origen, instancia);
        update_route_properties(ruta_destino, instancia);
        return true;
    } else {
        // Si no es factible, revertimos los cambios
        ruta_origen.nodos = original_ruta_origen;
        ruta_destino.nodos = original_ruta_destino;
        return false;
    }
}

// Estrategia 3 (relocate entre rutas) best improvement  
vector<Ruta> local_search_relocate_best(vector<Ruta>& initial_solution, const VRPTWInstancia& instancia) {
    vector<Ruta> best_solution = initial_solution;
    bool improved = true;
    
    while (improved) {
        improved = false;
        double best_improvement = 0;
        int best_r1 = -1, best_r2 = -1, best_nodo_pos = -1, best_insert_pos = -1;
        
        for (int r1 = 0; r1 < best_solution.size(); ++r1) {
            for (int r2 = 0; r2 < best_solution.size(); ++r2) {
                if (r1 == r2) continue;
                for (int nodo_pos = 1; nodo_pos < best_solution[r1].nodos.size() - 1; ++nodo_pos) {
                    for (int insert_pos = 1; insert_pos < best_solution[r2].nodos.size(); ++insert_pos) {
                        Ruta temp_ruta1 = best_solution[r1];
                        Ruta temp_ruta2 = best_solution[r2];
                        
                        if (apply_relocate_between_routes(temp_ruta1, temp_ruta2, nodo_pos, insert_pos, instancia)) {
                            double improvement = (best_solution[r1].distancia + best_solution[r2].distancia) -
                                                 (temp_ruta1.distancia + temp_ruta2.distancia);
                            if (improvement > best_improvement) {
                                best_improvement = improvement;
                                best_r1 = r1; best_r2 = r2;
                                best_nodo_pos = nodo_pos; best_insert_pos = insert_pos;
                            }
                        }
                    }
                }
            }
        }
        
        if (best_improvement > 0) {
            apply_relocate_between_routes(best_solution[best_r1], best_solution[best_r2], best_nodo_pos, best_insert_pos, instancia);
            improved = true;
        }
    }
    
    return best_solution;
}

// Estrategia 3 (relocate entre rutas) first improvement  
vector<Ruta> local_search_relocate_first(vector<Ruta>& initial_solution, const VRPTWInstancia& instancia) {
    vector<Ruta> solution = initial_solution;
    bool improved = true;
    
    while (improved) {
        improved = false;
        
        for (int r1 = 0; r1 < solution.size() && !improved; ++r1) {
            for (int r2 = 0; r2 < solution.size() && !improved; ++r2) {
                if (r1 == r2) continue;
                for (int nodo_pos = 1; nodo_pos < solution[r1].nodos.size() - 1 && !improved; ++nodo_pos) {
                    for (int insert_pos = 1; insert_pos < solution[r2].nodos.size() && !improved; ++insert_pos) {
                        double original_distance = solution[r1].distancia + solution[r2].distancia;
                        
                        if (apply_relocate_between_routes(solution[r1], solution[r2], nodo_pos, insert_pos, instancia)) {
                            double new_distance = solution[r1].distancia + solution[r2].distancia;
                            if (new_distance < original_distance) {
                                improved = true;
                                break;
                            } else {
                                // Revertir el cambio si no mejoró
                                apply_relocate_between_routes(solution[r2], solution[r1], insert_pos, nodo_pos, instancia);
                            }
                        }
                    }
                }
            }
        }
    }
    
    return solution;
}

// VND usando la estrategia best improvement en las búsquedas locales
vector<Ruta> VND_best(vector<Ruta>& solucion_inicial, const VRPTWInstancia& instancia_problema, double time_limit) {
    auto start_time = chrono::high_resolution_clock::now();

    vector<Ruta> mejor_solucion = solucion_inicial;
    bool mejora_encontrada = true;

    while (mejora_encontrada) {
        mejora_encontrada = false;

        // Aplicar 2-opt en una ruta
        auto current_time = chrono::high_resolution_clock::now();
        vector<Ruta> nueva_solucion = local_search_2opt_best(mejor_solucion, instancia_problema);
        double tiempo_transcurrido = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();
        if (tiempo_transcurrido >= time_limit) break;  // Verificar si excede el límite de tiempo

        if (nueva_solucion.size() < mejor_solucion.size() || 
            (nueva_solucion.size() == mejor_solucion.size() && calcularDistanciaTotal(nueva_solucion) < calcularDistanciaTotal(mejor_solucion))) {
            mejor_solucion = nueva_solucion;
            mejora_encontrada = true;
            continue; // Volver al inicio del bucle si hay mejora
        }

        // Aplicar 2-opt entre rutas
        current_time = chrono::high_resolution_clock::now();
        nueva_solucion = local_search_2opt_star_best(mejor_solucion, instancia_problema);
        tiempo_transcurrido = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();
        if (tiempo_transcurrido >= time_limit) break;

        if (nueva_solucion.size() < mejor_solucion.size() || 
            (nueva_solucion.size() == mejor_solucion.size() && calcularDistanciaTotal(nueva_solucion) < calcularDistanciaTotal(mejor_solucion))) {
            mejor_solucion = nueva_solucion;
            mejora_encontrada = true;
            continue;
        }

        // Aplicar Best Insertion
        current_time = chrono::high_resolution_clock::now();
        nueva_solucion = local_search_relocate_best(mejor_solucion, instancia_problema);
        tiempo_transcurrido = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();
        if (tiempo_transcurrido >= time_limit) break;

        if (nueva_solucion.size() < mejor_solucion.size() || 
            (nueva_solucion.size() == mejor_solucion.size() && calcularDistanciaTotal(nueva_solucion) < calcularDistanciaTotal(mejor_solucion))) {
            mejor_solucion = nueva_solucion;
            mejora_encontrada = true;
            continue;
        }

    }

    return mejor_solucion;
}

// VND usando la estrategia first improvement en las búsquedas locales
vector<Ruta> VND_first(vector<Ruta>& solucion_inicial, const VRPTWInstancia& instancia_problema, double time_limit) {
    auto start_time = chrono::high_resolution_clock::now();

    vector<Ruta> mejor_solucion = solucion_inicial;
    bool mejora_encontrada = true;

    while (mejora_encontrada) {
        mejora_encontrada = false;

        // Aplicar 2-opt en una ruta
        auto current_time = chrono::high_resolution_clock::now();
        vector<Ruta> nueva_solucion = local_search_2opt_first(mejor_solucion, instancia_problema);
        double tiempo_transcurrido = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();
        if (tiempo_transcurrido >= time_limit) break;  // Verificar si excede el límite de tiempo

        if (nueva_solucion.size() < mejor_solucion.size() || 
            (nueva_solucion.size() == mejor_solucion.size() && calcularDistanciaTotal(nueva_solucion) < calcularDistanciaTotal(mejor_solucion))) {
            mejor_solucion = nueva_solucion;
            mejora_encontrada = true;
            continue; // Volver al inicio del bucle si hay mejora
        }

        // Aplicar 2-opt entre rutas
        current_time = chrono::high_resolution_clock::now();
        nueva_solucion = local_search_2opt_star_first(mejor_solucion, instancia_problema);
        tiempo_transcurrido = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();
        if (tiempo_transcurrido >= time_limit) break;

        if (nueva_solucion.size() < mejor_solucion.size() || 
            (nueva_solucion.size() == mejor_solucion.size() && calcularDistanciaTotal(nueva_solucion) < calcularDistanciaTotal(mejor_solucion))) {
            mejor_solucion = nueva_solucion;
            mejora_encontrada = true;
            continue;
        }

        // Aplicar Best Insertion
        current_time = chrono::high_resolution_clock::now();
        nueva_solucion = local_search_relocate_first(mejor_solucion, instancia_problema);
        tiempo_transcurrido = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();
        if (tiempo_transcurrido >= time_limit) break;

        if (nueva_solucion.size() < mejor_solucion.size() || 
            (nueva_solucion.size() == mejor_solucion.size() && calcularDistanciaTotal(nueva_solucion) < calcularDistanciaTotal(mejor_solucion))) {
            mejor_solucion = nueva_solucion;
            mejora_encontrada = true;
            continue;
        }

    }

    return mejor_solucion;
}

// Función de enfriamiento para el SA
double cooling_schedule(double temperatura_inicial, double tiempo_transcurrido, double time_limit, double cooling_rate = 0.95) {
    double progreso = tiempo_transcurrido / time_limit;
    // cooling rate
    return temperatura_inicial * pow(cooling_rate, progreso);
}

// Función de perturbación para el ILS
vector<Ruta> perturb_solution(const vector<Ruta>& solucion_inicial, const VRPTWInstancia& instancia) {
    
    vector<Ruta> nueva_solucion = solucion_inicial;
    // Al menos dos rutas para poder hacer una perturbación
    if (nueva_solucion.size() < 2) {
        return nueva_solucion;
    }

    // Selecciona la primera ruta y un nodo
    int ruta1 = rand() % nueva_solucion.size();
    if (nueva_solucion[ruta1].nodos.size() < 3) return nueva_solucion;
    int nodo_pos1 = rand() % (nueva_solucion[ruta1].nodos.size() - 2) + 1;

    // Selecciona la segunda ruta (diferente a la primera)
    int ruta2 = rand() % nueva_solucion.size();
    while (ruta1 == ruta2) ruta2 = rand() % nueva_solucion.size();  // Evitar elegir la misma ruta
    int insert_pos2 = rand() % (nueva_solucion[ruta2].nodos.size() - 1) + 1;
    
    if (apply_relocate_between_routes(nueva_solucion[ruta1], nueva_solucion[ruta2], nodo_pos1, insert_pos2, instancia)) {
        // Si el movimiento fue exitoso, puedes devolver la nueva solución
        return nueva_solucion;
    }

    // Si no fue exitoso, devolver la solución inicial sin cambios
    return solucion_inicial;
}

// Función hash para identificar si una solución está en la lista tabú.
string hash_solucion(const vector<Ruta>& solucion) {
    stringstream ss;
    for (const Ruta& ruta : solucion) {
        for (int nodo : ruta.nodos) {
            ss << nodo << ",";
        }
        ss << "|";
    }
    return ss.str();
}

// Función principal para el algoritmo de búsqueda con ILS, Tabú y Simulated Annealing
vector<Ruta> metaheuristico_Tabu_SA_ILS(vector<Ruta>& solucion_inicial, const VRPTWInstancia& instancia_problema, 
        double time_limit, double temperatura_inicial = 100, double cooling_rate = 0.95, int tabu_tenure = 10) {
    
    // Iniciar el cronómetro
    auto start_time = chrono::high_resolution_clock::now();

    // Variables principales
    vector<Ruta> mejor_solucion_global = solucion_inicial;  // La mejor solución global
    double mejor_distancia_global = calcularDistanciaTotal(mejor_solucion_global);
    vector<Ruta> mejor_solucion = solucion_inicial;
    vector<Ruta> solucion_actual = solucion_inicial;
    double mejor_distancia = calcularDistanciaTotal(mejor_solucion);

    // Lista Tabú
    unordered_set<string> lista_tabu;
    //int tabu_tenure = 10;
    int iter_sin_mejora = 0;

    // Simulated Annealing
    //double temperatura_inicial = 100;
    
    bool mejorado = true;  // Si llegamos a un óptimo local se aplica la perturbación

    while (true) {
        // Verificar límite de tiempo
        auto current_time = chrono::high_resolution_clock::now();
        double tiempo_transcurrido = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();
        if (tiempo_transcurrido >= time_limit) break;

        mejorado = false;

        // Búsqueda local
        vector<Ruta> nueva_solucion;
        // 2
        if ((double) rand() / (RAND_MAX) < 0.5){
            nueva_solucion = VND_best(solucion_actual, instancia_problema, (int) time_limit-tiempo_transcurrido);
        } else {
            nueva_solucion = VND_first(solucion_actual, instancia_problema, (int) time_limit-tiempo_transcurrido);
        }
        // 1
        //vector<Ruta> nueva_solucion = local_search_relocate_best(solucion_actual, instancia_problema);
        double nueva_distancia = calcularDistanciaTotal(nueva_solucion);

        // Verificar si la nueva solución está en la lista tabú
        string solucion_hash = hash_solucion(nueva_solucion);
        if (lista_tabu.find(solucion_hash) != lista_tabu.end()) {
            solucion_actual = perturb_solution(solucion_actual, instancia_problema);
            iter_sin_mejora++;
            continue;  // Si está en la lista tabú, ignorar la solución
        }

        // Aplicar SA
        double temperatura_actual = cooling_schedule(temperatura_inicial, tiempo_transcurrido, time_limit, cooling_rate);
        if (nueva_solucion.size() < mejor_solucion.size() || 
            (nueva_solucion.size() == mejor_solucion.size() && nueva_distancia < mejor_distancia) || 
            (exp((mejor_solucion.size() - nueva_solucion.size()) / temperatura_actual) > ((double) rand() / RAND_MAX))) {
            
            solucion_actual = nueva_solucion;
            mejor_solucion = nueva_solucion;
            mejor_distancia = nueva_distancia;
            mejorado = true;
            iter_sin_mejora = 0;

            // Actualizar la lista tabú
            lista_tabu.insert(solucion_hash);
            if (lista_tabu.size() > tabu_tenure) {
                lista_tabu.erase(lista_tabu.begin());
            }

            // Actualizar la mejor solución global si es la mejor hasta ahora
            if (nueva_solucion.size() < mejor_solucion_global.size() || 
                (nueva_solucion.size() == mejor_solucion_global.size() && nueva_distancia < mejor_distancia_global)) {
                mejor_solucion_global = nueva_solucion;
                mejor_distancia_global = nueva_distancia;
            }
        }

        // Si no mejoramos, aplicar perturbación (ILS)
        if (!mejorado) {
            solucion_actual = perturb_solution(mejor_solucion, instancia_problema);
            iter_sin_mejora++;
        }

        // Adaptar el tabu tenure según el número de iteraciones sin mejora
        //4
        if (iter_sin_mejora > 5) {
            tabu_tenure = min(tabu_tenure + 1, 20);
        } else if (iter_sin_mejora == 0) {
            tabu_tenure = max(tabu_tenure - 1, 5);
        }
    }

    return mejor_solucion_global;
}


// Función de fitness: minimiza el número de vehículos; si es igual, minimiza la distancia total.
double fitness(const vector<Ruta>& solucion) {
    int num_vehiculos = solucion.size();
    double distancia_total = calcularDistanciaTotal(solucion);
    return num_vehiculos * 1000 + distancia_total;
}

// Método de selección por ruleta
vector<Ruta> seleccion_por_ruleta(const vector<vector<Ruta>>& poblacion) {
    double fitness_total = 0.0;
    vector<double> fitness_poblacion;
    for (const auto& solucion : poblacion) {
        double fit = 1.0 / (1.0 + fitness(solucion));  // Inverso para minimizar
        fitness_poblacion.push_back(fit);
        fitness_total += fit;
    }

    // Calcular el punto de selección
    double punto_seleccion = (double)rand() / RAND_MAX * fitness_total;
    double suma_acumulada = 0.0;

    // Seleccionar el individuo correspondiente al punto de selección
    for (int i = 0; i < poblacion.size(); ++i) {
        suma_acumulada += fitness_poblacion[i];
        if (suma_acumulada >= punto_seleccion) {
            return poblacion[i];
        }
    }

    // Devolver el último elemento en caso de algún error
    return poblacion.back();
}

// Método de selección por torneo
vector<Ruta> seleccion_por_torneo(const vector<vector<Ruta>>& poblacion, int tam_torneo) {
    int mejor_idx = -1;
    double mejor_fitness = numeric_limits<double>::infinity();

    for (int i = 0; i < tam_torneo; ++i) {
        int idx = rand() % poblacion.size();
        double fit_actual = fitness(poblacion[idx]);
        if (fit_actual < mejor_fitness) {
            mejor_fitness = fit_actual;
            mejor_idx = idx;
        }
    }
    return poblacion[mejor_idx];
}

// Cruce de rutas entre dos padres
vector<Ruta> cruce(const vector<Ruta>& padre1, const vector<Ruta>& padre2, const VRPTWInstancia& instancia) {
    vector<Ruta> hijo;
    set<int> nodos_usados;
    nodos_usados.insert(0);

    // Seleccionar aleatoriamente rutas de ambos padres
    vector<bool> usar_padre1(max(padre1.size(), padre2.size()), false);
    for (size_t i = 0; i < usar_padre1.size(); ++i) {
        usar_padre1[i] = rand() % 2 == 0;
    }

    // Primero, copiar rutas seleccionadas del padre1
    for (size_t i = 0; i < padre1.size(); ++i) {
        if (usar_padre1[i]) {
            Ruta nueva_ruta = padre1[i];
            bool ruta_valida = true;
            
            // Verificar si los nodos ya están usados
            for (int nodo : nueva_ruta.nodos) {
                if (nodo != 0 && nodos_usados.count(nodo) > 0) {
                    ruta_valida = false;
                    break;
                }
            }

            if (ruta_valida && is_route_feasible(nueva_ruta, instancia)) {
                // Añadir nodos a usados
                for (int nodo : nueva_ruta.nodos) {
                    nodos_usados.insert(nodo);
                }
                hijo.push_back(nueva_ruta);
            }
        }
    }

    // Luego, copiar rutas seleccionadas del padre2
    for (size_t i = 0; i < padre2.size(); ++i) {
        if (!usar_padre1[i]) {
            Ruta nueva_ruta = padre2[i];
            bool ruta_valida = true;
            
            // Verificar si los nodos ya están usados
            for (int nodo : nueva_ruta.nodos) {
                if (nodo != 0 && nodos_usados.count(nodo) > 0) {
                    ruta_valida = false;
                    break;
                }
            }

            if (ruta_valida && is_route_feasible(nueva_ruta, instancia)) {
                // Añadir nodos a usados
                for (int nodo : nueva_ruta.nodos) {
                    nodos_usados.insert(nodo);
                }
                hijo.push_back(nueva_ruta);
            }
        }
    }

    // Insertar nodos faltantes
    vector<int> nodos_faltantes;
    for (int i = 1; i <= instancia.n; ++i) {
        if (nodos_usados.count(i) == 0) {
            nodos_faltantes.push_back(i);
        }
    }

    // Intentar insertar los nodos faltantes en las rutas existentes
    for (int nodo : nodos_faltantes) {
        bool insertado = false;

        for (Ruta& ruta : hijo) {
            // Intentar agregar el nodo a una de las rutas existentes
            for (size_t pos = 1; pos <= ruta.nodos.size(); ++pos) {
                vector<int> temp_nodos = ruta.nodos;
                temp_nodos.insert(temp_nodos.begin() + pos, nodo);
                
                Ruta temp_ruta;
                temp_ruta.nodos = temp_nodos;
                
                // Verificar la factibilidad después de agregar el nodo
                if (is_route_feasible(temp_ruta, instancia)) {
                    ruta.nodos.insert(ruta.nodos.begin() + pos, nodo);
                    update_route_properties(ruta, instancia);
                    nodos_usados.insert(nodo);
                    insertado = true;
                    break;
                }
            }
            if (insertado) break;
        }

        // Si el nodo no se pudo insertar en ninguna ruta existente, crear una nueva ruta
        if (!insertado) {
            Ruta nueva_ruta;
            nueva_ruta.nodos.push_back(0);
            nueva_ruta.nodos.push_back(nodo);
            nueva_ruta.nodos.push_back(0);
            update_route_properties(nueva_ruta, instancia);
            hijo.push_back(nueva_ruta);
            nodos_usados.insert(nodo);
        }
    }

    return hijo;
}

// Mutación mediante perturbación ILS
vector<Ruta> mutacion(vector<Ruta> solucion, const VRPTWInstancia& instancia) {
    return perturb_solution(solucion, instancia);
}

// Algoritmo evolutivo
vector<Ruta> algoritmo_evolutivo(const vector<Ruta>& solucion_inicial, const VRPTWInstancia& instancia, double time_limit) {
    auto start_time = chrono::high_resolution_clock::now();

    int tam_poblacion = 200; // Parametros usados: 100, 200, 220
    int tam_torneo = 10; // Parametros usados 5, 10, 12
    double prob_mutacion = 0.1; // Parametros usados 0.5, 0.1, 0.15
    vector<vector<Ruta>> poblacion;

    // Generar población inicial
    for (int i = 0; i < tam_poblacion; ++i) {
        vector<Ruta> solucion = perturb_solution(solucion_inicial, instancia);
        poblacion.push_back(solucion);
        //poblacion.push_back(VND_best(solucion, instancia, time_limit));
    }

    // Evolución
    while (true) {
        auto current_time = chrono::high_resolution_clock::now();
        double tiempo_transcurrido = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();
        if (tiempo_transcurrido >= time_limit) break;

        vector<vector<Ruta>> nueva_poblacion;

        // Selección, cruce y mutación
        while (nueva_poblacion.size() < tam_poblacion) {
            vector<Ruta> padre1 = seleccion_por_torneo(poblacion, tam_torneo);
            vector<Ruta> padre2 = seleccion_por_torneo(poblacion, tam_torneo);
            //vector<Ruta> padre1 = seleccion_por_ruleta(poblacion);
            //vector<Ruta> padre2 = seleccion_por_ruleta(poblacion);

            vector<Ruta> hijo = cruce(padre1, padre2, instancia);
            if ((double)rand() / RAND_MAX < prob_mutacion) {
                hijo = mutacion(hijo, instancia);
            }

            nueva_poblacion.push_back(hijo);
            //nueva_poblacion.push_back(VND_best(hijo, instancia, time_limit - tiempo_transcurrido));
        }

        // Reemplazo generacional
        poblacion = nueva_poblacion;
    }

    // Seleccionar la mejor solución de la población final
    vector<Ruta> mejor_solucion = poblacion[0];
    for (const auto& solucion : poblacion) {
        if (fitness(solucion) < fitness(mejor_solucion)) {
            mejor_solucion = solucion;
        }
    }

    return mejor_solucion;
}

// Algoritmo evolutivo híbrido
vector<Ruta> algoritmo_evolutivo_hibrido(const vector<Ruta>& solucion_inicial, const VRPTWInstancia& instancia, double time_limit) {
    auto start_time = chrono::high_resolution_clock::now();

    int tam_poblacion = 200; // 100, 200, 220
    int tam_torneo = 20; // 5, 10, 12
    double prob_mutacion = 0.1; // 0.5, 0.1
    vector<vector<Ruta>> poblacion;

    // Generar población inicial
    for (int i = 0; i < tam_poblacion; ++i) {
        vector<Ruta> solucion = perturb_solution(solucion_inicial, instancia);
        if ((double)rand() / RAND_MAX < 0.5) {
            poblacion.push_back(VND_best(solucion, instancia, time_limit));
        } else {
            poblacion.push_back(VND_first(solucion, instancia, time_limit));
        }
    }

    // Evolución
    while (true) {
        auto current_time = chrono::high_resolution_clock::now();
        double tiempo_transcurrido = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count();
        if (tiempo_transcurrido >= time_limit) break;

        vector<vector<Ruta>> nueva_poblacion;

        // Selección, cruce y mutación
        while (nueva_poblacion.size() < tam_poblacion) {
            vector<Ruta> padre1 = seleccion_por_torneo(poblacion, tam_torneo);
            vector<Ruta> padre2 = seleccion_por_torneo(poblacion, tam_torneo);
            //vector<Ruta> padre1 = seleccion_por_ruleta(poblacion);
            //vector<Ruta> padre2 = seleccion_por_ruleta(poblacion);

            vector<Ruta> hijo = cruce(padre1, padre2, instancia);
            if ((double)rand() / RAND_MAX < prob_mutacion) {
                hijo = mutacion(hijo, instancia);
            }

            if ((double)rand() / RAND_MAX < 0.5) {
                nueva_poblacion.push_back(VND_best(hijo, instancia, time_limit - tiempo_transcurrido));
            } else {
                nueva_poblacion.push_back(VND_first(hijo, instancia, time_limit - tiempo_transcurrido));
            }
            //nueva_poblacion.push_back(metaheuristico_Tabu_SA_ILS(hijo, instancia, time_limit/50));
            
        }

        // Reemplazo generacional
        poblacion = nueva_poblacion;
    }

    // Seleccionar la mejor solución de la población final
    vector<Ruta> mejor_solucion = poblacion[0];
    for (auto& solucion : poblacion) {
        if (fitness(solucion) < fitness(mejor_solucion)) {
            mejor_solucion = solucion;
        }
    }

    return mejor_solucion;
}


void print_solucion(const vector<Ruta>& rutas, double duration, const string& archivo_salida) {
    ofstream output_file(archivo_salida);
    if (!output_file.is_open()) {
        cerr << "Error al abrir el archivo de salida\n";
        return;
    }

    // Quitar rutas vacías
    vector<Ruta> non_empty_rutas;
    for (const auto& ruta : rutas) {
        if (ruta.nodos.size() > 2) {  // Incluir únicamente rutas con al menos un nodo que no sea de depósito
            non_empty_rutas.push_back(ruta);
        }
    }

    output_file << non_empty_rutas.size() << "," 
                << fixed << setprecision(3) << calcularDistanciaTotal(non_empty_rutas) ;
    output_file << "," << static_cast<int>(duration) << "\n";

    for (const Ruta& ruta : non_empty_rutas) {
        output_file << ruta.nodos.size() - 2 << ",";

        for (size_t i = 0; i < ruta.nodos.size(); ++i) {
            output_file << ruta.nodos[i];
            if (i < ruta.nodos.size() - 1) output_file << ",";
        }
        output_file << ",";

        // Tiempos de llegada
        for (size_t i = 0; i < ruta.tiempos_llegada.size(); ++i) {
            output_file << fixed << setprecision(3) << ruta.tiempos_llegada[i];
            if (i < ruta.tiempos_llegada.size() - 1) output_file << ",";
        }

        // Carga total
        output_file << "," << ruta.carga << "\n";
    }

    output_file.close();
}

// Función para leer la solución inicial
vector<Ruta> leer_solucion_inicial(const string& file_path, const VRPTWInstancia& instancia) {
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo de solución inicial: " << file_path << "\n";
        exit(1);
    }

    vector<Ruta> rutas;
    int num_vehiculos;
    double distancia_total;
    int tiempo_computo;

    // Leer la primera línea
    file >> num_vehiculos >> distancia_total >> tiempo_computo;

    for (int i = 0; i < num_vehiculos; ++i) {
        Ruta ruta;
        int num_nodos;

        // Leer número de nodos a visitar (sin contar el depósito)
        file >> num_nodos;

        // Leer la ruta (incluyendo depósitos)
        ruta.nodos.resize(num_nodos + 2);
        for (int j = 0; j < num_nodos + 2; ++j) {
            file >> ruta.nodos[j];
        }

        // Leer tiempos de llegada
        ruta.tiempos_llegada.resize(num_nodos + 2);
        for (int j = 0; j < num_nodos + 2; ++j) {
            file >> ruta.tiempos_llegada[j];
        }

        // Leer carga total del vehículo
        file >> ruta.carga;

        // Calcular distancia y tiempo total de la ruta
        ruta.distancia = 0;
        ruta.tiempo = 0;
        for (int j = 1; j < ruta.nodos.size(); ++j) {
            int nodo_actual = ruta.nodos[j];
            int nodo_anterior = ruta.nodos[j-1];
            double dist = calcular_distancia(instancia.nodos[nodo_anterior], instancia.nodos[nodo_actual]);
            ruta.distancia += dist;
            ruta.tiempo = ruta.tiempos_llegada[j];
        }

        rutas.push_back(ruta);
    }

    file.close();

    return rutas;
}

int main() {
    fast_cin();

    // Solucion inicial: MH del ACO con 2-opt

    string base_dir = "EVOLUTIVO7";
    string output_dir1 = base_dir + "/Best";

    if (!fs::exists(base_dir)) fs::create_directory(base_dir);
    if (!fs::exists(output_dir1)) fs::create_directory(output_dir1);

    for (int i = 1; i <= 18; ++i) {
        string file_name_problema = "VRPTW Instances/VRPTW" + to_string(i) + ".txt";
        string file_name_solucion = "MH_txt/Best/VRPTW" + to_string(i) + ".txt";
        string output_csv1 = output_dir1 + "/VRPTW" + to_string(i) + ".csv";
        
        VRPTWInstancia instancia_problema = leer_instancia(file_name_problema);
        vector<Ruta> solucion_inicial = leer_solucion_inicial(file_name_solucion, instancia_problema);

        auto inicio1 = chrono::high_resolution_clock::now();
        double time_limit = (i >= 1 && i <= 6) ? 50 : (i >= 7 && i <= 12) ? 200 : 750;
        vector<Ruta> rutas_ga = algoritmo_evolutivo(solucion_inicial, instancia_problema, time_limit*1000);
        auto fin1 = chrono::high_resolution_clock::now();
        double duracion1 = chrono::duration_cast<chrono::milliseconds>(fin1 - inicio1).count();
        
        print_solucion(rutas_ga, duracion1, output_csv1);
    }

    base_dir = "HIBRIDO7";
    output_dir1 = base_dir + "/Best";

    if (!fs::exists(base_dir)) fs::create_directory(base_dir);
    if (!fs::exists(output_dir1)) fs::create_directory(output_dir1);

    for (int i = 1; i <= 18; ++i) {
        string file_name_problema = "VRPTW Instances/VRPTW" + to_string(i) + ".txt";
        string file_name_solucion = "MH_txt/Best/VRPTW" + to_string(i) + ".txt";
        string output_csv1 = output_dir1 + "/VRPTW" + to_string(i) + ".csv";
        
        VRPTWInstancia instancia_problema = leer_instancia(file_name_problema);
        vector<Ruta> solucion_inicial = leer_solucion_inicial(file_name_solucion, instancia_problema);

        auto inicio1 = chrono::high_resolution_clock::now();
        double time_limit = (i >= 1 && i <= 6) ? 50 : (i >= 7 && i <= 12) ? 200 : 750;
        vector<Ruta> rutas_ga = algoritmo_evolutivo_hibrido(solucion_inicial, instancia_problema, time_limit*1000);
        auto fin1 = chrono::high_resolution_clock::now();
        double duracion1 = chrono::duration_cast<chrono::milliseconds>(fin1 - inicio1).count();

        print_solucion(rutas_ga, duracion1, output_csv1);
    }
    
    return 0;
}