#include "compresse.h"

uint8_t compress(FILE* file, FILE* encoded_file, uint16_t size_block)
{
    /*Declaration of variable*/
    uint32_t i = 0;
    uint32_t size_alphabet = 0;
    int8_t status = 0;
    binary_block buff = {0,0};
    uint64_t buff_head = 0;

    tree t = create_huffman_tree(file,size_block,&size_alphabet);
    binary_block * origin = malloc(size_alphabet*sizeof(binary_block));
    binary_block * translation = malloc(size_alphabet*sizeof(binary_block));
    extract_alphabet_from_tree(t,origin,translation);

    destroy_tree(t);

    /*Cursor is at the beginning of the file*/
    fseek(file,0,SEEK_SET);

    /*Writing of the header's file*/
    /*Number of octet in the original file.*/
    buff_head = file_size(file);
    fwrite(&buff_head,sizeof(uint64_t),1,encoded_file);

    /*Different separation in the encoded_file*/
    buff_head = 32;
    fwrite(&buff_head,sizeof(uint64_t),1,encoded_file);
    buff_head = sizeof(uint8_t)*size_alphabet;
    fwrite(&buff_head,sizeof(uint64_t),1,encoded_file);
    buff_head = size_binary_block_table(translation,size_alphabet);
    buff_head = (buff_head%64) == 0 ? buff_head/8 : (buff_head/64+1)*8;
    fwrite(&buff_head,sizeof(uint64_t),1,encoded_file);
    buff_head = sizeof(uint16_t);
    fwrite(&buff_head,sizeof(uint64_t),1,encoded_file);
    /*-----------------------------------*/

    /*Separation of translated alphabet*/
    for(i=0;i<size_alphabet;i++)
    {
        fwrite(&(translation[i].number_bit_use),sizeof(uint8_t),1,encoded_file);

    }
    /*Translated alphabet*/
    for(i=0;i<size_alphabet;i++)
    {
        write_bit(encoded_file,(translation[i].block),translation[i].number_bit_use);
    }
    write_bit(encoded_file,0,-1);
    /*Size of read block*/
    fwrite(&size_block,sizeof(uint16_t),1,encoded_file);

    /*Original alphabet*/
    for(i=0;i<size_alphabet;i++)
    {
        write_bit(encoded_file,(origin[i].block),origin[i].number_bit_use);
    }

    write_bit(encoded_file,0,-1);
    printf("Header written\n");
    /*Cursor is at the beginning of the file*/
    fseek(file,0,SEEK_SET);
    read_bit(file,RESET,NULL);

    /*Compression*/

    while(status>=0)
    {
        buff.number_bit_use = size_block;
        status = read_bit(file,size_block,&(buff.block));
        buff = get_translation(origin,translation,size_alphabet,buff);
        write_bit(encoded_file,(buff.block),buff.number_bit_use);
    }
    write_bit(encoded_file,0,-1);
    free(origin);
    free(translation);
    return 0;
}

void print_binary_block_table(binary_block* t, uint32_t size)
{
    uint32_t i = 0;
    for(i=0;i<size;i++)
    {
        printf("nombre_bit: %d",t[i].number_bit_use);
        printf("\t\t");binary_print64(t[i].block);
    }
}
uint32_t size_binary_block_table(binary_block* b, uint32_t size)
{
    uint32_t i = 0, somme = 0;
    for(i=0;i<size;i++)
    {
        somme += b[i].number_bit_use;
    }
    return somme;
}

void print_binary_block(binary_block t)
{
    printf("nombre_bit: %d",t.number_bit_use);
    printf("\t\t");binary_print64(t.block);
}

binary_block get_origin(binary_block* origin, binary_block* translation, uint32_t size, binary_block b)
{
    b.block <<= 64-b.number_bit_use;
    uint32_t i = 0;
    for(i=0;i<size;i++)
    {
        if(translation[i].block == b.block && translation[i].number_bit_use == b.number_bit_use)
        {
            return origin[i];
        }
    }
    printf("Fail to match character\n");
    return b;
}

