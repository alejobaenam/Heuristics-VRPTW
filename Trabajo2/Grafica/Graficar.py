import matplotlib.pyplot as plt

# Función para leer nodos desde un archivo de texto
def leer_nodos(file_path):
    nodos = []
    with open(file_path, 'r') as file:
        for line in file.readlines()[2:]:  # Saltar las dos primeras líneas
            datos = list(map(float, line.split()))
            nodos.append((datos[1], datos[2]))  # Coordenadas (x, y)
    return nodos

# Función para leer rutas desde el archivo CSV
def leer_solucion_csv(file_path):
    rutas = []
    with open(file_path, 'r') as file:
        for line in file:
            datos = list(map(float, line.split(',')))  # Usar float para manejar números decimales
            ruta = list(map(int, datos[1:]))  # Convertir a int después de la primera columna
            rutas.append(ruta)
    return rutas

# Función para graficar las rutas
def graficar_rutas(ax, rutas, nodos, color_map):
    for idx, ruta in enumerate(rutas):
        if len(ruta) > 0:  # Asegúrate de que la ruta no esté vacía
            x = []
            y = []
            for i in ruta:
                if i < len(nodos):  # Verifica que el índice sea válido
                    x.append(nodos[i][0])
                    y.append(nodos[i][1])
            if x and y:  # Solo grafica si x e y tienen elementos
                ax.plot(x, y, marker='o', color=color_map[idx % len(color_map)], label=f'Ruta {idx + 1}')
                ax.text(x[0], y[0], 'Depot', fontsize=12, verticalalignment='bottom')

# Colores para las rutas
color_map = ['blue', 'orange', 'green', 'red', 'purple', 'brown', 'pink', 'gray']

# Graficar para cada iteración
for i in range(2, 3):
    file_csv_B = f'Grafica/Constructivo/LocalSearch2F/VRPTW{i}.csv'
    file_csv_F = f'Grafica/VRPTW sol-csv-constructivo/VRPTW{i}.csv'
    #file_csv_F = f'Constructivo/LocalSearch2F/VRPTW{i}.csv'
    file_nodos = f'Grafica/VRPTW Instances/VRPTW{i}.txt'
    
    nodos = leer_nodos(file_nodos)
    rutas_B = leer_solucion_csv(file_csv_B)
    rutas_F = leer_solucion_csv(file_csv_F)

    fig, axs = plt.subplots(1, 2, figsize=(12, 6))  # Crear dos subgráficas

    # Graficar rutas de LocalSearch2B en la subgráfica izquierda
    graficar_rutas(axs[0], rutas_B, nodos, color_map)
    axs[0].set_title(f'LocalSearch2F - Iteración {i}')
    axs[0].set_xlabel("Coordenada X")
    axs[0].set_ylabel("Coordenada Y")
    axs[0].grid()
    axs[0].legend()

    # Graficar rutas de LocalSearch2F en la subgráfica derecha
    graficar_rutas(axs[1], rutas_F, nodos, color_map)
    axs[1].set_title(f'Solución inicial constructivo')
    axs[1].set_xlabel("Coordenada X")
    axs[1].set_ylabel("Coordenada Y")
    axs[1].grid()
    axs[1].legend()

    plt.tight_layout()  # Ajustar el layout para que no se superpongan
    plt.show()
