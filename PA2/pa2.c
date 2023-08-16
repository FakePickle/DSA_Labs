#include "../include/common.h"
#include "pa2.h"


// don't remove these globals
static struct record *bst_root = NULL;
static struct record *avl_root = NULL;
static size_t num_bst_nodes = 0;
static size_t num_avl_nodes = 0;

// don't modify this function
struct record *get_bst_root()
{
	return bst_root;
}

// don't modify this function
struct record *get_avl_root()
{
	return avl_root;
}

// don't modify this function
// compare two uids
// return -1 if uid1 < uid2
// return 0 if uid1 == uid2
// return 1 if uid1 > uid2
static int cmp_uid(char *uid1, char *uid2)
{
	int i;

	for (i = 0; i < MAX_LEN; i++)
	{
		if (uid1[i] > uid2[i])
		{
			return 1;
		}
		else if (uid1[i] < uid2[i])
		{
			return -1;
		}
	}
	return 0;
}

// don't modify this function
// compare two records
// we assume that uid is unique and
// there can't be two records with the
// same uid
// return -1 if record1 < record2
// return 0 if record1 == record2
// return 1 if record1 > record2
static int cmp_record(struct record *r1, struct record *r2)
{
	return cmp_uid(r1->uid, r2->uid);
}

// don't modify this function
static void *allocate_memory(size_t size)
{
	return __mymalloc(size);
}

// don't modify this function
static void free_memory(void *ptr)
{
	__myfree(ptr);
}

// ---------------- Add your code below this line ----------------- //

// Return the total number of records in the
// BST rooted at bst_root.
size_t get_num_bst_records()
{
	return num_bst_nodes;
}

// Return the total number of records in the
// AVL tree rooted at avl_root.
size_t get_num_avl_records()
{
	return num_avl_nodes;
}

struct record *getting_records(struct record *root, char uid[MAX_LEN])
{
	if (root == NULL)
	{
		return NULL;
	}
	if (cmp_uid(uid, root->uid) == 0)
	{
		return root;
	}
	else if (cmp_uid(uid, root->uid) == 1)
	{
		return getting_records(root->right, uid);
	}
	else if (cmp_uid(uid, root->uid) == -1)
	{
		return getting_records(root->left, uid);
	}
}

struct record *insert_bst(struct record *root, struct record *r)
{
	if (root == NULL)
	{
		root = r;
		return root;
	}
	if (cmp_uid(r->uid, root->uid) == 1)
	{
		root->right = insert_bst(root->right, r);
	}
	else if (cmp_uid(r->uid, root->uid) == -1)
	{
		root->left = insert_bst(root->left, r);
	}
	return root;
}

// Insert record r in the BST rooted at bst_root.
void insert_record_bst(struct record r)
{
	struct record *root;
	root = (struct record *)allocate_memory(sizeof(struct record));
	for (int i = 0; i < 16; i++)
	{
		root->uid[i] = r.uid[i];
		root->name[i] = r.name[i];
	}
	root->friends = r.friends;
	root->status = r.status;
	bst_root = insert_bst(bst_root, root);
	num_bst_nodes++;
}

// AVL Insertion
int get_balance(struct record *n)
{
	int l_height = (n->left) ? n->left->height : -1;
	int r_height = (n->right) ? n->right->height : -1;
	return l_height - r_height;
}
void set_height(struct record *n)
{
	int l_height = (n->left) ? n->left->height : -1;
	int r_height = (n->right) ? n->right->height : -1;
	n->height = (l_height > r_height) ? l_height + 1 : r_height + 1;
}

struct record *LL(struct record *n)
{
	struct record *new_root = n->left;
	n->left = new_root->right;
	new_root->right = n;
	set_height(n);
	set_height(new_root);
	return new_root;
}

struct record *RR(struct record *n)
{
	struct record *new_root = n->right;
	n->right = new_root->left;
	new_root->left = n;
	set_height(n);
	set_height(new_root);
	return new_root;
}

static struct record *LR(struct record *n)
{
	struct record *mid = n->left;
	struct record *new_root = mid->right;
	mid->right = new_root->left;
	n->left = new_root->right;
	new_root->left = mid;
	new_root->right = n;
	set_height(n);
	set_height(mid);
	set_height(new_root);
	return new_root;
}