binary_block get_translation(binary_block* origin, binary_block* translation, uint32_t size, binary_block b)
{
    b.block <<= 64-b.number_bit_use;
    uint32_t i = 0;
    for(i=0;i<size;i++)
    {
        if(origin[i].block == b.block && origin[i].number_bit_use == b.number_bit_use)
        {
            return translation[i];
        }
    }
    printf("Fail to match character\n");
    return b;
}

tree create_huffman_tree(FILE* file, uint8_t size_block, uint32_t* size_alphabet)
{

    uint64_t char_buffer = 0;
    tree result = NULL;
    binary_node* list_of_character = NULL;
    binary_node* beginning = NULL;
    bool saved = false;
    int8_t status = 0;
    binary_node* node = NULL;

    /*Cursor is at the beginning of the file*/
    fseek(file,0,SEEK_SET);

    /*Creating a list of the different character encountered in the file*/
    /*We use the fact that binary node is a node from a tree to use it like a list*/

    
    while( status >= 0)
    {
        status = read_bit(file,size_block,&char_buffer);
        
        node = exist_node_list(beginning, &char_buffer);
        if( node == NULL)
        {
            node = create_node();
            node->clear.block = char_buffer;
            node->clear.number_bit_use=size_block;
            node->weight = 1;
            if(saved == false)
            {
                list_of_character = node;
                beginning = list_of_character;
                saved =true;
            }
            else
            {
                list_of_character->left_child = node;
                list_of_character = node;
            }


        }
        else
        {
            node->weight++;
        }
    }

    /*Now we have the list*/
    /*Build the tree*/
    *size_alphabet = size_list(beginning);
    binary_node** table = malloc((*size_alphabet)*sizeof(binary_node*));
    list_to_table(beginning, table);
    while(use_size_table(table,*size_alphabet)>1)
    {
        merge_min_from_table(table,*size_alphabet);
    }
    result = table[0];
    free(table);

    fill_huffman_tree(result,0,0);
    return result;

}

void extract_alphabet_from_tree(tree t, binary_block* origin, binary_block* translation)
{
    static uint32_t index = 0;
    if (t!=NULL)
    {
        if(t->left_child == NULL && t->right_child==NULL)
        {
            origin[index] = t->clear;
            origin[index].block <<= 64-origin[index].number_bit_use;
            translation[index] = t->encode;
            translation[index].block <<= 64-translation[index].number_bit_use;
            index++;
        }
        else
        {
            extract_alphabet_from_tree(t->left_child,origin,translation);
            extract_alphabet_from_tree(t->right_child,origin,translation);
        }

    }

}

void destroy_tree(tree t)
{
    if(t != NULL)
    {
        destroy_tree(t->left_child);
        destroy_tree(t->right_child);
        free(t);
    }
}

void print_table(binary_node** tab, uint32_t size)
{
    int i = 0;
    for(i=0; i<size;i++)
    {
        printf("|%p",tab[i]);
    }
    printf("|\n");
}

void merge_min_from_table(binary_node** table, uint32_t size)
{
    binary_node* new = create_node();
    new->left_child = remove_min_node_from_table(table,size);
    new->right_child = remove_min_node_from_table(table,size);
    new->weight =new->left_child->weight + new->right_child->weight;
    table[find_place(table,size)] = new;
}

uint32_t find_place(binary_node** table, uint32_t size)
{
    uint32_t i = 0;
    for(i=0;i<size;i++)
    {
        if(table[i] == NULL)
            break;
    }
    if(i<size)
        return i;
    else
    {
        printf("Error : no place found");
        return 0;
    }

}


binary_node* remove_min_node_from_table(binary_node** table,uint32_t size)
{
    uint32_t i = 0,j = 0;
    uint64_t min_w = 0xFFFFFFFFFFFFFFFF;
    binary_node* result = table[0];
    for(i=0; i<size;i++)
    {
        if(table[i]!=NULL && table[i]->weight < min_w)
        {
            min_w = table[i]->weight;
            j = i;
        }
    }
    result = table[j];
    table[j]=NULL;



    return result;
}

