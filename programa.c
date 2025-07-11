#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "funciones.h"

void limpiar_pantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int validar_nombre_zona(const char *nombre) {
    for (int i = 0; i < strlen(nombre); i++) {
        char c = nombre[i];
        if (!(isalpha(c) || isdigit(c) || c == ' ')) {
            return 0;
        }
    }
    return 1;
}

void monitoreo_contaminacion_actual(struct clima *zona) {
    int opcion_de_submenus;

    do {
        limpiar_pantalla();
        printf("=== MONITOREO DE CONTAMINACION ACTUAL ===\n");
        printf("1. Ingresar datos manualmente\n");
        printf("2. Cargar datos desde archivo\n");
        printf("3. Mostrar datos actuales\n");
        printf("4. Guardar datos completos\n");
        printf("5. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion_de_submenus);

        switch (opcion_de_submenus) {
            case 1: {
                printf("\nIngrese los datos de la zona:\n");
                printf("Nombre: ");
                scanf(" %99[^\n]", zona->nombre);
                
                printf("CO2 (ppm): "); scanf("%f", &zona->co2);
                printf("SO2 (µg/m³): "); scanf("%f", &zona->so2);
                printf("NO2 (µg/m³): "); scanf("%f", &zona->no2);
                printf("PM2.5 (µg/m³): "); scanf("%f", &zona->pm25);
                printf("Temperatura (°C): "); scanf("%f", &zona->temperatura);
                printf("Viento (m/s): "); scanf("%f", &zona->viento);
                printf("Humedad (%%): "); scanf("%f", &zona->humedad);

                int dias_historicos;
                do {
                    printf("\nCuantos dias de datos historicos desea ingresar (1-%d)? ", DIAS_HISTORICO);
                    scanf("%d", &dias_historicos);
                } while (dias_historicos < 1 || dias_historicos > DIAS_HISTORICO);

                zona->dias_ingresados = dias_historicos;

                printf("\n=== INGRESO DE DATOS HISTORICOS ===\n");
                for (int i = 0; i < dias_historicos; i++) {
                    printf("\nDia %d:\n", i + 1);
                    printf("CO2 (ppm): "); scanf("%f", &zona->historico_co2[i]);
                    printf("SO2 (µg/m³): "); scanf("%f", &zona->historico_so2[i]);
                    printf("NO2 (µg/m³): "); scanf("%f", &zona->historico_no2[i]);
                    printf("PM2.5 (µg/m³): "); scanf("%f", &zona->historico_pm25[i]);
                }

                // Guardar archivo histórico
                time_t t = time(NULL);
                struct tm *tm_info = localtime(&t);
                char filename[150];
                snprintf(filename, sizeof(filename), "%s_%02d%02d%04d_historial.txt",
                         zona->nombre,
                         tm_info->tm_mday,
                         tm_info->tm_mon + 1,
                         tm_info->tm_year + 1900);
                
                FILE *archivo = fopen(filename, "w");
                if (archivo != NULL) {
                    fprintf(archivo, "Zona: %s\n", zona->nombre);
                    for (int i = 0; i < zona->dias_ingresados; i++) {
                        fprintf(archivo, "Dia %d - CO2: %.2f, SO2: %.2f, NO2: %.2f, PM2.5: %.2f\n",
                                i + 1,
                                zona->historico_co2[i],
                                zona->historico_so2[i],
                                zona->historico_no2[i],
                                zona->historico_pm25[i]);
                    }
                    fclose(archivo);
                    printf("\nDatos historicos guardados en: %s\n", filename);
                } else {
                    printf("\nError al guardar archivo historico.\n");
                }
                break;
            }
            case 2:
            {
                char archivo[100];
                printf("\nIngrese el nombre del archivo a cargar (ej: datos.txt): ");
                scanf(" %99s", archivo);

                if (!verificar_formato_archivo(archivo))
                {
                    printf("\nError: El archivo no tiene el formato correcto o no existe.\n");
                    printf("Por favor verifique que el archivo tenga el formato esperado.\n");
                }
                else
                {
                    cargar_datos_desde_archivo(zona, archivo);
                }
                break;
            }
            case 3:
                mostrar_datos_zona(zona);
                break;
            case 4:
                guardar_datos_completos(zona);
                break;
            case 5:
                printf("\nSaliendo del monitoreo de contaminacion actual.\n");
                break;
            default:
                printf("\nOpcion no valida. Intente nuevamente.\n");
        }

        printf("\nPresione Enter para continuar...");
        while (getchar() != '\n');
        getchar();

    } while (opcion_de_submenus != 5);
}
void cargar_datos_desde_archivo(struct clima *zona, const char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        printf("\nError: Archivo no encontrado o sin permisos\n");
        return;
    }

    char linea[256];
    int linea_num = 0;
    zona->dias_ingresados = 0;  // inicializar

    while (fgets(linea, sizeof(linea), archivo)) {
        linea_num++;

        // Ignorar comentarios (líneas que empiezan con #)
        if (linea[0] == '#') continue;

        // Eliminar salto de línea
        linea[strcspn(linea, "\n")] = 0;

        if (linea_num == 1) {
            // Línea 1: Nombre de la zona
            strncpy(zona->nombre, linea, sizeof(zona->nombre) - 1);
            zona->nombre[sizeof(zona->nombre) - 1] = '\0'; // seguridad
        }
        else if (linea_num == 2) {
            // Línea 2: Contaminantes CO2 SO2 NO2 PM2.5
            if (sscanf(linea, "%f %f %f %f", &zona->co2, &zona->so2, &zona->no2, &zona->pm25) != 4) {
                printf("Error en línea %d: Faltan valores de contaminantes\n", linea_num);
                fclose(archivo);
                return;
            }
        }
        else if (linea_num == 3) {
            // Línea 3: Clima
            if (sscanf(linea, "%f %f %f", &zona->temperatura, &zona->viento, &zona->humedad) != 3) {
                printf("Error en línea %d: Faltan valores climáticos\n", linea_num);
                fclose(archivo);
                return;
            }
        }
        else {
            // Líneas históricas: CO2 SO2 NO2 PM2.5
            if (zona->dias_ingresados >= DIAS_HISTORICO) break;

            float val1, val2, val3, val4;
            int leidos = sscanf(linea, "%f %f %f %f", &val1, &val2, &val3, &val4);
            if (leidos == 4) {
                zona->historico_co2[zona->dias_ingresados] = val1;
                zona->historico_so2[zona->dias_ingresados] = val2;
                zona->historico_no2[zona->dias_ingresados] = val3;
                zona->historico_pm25[zona->dias_ingresados] = val4;
                zona->dias_ingresados++;
            } else {
                printf("Advertencia: Línea %d ignorada, datos incompletos (%d campos)\n", linea_num, leidos);
            }
        }
    }

    fclose(archivo);
    printf("\nDatos cargados exitosamente desde %s\n", nombre_archivo);
    printf("Días históricos cargados: %d\n", zona->dias_ingresados);
}

