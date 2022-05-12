/* Questo file contiene le dichiarazioni delle funzioni crea_mappa(), gioca(), termina_gioco().
 Continene anche le definizioni dei tipi utilizzati in gamelib.c:
 struct Giocatore, struct Zona, enum Stato_giocatore, enum Tipo_zona, enum Tipo_oggetto. */

void crea_mappa(void);
void gioca(void);
void termina_gioco(void);

void start(void);

void input_num(void); 	//serve per ignorare i caratteri durante l'inserimento di in numero

typedef enum Tipo_zona {cucina, soggiorno, rimessa, strada, lungo_lago, uscita_campeggio} Tipo_zona;
typedef enum Stato_giocatore {morto, ferito, vivo} Stato_giocatore;
typedef enum Tipo_oggetto {cianfrusaglia, bende, coltello, pistola, benzina, adrenalina, vuoto} Tipo_oggetto;


typedef struct Giocatore{
	enum Stato_giocatore stato;
	unsigned short zaino[6];
	struct Zona* posizione;
} Giocatore;

typedef struct Zona{
	enum Tipo_zona zona;
	enum Tipo_oggetto oggetto;
	struct Zona* zona_successiva;
} Zona;