void fill_huffman_tree(tree t, uint8_t level, uint64_t block)
{
    if(t != NULL)
    {
        t->encode.block=block;
        t->encode.number_bit_use=level;
        fill_huffman_tree(t->left_child,level+1,(block<<1));
        fill_huffman_tree(t->right_child,level+1,(block<<1)+1);
    }
}

uint32_t use_size_table(binary_node** table,uint32_t size)
{
    uint32_t result = 0, i = 0;
    for(i=0; i<size;i++)
    {
        if(table[i] != NULL)
            result++;
    }
    return result;
}

void list_to_table(binary_node* list, binary_node** table)
{
    int i = 0;
    while(list!=NULL)
    {
        table[i]=list;
        list = list->left_child;
        table[i]->left_child=NULL;
        i++;
    }
}

void print_list(tree t)
{
    while(t !=  NULL)
    {
        binary_print64(t->clear.block);
        printf("%ld\n",t->weight);
        t=t->left_child;
    }
}

uint32_t size_list(binary_node* list)
{
    uint32_t result = 0;
    while(list!=NULL)
    {
        result++;
        list = list->left_child;
    }
    return  result;
}

tree merge_node_from_list(tree n1, tree n2)
{
    tree new = create_node();
    new->left_child = n1;
    new->right_child = n2;
    new->weight = n1->weight + n2->weight;
    return new;
}

void add_to_list(binary_node* list, binary_node* node)
{
    binary_node* tmp = NULL;
    tmp = list->left_child;
    list->left_child = node;
    node->left_child = tmp;
}


void print_node(binary_node* n)
{
    printf("---Node---\n");
    printf("Adress : %p\n",n);
    printf("Character : "); binary_print64(n->clear.block);
    printf("Weight : %ld\n",n->weight);
    printf("R Child : %p\n",n->right_child);
    printf("L Child : %p\n",n->left_child);
    printf("----------\n");
}
void binary_print64(uint64_t n)
{
    int i = 0;
    for(i=63; i>=0; i--)
    {
        printf("%ld", n%2 );
        n/=2;
    }
    printf("\n");
}

void binary_print8(uint64_t n)
{
    int i = 0;
    for(i=7; i>=0; i--)
    {
        printf("%ld", n%2 );
        n/=2;
    }
    printf("\n");
}

binary_node* remove_node_from_list(tree t, uint32_t c)
{
    binary_node* tmp = NULL;
    while(t->left_child->clear.block !=  c || t->right_child->clear.block)
    {
        t=t->left_child;
        if(t==NULL)
            break;

    }
    if(t != NULL)
    {
        tmp = t-> left_child;
        t->left_child = tmp->left_child;
        return tmp;
    }

    return NULL;
}

binary_node* remove_min_node_from_list(binary_node* t)
{
    binary_node* tmp = NULL;
    uint64_t min_w = t->weight;
    while(t->left_child!=NULL)
    {
        if(t->left_child->weight < min_w)
        {
            min_w = t->left_child->weight;
            tmp = t;
        }
    }
    if(tmp != NULL)
    {
        t = tmp;
        tmp = t-> left_child;
        t->left_child = tmp->left_child;
        return tmp;
    }

    return NULL;
}


/*Warning, set number to RESET to restart from the begining.*/
int8_t read_bit(FILE* file, int8_t number, uint64_t* resultat)
{
    
	static uint8_t  buffer = 0;
	int8_t compteur = 0;
	static int8_t nb_bit_lus = -1;
    uint8_t status_global=0;
    int8_t status=0;
    nb_byte_read=0;
    if(resultat!=NULL)*resultat = 0;

    if(number == -2) //If we stop to read bit by bit. Allow to start from a new byte.
    {
        compteur = 0;
        buffer = 0;
        nb_bit_lus = -1;
        return 0;
    }

	if(nb_bit_lus == -1)//If it's the first time that we read (i.e buuffer is empty).
	{
        status = fread(&buffer,sizeof(uint8_t),1,file);
        if(status<=0) return -1;
        nb_byte_read++;
        status_global += status;
		nb_bit_lus=0;
	}
	
    for( compteur =0; compteur<number; compteur++)
    {
        if(nb_bit_lus >= 8)
        {
            status = fread(&buffer,sizeof(uint8_t),1,file);
            if(status<=0) return -1;
            nb_bit_lus = 0;
        }
        *resultat <<=1;
        *resultat += get_most_significant_bit(buffer,8-nb_bit_lus);
        nb_bit_lus++;
    }
    return status;
}

    