static struct record *RL(struct record *n)
{
	struct record *mid = n->right;
	struct record *new_root = mid->left;
	mid->left = new_root->right;
	n->right = new_root->left;
	new_root->left = n;
	new_root->right = mid;
	set_height(n);
	set_height(mid);
	set_height(new_root);
	return new_root;
}

struct record *try_rotate(struct record *n)
{
	int bal = get_balance(n);
	if (bal < -1)
	{
		bal = get_balance(n->right);
		if (bal > 0)
		{
			n = RL(n);
		}
		else
		{
			n = RR(n);
		}
	}
	else if (bal > 1)
	{
		bal = get_balance(n->left);
		if (bal < 0)
		{
			n = LR(n);
		}
		else
		{
			n = LL(n);
		}
	}
	return n;
}

struct record *insert_avl(struct record *root, struct record *r)
{
	struct record *n;
	if (root == NULL)
	{
		root = r;
		return root;
	}
	if (cmp_uid(r->uid, root->uid) == 1)
	{
		root->right = insert_avl(root->right, r);
	}
	else if (cmp_uid(r->uid, root->uid) == -1)
	{
		root->left = insert_avl(root->left, r);
	}
	set_height(root);
	n = try_rotate(root);
	return n;
}

// Insert record r in the AVL tree rooted at avl_root.
void insert_record_avl(struct record r)
{
	struct record *root;
	root = (struct record *)allocate_memory(sizeof(struct record));
	for (int i = 0; i < 16; i++)
	{
		root->uid[i] = r.uid[i];
		root->name[i] = r.name[i];
	}
	root->friends = r.friends;
	root->status = r.status;
	avl_root = insert_avl(avl_root, root);
	num_avl_nodes++;
}

// Search the record corresponding to uid in the BST
// rooted at bst_root.
// If the record is not present, return a dummy record
// with -1 in the status field.
struct record search_record_bst(char uid[MAX_LEN])
{
	struct record dummy;
	dummy.status = -1;
	struct record *record;
	record = getting_records(bst_root, uid);
	if (record == NULL)
	{
		return dummy;
	}
	else
	{
		return *record;
	}
}

// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// Return the head of the linked list of friends
// of the user with uid uid in the BST rooted at bst_root.
// If the corresponding record doesn't exist, return NULL.
struct list_records *get_friends_list_bst(char uid[MAX_LEN])
{
	return getting_records(bst_root, uid)->friends;
}

// Delete the record (say n) corresponding to uid from the BST.
// Remove n from the lists of friends of other records
// and release the memory for the linked list nodes.
// Release memory for all the nodes in the list of friends of n.
// Return a copy of the value of the deleted node.
// If the node is not present, return a dummy record
// with -1 in the status field.
struct record *minValueNode(struct record *node)
{
	struct record *current = node;
	while (current->left != NULL)
	{
		current = current->left;
	}
	return current;
}

struct list_records *removeFriend(struct list_records *head, char uid[MAX_LEN])
{
	struct list_records *prev = NULL;
	struct list_records *current = head;
	while (current != NULL) {
		if (cmp_uid(current->record->uid, uid) == 0) {
			if (prev == NULL) {
				head = current->next;
			} else {
				prev->next = current->next;
			}
			free_memory(current);
			return head;
		}
		prev = current;
		current = current->next;
	}
	return head;
}

