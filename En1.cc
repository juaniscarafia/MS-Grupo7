/* Modelo ControlStock1 - Un Servidor, Una Cola  */


#include "simlib.c"
#include <stdio.h>
#include <stdlib.h> 


#define Llegada_host					1  /* Tipo de Evento 1: Llegada pedido de host      */
#define Llegada_srv						2  /* Tipo de Evento 1: Llegada pedido de servidor  */
#define Reinicio_General				3  /* Tipo de Evento 3: Reinicio General			*/
#define Liberar_Canal					4  /* Tipo de Evento 3: Liberar Canal de red		*/
#define Red								1  /* Lista 1: Servidor								*/
#define Cola							2  /* Lista 2: Cola									*/
#define Cola_General					3  /* Lista 3: Cola									*/
#define Demora_cola						1  /* Sampst 1: Demora en cola						*/

/* Declaraci¢n de variables propias */
float media_interarribo_host,media_interarribo_srv;, media_interarribo_error,media_servicio_host;
float media_servicio_srv, media_servicio_host, media_servicio_srv, media_interarribo_error;
int hostusados [6];


/* Declaraci¢n de Funciones propias */
void inicializa(void);
void llegada_host(void);
void llegada_srv(void);
void reinicio_general(void);
void liberar_canal(void);
void reporte(void);


int main()  /* Main function. */
{
	/* Apertura de Archivos que sean necesarios */

	media_interarribo_host = 1,2;
	media_interarribo_srv = 3,7;
	media_interarribo_error = 0,5;
	media_servicio_host = 2,4;
	media_servicio_srv = 3,7;
	///Error cada 3 o 4 hs
	
	/* Initializar Simlib */
	init_simlib();


	/* Establecer maxatr = M ximo n£mero de Atributos utilizados  */
	maxatr = 4;

	/* Initializar el Sistema */
	inicializa();

	/* Ejecutar la simulaci¢n. */
	while (sim_time < 2000)
	{
		/* Determinar pr¢ximo Evento */
		timing();

		/* Invoke the appropriate event function. */
		switch( next_event_type )
		{
			case Llegada_host:
				llegada_host();
				break;
			case Llegada_srv:
				llegada_srv();
				break;
			case Reinicio_General:
				reinicio_general();
				break;	
			case Liberar_Canal:
				liberar_canal();
				break;
        }
	}

	/* Llamada al Reporte para mostrar los resultados */
	reporte();
	system("pause");
}


void inicializa(void)  /* Inicializar el Sistema */
{
	/* Se cargan los primeros perdidos y reinicio en la Lista de Eventos */

	transfer[1] = sim_time + expon(media_interarribo_host, Llegada_host);
	transfer[2] = Llegada_host;
	list_file(INCREASING,LIST_EVENT);

	transfer[1] = sim_time + expon(media_interarribo_srv, Llegada_srv);
	transfer[2] = Llegada_srv;
	list_file(INCREASING, LIST_EVENT);

	transfer[1] = sim_time + expon(media_interarribo_error, Reinicio_General);
	transfer[2] = Reinicio_General;
	list_file(INCREASING, LIST_EVENT);
}

void llegada_host(void)
{
	float numerohost;
	bool esta = false;
	numerohost = uniform(1,6,1);
	int x = (int) numerohost;
	transfer[1] = sim_time + expon(media_interarribo_host, Llegada_host);
	transfer[2] = Llegada_host;
	list_file(INCREASING,LIST_EVENT);
	
	for (int c = 1; c == 6; c++)
	{
		if (hostusados[c] == x)
		{
			esta = true;
		}
	}
	if (esta == false)
	{
		hostusados[x] = x;
		if (list_size[Red]<3)
		{
			transfer[3] = x;
			list_file(LAST, Red);
			sampst(0.0, Demora_cola);
		
			transfer[1] = sim_time + expon(media_servicio_host, Liberar_Canal);
			transfer[2] = Liberar_Canal;
			transfer[3] = x;
			list_file(INCREASING,LIST_EVENT);
		}
		else
		{
			transfer[1] = sim_time;
			transfer[3] = x;
			list_file(LAST, Cola);
		}
	}
}

void llegada_srv(void)
{
	if (list_size[Red]==1)
	{
		transfer[3] = 7;
		list_file(FIRST, Red);
		list_file(FIRST, Red);
		sampst(0.0, Demora_cola);
		
		transfer[1] = sim_time + expon(media_servicio_srv, Liberar_Canal);
		transfer[2] = Liberar_Canal;
		transfer[3] = 7;
		list_file(INCREASING,LIST_EVENT);
	}
	else if (list_size[Red]==2)
	{
		list_remove(FIRST, Red);

		if (transfer[3]==7)
		{
			list_remove(FIRST, Cola);
			while (transfer[3] == 7)
			{
				list_file(FIRST, Cola_General);
				if (list_size[Cola] > 0)
					list_remove(FIRST, Cola);
				else
					transfer[3] = 8;
			}
			if (transfer[3] == 8)
			{
				transfer[1] = sim_time;
				transfer[3] = 7;
				list_file(FIRST, Cola);
				
			
				while (list_size[Cola_General] > 0)
				{
					list_remove(LAST, Cola_General);
					list_file(FIRST, Cola);
				}
			}else
			{
				if (transfer[3] =! 7)
				{
					list_file(FIRST, Cola);

					transfer[1] = sim_time;
					transfer[3] = 7;
					list_file(FIRST, Cola);

					while (list_size[Cola_General] > 0)
					{
						list_remove(LAST, Cola_General);
						list_file(FIRST, Cola);
					}

				}
			}
			
		}
	}else
	{
		/// FALTA SI OCUPAN 3 O NINGUNO xD
	}
}

void liberar_canal(void)
{	

}

void reinicio_general(void)
{

}

void reporte( void )  /* Generar Reporte de la Simulaci¢n */
{

	/* Mostrar Par metros de Entrada */

	/* -------- Por pantalla -------- */

	printf( "Sistema Control_Stock - Simulaci¢n con Simlib \n\n" );
	printf( "Media de Interarribos          : %8.3f minutos\n", media_interarribos);
	printf( "Media de Servicios             : %8.3f minutos\n", media_servicio);
	printf( "Cantidad de Clientes Demorados : %i \n\n", num_demandas);

	/* Calcular los Estad¡sticos */

	/* Estad¡sticos Escalaras - Sampst */
	sampst(0.0,-Demora_cola);
	printf( "\nDemora en Cola                 : %f \n ",transfer[1]);


	/* Estad¡sticos Temporales - Timest y Filest */

	filest(Cola);
	printf( "\nN£mero Promedio en Cola        : %f \n ",transfer[1]);
	filest(Red);
	printf( "\nUtilizaci¢n Servidor           : %f \n ",transfer[1]);
	printf( "\nTiempo Final de Simulation     : %10.3f minutes\n", sim_time );


		/* Estad¡sticos Temporales - Timest y Filest */

	filest(Cola);
	printf( "\nN£mero Promedio en Cola        : %f \n ",transfer[1]);
	filest(Red);
	printf( "\nUtilizaci¢n Servidor           : %f \n ",transfer[1]);
	printf( "\nSimulation end time            : %10.3f minutes\n", sim_time );
}


