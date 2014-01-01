// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "mm.h"
#include "rbtree.h"
#include "utils.h"

#define RBTREE_BLACK	0
#define RBTREE_RED	1

struct rbtree_node
{
	int color;
	void * key, * data;
	struct rbtree_node * right, * left, * parent;
};

struct rbtree
{
	rbtree_key_compare_func compare_key;
	rbtree_key_free_func free_key;
	rbtree_data_free_func free_data;
	rbtree_key_duplicate_func dup_key;
	struct rbtree_node * nil, * root;
};

static void free_node_recursively(struct rbtree * tree, struct rbtree_node * node);
static void insert_fixup(struct rbtree * tree, struct rbtree_node * z);
static void left_rotate(struct rbtree * tree, struct rbtree_node * node);
static void right_rotate(struct rbtree * tree, struct rbtree_node * node);
static void delete_fixup(struct rbtree * tree, struct rbtree_node * x);
static void transplant(struct rbtree * tree, struct rbtree_node * u, struct rbtree_node * v);
static struct rbtree_node * tree_minimum(struct rbtree * tree, struct rbtree_node * x);
static struct rbtree_node * find_node(struct rbtree * tree, void * key);

// Function used when no compare function is given:
static int compare_pointer_values(void * a, void * b);

struct rbtree * rbtree_new(rbtree_key_compare_func key_compare, rbtree_key_free_func key_free,
	rbtree_key_duplicate_func key_dup, rbtree_data_free_func data_free)