void mostrar_datos_zona(struct clima *zona) {
    printf("\n=== DATOS ACTUALES DE LA ZONA: %s ===\n", zona->nombre);
    printf("Contaminantes:\n");
    printf("  CO2: %.2f ppm (Límite: %d)\n", zona->co2, LIMITE_CO2);
    printf("  SO2: %.2f µg/m³ (Límite: %d)\n", zona->so2, LIMITE_SO2);
    printf("  NO2: %.2f µg/m³ (Límite: %d)\n", zona->no2, LIMITE_NO2);
    printf("  PM2.5: %.2f µg/m³ (Límite: %d)\n", zona->pm25, LIMITE_PM25);
    
    printf("\nCondiciones climáticas:\n");
    printf("  Temperatura: %.2f °C (Límite: %d)\n", zona->temperatura, LIMITE_TEMPERATURA);
    printf("  Viento: %.2f m/s (Límite: %d)\n", zona->viento, LIMITE_VIENTO);
    printf("  Humedad: %.2f%% (Límite: %d)\n", zona->humedad, LIMITE_HUMEDAD);
    
    if (zona->dias_ingresados > 0) {
        printf("\nÚltimos %d días históricos:\n", zona->dias_ingresados);
        printf("Día\tCO2\tSO2\tNO2\tPM2.5\n");
        for (int i = 0; i < (zona->dias_ingresados > 5 ? 5 : zona->dias_ingresados); i++) {
            printf("%d\t%.2f\t%.2f\t%.2f\t%.2f\n", 
                   i+1, 
                   zona->historico_co2[i], 
                   zona->historico_so2[i], 
                   zona->historico_no2[i], 
                   zona->historico_pm25[i]);
        }
        if (zona->dias_ingresados > 5) {
            printf("...\n(mostrando 5 de %d días)\n", zona->dias_ingresados);
        }
    } else {
        printf("\nNo hay datos históricos registrados.\n");
    }
}

