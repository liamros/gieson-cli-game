#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gamelib.h"



int main (void) {
	system("cvlc soundtrack/Michael\\ Myers\\ Theme\\ Song.mp3&");
	sleep(2);
	start();
    short scelta = 0;
    printf("WELCOME TO FRIDAT THE 13th\n\n");
    while(scelta != 3)
    {
    	printf("MENU PRINCIPALE\n\n");
    	printf("Inserire :\n1) Crea Mappa\n2) Gioca\n3) Esci\n\nInput: ");
    	scanf("%hd", &scelta);
		input_num();
    	if(scelta < 1 || scelta > 3)
    	{
    		printf("Ripetere inserimento, numero non valido\n\n");
    		continue;
    	}
	   	switch(scelta)
    	{
    		case 1:
    				crea_mappa();
    				break;
    		case 2:
    				gioca();
    				break;
    		case 3:
    				termina_gioco();
    				break;
    	}
    }
    return 0;
}
