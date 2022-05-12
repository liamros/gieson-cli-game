#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "gamelib.h"
#define CAPIENZA 6		//capienza zaino


static void ins_zona(void);
static void canc_zona(void);
static void stampa_mappa(void);
static void chiudi_mappa(short* k);

static void file_mappa(char* file);
static void salvataggio(char* file);

static void avanza(int* cerca_g, int* cerca_m, int* prendi_g, int* prendi_m);
static void cerca_oggetto(int* cerca_g, int* cerca_m);
static void prendi_oggetto(int* cerca_g, int* cerca_m, int* prendi_g, int* prendi_m);
static void cura(void);
static void usa_adrenalina(int turno, int* cerca_g, int* cerca_m, int* prendi_g, int* prendi_m);

static void prime_zone(void);
static void libera_memoria(void);			//libera la mem dinamica
static void assegna_zona(Zona* qualsiasi_zona, int numero_zona);  //verifica se l'inserimento da tastiera sia corretto
static void assegna_oggetto(Zona* qualsiasi_zona);		//assegna l'oggetto in modo randomico
static char* mostra_oggetto(unsigned int oggetto);  //verifica il tipo di oggetto e restituisce la stringa
static char* mostra_zona(unsigned int zona);	//verifica il tipo di zona e restituisce la stringa
static char* mostra_stato(unsigned int stato);
static void mostra_zaino(void);

static void verifica_stato(int* x, int* k);
static void cambia_turno(short* mossa, int x);		//cambia turno del player in gioca()
static void stampa_menu(int cerca_g, int cerca_m);		//stampa il menu e le info in gioca()
static void gieson(void);
static void incontro(void);
static void gas(int* benzina);
static void spara(int gun);
static void difesa(int knife);
static void nascondi(void);
static void verifica_zaino(int* cerca, int* prendi);
static void premi_invio(void);
static void reset(void);		//alla fine della partita resetta i valori di giacomo e marzia
static void end(void);

static Zona* prima_zona = NULL;
static Zona* ultima_zona = NULL;

static Giocatore* player = NULL;		//serve per gioca()


static Giocatore Marzia = {
	vivo,
	{adrenalina, adrenalina, vuoto, vuoto, vuoto, vuoto},
	NULL
};
static Giocatore Giacomo = {
	vivo,
	{coltello, vuoto, vuoto, vuoto, vuoto, vuoto},
	NULL,
};

static int const prob[6][6] = {{30, 50, 90, 0, 0, 100}, {20, 30, 40, 70, 0, 100}, {20, 30, 60, 0, 90, 100}, {80, 0, 90, 0, 100, 0}, {70, 0, 80, 0 ,100, 0}, {90, 0, 100, 0, 0, 0}};  //prima dimensione zone, seconda dimensione oggetti

static int n = 0;		//numero di zone desiderate


void crea_mappa(void)
{
	time_t t;
	srand((unsigned) time(&t));


	char v = 'n';
	while(n != 0)
	{
		printf("Eliminare la mappa precedente (s/n)? ");
		scanf("%c", &v);
		getchar();
		if(v != 's' && v != 'n')
		{
			printf("Valore inserito non valido\n");		//da rivedere
			continue;
		}
		break;
	}


	while(n == 0 && fopen("save.txt", "r") != NULL)
	{
		char v;
		printf("Utilizzare la mappa salvata nella partita scorsa (s/n)? ");
		scanf("%c", &v);
		getchar();
		if(v == 's')
		{
			salvataggio("save.txt");
			premi_invio();
			break;
		}
		else if(v == 'n')
		{
			premi_invio();
			break;
		}
			else
			{
				printf("Valore inserito non valido\n");
				premi_invio();
			}
		}


	if(n == 0)
		prime_zone();
	else if (v == 's')
		{
			libera_memoria();
			prime_zone();
		}


	short k = 0;
	while(k != 4)
	{
		printf("\nMENU MAPPA\n\n");
		printf("Scegliere opzione:\n1) Inserisci una zona\n2) Cancella una zona\n3) Stampa mappa\n4) Torna al menu principale\n\nInput: ");
		scanf("%hd", &k);
		input_num();
		switch(k)
		{
			case 1:
				ins_zona();
				break;
			case 2:
				canc_zona();
				break;
			case 3:
				stampa_mappa();
				break;
			case 4:
				chiudi_mappa(&k);
				break;
			default:
				printf("Valore inserito non valido, ritenta\n\n");
				input_num();
		}
		premi_invio();
	}
}




