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
# constructivo
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Constructivo/LocalSearch1B', 'excel-constructivo/VRPTW_LuisAlejandroBaena_C-1B.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Constructivo/LocalSearch1F', 'excel-constructivo/VRPTW_LuisAlejandroBaena_C-1F.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Constructivo/LocalSearch2B', 'excel-constructivo/VRPTW_LuisAlejandroBaena_C-2B.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Constructivo/LocalSearch2F', 'excel-constructivo/VRPTW_LuisAlejandroBaena_C-2F.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Constructivo/LocalSearch3B', 'excel-constructivo/VRPTW_LuisAlejandroBaena_C-3B.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Constructivo/LocalSearch3F', 'excel-constructivo/VRPTW_LuisAlejandroBaena_C-3F.xlsx')
# grasp
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Grasp/LocalSearch1B', 'excel-grasp/VRPTW_LuisAlejandroBaena_G-1B.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Grasp/LocalSearch1F', 'excel-grasp/VRPTW_LuisAlejandroBaena_G-1F.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Grasp/LocalSearch2B', 'excel-grasp/VRPTW_LuisAlejandroBaena_G-2B.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Grasp/LocalSearch2F', 'excel-grasp/VRPTW_LuisAlejandroBaena_G-2F.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Grasp/LocalSearch3B', 'excel-grasp/VRPTW_LuisAlejandroBaena_G-3B.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Grasp/LocalSearch3F', 'excel-grasp/VRPTW_LuisAlejandroBaena_G-3F.xlsx')
# aco
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Aco/LocalSearch1B', 'excel-aco/VRPTW_LuisAlejandroBaena_A-1B.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Aco/LocalSearch1F', 'excel-aco/VRPTW_LuisAlejandroBaena_A-1F.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Aco/LocalSearch2B', 'excel-aco/VRPTW_LuisAlejandroBaena_A-2B.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Aco/LocalSearch2F', 'excel-aco/VRPTW_LuisAlejandroBaena_A-2F.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Aco/LocalSearch3B', 'excel-aco/VRPTW_LuisAlejandroBaena_A-3B.xlsx')
csvs_to_excel('C:/Users/alejo/OneDrive - Universidad EAFIT/EAFIT/Semestre6/Heuristica/T2/T2V7/Aco/LocalSearch3F', 'excel-aco/VRPTW_LuisAlejandroBaena_A-3F.xlsx')