int write_bit(FILE* file, uint64_t data, int8_t size)
{
    static uint64_t buffer=0;
    static int8_t nb_bit_ecrit=0;
    int8_t compteur = 0;
    uint8_t status = 0;
    if(size<0)
    {
        if(nb_bit_ecrit==0) return -1;
        status = fwrite(&buffer,sizeof(uint8_t),1,file);
        nb_bit_ecrit=0;
        buffer=0;
        return -1;
    }
    if(nb_bit_ecrit + size >= 8)
    {
        transfer_most_n_significant_bit(&buffer,8-nb_bit_ecrit,&data,64,8-nb_bit_ecrit);
        status = fwrite(&buffer,sizeof(uint8_t),1,file);
        buffer=0;
        compteur += 8-nb_bit_ecrit;
        nb_bit_ecrit = 0;        
        uint8_t nb_tour = (size-compteur)/8;
        for(int i=0; i< nb_tour;i++)
        {
            transfer_most_n_significant_bit(&buffer,8,&data,64-compteur,8);
            status = fwrite(&buffer,sizeof(uint8_t),1,file);
            compteur += 8;
            buffer = 0;
        }
        if(compteur < size)
        {
        
            transfer_most_n_significant_bit(&buffer,8,&data,64-compteur,size-compteur);
            nb_bit_ecrit=size-compteur;
        }
        compteur = 0;
    }
    else
    {
        transfer_most_n_significant_bit(&buffer,8-nb_bit_ecrit,&data,64,size);
        nb_bit_ecrit+=size;
    }
    return status;
}


uint64_t get_mask(uint8_t size)
{
    uint64_t m = 0;
    int i = 0;
    for(i = 0; i<size; i++)
    {
        m = (m<<1)+1;
    }
    return m;
}

uint64_t get_most_significant_bit(uint64_t data, uint8_t size_data)
{
    return (data>>(size_data-1))&MASK_BIT;
}

uint64_t get_most_n_significant_bit(uint64_t data, uint8_t size_data, uint8_t number_bit)
{
    uint64_t result = 0;
    for(; number_bit > 0;number_bit--)
    {
        result = result<<1;
        result += get_most_significant_bit(data,size_data);
        size_data--;
    }
    return result;
    
}

void set_most_significant_bit(uint64_t* data, uint8_t size_data, uint64_t bit)
{
    uint8_t msb = get_most_significant_bit(*data,size_data);
    if(msb==bit)
    {
        return;
    }
    else
    {
        if( bit > 0)
        {
            bit /= bit; //We make sure  it is 1
            *data += bit<<(size_data-1); 
        }
        else
        {
            *data &= get_mask(size_data-1);
        }
        
    }
    
}
void transfer_most_n_significant_bit(uint64_t* dest, uint8_t dest_size, uint64_t* src, uint8_t src_size, uint8_t n_bit)
{
    for(; n_bit > 0; n_bit--)
    {
        set_most_significant_bit(dest,dest_size,get_most_significant_bit(*src,src_size));
        src_size--;
        dest_size--;
    }
    
}

binary_node* exist_node_list(binary_node* list, uint64_t* character)
{
    if(list != NULL)
    {    if(*character == list->clear.block)
        {
            return list;
        }
        else
        {
            return exist_node_list(list->left_child,character);
        }
    }
    return NULL;
}

uint64_t file_size(FILE* f)
{
    int prev = ftell(f);
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f,prev,SEEK_SET);
    return size;
}