{
	struct rbtree * retval = mm_allocate(sizeof(struct rbtree), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	retval->compare_key = key_compare ? key_compare : compare_pointer_values;
	retval->free_key = key_free;
	retval->dup_key = key_dup;
	retval->free_data = data_free;

	retval->nil = mm_allocate(sizeof(struct rbtree_node), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	memclr(retval->nil, sizeof(struct rbtree_node));
	retval->nil->color = RBTREE_BLACK;
	retval->nil->left = retval->nil;
	retval->nil->right = retval->nil;

	retval->root = retval->nil;

	return retval;
}

void rbtree_free(struct rbtree * tree)
{
	if(tree->root != 0)
		free_node_recursively(tree, tree->root);
	mm_free(tree->nil);
}

static void free_node_recursively(struct rbtree * tree, struct rbtree_node * node)
{
	if(node->left != tree->nil)
		free_node_recursively(tree, node->left);
	if(node->right != tree->nil)
		free_node_recursively(tree, node->right);

	if(tree->free_data)
		tree->free_data(node->data);
	if(tree->free_key)
		tree->free_key(node->key);
	mm_free(node);
}

void rbtree_insert(struct rbtree * tree, void * key, void * data)
{
	struct rbtree_node * new_node = mm_allocate(sizeof(struct rbtree_node),
		MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	new_node->left = tree->nil;
	new_node->right = tree->nil;
	new_node->color = RBTREE_RED;
	if(tree->dup_key)
		new_node->key = tree->dup_key(key);
	else
		new_node->key = key;
	new_node->data = data;

	struct rbtree_node * current = tree->root, * parent = tree->nil;
	while(current != tree->nil)
	{
		parent = current;
		if(tree->compare_key(key, current->key) < 0)
			current = current->left;
		else
			current = current->right;
	}

	new_node->parent = parent;
	if(parent == tree->nil)
		tree->root = new_node;
	else if(tree->compare_key(key, parent->key) < 0)
		parent->left = new_node;
	else
		parent->right = new_node;

	insert_fixup(tree, new_node);
}

static void insert_fixup(struct rbtree * tree, struct rbtree_node * z)
{
	while(z->parent->color == RBTREE_RED)
	{
		if(z->parent == z->parent->parent->left)
		{
			struct rbtree_node * y = z->parent->parent->right;
			if(y->color == RBTREE_RED)
			{
				z->parent->color = RBTREE_BLACK;
				y->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				z = z->parent->parent;
			} else {
				if(z == z->parent->right)
				{
					z = z->parent;
					left_rotate(tree, z);
				}
				z->parent->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				right_rotate(tree, z->parent->parent);
			}
		} else {
			struct rbtree_node * y = z->parent->parent->left;
			if(y->color == RBTREE_RED)
			{
				z->parent->color = RBTREE_BLACK;
				y->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				z = z->parent->parent;
			} else {
				if(z == z->parent->left)
				{
					z = z->parent;
					right_rotate(tree, z);
				}
				z->parent->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				left_rotate(tree, z->parent->parent);
			}

		}
	}

	tree->root->color = RBTREE_BLACK;
}

static void left_rotate(struct rbtree * tree, struct rbtree_node * x)
{
	struct rbtree_node * y = x->right;
	x->right = y->left;

	if(y->left != tree->nil)
		y->left->parent = x;
	y->parent = x->parent;

	if(x->parent == tree->nil)
		tree->root = y;
	else if(x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	y->left = x;
	x->parent = y;
}

static void right_rotate(struct rbtree * tree, struct rbtree_node * x)
{
	struct rbtree_node * y = x->left;
	x->left = y->right;

	if(y->right != tree->nil)
		y->right->parent = x;
	y->parent = x->parent;

	if(x->parent == tree->nil)
		tree->root = y;
	else if(x == x->parent->right)
		x->parent->right = y;
	else
		x->parent->left = y;

	y->right = x;
	x->parent = y;
}

void * rbtree_delete(struct rbtree * tree, void * key)
{
	struct rbtree_node * node = find_node(tree, key);
	if(node == 0 || node == tree->nil)
		return 0;

	void * retval = node->data;

	struct rbtree_node * z = node;
	struct rbtree_node * y = z;
	struct rbtree_node * x;
	int y_orig_color = y->color;

	if(z->left == tree->nil)
	{
		x = z->right;
		transplant(tree, z, z->right);
	} else if(z->right == tree->nil)
	{
		x = z->left;
		transplant(tree, z, z->left);
	} else {
		y = tree_minimum(tree, z->right);
		y_orig_color = y->color;
		x = y->right;
		if(y->parent == z)
			x->parent = y;
		else {
			transplant(tree, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}

		transplant(tree, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}

	if(y_orig_color == RBTREE_BLACK)
		delete_fixup(tree, x);

	if(tree->free_key)
		tree->free_key(node->key);
	if(tree->free_data)
		tree->free_data(node->data);
	mm_free(node);
	return retval;
}

static void delete_fixup(struct rbtree * tree, struct rbtree_node * x)
{
	while(x != tree->root && x->color == RBTREE_BLACK)
	{
		if(x == x->parent->left)
		{
			struct rbtree_node * w = x->parent->right;

			if(w->color == RBTREE_RED)
			{
				w->color = RBTREE_BLACK;
				x->parent->color = RBTREE_RED;
				left_rotate(tree, x->parent);
				w = x->parent->right;
			}

			if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
			{
				w->color = RBTREE_RED;
				x = x->parent;
			} else {
				if(w->right->color == RBTREE_BLACK)
				{
					w->left->color = RBTREE_BLACK;
					w->color = RBTREE_RED;
					right_rotate(tree, w);
					w = x->parent->right;
				}

				w->color = x->parent->color;
				x->parent->color = RBTREE_BLACK;
				w->right->color = RBTREE_BLACK;

				left_rotate(tree, x->parent);
				x = tree->root;
			}
		} else {
			struct rbtree_node * w = x->parent->left;

			if(w->color == RBTREE_RED)
			{
				w->color = RBTREE_BLACK;
				x->parent->color = RBTREE_RED;
				right_rotate(tree, x->parent);
				w = x->parent->left;
			}

			if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
			{
				w->color = RBTREE_RED;
				x = x->parent;
			} else {
				if(w->left->color == RBTREE_BLACK)
				{
					w->right->color = RBTREE_BLACK;
					w->color = RBTREE_RED;
					left_rotate(tree, w);
					w = x->parent->left;
				}

				w->color = x->parent->color;
				x->parent->color = RBTREE_BLACK;
				w->left->color = RBTREE_BLACK;
				
				right_rotate(tree, x->parent);
				x = tree->root;
			}
		}
	}

	x->color = RBTREE_BLACK;
}

static void transplant(struct rbtree * tree, struct rbtree_node * u, struct rbtree_node * v)
{
	if(u->parent == tree->nil)
		tree->root = v;
	else if(u == u->parent->left)
		u->parent->left = v;
	else
		u->parent->right = v;
	v->parent = u->parent;
}

static struct rbtree_node * tree_minimum(struct rbtree * tree, struct rbtree_node * x)
{
	while(x->left != tree->nil)
		x = x->left;
	return x;
}

void * rbtree_get_value(struct rbtree * tree, void * key)
{
	struct rbtree_node * node = find_node(tree, key);
	return node == 0 ? 0 : node->data;
}

bool rbtree_key_exists(struct rbtree * tree, void * key)
{
	if(find_node(tree, key))
		return true;
	else
		return false;
}

static struct rbtree_node * find_node(struct rbtree * tree, void * key)
{
	struct rbtree_node * current = tree->root;

	while(current != tree->nil)
	{
		if(tree->compare_key(key, current->key) == 0)
			return current;
		else if(tree->compare_key(key, current->key) < 0)
			current = current->left;
		else
			current = current->right;
	}

	return 0;
}

static int compare_pointer_values(void * a, void * b)
{
	if(a < b)
		return -1;
	else if(a == b)
		return 0;
	else
		return 1;
}