void prediccion_niveles_futuros(struct clima *zona) {
    if (zona->dias_ingresados == 0) {
        printf("\nError: No hay datos históricos para realizar la predicción.\n");
        return;
    }

    // Calcular promedios históricos
    float suma_co2 = 0, suma_so2 = 0, suma_no2 = 0, suma_pm25 = 0;
    for (int i = 0; i < zona->dias_ingresados; i++) {
        suma_co2 += zona->historico_co2[i];
        suma_so2 += zona->historico_so2[i];
        suma_no2 += zona->historico_no2[i];
        suma_pm25 += zona->historico_pm25[i];
    }

    float prom_co2 = suma_co2 / zona->dias_ingresados;
    float prom_so2 = suma_so2 / zona->dias_ingresados;
    float prom_no2 = suma_no2 / zona->dias_ingresados;
    float prom_pm25 = suma_pm25 / zona->dias_ingresados;

    // DEBUG opcional (puedes quitarlo)
    printf("\n=== DEBUG: PROMEDIOS ===\n");
    printf("CO2=%.2f SO2=%.2f NO2=%.2f PM2.5=%.2f\n", prom_co2, prom_so2, prom_no2, prom_pm25);

    // Factores de ajuste basados en condiciones climáticas actuales
    float factor_temp = (zona->temperatura - 25) / 20; // Normalizado alrededor de 25°C
    float factor_viento = zona->viento / 10;           // Normalizado para 10 m/s
    float factor_humedad = zona->humedad / 100;        // Normalizado a 100%

    // Ajustes corregidos: reducimos impacto del viento y garantizamos mínimo
    float factor_co2 = 1 + factor_temp * 0.5 - factor_viento * 0.1 + factor_humedad * 0.1;
    if (factor_co2 < 0.1) factor_co2 = 0.1;

    float factor_so2 = 1 + factor_temp * 0.3 - factor_viento * 0.1 + factor_humedad * 0.05;
    if (factor_so2 < 0.1) factor_so2 = 0.1;

    float factor_no2 = 1 + factor_temp * 0.4 - factor_viento * 0.1 + factor_humedad * 0.08;
    if (factor_no2 < 0.1) factor_no2 = 0.1;

    float factor_pm25 = 1 + factor_temp * 0.6 - factor_viento * 0.1 + factor_humedad * 0.15;
    if (factor_pm25 < 0.1) factor_pm25 = 0.1;

    // Predicciones finales
    float pred_co2 = prom_co2 * factor_co2;
    float pred_so2 = prom_so2 * factor_so2;
    float pred_no2 = prom_no2 * factor_no2;
    float pred_pm25 = prom_pm25 * factor_pm25;

    printf("\n=== PREDICCION PARA LAS PROXIMAS 24 HORAS ===\n");
    printf("Zona: %s (basado en %d días históricos)\n", zona->nombre, zona->dias_ingresados);
    printf("Condiciones actuales:\n");
    printf("  Temperatura: %.2f°C, Viento: %.2f m/s, Humedad: %.2f%%\n", 
           zona->temperatura, zona->viento, zona->humedad);

    printf("\nNiveles estimados:\n");
    printf("  CO2: %.2f ppm (Límite: %d) - %s\n", 
           pred_co2, LIMITE_CO2, pred_co2 > LIMITE_CO2 ? "ALERTA" : "Normal");
    printf("  SO2: %.2f µg/m³ (Límite: %d) - %s\n", 
           pred_so2, LIMITE_SO2, pred_so2 > LIMITE_SO2 ? "ALERTA" : "Normal");
    printf("  NO2: %.2f µg/m³ (Límite: %d) - %s\n", 
           pred_no2, LIMITE_NO2, pred_no2 > LIMITE_NO2 ? "ALERTA" : "Normal");
    printf("  PM2.5: %.2f µg/m³ (Límite: %d) - %s\n", 
           pred_pm25, LIMITE_PM25, pred_pm25 > LIMITE_PM25 ? "ALERTA" : "Normal");
}

