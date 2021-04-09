#include "node.h"

/*---Creation of an empty node---*/

binary_node* create_node()
{
    binary_node *node = (binary_node*)malloc(sizeof(binary_node));      /*Allocation*/
    /*---Initialisation---*/
    node->left_child = NULL;
    node->right_child = NULL;
    node->clear.block = 0;
    node->clear.number_bit_use = 0;
    node->encode.number_bit_use = 0;
    node->encode.block = 0;
    node->weight = 0;
    /*--------------------*/

    return node;
}
/*-------------------------------*/


/*---Definition of setter and getter of node structure---*/

//Getter

tree right_child(tree parent)
{
    return parent->right_child;
}

tree left_child(tree parent)
{
    return parent->left_child;
}

uint8_t number_bit_use(binary_node* node)
{
    return node->encode.number_bit_use;
}

uint32_t character(binary_node* node)
{
    return node->clear.block;
}

uint64_t weight(binary_node* node)
{
    return node->weight;
}

//Setter

void set_right_child(tree parent, tree parameter)
{
    parent->right_child = parameter;
}

void set_left_child(tree parent, tree parameter)
{
    parent->left_child = parameter;
}

void set_encoded_character(binary_node* node, uint64_t parameter)
{
    node->encode.block = parameter;
}

void set_number_bit_use(binary_node* node, uint8_t parameter)
{
    node->encode.number_bit_use = parameter;
}

void set_character(binary_node* node, uint32_t parameter)
{
    node->clear.block = parameter;
}

void set_weight(binary_node* node, uint64_t parameter)
{
    node->weight = parameter;
}

/*-------------------------------------------------------*/

void destroy_node(binary_node* n)
{
    free(n);
}
