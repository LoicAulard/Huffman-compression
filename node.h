#ifndef NODE_H
#define NODE_H
#include <stdlib.h>
#include <stdint.h>

/*---Structure that contain the information to build a bianry tree---*/
typedef struct
{
    uint8_t number_bit_use;
    uint64_t block;
} binary_block;

typedef struct node
{
    struct node *right_child;           /*Right child*/
    struct node *left_child;            /*Left child*/
    binary_block encode;           /*The character after the huffman coding*/
    binary_block clear;                 /*The character to encode 32 bits max if in UTF-8*/
    uint64_t weight;                    /*Number of occurence in the file*/

} binary_node;



typedef binary_node* tree;


/*---Creation of an empty node---*/
binary_node* create_node();


/*---Declaration of setter and getter of node structure---*/
tree right_child(tree parent);
tree left_child(tree parent);
uint8_t number_bit_use(binary_node* node);
uint32_t character(binary_node* node);
uint64_t weight(binary_node* node);
void set_right_child(tree parent, tree parameter);
void set_left_child(tree parent, tree parameter);
void set_number_bit_use(binary_node* node, uint8_t parameter);
void set_character(binary_node* node, uint32_t parameter);
void set_weight(binary_node* node, uint64_t parameter);

#endif