struct record *deleteNodebst(struct record *root, char uid[MAX_LEN], struct record *ans, int i)
{
	if (root == NULL)
	{
		return NULL;
	}
	else if (cmp_uid(uid,root->uid)<0)
	{
		root->left = deleteNodebst(root->left, uid, ans, i);
	}
	else if (cmp_uid(uid,root->uid)>0)
	{
		root->right = deleteNodebst(root->right, uid, ans, i);
	}
	else
	{
		if (i == 1)
		{
			if (root->friends != NULL)
			{
				struct list_records *current = root->friends;
				while (current != NULL)
				{
					struct record *friendNode = getting_records(bst_root, current->record->uid);
					if (friendNode != NULL)
					{
						friendNode->friends = removeFriend(friendNode->friends, uid);
					}
					current = current->next;
				}
				current = root->friends;
				while (current != NULL)
				{
					struct list_records *t = current;
					current = current->next;
					free_memory(t);
				}
			}
			memcpy(ans->uid, root->uid, MAX_LEN);
			ans->status = root->status;
		}
		if (root->left == NULL)
		{
			struct record *temp = root->right;
			free_memory(root);
			return temp;
		}
		else if (root->right == NULL)
		{
			struct record *temp = root->left;
			free_memory(root);
			return temp;
		}
		struct record *temp = minValueNode(root->right);
		memcpy(root->uid, temp->uid, MAX_LEN);
		memcpy(root->name, temp->name, MAX_LEN);
		root->status = temp->status;
		root->friends = temp->friends;
		root->height = temp->height;
		root->parent = temp->parent;
		root->right = deleteNodebst(root->right, temp->uid, ans, i + 1);
	}
	return root;
}

struct record temporary;
struct record delete_record_bst(char uid[MAX_LEN])
{
	struct record dummy;
	dummy.status = -1;
	
	if (getting_records(bst_root, uid) == NULL)
	{
		return dummy;
	}
	bst_root = deleteNodebst(bst_root, uid, &temporary, 1);
	num_bst_nodes--;
	return temporary;
}

// Search the record corresponding to uid in the AVL tree
// rooted at avl_root.
// If the record is not present, return a dummy record
// with -1 in the status field.
struct record search_record_avl(char uid[MAX_LEN])
{
	struct record dummy;
	dummy.status = -1;
	struct record *record;
	record = getting_records(avl_root, uid);
	if (record == NULL)
	{
		return dummy;
	}
	else
	{
		return *record;
	}
}

// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// Return the head of the linked list of friends
// of the user with uid uid in the AVL tree rooted at avl_root.
// If the corresponding record doesn't exist, return NULL.
struct list_records *get_friends_list_avl(char uid[MAX_LEN])
{
	return getting_records(avl_root, uid)->friends;
}

// Delete the record (say n) corresponding to uid from the AVL tree.
// Remove n from the lists of friends of other records
// and release the memory for the linked list nodes.
// Release memory for all the nodes in the list of friends of n.
// Return a copy of the value of the deleted node.
// If the node is not present, return a dummy record
// with -1 in the status field.
struct record *deleteNodeavl(struct record *root, char uid[MAX_LEN], struct record *ans, int i)
{
	if (root == NULL)
	{
		return NULL;
	}
	else if (cmp_uid(uid,root->uid)<0)
	{
		root->left = deleteNodeavl(root->left, uid, ans, i);
	}
	else if (cmp_uid(uid,root->uid)>0)
	{
		root->right = deleteNodeavl(root->right, uid, ans, i);
	}
	else
	{
		if (i == 1)
		{
			if (root->friends != NULL)
			{
				struct list_records *current = root->friends;
				while (current != NULL)
				{
					struct record *friendNode = getting_records(avl_root, current->record->uid);
					if (friendNode != NULL)
					{
						friendNode->friends = removeFriend(friendNode->friends, uid);
					}
					current = current->next;
				}
				current = root->friends;
				while (current != NULL)
				{
					struct list_records *t = current;
					current = current->next;
					free_memory(t);
				}
			}
			memcpy(ans->uid, root->uid, MAX_LEN);
			ans->status = root->status;
		}
		if (root->left == NULL)
		{
			struct record *temp = root->right;
			free_memory(root);
			return temp;
		}
		else if (root->right == NULL)
		{
		
			struct record *temp = root->left;
			free_memory(root);
			return temp;
		}
		
		struct record *temp = minValueNode(root->right);
		memcpy(root->uid, temp->uid, MAX_LEN);
		memcpy(root->name, temp->name, MAX_LEN);
		root->status = temp->status;
		root->friends = temp->friends;
		root->height = temp->height;
		root->parent = temp->parent;
		root->right = deleteNodeavl(root->right, temp->uid, ans, i + 1);
	}
	set_height(root);
	root = try_rotate(root);
	return root;
}

