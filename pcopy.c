/*  INF3173 - TP0 
 *  Session : automne 2021
 *  Tous les groupes
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>  
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void extraireCouple(int *offset,int *bytes, char *couple){//Fonction pour extraire les 2 int a partir du couple
    
    *offset = 0;
    *bytes = 0;
    int longueur = strlen(couple);
    int k = 0;
    bool tiret_atteint= false;
    for (int k = 0; k<longueur;k++){//On parcourt toute la chaine "couple"    
        if (couple[k] == '-'){
            tiret_atteint = true;
        }
        else{
            if (tiret_atteint== false){//Cas pour le offset
                *offset=*offset*10;
                *offset+= (couple[k]-'0');
            }
            if (tiret_atteint == true){//Cas pour bytes
                *bytes=*bytes*10;
                *bytes+= (couple[k]-'0');
            }
        }
    }
} 

char* extraireNom(char *path){//Fonction pour extraire le nom a partir du chemin
    
    bool nomSimple = true;
    for (int i = 0; i<strlen(path);i++){//On vérifie si le chemin est deja le nom du fichier
        if (path[i]== '/')
            nomSimple=false;
    }
    if (nomSimple == true)//Cas ou le chemin est deja le nom du fichier
        return path;
    else{//Cas ou il faut extraire le nom a partir du path
        char *tmp = strtok(path,"/");
        char *nomFinal;
        while (tmp!= NULL){
            nomFinal= tmp;
            tmp = strtok(NULL,"/");
        }
        return nomFinal;
    }
}
 
int main(int argc, char ** argv) {
    
    //Gestion des arguments
    int nb_fichiers= (argc-1)/2;
    char* PATH;
    char* couple;
    int OFFSET=0;
    int BYTES=0;
    int j=1;
    int total = 0;//Nombre d'octets copiés
    for(int i=1;i<=nb_fichiers;i++){ //Boucle pour parcourir tous les arguments
        PATH = argv[j];
        couple = argv[j+1];
        extraireCouple(&OFFSET, &BYTES, couple);
        j+=2;
        //Gestion du répertoire "copies"
        DIR* rep = opendir("copies");
        if (rep)
            ;//Le répertoire existe dejà, on ne fait rien
        else if (ENOENT == errno){//Le répertoire n'existe pas, on le crée
            int verif = mkdir("copies",0777);
            if (verif == -1)
                return 1;//La création du repertoire "copies" a échoué
        }
        //On ouvre le fichier source
        int filedesc= 5;//"File descriptor du fichier source"
        int newfiledesc = 6;//File descriptor du fichier destination"
        filedesc = open(PATH, O_RDONLY);
        if (filedesc < 0){
            printf("%d \n",total);
            return 1;//L'ouverture du fichier source a échoué
        }
        else {
            //Création du fichier destination
            PATH = extraireNom(PATH);
            char *pathNouv = malloc(strlen("copies/")+strlen(PATH));//Création du chemin vers le fichier destination
            strcat(pathNouv,"copies/");
            strcat(pathNouv,PATH);
            newfiledesc = open(pathNouv, O_WRONLY | O_APPEND | O_TRUNC | O_CREAT, 0644);
            if (newfiledesc < 0){
                printf("%d \n",total);
                return 1;//La création du fichier destination a échoué
            }
            else{
                int taille_fichier;
                taille_fichier=lseek(filedesc, 0,SEEK_END);//On calcule la taille du fichier source
                if (taille_fichier < 0)
                    return 1;//L'appel systeme "lseek" a échoué
                int retour_lseek = lseek(filedesc,0,SEEK_SET);
                if (retour_lseek < 0)
                    return 1;//L'appel systeme "lseek" a échoué
                if (OFFSET >= taille_fichier){//Cas ou OFFSET dépasse la taille du fichier
                    printf("%d \n",total);
                    return 1;
                }
                if (OFFSET + BYTES > taille_fichier)//Cas ou la position de OFFSET+BYTES dépasse la taille du fichier
                    BYTES = taille_fichier-OFFSET;//On adapte les BYTES pour que ça s'arrête à la fin du fichier
                retour_lseek = lseek(filedesc,OFFSET,SEEK_SET);//On positionne le pointeur
                if (retour_lseek < 0)
                    return 1;//L'appel systeme "lseek" a échoué
                char donees[4096];//D'après le sujet la taille maximale qui sera copiée est 4096 octets
                int octets_lus=0;
                octets_lus=read(filedesc,donees,BYTES);
                if (octets_lus < 0){
                    printf("%d \n",total);
                    return 1;//La fonction read a échoué
                }
                int octets_copies=0;
                octets_copies= write(newfiledesc,donees,BYTES);
                if (octets_copies < 0){
                    printf("%d \n",total);
                    return 1;//La fonction write a échoué
                }
                total+= octets_copies;
            }
        }
        int retour_close1 = close(filedesc);
        if (retour_close1 < 0){
            printf("%d \n",total);
            return 1;//L'appel systeme close a échoué
        }
        int retour_close2 = close(newfiledesc);
        if (retour_close2 < 0){
            printf("%d \n",total);
            return 1;//L'appel systeme close a échoué
        }
    }
    printf("%d\n",total);
    return 0;
}