static void prime_zone(void)
{
	do
	{
		printf("Inserisci il numero di Zone desiderate = ");
		scanf("%d", &n);
		input_num();
		if(n < 0)
			printf("Impossibile, ritenta\n\n");
	}while(n < 0);
	if(n == 0)
	{
		premi_invio();
		return;
	}
	if(n == 1)
	{
		prima_zona = (Zona*) malloc(sizeof(Zona));
		ultima_zona = prima_zona;
		ultima_zona->zona = uscita_campeggio;
		printf("\nZona assegnata automaticamente a uscita campeggio\n\n");
		premi_invio();
		return;
	}
	Zona* temp = NULL;
	for(int i = 1; i <= n; i++)  //inizio creazione mappe
	{
		if( i == 1)
		{
			temp = (Zona*) malloc(sizeof(Zona));
			assegna_zona(temp, i);  //verifica se il numero è corretto e gli assegna la zona
			assegna_oggetto(temp);
			prima_zona = temp;
			ultima_zona = prima_zona;
			temp->zona_successiva = NULL;
		}
		else if (i > 1)
		{
			temp->zona_successiva = (Zona*) malloc(sizeof(Zona));
			ultima_zona = temp->zona_successiva;
			temp = temp->zona_successiva;
			if(i < n)
			{
				assegna_zona(temp, i);
				assegna_oggetto(temp);
			}
			temp->zona_successiva = NULL;
		}
		if(i == n)
		 printf("\nUltima zona assegnata automaticamente a USCITA CAMPEGGIO\n");
	}
	ultima_zona->zona = uscita_campeggio;
	assegna_oggetto(ultima_zona);
	premi_invio();
}


static void assegna_zona(Zona* temp, int i)
{
	printf("\nScelta tipo delle zone: 0) cucina, 1) soggiorno, 2) rimessa, 3) strada, 4) lungo_lago\n");
	printf("Tipo %d° zona = ", i);
	scanf("%u", &temp->zona);
	input_num();
	while(temp->zona < 0 || temp->zona > 4)  //verifica se il valore di temp->zona sia corretto
	{
		printf("Valore non valido, ritenta -> ");
		scanf("%u", &temp->zona);
		input_num();
	}
}


static void assegna_oggetto(Zona* temp)
{
	int k = rand() % 100 + 1;	//variabile da cui dipende assegnazione oggetto
	for(int i = 0; i < 6; i++)   //assegna l'oggetto ad ogni zona
	{
		if(i != temp->zona)
			continue;
		for(int j = 0; j < 6; j++)
		{
			if(k <= prob[i][j])		//prob ha i valori della probabilità, se k è minore, viene assegnato j e rompe il ciclo
			{
				temp->oggetto = j;
				break;
			}
		}
	}
}

static void libera_memoria(void)
{
	Zona* temp = NULL;
	for(int i = 1; i <= n; i++)
	{
		temp = prima_zona;
		prima_zona = prima_zona->zona_successiva;
		free(temp);
	}
	temp = NULL;
	prima_zona = NULL;
	ultima_zona = NULL;
}

static void ins_zona(void)
{
	if(n == 0)
	{
		prima_zona = (Zona*) malloc(sizeof(Zona));
		ultima_zona = prima_zona;
		ultima_zona->zona = uscita_campeggio;
		n++;
		printf("\nInserita la prima zona e assegnata automaticamente a USCITA CAMPEGGIO\n\n");
		return;
	}
	int x = 0;
	Zona* temp = NULL;
	printf("In quale posizione desideri inserire la zona? ");
	scanf("%d", &x);
	input_num();
	if(x < 1 || x > n+1)
	{
		printf("\nImpossibile\n\n");
		return;
	}
	else if(x == 1)		//inserimento testa
		{
			temp = (Zona*) malloc(sizeof(Zona));
			temp->zona_successiva = prima_zona;
			prima_zona = temp;
			assegna_zona(prima_zona, x);
			assegna_oggetto(prima_zona);
		}
		else if(x == n+1)	//inserimento coda
			{
				temp = (Zona*) malloc(sizeof(Zona));
				ultima_zona->zona_successiva = temp;
				printf("\nZona inserita in ultima posizione e assegnata automaticamente a USCITA CAMPEGGIO\n");
				assegna_zona(ultima_zona, x-1);
				assegna_oggetto(ultima_zona);
				ultima_zona = temp;
				ultima_zona->zona = 5;
				ultima_zona->zona_successiva = NULL;
			}
			else	//inserimento interno
			{
				Zona* ptr = NULL;
				temp = prima_zona;
				for(int i = 2; i < x; i++)
					temp = temp->zona_successiva;
				ptr = temp->zona_successiva;
				temp->zona_successiva = NULL;
				temp->zona_successiva = (Zona*) malloc(sizeof(Zona));
				temp = temp->zona_successiva;
				temp->zona_successiva = ptr;
				assegna_zona(temp, x);
				assegna_oggetto(temp);
			}
	n++;
}


