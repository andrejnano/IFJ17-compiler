#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"

#define DEF_NODE_STACK_SIZE 20
void b_tree_init(b_tree *tree)
{
	tree->top = NULL;
}

/* \brief Function for inserting values into binary tree.
 * \param tree Whole tree pointer.
 * \param key Identifier of inserted node.
 * \param varType Type of variable in val.
 * \param val Variable with different possible types.
 */
int b_tree_insert(b_tree *tree, char *key, int varType, val_union val)
{
	if (!tree)
		return 1;
	if (tree->top == NULL)
	{
		tree->top = (node_t *) malloc(sizeof(node_t));
		memset(tree->top, 0, sizeof(node_t));
		tree->top->key = (char *) malloc(sizeof(char) * strlen(key) + 1);
		strcpy(tree->top->key, key);
		tree->top->val.type = varType;
		tree->top->val.value = val;
		return 0;
	}
	node_t *tmp = tree->top;
	while (tmp)
	{
		int cmp = strcmp(tmp->key, key);
		if (!cmp)
		{
			 tmp->val.type = varType;
			 tmp->val.value = val;
			return 0;
		}
		else if (cmp > 0)
		{
			if (!tmp->l_ptr)
			{
				tmp->l_ptr = (node_t *) malloc(sizeof(node_t));
				memset(tmp->l_ptr, 0, sizeof(node_t));
				tmp = tmp->l_ptr;
				break;
			}
			tmp = tmp->l_ptr;
		}
		else
		{
			if (!tmp->r_ptr)
			{
				tmp->r_ptr = (node_t *) malloc(sizeof(node_t));
				memset(tmp->r_ptr, 0, sizeof(node_t));
				tmp = tmp->r_ptr;
				break;
			}
			tmp = tmp->r_ptr;
		}
	}
	tmp->key = (char *) malloc(sizeof(char) * strlen(key) + 1);
	strcpy(tmp->key, key);
	tmp->val.type = varType;
	tmp->val.value = val;
	return 0;
}

/*
 * \brief Function searches binary tree for str key.
 * \param tree Whole tree pointer.
 * \param key Key for comparing.
 */
node_val_t *b_tree_search(b_tree *tree, char *key)
	{
	node_t *tmp = tree->top;
	while (tmp)
	{
		int cmp = strcmp(tmp->key, key);
		if (!cmp)
		{
			return &(tmp->val);
		}
		else if (cmp > 0)
		{
			tmp = tmp->l_ptr;
		}
		else
		{
				tmp = tmp->r_ptr;
			}
		}
	return NULL;
}

typedef struct
{
	node_t **arr;
	int idx;
	unsigned len;   
} stack_t;

/*
 * \brief Function searches for furthest node accesible with just l_ptr.
 * \param start Starting node.
 * \param s Stack for storing nodes, which it went through.
 * \return Pointer to most left node.
 */
node_t *most_left(node_t *start, stack_t *s)
{
	if (!start)
	{
		return NULL;
	}
	node_t *tmp = start;
	while (1)
	{
		if (s->idx <= s->len)
		{
			s->len *= 2;
			s->arr = (node_t **)realloc(s->arr, s->len);
		}
		if (tmp->r_ptr || tmp->l_ptr)
		{
			s->arr[s->idx] = tmp;
			s->idx++;
		}
		if (!tmp->l_ptr)
		{
			break;
		}
		tmp = tmp->l_ptr;
	}
	return tmp;
}

//TODO node conections print
void print_tree(node_t *t, int i)
{
	if (t->l_ptr)
	{
		int j = i;
		while (j != 0)
		{
			printf("  ");
			j--;
		}
		print_tree(t->l_ptr, i+1);
	}
	if (t)
	{
		int j = i;
		while (j != 0)
		{
			printf("  ");
			j--;
		}
		printf("%s\n", t->key);
	}
	if (t->r_ptr)
	{
		int j = i;
		while (j != 0)
		{
			printf("  ");
			j--;
		}
		print_tree(t->r_ptr, i+1);
	}
}
void b_tree_free(node_t *start)
{
	print_tree(start, 0);
	stack_t s;
	s.len = DEF_NODE_STACK_SIZE;
	s.arr = (node_t **)malloc(sizeof(node_t *) * s.len);
	s.idx = 0;
	node_t *cur = start; // current node
	while (1)
	{
		cur = most_left(cur, &s);
		if (!cur->r_ptr)
		{   // most left and !r_ptr => leaf
			free(cur->key);
			free(cur);
			s.idx--;
			if (s.idx < 0)
			{
				free(s.arr);
				return;
			}
			if (s.arr[s.idx]->l_ptr == cur)
			{   // if cur was accesd by l_ptr set l_ptr to NULL beacuse cur is freed
				cur = s.arr[s.idx];
				cur->l_ptr = NULL;
			} else
			{ // same wih r_ptr
				cur = s.arr[s.idx];
				cur->r_ptr = NULL;
			}
		}
		else
		{ // if r_ptr exist
			cur = cur->r_ptr;
		}
	}
}
