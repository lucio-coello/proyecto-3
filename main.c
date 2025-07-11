#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

int main() {
    struct clima zonas[ZONAS];
    memset(zonas, 0, sizeof(zonas));

    int opcion;
    int zona_actual = 0;

    do {
        printf("\n=== SISTEMA DE GESTION DE CONTAMINACION DEL AIRE ===\n");
        printf("1. Monitoreo de contaminacion actual\n");
        printf("2. Prediccion de niveles futuros\n");
        printf("3. Alertas preventivas\n");
        printf("4. Calculo de promedios historicos\n");
        printf("5. Generacion de recomendaciones\n");
        printf("6. Exportacion de datos\n");
        printf("7. Cambiar zona activa (actual: %d)\n", zona_actual + 1);
        printf("8. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        limpiar_pantalla();
        switch (opcion) {
            case 1:
                monitoreo_contaminacion_actual(&zonas[zona_actual]);
                break;
            case 2:
                prediccion_niveles_futuros(&zonas[zona_actual]);
                break;
            case 3:
                alertas_preventivas(&zonas[zona_actual]);
                break;
            case 4:
                calculo_promedios_historicos(&zonas[zona_actual]);
                break;
            case 5:
                generacion_recomendaciones(&zonas[zona_actual]);
                break;
            case 6: {
                int subopcion;
                printf("\n--- Submenu Exportacion de Datos ---\n");
                printf("1. Mostrar archivos de zonas disponibles\n");
                printf("2. Agregar datos a un archivo existente\n");
                printf("3. Exportar como un archivo nuevo\n");
                printf("Seleccione una opcion: ");
                scanf("%d", &subopcion);

                if (subopcion == 1) {
                    system("dir /b *.txt");
                } else if (subopcion == 2 || subopcion == 3) {
                    char nombre_archivo[100];
                    printf("Ingrese el nombre del archivo (sin .txt): ");
                    scanf(" %99s", nombre_archivo);
                    char archivo_final[150];
                    snprintf(archivo_final, sizeof(archivo_final), "%s.txt", nombre_archivo);

                    if (subopcion == 3) {
                        FILE *nuevo = fopen(archivo_final, "w");
                        if (nuevo != NULL) fclose(nuevo);
                    }

                    exportacion_datos(&zonas[zona_actual], archivo_final);
                } else {
                    printf("Opcion invalida.\n");
                }
                break;
            }
            case 7:
                printf("Ingrese el numero de zona activa (1 a %d): ", ZONAS);
                int temp;
                scanf("%d", &temp);
                if (temp >= 1 && temp <= ZONAS)
                    zona_actual = temp - 1;
                else
                    printf(" Zona invalida.\n");
                break;
            case 8:
                printf(" Saliendo del programa...\n");
                break;
            default:
                printf(" Opcion invalida. Intente nuevamente.\n");
        }

        printf("\nPresione Enter para continuar...");
        while (getchar() != '\n'); // Limpiar buffer
        getchar(); // Esperar Enter

    } while (opcion != 8);

    return 0;
}