static void canc_zona(void)
{
	if(n == 0)
	{
		printf("Non ci sono zone da cancellare\n");
		premi_invio();
		return;
	}
	if(n == 1)
	{
		printf("Prima zona cancellata\n");
		free(prima_zona);
		prima_zona = NULL;
		ultima_zona = NULL;
		n--;
		premi_invio();
		return;
	}
	int x = 0;
	Zona* temp = NULL;
	printf("Quale zona desideri cancellare? ");
	scanf("%d", &x);
	input_num();
	if(x < 1 || x > n)
	{
		printf("\nImpossibile\n");
		premi_invio();
		return;
	}
	else if(x == 1)		//elimina testa
		{
			temp = prima_zona->zona_successiva;
			free(prima_zona);
			prima_zona = temp;
			printf("\nPrima zona cancellata\n");
		}
		else if(x == n)	//elimina coda
			{
				temp = prima_zona;
				for(int i = 2; i < n; i++)
					temp = temp->zona_successiva;
				free(ultima_zona);
				ultima_zona = temp;
				ultima_zona->zona = uscita_campeggio;
				assegna_oggetto(ultima_zona);
				ultima_zona->zona_successiva = NULL;
				printf("\nUltima zona cancellata, e penultima assegnata a USCITA CAMPEGGIO\n");
			}
			else	//elimina interno
			{
				Zona* dopo = prima_zona;
				temp = prima_zona;
				for(int i = 1; i < x; i++)		//i=1 perchè temp è già uguale a prima_zona
					temp = temp->zona_successiva;
				for(int i = 1; i <= x; i++)
					dopo = dopo->zona_successiva;
				free(temp);
				temp = prima_zona;
				for(int i = 2; i < x; i++)
					temp = temp->zona_successiva;
				temp->zona_successiva = dopo;
				printf("\n%d° zona cancellata\n", x);
			}
	n--;
}


static void stampa_mappa(void)
{
	if(n == 0)
		puts("\nNessuna zona presente\n\n");
	Zona* temp = NULL;
	temp = prima_zona;
	for(int i = 1; i <= n; i++)
	{
		printf("\n%d° zona -> %s oggetto -> %s\n\n", i, mostra_zona(temp->zona), mostra_oggetto(temp->oggetto));
		temp = temp->zona_successiva;
	}
}

static char* mostra_oggetto(unsigned int oggetto)
{
	if(oggetto == 0)
		return("CIANFRUSAGLIA");
	if(oggetto == 1)
		return("BENDE");
	if(oggetto == 2)
		return("COLTELLO");
	if(oggetto == 3)
		return("PISTOLA");
	if(oggetto == 4)
		return("BENZINA");
	if(oggetto == 5)
		return("ADRENALINA");
	if(oggetto == 6)
		return("VUOTO");
	return("ERRORE");
}


static char* mostra_zona(unsigned int zona)
{
	if(zona == 0)
		return("CUCINA");
	if(zona == 1)
		return("SOGGIORNO");
	if(zona == 2)
		return("RIMESSA");
	if(zona == 3)
		return("STRADA");
	if(zona == 4)
		return("LUNGO_LAGO");
	if(zona == 5)
		return("USCITA_CAMPEGGIO");
	return("ERRORE");
}

static char* mostra_stato(unsigned int stato)
{
	if(stato == 0)
		return("MORTO");
	if(stato == 1)
		return("FERITO");
	if(stato == 2)
		return("VIVO");
	return("ERRORE");
}

static void mostra_zaino(void)
{
	printf("Contenuto Zaino:\n");
	for(int i = 0; i < 6; i++)
		printf("%d) %s\n", i, mostra_oggetto(player->zaino[i]));
	putchar('\n');
}

static void chiudi_mappa(short* k)
{
	int x = 1;
	Zona* temp = NULL;
	temp = prima_zona;
	while (temp->zona_successiva != NULL)
	{
		temp = temp->zona_successiva;
		x++;
	}
	if(n == 0)
	{
		printf("Non sono state create zone\n\n");
		*k = 0;
	}
	else if(x < 8)
		{
			printf("\nLe zone sono meno di 8, non avere paura, mettine di più!\n\n");
			*k = 0;
		}
		else if(ultima_zona->zona != 5)
			{
				printf("\nL'ultima zona non è USCITA CAPEGGIO -> %u\n\n", ultima_zona->zona);
				*k = 0;
			}
    file_mappa("save.txt");		//salva la mappa creata in un file
}


