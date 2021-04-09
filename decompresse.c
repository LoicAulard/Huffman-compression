#include "decompresse.h"
#define CESURE_1 32

uint64_t decompress(FILE* encoded_file, FILE* file)
{
    fseek(file,0,SEEK_SET);
    fseek(encoded_file,0,SEEK_SET);


    uint64_t nb_elements=0;
    uint64_t cesures[4];
    uint64_t status;



    //extraction nombre de caractères(en octets) dans le corps du fichier
    fread(&nb_elements,sizeof(uint64_t),1,encoded_file);

    //extraction des césures du fichier
    fread(cesures,sizeof(uint64_t),4,encoded_file);

    uint64_t nb_caracteres_alphabet = cesures[1];
    uint8_t* buffer_pos = (uint8_t*)malloc(nb_caracteres_alphabet);
    uint64_t* buffer_car = (uint64_t*)malloc(nb_caracteres_alphabet * sizeof(uint64_t));

    //extraction positions de l'alphabet encodé
    for(int i=0;i<nb_caracteres_alphabet;i++)
    {
        fread(buffer_pos+i,sizeof(uint8_t),1,encoded_file);
    }

    //extraction de l'alphabet encodé
    for(int i=0;i<  nb_caracteres_alphabet  ; i++  )
    {
        status = read_bit(encoded_file,buffer_pos[i],buffer_car+i);
    }
    
    read_bit(file,RESET,NULL);

    //extraction de la taille des blocs

    uint16_t taille_blocs=0;
    
    fread(&taille_blocs,sizeof(uint16_t),1,encoded_file);


    uint64_t* buffer_car_origin = (uint64_t*)malloc(nb_caracteres_alphabet * sizeof(uint64_t));

    for(int i=0;i<  nb_caracteres_alphabet  ; i++  )
    {
        read_bit(encoded_file,taille_blocs,buffer_car_origin+i);
    }
    read_bit(file,RESET,NULL);

    uint64_t buffer_nv_bit = 0;//0 ou 1
    binary_block buffer_tmp={0,0};

    for(uint64_t i=0;i< nb_elements*8;i++ )//nb_elements *8 = nombre de bits à parcourir
    {
        read_bit(encoded_file,1,&buffer_nv_bit);//on lit un bit

        if(buffer_tmp.number_bit_use > 0 )//si on ne lit pas le premier bit on décale d'un bit vers la gauche ce qu'on a lu jusque la dans le bloc et on y ajoute le nouveau bit lu
        {
            buffer_tmp.block = buffer_tmp.block << 1; //ex: 1110 devient 11100
            buffer_tmp.block += buffer_nv_bit ; //ex: 11100 reste comme tel ou devient 11101
            buffer_tmp.number_bit_use ++;
        }
        else//sinon on décale pas
        {
            buffer_tmp.block = buffer_nv_bit;
            buffer_tmp.number_bit_use = 1;
        }
        buffer_nv_bit = 0;
        for(int j=0;j<nb_caracteres_alphabet;j++)//on parcours les caractères de l'alphabet compressé
        {

            if(buffer_tmp.block == buffer_car[j] && buffer_tmp.number_bit_use==buffer_pos[j])//si les bits sélectionnés corespondent à un de ces caractères
            {
                write_bit(file, buffer_car_origin[j]<<(64-taille_blocs), taille_blocs);
                buffer_tmp.number_bit_use=0;// on vide le buffer
                buffer_tmp.block=0;// on vide le buffer
                break;
            }
        }
    }

    write_bit(file, 0,-1);


    free(buffer_car_origin);
    free(buffer_car);
    free(buffer_pos);
    return status;
}

int main(int argc, char* argv[])
{

    if(argc != 3)
    {
        printf("Warning : \n<usage> : cmd src_file dest_file\n");
        return 1;
    }
    

    FILE* fichier_compresse = fopen(argv[1],"rb");
    FILE* fichier_decompresse = fopen(argv[2],"wb");
    decompress(fichier_compresse,fichier_decompresse);
    if(fichier_compresse == NULL)
    {
        printf("erreur d'ouverture du fichier compressé");
    }
    if(fichier_decompresse == NULL)
    {
        printf("erreur d'ouverture du fichier décompressé");
    }
    fclose(fichier_compresse);
    fclose(fichier_decompresse);
    return 0;
}

/* Head of the Encrypted File
* 32 1052 3054 25104 (uint64_t) Position des césures
2 3 2 2  (uint32_t)           Position du prochain caractère encodé
10 000 01 11  (---/?)         Liste des caractères encodés
3        (uint_16)            Taille des blocks
110 001 111101 (---/3)        Liste des blocks
*/
