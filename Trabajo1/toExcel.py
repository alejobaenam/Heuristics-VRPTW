import csv
import os
from openpyxl import Workbook

def csvs_to_excel(csv_dir, excel_file):
    # Crear un nuevo libro de Excel
    workbook = Workbook()
    # Eliminar la hoja predeterminada
    workbook.remove(workbook.active)

    # Ordenar archivos CSV
    csv_files = sorted(
        [f for f in os.listdir(csv_dir) if f.endswith('.csv')],
        key=lambda x: int(os.path.splitext(x)[0].replace('VRPTW', ''))
    )

    # Itera sobre los archivos csv
    for csv_file in csv_files:
        sheet_name = os.path.splitext(csv_file)[0]
        
        # Crea una nueva hoja para cada csv
        worksheet = workbook.create_sheet(title=sheet_name[:31])

        with open(os.path.join(csv_dir, csv_file), newline='') as f:
            reader = csv.reader(f)
            for row in reader:
                # Procesa cada celda de la columna
                new_row = []
                for cell in row:
                    # Convertir a número
                    try:
                        new_row.append(float(cell))
                    except ValueError:
                        new_row.append(cell)
                
                worksheet.append(new_row)
    
    # Guardar el libro de Excel
    workbook.save(excel_file)

# Guardar
# cambiar direcciones
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/Trabajo1/Constructivo_sol', 'VRPTW_LuisAlejandroBaena_Constructivo.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/Trabajo1/Grasp_sol', 'VRPTW_LuisAlejandroBaena_Grasp.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/Trabajo1/Aco_sol', 'VRPTW_LuisAlejandroBaena_Aco.xlsx')