#include "util.h"
#include "aes.h"

void chiffrement_AES_ModeCompteur(char* data,unsigned int nbChar, unsigned char nonce[16], unsigned char Key[16])
{
	
	unsigned char bloc[16],nonce_cipher[16];
	unsigned char cypherKey[4][4];
	unsigned char keySchedule[4][44];        
	unsigned char *pt_courant;
	int i, nbBlocs,nbReste;
	
	for(i=0; i< 4; i++)
		memcpy(&cypherKey[i], &Key[i*4],4);
	
	KeyExpansion (cypherKey,keySchedule);



	//initialisations
	pt_courant = (unsigned char *)data;
	
	nbBlocs = nbChar/16;
	nbReste = nbChar%16;
	while(nbBlocs >0)
	{
		//chiffrer le compteur avec AES et la clé de chiffrement	
		
		eas_encryption(nonce,keySchedule,nonce_cipher);
		//recuperer un bloc de 128 bits
		memcpy(bloc,pt_courant,16);
		
		
		//XOR
		for(i=0; i< 16; i++)
			bloc[i] = nonce_cipher[i] ^ bloc[i];
		
		memcpy(pt_courant,bloc,16);
		pt_courant=&pt_courant[16];
		
		//incrémentation de la nonce
		for(i=0; i< 16; i++)
			if(++nonce[i] != 0)break;
			
		nbBlocs--;
	}
	if(nbReste != 0)
	{
		//chiffrer le compteur avec AES et la clé de chiffrement
		eas_encryption(nonce,keySchedule,nonce_cipher);
		
		//recuperer un bloc de 128 bits
		memcpy(bloc,pt_courant,nbReste);
		
		//XOR
		for(i=0; i< 16; i++)
			bloc[i] = nonce_cipher[i] ^ bloc[i];
		memcpy(pt_courant,bloc,nbReste);
	}
	
	
			
}


int main(int argc , char **argv)
{
	unsigned int i, taille, op;
	char *data;
	struct stat s;
	FILE * f= NULL;
	unsigned char cle[16];
	unsigned char nonce[16];	
	char fichier[50];
	
	printf("\n");
	//test des arguents et analyse
	if(argc >5 || argc <4)
	{
		printf("-----------Utilisation----------------\n\n");
		printf(" %s F O C [N]\n",argv[0]);
		
		printf("F : le  fichier à chiffrer ou à déchiffrer\n");
		printf("O : l'opération voulue (1 pour chiffrer et 2 pour déchiffrer)\n");
		printf("C : le  fichier contenant la cle de (dé)chiffrement\n");
		printf("N : Dans le cas d'un déchiffrement (2), c'est le  fichier contenant la nonce\n");
		
		printf("\n");
		exit(EXIT_FAILURE);
		
	}
	
	
	
	f = fopen(argv[3],"rb");
	
	if(f == NULL)
		erreur("Ouverture du fichier devant contenir la cle");
	else if(fread(cle,1,16,f)!=16)
		erreur("Lecture dans le fichier contenant la cle de chiffrement / Contenu");
	
	op = atoi(argv[2]);
	
	if(op == 1)//préparation pour le chiffrement
	{
		if(argc != 4)
			erreur("Le nombre d'argument n'est pas compatible avec l'opération demandée");
		
		
		srand(time(NULL));
		for(i=0; i< 16; i++)
			nonce[i] = (unsigned char)rand();
		
		f = fopen("nonce.txt","wb");
		if(f == NULL)
			erreur("Création du fichier qui contiendra la nonce");
		if(fwrite(nonce,1,16,f) != 16)
			erreur("Ecriture dans le fichier qui contiendra la nonce");
		fclose(f);
		
	}
	else if(op == 2)//préparation pour le déchiffrement
	{
		if(argc != 5)
			erreur("Le nombre d'argument n'est pas compatible avec l'opération demandée");
		
		f = fopen(argv[4],"rb");
		if(f == NULL) 
			erreur("Ouverture du fichier  contenant la nonce");
		if(fread(nonce,1,16,f)!=16)
			erreur("Lecture dans le fichier contenant la nonce / Contenu");
		fclose(f);
		
	}
        
        //on récupère des information sur le fichier dont sa taille
	if(stat(argv[1],&s)==-1)
	erreur("Echec: Impossible de déterminer la taille du fichier ");
	
	taille = s.st_size + 10;
	
	if(taille == 1)//fichier vide
		erreur("Echec: Ce fichier est vide\n");
		
	data = malloc(taille*sizeof(char));
	if(data == NULL)
		erreur("Allocation mémoire pour la chaine devant contenir le fichier");
	
	//on récupère le fichier à chiffrer ou déchiffrer, effectue
	// l'opération et le stock dans un fichier dédié
	lire(argv[1],data,taille);
	chiffrement_AES_ModeCompteur(data,taille,nonce, cle);
	if(creation_fichier(argv[1],op,fichier)== 1)
		ecrire(data,taille,argv[1]);
	else
		ecrire(data,taille,fichier);
	
	//succes de l'opération
	if(op == 1)
	{
		printf("fin du chiffrement\n");
		printf("vous trouverez le chiffré dans %s ou dans le source pour le \".png\"\n",fichier);
		printf("vous trouverez la nonce dans le fichier nonce.txt\n");
	}
	else
	{
		printf("fin du déchiffrement\n");
		printf("vous trouverez le déchiffré dans %s ou dans le source pour .png\n",fichier);
	}
	
	printf("\n");
	exit(EXIT_SUCCESS);
}