void alertas_preventivas(struct clima *zona) {
    printf("\n=== ALERTAS PREVENTIVAS PARA %s ===\n", zona->nombre);
    
    int alertas = 0;
    
    if (zona->co2 > LIMITE_CO2) {
        printf("ALERTA: Niveles de CO2 (%.2f ppm) superan el límite (%d ppm)\n", zona->co2, LIMITE_CO2);
        alertas++;
    }
    if (zona->so2 > LIMITE_SO2) {
        printf("ALERTA: Niveles de SO2 (%.2f µg/m³) superan el límite (%d µg/m³)\n", zona->so2, LIMITE_SO2);
        alertas++;
    }
    if (zona->no2 > LIMITE_NO2) {
        printf("ALERTA: Niveles de NO2 (%.2f µg/m³) superan el límite (%d µg/m³)\n", zona->no2, LIMITE_NO2);
        alertas++;
    }
    if (zona->pm25 > LIMITE_PM25) {
        printf("ALERTA: Niveles de PM2.5 (%.2f µg/m³) superan el límite (%d µg/m³)\n", zona->pm25, LIMITE_PM25);
        alertas++;
    }
    if (zona->temperatura > LIMITE_TEMPERATURA) {
        printf("ALERTA: Temperatura (%.2f°C) supera el límite (%d°C)\n", zona->temperatura, LIMITE_TEMPERATURA);
        alertas++;
    }
    if (zona->viento > LIMITE_VIENTO) {
        printf("ALERTA: Velocidad del viento (%.2f m/s) supera el límite (%d m/s)\n", zona->viento, LIMITE_VIENTO);
        alertas++;
    }
    if (zona->humedad > LIMITE_HUMEDAD) {
        printf("ALERTA: Humedad (%.2f%%) supera el límite (%d%%)\n", zona->humedad, LIMITE_HUMEDAD);
        alertas++;
    }
    
    if (alertas == 0) {
        printf("No se detectaron niveles peligrosos en esta zona.\n");
    }
}

void calculo_promedios_historicos(struct clima *zona) {
    if (zona->dias_ingresados == 0) {
        printf("\nError: No hay datos históricos para calcular promedios.\n");
        return;
    }

    float suma_co2 = 0, suma_so2 = 0, suma_no2 = 0, suma_pm25 = 0;
    float max_co2 = zona->historico_co2[0], min_co2 = zona->historico_co2[0];
    float max_so2 = zona->historico_so2[0], min_so2 = zona->historico_so2[0];
    float max_no2 = zona->historico_no2[0], min_no2 = zona->historico_no2[0];
    float max_pm25 = zona->historico_pm25[0], min_pm25 = zona->historico_pm25[0];
    
    for (int i = 0; i < zona->dias_ingresados; i++) {
        suma_co2 += zona->historico_co2[i];
        suma_so2 += zona->historico_so2[i];
        suma_no2 += zona->historico_no2[i];
        suma_pm25 += zona->historico_pm25[i];
        
        if (zona->historico_co2[i] > max_co2) max_co2 = zona->historico_co2[i];
        if (zona->historico_co2[i] < min_co2) min_co2 = zona->historico_co2[i];
        
        if (zona->historico_so2[i] > max_so2) max_so2 = zona->historico_so2[i];
        if (zona->historico_so2[i] < min_so2) min_so2 = zona->historico_so2[i];
        
        if (zona->historico_no2[i] > max_no2) max_no2 = zona->historico_no2[i];
        if (zona->historico_no2[i] < min_no2) min_no2 = zona->historico_no2[i];
        
        if (zona->historico_pm25[i] > max_pm25) max_pm25 = zona->historico_pm25[i];
        if (zona->historico_pm25[i] < min_pm25) min_pm25 = zona->historico_pm25[i];
    }

    printf("\n=== PROMEDIOS HISTORICOS PARA %s ===\n", zona->nombre);
    printf("Período: %d días\n\n", zona->dias_ingresados);
    
    printf("CONTAMINANTE\tPROMEDIO\tMÁXIMO\t\tMÍNIMO\t\tLÍMITE\n");
    printf("CO2\t\t%.2f ppm\t%.2f ppm\t%.2f ppm\t%d ppm\n", 
           suma_co2 / zona->dias_ingresados, max_co2, min_co2, LIMITE_CO2);
    printf("SO2\t\t%.2f µg/m³\t%.2f µg/m³\t%.2f µg/m³\t%d µg/m³\n", 
           suma_so2 / zona->dias_ingresados, max_so2, min_so2, LIMITE_SO2);
    printf("NO2\t\t%.2f µg/m³\t%.2f µg/m³\t%.2f µg/m³\t%d µg/m³\n", 
           suma_no2 / zona->dias_ingresados, max_no2, min_no2, LIMITE_NO2);
    printf("PM2.5\t\t%.2f µg/m³\t%.2f µg/m³\t%.2f µg/m³\t%d µg/m³\n", 
           suma_pm25 / zona->dias_ingresados, max_pm25, min_pm25, LIMITE_PM25);
    
    printf("\nANÁLISIS:\n");
    if (suma_co2 / zona->dias_ingresados > LIMITE_CO2) 
        printf("- CO2: Promedio histórico supera el límite aceptable\n");
    if (suma_so2 / zona->dias_ingresados > LIMITE_SO2) 
        printf("- SO2: Promedio histórico supera el límite aceptable\n");
    if (suma_no2 / zona->dias_ingresados > LIMITE_NO2) 
        printf("- NO2: Promedio histórico supera el límite aceptable\n");
    if (suma_pm25 / zona->dias_ingresados > LIMITE_PM25) 
        printf("- PM2.5: Promedio histórico supera el límite aceptable\n");
}