void gioca(void)
{
	if(n < 8)
	{
		printf("Impossibile, non hai creato la mappa\n\n");
		return;
	}
	system("killall vlc");
	system("cvlc soundtrack/Halloween\\ Theme.mp3 &");
	printf("\e[31m");
	sleep(2);
	printf("\nCOMINCIANO I GIOCHI!\n\n");
	Giacomo.posizione = prima_zona;
	Marzia.posizione = prima_zona;
	sleep(2);
	int turno = 0;		//contatore dei turni
	int k = 1;
	int cerca_g = 0;	//prendi e cerca verifica se si è già cercato o preso l'oggetto
	int prendi_g = 0;
	int cerca_m = 0;	// _g verifica giacomo, _m verifica marzia
	int prendi_m = 0;
	while(k == 1)		//loop che termina quando la partita finisce
	{
		turno++;
		int x = rand() % 2 + 1;		//determina chi inizierà il turno
		if(x == 1)
			player = &Marzia;		//player indica il giocatore che svolge il turno
		else
			player = &Giacomo;
		verifica_stato(&x, &k);  //gestisce i loop in base ai morti e gestisce le condizioni di vittoria/sconfitta
		short mossa = 0;
		while(x <= 2)	//loop che termina alla fine dei 2 turni dei giocatori, x = 1 turno del primo, x = 2 turno del secondo
		{
			system("clear");
			printf("%d° Turno\n\n", turno);
			cambia_turno(&mossa, x);	 //gestisce il turno del player in base alla x e ad altre condizioni
			stampa_menu(cerca_g, cerca_m);		//stampa il menu e le info
			scanf("%hd", &mossa);
			input_num();
			switch(mossa)	//la x varia solo se il giocatore fa una mossa valida
			{
				case 1:
					avanza(&cerca_g, &cerca_m, &prendi_g, &prendi_m);
					x++;
					break;
				case 2:
					cerca_oggetto(&cerca_g, &cerca_m);
					x++;
					break;
				case 3:
					prendi_oggetto(&cerca_g, &cerca_m, &prendi_g, &prendi_m);
					x++;
					break;
				case 4:
					mostra_zaino();
					premi_invio();
					break;
				case 5:
					cura();
					x++;
					break;
				case 6:
					usa_adrenalina(turno, &cerca_g, &cerca_m, &prendi_g, &prendi_m);
					x++;
					break;
				default:
					printf("Valore non valido\n\n");
					premi_invio();
			}
		}
	}

	reset();		//resetta i personaggi e riassegna gli oggetti alle zone

	sleep(2);
	system("killall vlc");
	system("cvlc soundtrack/Michael\\ Myers\\ Theme\\ Song.mp3&");
	sleep(2);
	printf("\033[1;32m");
}

static void verifica_stato(int* x, int* k)		//x controlla i loop delle mosse, k il loop dei turni
{
	if(Giacomo.stato == morto && Marzia.stato == morto)
	{
		printf("Giacomo e Marzia sono morti\n\nHAI PERSO\n\n");
		*k = 0;
		*x = 3;
		premi_invio();
		system("clear");
	}
	else if(Giacomo.posizione == NULL && Marzia.posizione == NULL)
		{
			printf("Giacomo e Marzia stanno in salvo\n\nCongratulazioni, HAI VINTO!\n\n");
			*k = 0;
			*x = 3;
			premi_invio();
		}
		else if(Giacomo.posizione == NULL && Marzia.stato == morto)
			{
				printf("Giacomo è in salvo, Marzia è morta\n\nPotev fare meglio, PARTITA FINITA\n\n");
				*k = 0;
				*x = 3;
				premi_invio();
			}
			else if (Giacomo.stato == morto && Marzia.posizione == NULL)
			{
				printf("Marzia è in salvo, Giacomo è morto\n\nPotevi fare meglio, PARTITA FINITA\n\n");
				*k = 0;
				*x = 3;
				premi_invio();
			}
				else if(Giacomo.stato == morto || Marzia.stato == morto)
						*x = 2;
					else if(Marzia.posizione == NULL || Giacomo.posizione == NULL)
							*x = 2;
						else
							*x = 1;
}


static void cambia_turno(short* p, int x)		//gestisce il puntatore player
{
	if(x == 1 && player == &Marzia)
		printf("Prima mossa: Marzia\n\n");
	else if(x == 1 && player == &Giacomo)
			printf("Prima mossa: Giacomo\n\n");
		else if(x == 2 && player == &Marzia && Giacomo.stato != morto && Marzia.stato != morto)
			{
				if(*p == 4)			//p punta a scelta e se è 4 si è scelto mostra_zaino(), quindi player non deve cambiare
					printf("Seconda mossa: Marzia\n\n");
				else
				{
					printf("Seconda mossa: Giacomo\n\n");		//al cambio del turno il puntatore player cambia giocatore
					player = &Giacomo;
				}
			}
			else if(x == 2 && player == &Giacomo && Giacomo.stato != morto && Marzia.stato != morto)
				{
					if(*p == 4)
						printf("Seconda mossa: Giacomo\n\n");
					else
					{
						printf("Seconda mossa: Marzia\n\n");
						player = &Marzia;
					}
				}
	if(Giacomo.stato == morto)
	{
		printf("Giacomo è morto, turno di Marzia\n\n");
		player = &Marzia;
	}
	if(Marzia.stato == morto)
	{
		printf("Marzia è morta, turno di Giacomo\n\n");
		player = &Giacomo;
	}
	if(Giacomo.posizione == NULL)
	{
		printf("Giacomo è in salvo, turno di Marzia\n\n");
		player = &Marzia;
	}
	if(Marzia.posizione == NULL)
	{
		printf("Marzia è in salvo, turno di Marzia\n\n");
		player = &Giacomo;
	}
}

