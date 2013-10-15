// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#include "debug.h"
#include "mm.h"
#include "rbtree.h"
#include "utils.h"

struct rbtree
{
	struct rbtree_node * root;
};

struct rbtree_node
{
	uint32_t key;
	void * data;
	struct rbtree_node * left, * right, * parent;
	enum { RBTREE_RED = 0, RBTREE_BLACK } color;
};

// Fixes up the tree after an insertion:
static void insertion_fixup(struct rbtree * tree, struct rbtree_node * n);
// Left rotates the tree at the specified node:
static void left_rotate(struct rbtree * tree, struct rbtree_node * x);
// Right rotates the tree at the specified node:
static void right_rotate(struct rbtree * tree, struct rbtree_node * x);
static struct rbtree_node * find_node(struct rbtree * tree, uint32_t key);

struct rbtree * rbtree_new(void)
{
	struct rbtree * retval = mm_allocate(sizeof(struct rbtree), MM_DEFAULT_ALIGNMENT, MM_MEM_NORMAL);
	retval->root = 0;
	return retval;
}

void rbtree_free(struct rbtree * tree)
{
	mm_free(tree);
	// TODO: Free nodes.
}

void rbtree_insert(struct rbtree * tree, uint32_t key, void * data)
{
	struct rbtree_node * current = tree->root;
	struct rbtree_node * parent = 0;

	while(current != 0)
	{
		parent = current;
		if(key < current->key)
			current = current->left;
		else
			current = current->right;
	}

	struct rbtree_node * new_node = mm_allocate(sizeof(struct rbtree_node), MM_DEFAULT_ALIGNMENT,
		MM_MEM_NORMAL);
	memclr(new_node, sizeof(struct rbtree_node));
	new_node->key = key;
	new_node->data = data;
	new_node->parent = parent;
	new_node->color = RBTREE_RED;

	if(parent == 0)
		tree->root = new_node;
	else if(key < parent->key)
		parent->left = new_node;
	else
		parent->right = new_node;

	insertion_fixup(tree, new_node);
}

void * rbtree_remove(struct rbtree * tree, uint32_t key)
{
	// TODO: write me
	debug_printf("Fixme: deleting from red-black trees not implemented yet\n");
	return 0;
}

void * rbtree_get_value(struct rbtree * tree, uint32_t key)
{
	struct rbtree_node * n = find_node(tree, key);
	if(n == 0)
		return 0;
	else
		return n->data;
}

bool rbtree_key_exists(struct rbtree * tree, uint32_t key)
{
	if(find_node(tree, key) != 0)
		return true;
	else
		return false;
}

// FIXME: The insertion_fixup function below has some serious issues.
// FIXME: It has evolved by bugs being fixed by simple hacks, thus all the null checks.
static void insertion_fixup(struct rbtree * tree, struct rbtree_node * n)
{
	struct rbtree_node * y;

	if(n == tree->root)
	{
		n->color = RBTREE_BLACK;
		return;
	}
	if(n->parent != 0 && n->parent->color == RBTREE_BLACK)
		return;

	while(n != tree->root && n->parent->parent != 0 && n->parent->color == RBTREE_RED)
	{
		if(n->parent == n->parent->parent->left)
		{
			if(n->parent->parent->right == 0)
				break;
			y = n->parent->parent->right;

			if(y->color == RBTREE_RED)
			{
				n->parent->color = RBTREE_BLACK;
				if(y != 0)
					y->color = RBTREE_BLACK;
				if(y != 0 && y->parent != 0 && y->parent->parent != 0)
					y->parent->parent->color = RBTREE_RED;
				n = n->parent->parent;
			} else {
				if(n == n->parent->right)
				{
					n = n->parent;
					left_rotate(tree, n);
				}

				n->parent->color = RBTREE_BLACK;
				n->parent->parent->color = RBTREE_RED;
				right_rotate(tree, n->parent->parent);
			}
		} else {
			if(n->parent->parent->left == 0)
				break;
			y = n->parent->parent->left;

			if(y != 0 && y->color == RBTREE_RED)
			{
				n->parent->color = RBTREE_BLACK;
				if(y != 0)
					y->color = RBTREE_BLACK;
				if(y != 0 && y->parent != 0 && y->parent->parent != 0)
					y->parent->parent->color = RBTREE_RED;
				n = n->parent->parent;
			} else {
				if(n == n->parent->left)
				{
					n = n->parent;
					right_rotate(tree, n);
				}

				n->parent->color = RBTREE_BLACK;
				n->parent->parent->color = RBTREE_RED;
				left_rotate(tree, n->parent->parent);
			}
		}
	}

	tree->root->color = RBTREE_BLACK;
}

static void left_rotate(struct rbtree * tree, struct rbtree_node * x)
{
	struct rbtree_node * y = x->right;
	x->right = y->left;

	if(y->left != 0)
		y->left->parent = x;

	y->parent = x->parent;

	if(x->parent == 0)
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

	if(y->right != 0)
		y->right->parent = x;
	
	y->parent = x->parent;

	if(x->parent == 0)
		tree->root = y;
	else if(x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	y->right = x;
	x->parent = y;
}

static struct rbtree_node * find_node(struct rbtree * tree, uint32_t key)
{
	struct rbtree_node * current = tree->root;
	while(current != 0)
	{
		if(key == current->key)
			return current;
		else if(key < current->key)
			current = current->left;
		else
			current = current->right;
	}

	return 0;
}

