#ifndef COMP_H
#define COMP_H

#include <stdlib.h>
#include "node.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#define RESET -2
#define MASK_BIT 1

//To delete
uint64_t nb_byte_read;

/*--- ---*/
uint8_t compress(FILE* file, FILE* encoded_file, uint16_t size_block);

/*---Create Huffman tree---*/
/* file : is a file stream pointer to a file open in binary readabilty*/
tree create_huffman_tree(FILE* file, uint8_t size_block, uint32_t* size_alphabet);


/*--- Check if a node exist in a list---*/
binary_node* exist_node_list(binary_node* list, uint64_t* c);

/*---Return a mask of size (i.e 'size' least signifaicant bit are set to 1)---*/
uint64_t get_mask(uint8_t size);

/*---Function that read 'number' bit from a file---*/
int8_t read_bit(FILE* file, int8_t number, uint64_t* result);

/*---Function that write the 'size' most significant bit of 'data' in 'file---*/
int write_bit(FILE* file, uint64_t data, int8_t size);

/*---return the size of a file---*/
uint64_t file_size(FILE* f);

/*---Print the binary form of number on 64bits---*/
void binary_print64(uint64_t n);
/*---Print the binary form of number on 8bits---*/
void binary_print8(uint64_t n);

/*---Print a binary node---*/
void print_node(binary_node* n);
/*---Fill the huffman tree to associate the huffman code to each value---*/
void fill_huffman_tree(tree t, uint8_t level, uint64_t block);

/*---Print a list of binary_node linked by their left-child---*/
void print_list(tree t);
/*---Give the size f a list---*/
uint32_t size_list(binary_node* list);
/*---Associate two node under a common node---*/
tree merge_node_from_list(tree n1, tree n2);
/*---Same as above but for an array---*/
void merge_min_from_table(binary_node** table, uint32_t size);
/*---Add a binary node to a list---*/
void add_to_list(binary_node* list, binary_node* node);
/*---Remove the node of minimal weight from the list---*/
binary_node* remove_min_node_from_list(binary_node* t);
/*---Same as above but for an array---*/
binary_node* remove_min_node_from_table(binary_node** table,uint32_t size);

/*---Convert a list of binary_node into an array of binary_node* --*/
void list_to_table(binary_node* list, binary_node** table);

/*Return the number of used slot in the array---*/
uint32_t use_size_table(binary_node** table,uint32_t size);
/*---Return the index of an array that is not used to stoore a node---*/
uint32_t find_place(binary_node** table, uint32_t size);

/*---Print an array of node---*/
void print_table(binary_node** tab, uint32_t size);

/*--- Free a tree of binary_node---*/
void destroy_tree(tree t);

/*---Store the information of the tree in two array---*/
void extract_alphabet_from_tree(tree t, binary_block* origin, binary_block* translation);

/*---Pass from a  caractere to another---*/
binary_block get_translation(binary_block* origin, binary_block* translation, uint32_t size, binary_block b);
binary_block get_origin(binary_block* origin, binary_block* translation, uint32_t size, binary_block b);
/*---------------------------------------*/

/*Return the size of the usefull information---*/
uint32_t size_binary_block_table(binary_block* b, uint32_t size);
/*---Print a binaryblock array---*/
void print_binary_block_table(binary_block* t, uint32_t size);

/*---Print a binaryblock---*/
void print_binary_block(binary_block b);

/*---Function to do  bitwise opération---*/
uint64_t get_most_significant_bit(uint64_t data, uint8_t size_data);
uint64_t get_most_n_significant_bit(uint64_t data, uint8_t size_data, uint8_t number_bit);
void set_most_significant_bit(uint64_t* data, uint8_t size_data, uint64_t bit);
void transfer_most_n_significant_bit(uint64_t* dest, uint8_t dest_size, uint64_t* src, uint8_t src_size, uint8_t n_bit);



/* Head of the Encrypted File
* [Nombre de  caractère dans le corps du fichier en octet] 32 1052 16 25104 (uint64_t) Position des césures
2 3 2 2  (uint8_t)    ||         Position du prochain caractère encodé
10 000 01 11  (---/?) ||        Liste des caractères encodés
3        (uint_16)           Taille des blocks
110 001 111101 (---/3)||      Liste des blocks
blablabla.
*/

#endif