struct record temp;

struct record delete_record_avl(char uid[MAX_LEN])
{
	struct record dummy;
	dummy.status = -1;
	
	if (getting_records(avl_root, uid) == NULL)
	{
		return dummy;
	}
	struct record* n;
	avl_root = deleteNodeavl(avl_root, uid, &temp, 1);
	num_avl_nodes--;
	return temp;
}

// Release memory for all BST nodes and their lists of friends.
// Make bst_root points to an empty tree.
void del_friends_list(struct list_records *friends)
{
	struct list_records *curr = friends;
	struct list_records *next;
	while (curr != NULL)
	{
		next = curr->next;
		free_memory(curr);
		curr = next;
	}
	friends = NULL;
}

void del_node(struct record *root)
{
	if (root == NULL)
	{
		return;
	}
	del_node(root->left);
	del_node(root->right);
	del_friends_list(root->friends);
	free_memory(root);
	root = NULL;
}

void destroy_bst()
{
	del_node(bst_root);
	num_bst_nodes = 0;
}

// Release memory for all AVL nodes and their lists of friends.
// Make avl_root points to an empty tree.
void destroy_avl()
{
	del_node(avl_root);
	num_avl_nodes = 0;
}

// Make users with uids uid1 and uid2 in the BST rooted at bst_root
// friends of each other if they aren't already friends.
// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// To make the user with record A a friend of the user with record B,
// add A to B's list of friends and add B to A's list of friends.
// Return 1 if uid1 and uid2 are already friends before this call.
// Return 0 if they become friends during this call.
int make_friends_bst(char uid1[MAX_LEN], char uid2[MAX_LEN])
{
	struct record *user1 = getting_records(bst_root, uid1);
	struct record *user2 = getting_records(bst_root, uid2);

	// Check if the users are already friends
	struct list_records *curr = user1->friends;
	while (curr != NULL)
	{
		if (cmp_uid(curr->record->uid, user2->uid) == 0)
		{
			return 1;
		}
		curr = curr->next;
	}

	// Add user1 to the end of user2's list of friends
	struct list_records *new_friend = allocate_memory(sizeof(struct list_records));
	new_friend->record = user1;
	new_friend->next = NULL;
	if (user2->friends == NULL) {
		user2->friends = new_friend;
	} else {
		struct list_records *last = user2->friends;
		while (last->next != NULL) {
			last = last->next;
		}
		last->next = new_friend;
	}

	// Add user2 to the end of user1's list of friends
	struct list_records *new_friend2 = allocate_memory(sizeof(struct list_records));
	new_friend2->record = user2;
	new_friend2->next = NULL;
	if (user1->friends == NULL) {
		user1->friends = new_friend2;
	} else {
		struct list_records *last2 = user1->friends;
		while (last2->next != NULL) {
			last2 = last2->next;
		}
		last2->next = new_friend2;
	}
	return 0;
}

// Make users with uids uid1 and uid2 in the AVL tree rooted at avl_root
// friends of each other if they aren't already friends.
// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// To make the user with record A a friend of the user with record B,
// add A to B's list of friends and add B to A's list of friends.
// Return 1 if uid1 and uid2 are already friends before this call.
// Return 0 if they become friends during this call.
int make_friends_avl(char uid1[MAX_LEN], char uid2[MAX_LEN])
{
	struct record *user1 = getting_records(avl_root, uid1);
	struct record *user2 = getting_records(avl_root, uid2);

	// Check if the users are already friends
	struct list_records *curr = user1->friends;
	while (curr != NULL)
	{
		if (cmp_uid(curr->record->uid, user2->uid) == 0)
		{
			return 1;
		}
		curr = curr->next;
	}

	// Add user1 to user2's list of friends
	struct list_records *new_friend = allocate_memory(sizeof(struct list_records));
	new_friend->record = user1;
	new_friend->next = user2->friends;
	user2->friends = new_friend;

	// Add user2 to user1's list of friends
	struct list_records *new_friend2 = allocate_memory(sizeof(struct list_records));
	new_friend2->record = user2;
	new_friend2->next = user1->friends;
	user1->friends = new_friend2;
	return 0;
}