static void stampa_menu(int cerca_g, int cerca_m)
{
	printf("MENU MOSSE\n\nScegliere una mossa tra:\n");		//stampa il menu
	printf("1) Avanza\n2) Cerca oggetto nella zona\n3) Prendi oggetto trovato\n4) Mostra zaino (mossa gratuita)\n");
	printf("5) Curati\n6) Usa adrenalina\n\n");
	printf("Zona attuale: %s\n", mostra_zona(player->posizione->zona));
	if(player == &Giacomo && cerca_g != 0)
		printf("Oggetto presente nella Zona: %s\n", mostra_oggetto(player->posizione->oggetto));
	if(player == &Marzia && cerca_m != 0)
		printf("Oggetto presente nella Zona: %s\n", mostra_oggetto(player->posizione->oggetto));
	printf("Il tuo stato: %s\n\n", mostra_stato(player->stato));
	printf("Input: ");
}

static void gieson(void)
{
	int prob = 30;
	static int benzina = 0;
	gas(&benzina);		//verifica se è posseduta benzina e in caso incrementa benzina di 8 o di 4 (4 turni = 8 o 4 mosse)
	if(benzina > 0)		//da rivedere, metterlo in funzione del turno
	{
		prob = 0;
		benzina--;
		benzina == 0 ? printf("Effetto della benzina terminato\n\n") : printf("Effetto benzina per ancora %d mossa/e\n\n", benzina);
		premi_invio();
		return;
	}
	else if(player->posizione->zona == uscita_campeggio)
			prob = 75;
		else if(Giacomo.stato == morto || Marzia.stato == morto || Giacomo.posizione == NULL || Marzia.posizione == NULL)
				prob = 50;

	for(int i = 0; i < 3; i++)
	{
		sleep(1);
		printf("\n...\n\n");
	}
	sleep(2);

	int jason = rand() % 100 + 1;
	if(jason <= prob)
		incontro();
	else
	{
		premi_invio();
		return;
	}
}

static void incontro(void)
{
	printf("\nÈ APPARSO GIESON!!!\n\n");
	short knife = 0;
	short gun = 0;
	for(int i = 0; i < 6; i++)
	{
		if(player->zaino[i] == coltello)
			knife++;
		if(player->zaino[i] == pistola)
			gun++;
	}
	printf("Nello zaino hai:\npistole = %hd\ncoltelli = %hd\n\nScegli una mossa:\n", gun, knife);
	printf("1) SPARA!\n2) DIFENDITI CON IL COLTELLO\n3) NASCONDITI\n\nInput: ");
	short scelta = 0;
	scanf("%hd", &scelta);
	input_num();
	switch(scelta)
	{
		case 1:
			spara(gun);
			break;
		case 2:
			difesa(knife);
			break;
		case 3:
			nascondi();
			break;
		default:
			printf("\nSei rimasto fermo a girarti i pollici... Gieson ti ha visto e il resto è storia... complimenti\n\n");
			player->stato = morto;
	}
	premi_invio();
}

static void gas(int* nafta)
{
	int k = 0;
	static int num_vivi = 2;	//verifica se muore un giocatore
	int ver = num_vivi;		//serve per verificare se un giocatore è morto nel turno stesso

	if ((Giacomo.stato == morto || Marzia.stato == morto) && num_vivi == 2)
		num_vivi--;


	for(int i = 0; i < CAPIENZA; i++)
	{
		if(player->zaino[i] == benzina)
		{
			k++;
			player->zaino[i] = vuoto;
			break;
		}
	}
	if(k > 0 && num_vivi < 2)
		*nafta += 4;
	else if(k > 0)
			*nafta += 8;

	if(num_vivi != ver)		//un giocatore non dovrebbe mai morire con la benzina attiva, ma non si sa mai
		*nafta /= 2;

}


static void spara(int gun)
{
	if(gun == 0)
	{
		printf("Cerchi di prendere la tua pistola, e ben presto scopri di non avercela\n");
		printf("La scena sarebbe esilerante se non fosse per Gieson che ti è arrivato davanti pronto a macellarti..\n\n");
		printf("GIESON TI HA UCCISO\n\n");
		player->stato = morto;
		sleep(3);
		return;
	}
	int i = 0;
	while(player->zaino[i] != pistola)
		i++;
	player->zaino[i] = 0;
	printf("BANG!");
	sleep(1);
	printf("\n\nGIESON È STATO COLPITO E SPARISCE\nPer questa volta la passi liscia...\n\n");
	sleep(3);
	return;
}

static void difesa(int knife)
{
	if(knife == 0)
	{
		printf("Con mai vista convinzione affronti Gieson, una volta davanti a lui estrai il coltello...\n");
		printf("È proprio quando scopri di non avercelo che capisci che sarebbe stato meglio nascondersi...\n\n");
		printf("GIESON TI HA UCCISO\n\n");
		player->stato = morto;
		sleep(3);
		return;
	}
	if(player->stato == ferito)
	{
		printf("Ritenti la bravata di prima ma questa volta non funziona, Gieson ti disarma e il resto si conosce...\n\n");
		printf("Eri già ferito -> GIESON TI HA UCCISO\n\n");
		player->stato = morto;
		sleep(3);
		return;
	}
	int i = 0;
	while(player->zaino[i] != coltello)
		i++;
	player->zaino[i] = 0;
	printf("Gieson si avvicina e ti afferra!\n\n");
	sleep(1);
	printf("ZING!\n\n");
	sleep(1);
	printf("LO COLPISCI CON IL COLTELLO E TI LIBERI!\nGieson sparisce, ma tu sei conciato male...\n\n");
	player->stato = ferito;
	sleep(3);
	return;
}

