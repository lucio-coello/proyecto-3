#ifndef FUNCIONES_H
#define FUNCIONES_H

#define ZONAS 5
#define DIAS_HISTORICO 30

// Limites de contaminantes y clima
#define LIMITE_CO2 400
#define LIMITE_SO2 125
#define LIMITE_NO2 200
#define LIMITE_PM25 25
#define LIMITE_TEMPERATURA 35
#define LIMITE_VIENTO 10
#define LIMITE_HUMEDAD 80

// Estructura principal
struct clima {
    char nombre[100];
    float co2, so2, no2, pm25;
    float temperatura, viento, humedad;
    float historico_co2[DIAS_HISTORICO];
    float historico_so2[DIAS_HISTORICO];
    float historico_no2[DIAS_HISTORICO];
    float historico_pm25[DIAS_HISTORICO];
    int dias_ingresados;
};

// Prototipos de funciones
void limpiar_pantalla();
void monitoreo_contaminacion_actual(struct clima *zonas);
void mostrar_datos_zona(struct clima *zona);
void cargar_datos_desde_archivo(struct clima *zona, const char *nombre_archivo);
void prediccion_niveles_futuros(struct clima *zona);
void alertas_preventivas(struct clima *zona);
void calculo_promedios_historicos(struct clima *zona);
void generacion_recomendaciones(struct clima *zona);
void exportacion_datos(struct clima *zona, const char *nombre_archivo);
void guardar_datos_completos(struct clima *zona);
int verificar_formato_archivo(const char *nombre_archivo);
#endif