void generacion_recomendaciones(struct clima *zona) {
    printf("\n=== RECOMENDACIONES PARA %s ===\n", zona->nombre);
    
    int recomendaciones = 0;
    
    if (zona->co2 > LIMITE_CO2 || 
        (zona->dias_ingresados > 0 && 
         zona->historico_co2[zona->dias_ingresados-1] > LIMITE_CO2)) {
        printf("- Reducir el uso de vehículos de combustión interna\n");
        printf("- Fomentar el uso de transporte público, bicicletas o caminatas\n");
        printf("- Optimizar rutas de transporte para reducir congestionamiento\n");
        recomendaciones++;
    }
    
    if (zona->so2 > LIMITE_SO2 || 
        (zona->dias_ingresados > 0 && 
         zona->historico_so2[zona->dias_ingresados-1] > LIMITE_SO2)) {
        printf("- Regular actividades industriales que emiten SO2\n");
        printf("- Implementar sistemas de filtrado en fábricas\n");
        printf("- Promover el uso de combustibles con bajo contenido de azufre\n");
        recomendaciones++;
    }
    
    if (zona->no2 > LIMITE_NO2 || 
        (zona->dias_ingresados > 0 && 
         zona->historico_no2[zona->dias_ingresados-1] > LIMITE_NO2)) {
        printf("- Controlar emisiones de vehículos diésel\n");
        printf("- Implementar horarios escalonados para reducir tráfico\n");
        printf("- Promover vehículos eléctricos o híbridos\n");
        recomendaciones++;
    }
    
    if (zona->pm25 > LIMITE_PM25 || 
        (zona->dias_ingresados > 0 && 
         zona->historico_pm25[zona->dias_ingresados-1] > LIMITE_PM25)) {
        printf("- Restringir actividades al aire libre en horas críticas\n");
        printf("- Implementar riego de calles para reducir polvo en suspensión\n");
        printf("- Controlar emisiones de construcción y obras públicas\n");
        recomendaciones++;
    }
    
    if (zona->temperatura > LIMITE_TEMPERATURA) {
        printf("- Habilitar centros de enfriamiento para población vulnerable\n");
        printf("- Promover hidratación constante\n");
        printf("- Evitar actividades físicas intensas en horas de mayor calor\n");
        recomendaciones++;
    }
    
    if (recomendaciones == 0) {
        printf("No se requieren acciones especiales en este momento.\n");
        printf("Se recomienda mantener monitoreo continuo y prácticas sostenibles.\n");
    }
}