static void nascondi(void)
{
	int prob = rand() % 100 + 1;
	if(prob <= 20)
	{
		printf("Con uno scatto felpato riesci a nasconderti nei paraggi, Gieson si guarda intorno perplesso per poi sparire\n\n");
		printf("Esci dal nascondiglio incredulo e paranoico..\n\n");
		sleep(3);
		return;
	}
	else
	{
		printf("Con uno scatto felpato riesci a nasconderti nei paraggi, Gieson si guarda intorno perplesso per poi sparire\n\n");
		printf("Esci dal nascondiglio incredulo e paranoico..\n\n");
		sleep(3);
		printf("Gieson ti appare alle spalle, e con tutta la sua furia fa il suo lavoro\n\nGIESON TI HA UCCISO\n\n");
		sleep(3);
		player->stato = morto;
	}
}

static void avanza(int* cerca_g, int* cerca_m, int* prendi_g, int* prendi_m)
{
	if(player == &Giacomo)
	{
		*cerca_g = 0;
		*prendi_g = 0;
	}
	if(player == &Marzia)
	{
		*cerca_m = 0;
		*prendi_m = 0;
	}
	player->posizione = player->posizione->zona_successiva;
	if(player->posizione == NULL)
	{
		printf("SEI FUGGITO!\n\n");
		return;
	}
	printf("Stai avanzando in %s...\n", mostra_zona(player->posizione->zona));
	gieson();
}

static void cerca_oggetto(int* cerca_g, int* cerca_m)
{
	if(player == &Giacomo)
	{
		if(*cerca_g == 0)
		{
			printf("Hai trovato %s\n\n", mostra_oggetto(player->posizione->oggetto));
			(*cerca_g)++;
		}
		else
			printf("Cerchi ancora per altri oggetti, e non trovi nulla, forse era meglio fare altro...\n\n");
	}
	if(player == &Marzia)
	{
		if(*cerca_m == 0)
		{
			printf("Hai trovato %s\n\n", mostra_oggetto(player->posizione->oggetto));
			(*cerca_m)++;
		}
		else
			printf("Cerchi ancora per altri oggetti, e non trovi nulla, forse era meglio fare altro...\n\n");
	}
	gieson();
}

static void verifica_zaino(int* cerca, int* prendi)		//verifica se nella zona attuale si ha già cercato/preso l'Oggetto
{														//e verifica se lo zaino è pieno, in caso affermativo da la possibilità
														//di sostituire un oggetto
	if(*cerca == 0)
	{
		printf("La tua immaginazione non ha limiti, raccogli una bacchetta magica, la prossima volta cerca prima\n\n");
		return;
	}
	if(*prendi > 0)
	{
		printf("Hai già raccolto l'oggetto, accontentati\n\n");
		return;
	}
	if(player->posizione->oggetto == vuoto)
	{
		printf("Non c'è nulla da prendere, la zona è vuota\n\n");
		return;
	}
	int spazio_libero = 0;
	if(*cerca > 0 && *prendi == 0)
	{
		for(int i = 0; i < 6; i++)
			if(player->zaino[i] == 6)
			{
				spazio_libero++;
				player->zaino[i] = player->posizione->oggetto;
				player->posizione->oggetto = 6;
				printf("Hai raccolto %s\n\n", mostra_oggetto(player->zaino[i]));
				(*prendi)++;
				return;
			}
		if(spazio_libero == 0)
		{
			printf("Non hai spazio nello zaino, vuoi sostituire qualcosa (s/n)? ");
			char c;
			scanf("\n%c", &c);
			getchar();
			while(c != 's' && c != 'n')
			{
				printf("Valore non valido, ripetere inserimento: ");
				scanf("\n%c", &c);
				getchar();
			}
			if(c == 's')
			{
				int i = 0;
				mostra_zaino();
				printf("Cosa vuoi sostituire (Inserisci num)? ");
				scanf("%d", &i);
				input_num();
				while(i < 0 || i > 5)
				{
					printf("Valore non valido, ripetere inserimento: ");
					scanf("%d", &i);
					input_num();
				}
				printf("Hai posato %s, ", mostra_oggetto(player->zaino[i]));
				printf(" e hai raccolto %s\n\n", mostra_oggetto(player->posizione->oggetto));
				int temp = player->zaino[i];
				player->zaino[i] = player->posizione->oggetto;
				player->posizione->oggetto = temp;
				(*prendi)++;
				return;
			}
			else
				return;
			}
		}
}

static void prendi_oggetto(int* cerca_g, int* cerca_m, int* prendi_g, int* prendi_m)
{
	if(player == &Giacomo)
		verifica_zaino(cerca_g, prendi_g);
	if(player == &Marzia)
		verifica_zaino(cerca_m, prendi_m);
	gieson();
}

static void cura(void)
{
    int heal = 0;
	int i = 0;
	for(i = 0; i < CAPIENZA; i++)
	{
		if(player->zaino[i] == bende)
		{
			heal++;
			break;
		}
	}
	if(heal != 0 && player->stato == ferito)
	{
		player->zaino[i] = vuoto;
		player->stato = vivo;
		printf("Avvolgi le bende intorno alla ferita\n\nStato: FERITO -> VIVO\n\n");
	}
	else if(heal == 0)
			printf("Non hai bende, bel tentativo\n\n");
		else if(player->stato == vivo && heal != 0)
				{
					printf("Avvolgi le bende intorno al braccio, e mentre lo fai ti chiedi il perchè\n\n");
					printf("Stato: VIVO -> MOLTO VIVO\n\n");
					player->zaino[i] = vuoto;
				}
	gieson();
}

static void usa_adrenalina(int turno, int* cerca_g, int* cerca_m, int* prendi_g, int* prendi_m)
{
	int k = 0;
	for(int i = 0; i < CAPIENZA; i++)
	{
		if(player->zaino[i] == adrenalina)
		{
			k++;
			player->zaino[i] = vuoto;
			break;
		}
	}
	if(k == 0)
	{
		printf("Non hai adrenalina, controlla lo zaino la prossima volta\n\n");
		gieson();
		return;
	}

	printf("Ti inietti l'ADRENALINA\n\n");
	gieson();
	if(player->stato == morto)		//necessario perchè nella funz non ci sono le verifiche dello stato
		return;
	int x = 1;
	short mossa = 0;
	while(x <= 2)	//loop che termina alla fine dell'effetto dell'adrenalina
		{
			system("clear");
			printf("%d° Turno\n\n", turno);
			if(player == &Giacomo)
				printf("Giacomo è sotto effetto di ADRENALINA\n\n");
			else
				printf("Marzia è sotto effetto di ADRENALINA\n\n");
			stampa_menu(*cerca_g, *cerca_m);
			scanf("%hd", &mossa);
			input_num();
			switch(mossa)
			{
				case 1:
					avanza(cerca_g, cerca_m, prendi_g, prendi_m);
					x++;
					break;
				case 2:
					cerca_oggetto(cerca_g, cerca_m);
					x++;
					break;
				case 3:
					prendi_oggetto(cerca_g, cerca_m, prendi_g, prendi_m);
					x++;
					break;
				case 4:
					mostra_zaino();
					premi_invio();
					break;
				case 5:
					cura();
					x++;
					break;
				case 6:
					usa_adrenalina(turno, cerca_g, cerca_m, prendi_g, prendi_m);
					x++;
					break;
				default:
					printf("Valore non valido\n\n");
			}
			if(player->stato == morto)
				return;
		}
}

void termina_gioco(void)
{
	libera_memoria();
	system("killall vlc");
	system("clear");
	end();
	sleep(3);
	system("clear");
}

static void premi_invio(void)
{
	printf("\nPremi INVIO per continuare");
	while (getchar()!='\n');
	system("clear");
}

static void reset(void)
{
	Marzia.stato = vivo;
	Marzia.posizione = NULL;
	Marzia.zaino[0] = adrenalina;
	Marzia.zaino[1] = adrenalina;
	for(int i = 2; i <= CAPIENZA; i++)
		Marzia.zaino[i] = vuoto;

	Giacomo.stato = vivo;
	Giacomo.posizione = NULL;
	Giacomo.zaino[0] = coltello;
	for(int i = 1; i <= CAPIENZA; i++)
		Giacomo.zaino[i] = vuoto;

	Zona* temp = prima_zona;
	for(int i = 1; i <= n; i++)
	{
		assegna_oggetto(temp);
		temp = temp->zona_successiva;
	}
}

void start(void)
{
	printf("\e[31m");
	printf("|||  |||  |||  ||||||  |||      |||||||  ||||||||  |||||||||||||  ||||||     |||||||  ||||||||     \n");
	usleep(100*1000);
	printf("|||  |||  |||  |||     |||      |||      |||  |||  |||  |||  |||  |||          |||    |||  |||     \n");
	usleep(100*1000);
	printf("|||  |||  |||  ||||||  |||      |||      |||  |||  |||  |||  |||  ||||||       |||    |||  |||     \n");
	usleep(100*1000);
	printf("|||  |||  |||  |||     |||      |||      |||  |||  |||  |||  |||  |||          |||    |||  |||     \n");
	usleep(100*1000);
	printf("|||||||||||||  ||||||  |||||||  |||||||  ||||||||  |||  |||  |||  ||||||       |||    ||||||||     \n");
	usleep(100*1000);
	putchar('\n');
	usleep(100*1000);
	putchar('\n');
	usleep(100*1000);
	printf("||||||  |||||||||  |||  ||||||     |||||||||  |||   |||     |||||||  |||   |||  ||||||\n");
	usleep(100*1000);
	printf("|||     |||   |||  |||  |||  |||   |||   |||  |||   |||       |||    |||   |||  |||   \n");
	usleep(100*1000);
  	printf("||||||  |||||||||  |||  |||   |||  |||||||||  |||||||||       |||    |||||||||  ||||||\n");
  	usleep(100*1000);
	printf("|||     ||| |||    |||  |||  |||   |||   |||     |||          |||    |||   |||  |||   \n");
	usleep(100*1000);
	printf("|||     |||   |||  |||  ||||||     |||   |||     |||          |||    |||   |||  ||||||\n");
	usleep(100*1000);
	putchar('\n');
	usleep(100*1000);
	putchar('\n');
	printf("  |||    ||||||     |||     |||     \n");
	usleep(100*1000);
	printf("|||||       |||  |||||||||  |||     \n");
	usleep(100*1000);
	printf("  |||    ||||||     |||     ||||||||\n");
	usleep(100*1000);
	printf("  |||       |||     |||     |||  |||\n");
	usleep(100*1000);
	printf("|||||||  ||||||     ||||||  |||  |||\n");
	usleep(100*1000);
	putchar('\n');
	usleep(100*1000);
	putchar('\n');
	usleep(100*1000);
	printf("\033[1;32m");
	premi_invio();
	system("clear");
}