void exportacion_datos(struct clima *zona, const char *nombre_archivo) {
    char archivo_final[150];
    if (strstr(nombre_archivo, ".txt") == NULL) {
        snprintf(archivo_final, sizeof(archivo_final), "%s.txt", nombre_archivo);
    } else {
        strncpy(archivo_final, nombre_archivo, sizeof(archivo_final));
    }

    FILE *archivo = fopen(archivo_final, "w"); // abrimos en modo escritura para que quede limpio
    if (archivo == NULL) {
        printf("\nError: No se pudo abrir el archivo %s\n", archivo_final);
        return;
    }

    // Línea 1: Nombre de la zona
    fprintf(archivo, "%s\n", zona->nombre);

    // Línea 2: Contaminantes actuales
    fprintf(archivo, "%.2f %.2f %.2f %.2f\n", zona->co2, zona->so2, zona->no2, zona->pm25);

    // Línea 3: Datos climáticos
    fprintf(archivo, "%.2f %.2f %.2f\n", zona->temperatura, zona->viento, zona->humedad);

    // Líneas siguientes: Datos históricos
    for (int i = 0; i < zona->dias_ingresados; i++) {
        fprintf(archivo, "%.2f %.2f %.2f %.2f\n",
            zona->historico_co2[i],
            zona->historico_so2[i],
            zona->historico_no2[i],
            zona->historico_pm25[i]);
    }

    fclose(archivo);
    printf("\nDatos exportados correctamente en formato compatible a %s\n", archivo_final);
}
void guardar_datos_completos(struct clima *zona) {
    char archivo[150];
    snprintf(archivo, sizeof(archivo), "%s_completo.txt", zona->nombre);
    FILE *f = fopen(archivo, "w");

    if (!f) {
        printf("\nError: No se pudo crear el archivo %s\n", archivo);
        return;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    
    fprintf(f, "=== DATOS COMPLETOS DE ZONA ===\n");
    fprintf(f, "Fecha: %02d/%02d/%d %02d:%02d:%02d\n",
            tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900,
            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    fprintf(f, "Zona: %s\n\n", zona->nombre);
    
    fprintf(f, "[DATOS ACTUALES]\n");
    fprintf(f, "CO2: %.2f ppm\n", zona->co2);
    fprintf(f, "SO2: %.2f µg/m³\n", zona->so2);
    fprintf(f, "NO2: %.2f µg/m³\n", zona->no2);
    fprintf(f, "PM2.5: %.2f µg/m³\n", zona->pm25);
    fprintf(f, "Temperatura: %.2f °C\n", zona->temperatura);
    fprintf(f, "Viento: %.2f m/s\n", zona->viento);
    fprintf(f, "Humedad: %.2f%%\n\n", zona->humedad);
    
    fprintf(f, "[DATOS HISTORICOS - %d días]\n", zona->dias_ingresados);
    fprintf(f, "Día\tCO2\tSO2\tNO2\tPM2.5\n");
    for (int i = 0; i < zona->dias_ingresados; i++) {
        fprintf(f, "%d\t%.2f\t%.2f\t%.2f\t%.2f\n", 
                i+1, 
                zona->historico_co2[i], 
                zona->historico_so2[i], 
                zona->historico_no2[i], 
                zona->historico_pm25[i]);
    }
    
    fprintf(f, "\n=== FIN DE ARCHIVO ===\n");
    fclose(f);
    printf("\nDatos guardados correctamente en %s\n", archivo);
}
int verificar_formato_archivo(const char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) return 0;

    char linea[256];

    // Nombre
    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        fclose(archivo); return 0;
    }

    // Contaminantes
    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        fclose(archivo); return 0;
    }

    int tokens = 0;
    char *token = strtok(linea, " ,;|\t\n");
    while (token) {
        tokens++;
        token = strtok(NULL, " ,;|\t\n");
    }

    if (tokens < 4) {
        fclose(archivo); return 0;
    }

    // Clima
    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        fclose(archivo); return 0;
    }

    tokens = 0;
    token = strtok(linea, " ,;|\t\n");
    while (token) {
        tokens++;
        token = strtok(NULL, " ,;|\t\n");
    }

    if (tokens < 3) {
        fclose(archivo); return 0;
    }

    fclose(archivo);
    return 1;
}