static void end(void)
{
	printf("\e[31m");
	printf("  ▄████  ▄▄▄       ███▄ ▄███▓▓█████     ▒█████   ██▒   █▓▓█████  ██▀███  \n");
	usleep(100*1000);
	printf(" ██▒ ▀█▒▒████▄    ▓██▒▀█▀ ██▒▓█   ▀    ▒██▒  ██▒▓██░   █▒▓█   ▀ ▓██ ▒ ██▒\n");
	usleep(100*1000);
	printf("▒██░▄▄▄░▒██  ▀█▄  ▓██    ▓██░▒███      ▒██░  ██▒ ▓██  █▒░▒███   ▓██ ░▄█ ▒\n");
	usleep(100*1000);
	printf("░▓█  ██▓░██▄▄▄▄██ ▒██    ▒██ ▒▓█  ▄    ▒██   ██░  ▒██ █░░▒▓█  ▄ ▒██▀▀█▄  \n");
	usleep(100*1000);
	printf("░▒▓███▀▒ ▓█   ▓██▒▒██▒   ░██▒░▒████▒   ░ ████▓▒░   ▒▀█░  ░▒████▒░██▓ ▒██▒\n");
	usleep(100*1000);
	printf(" ░▒   ▒  ▒▒   ▓▒█░░ ▒░   ░  ░░░ ▒░ ░   ░ ▒░▒░▒░    ░ ▐░  ░░ ▒░ ░░ ▒▓ ░▒▓░\n");
	usleep(100*1000);
	printf("  ░   ░   ▒   ▒▒ ░░  ░      ░ ░ ░  ░     ░ ▒ ▒░    ░ ░░   ░ ░  ░  ░▒ ░ ▒░\n");
	usleep(100*1000);
	printf("░ ░   ░   ░   ▒   ░      ░      ░      ░ ░ ░ ▒       ░░     ░     ░░   ░ \n");
	usleep(100*1000);
	printf("      ░       ░  ░       ░      ░  ░       ░ ░        ░     ░  ░   ░     \n");
	usleep(100*1000);
	printf("                                                     ░                   \n");
}


static void file_mappa(char* file)
{
    FILE* punt;
    punt = fopen(file, "w");
    Zona* temp = prima_zona;
    for(int i = 0; i < n; i++)
    {
        fprintf(punt, "%s\n", mostra_zona(temp->zona));
        temp = temp->zona_successiva;
    }
    fclose(punt);
}

static void salvataggio(char* file)
{
	FILE* punt;
	punt = fopen(file, "r");
	Zona* temp = NULL;
	while(fgetc(punt) != EOF)
	{

		if(fgetc(punt) == EOF)
		{
			fclose(punt);
			printf("\nMappa caricata\n");
			return;
		}

		if(n == 0)
		{
			fseek(punt, -1, SEEK_CUR);
			prima_zona = (Zona*) malloc(sizeof(Zona));
			temp = prima_zona;
			ultima_zona = prima_zona;
		}
		else
		{
			temp->zona_successiva = (Zona*) malloc(sizeof(Zona));
			temp = temp->zona_successiva;
			ultima_zona = temp;
		}

		fseek(punt, -1, SEEK_CUR);
		n++;		//numero di zone, variabile globale

		char string[12];
		fscanf(punt, "%s", string);
		int x = 0;
		printf("%s\n", string);
		for(x = 0; x < 6 ; x++)
		{
			if(strcmp(string, mostra_zona(x)) == 0)
				break;
		}
		temp->zona = x;
		assegna_oggetto(temp);
	}
	if(n == 0)
	{
		free(prima_zona);
		prima_zona = NULL;
		printf("\nNon esiste un salvataggio precedente\n");
	}
}

void input_num(void)
{
	char kill[50];
	fgets(kill, sizeof(kill), stdin);
}
