#include "../include/common.h"
#include "pa3.h"

// don't remove these globals
static struct trie_node *trie_root = NULL;
size_t num_msgs = 0;

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

// return the list that contains the history of
// events on msg.
// return NULL if the msg is not present in the trie.
struct list_events* get_history(char msg[MAX_MSG_LEN])
{
	struct trie_node* temp = trie_root->child;
	struct trie_node *prev;
	while (temp != NULL)
	{
		if (*msg == temp->val)
		{
			prev=temp;
			temp = temp->child;
			*msg++;
		}
		else
		{
			prev=temp;
			temp = temp->next;
		}
	}
	if (prev->history==NULL) return NULL;
	else return prev->history;
}


// remove all the delete and the corresponding 
// post events from the history of msg, before returning
// the head of the linked list that contains the history.
// return NULL if the msg is not present in the trie.
// For example, if the following events occur for the msg
// in the given order, where rightmost is the most recent event
// (u1, P), (u1, P), (u2, P), (u1, D), (u1, P), (u3, P), (u4, P), (u3, D), (u1, D)
// Here, u1, u2, u3 are users, P and D corresponds to post and delete
// events
// Then after the cleanup the sequence of events looks like
// (u1, P), (u2, P), (u4, P)
// Notice that the history list only contains the post events
// after the cleanup
// the order of the post events remains the same during the cleanup
struct list_events* get_clean_history(char msg[MAX_MSG_LEN]) {
	struct trie_node* temp = trie_root->child;
	struct trie_node* prev = NULL;

	// Traverse the trie to find the node corresponding to the message
	while (temp != NULL) {
		if (*msg == temp->val) {
			prev = temp;
			temp = temp->child;
			*msg++;
		} else {
			prev = temp;
			temp = temp->next;
		}
	}
	
	// If the message is not present in the trie, return NULL
	if (prev == NULL || prev->history == NULL) {
		return NULL;
	}
	
	// Remove delete events and their corresponding post events from the history
	struct list_events* current = prev->history;
	struct list_events* prev_event = NULL;
	
	while (current != NULL) {
		prev_event = current;
		
		if (current->action == DELETED) {
			
			struct list_events* temp = prev->history;
			struct list_events* prev_post = NULL;
			while (temp != NULL) {
				if (temp->action == POSTED && temp->record == current->record) {
					if (prev_post == NULL) {
						prev->history = temp->next;
					} else {
						prev_post->next = temp->next;
					}
					free_memory(temp);
					break;
				}
				prev_post = temp;
				temp = temp->next;
			}
			
			if (prev_event == prev->history) {
				prev->history = current->next;
				free_memory(current);
				current = prev->history;
			} else {
				prev_event->next = current->next;
				free_memory(current);
				current = prev_event->next;
			}
		} else {
			prev_event = current;
			current = current->next;
		}
	}
	

	return prev->history;
}



// the maximum length of the '\0' terminated message 
// stored in msg is MAX_MSG_LEN
// Insert the msg in the trie rooted at trie_root
// store the reference of the square node
// corresponding to the msg in the linked list r->posts,
// where r is "struct record*"
// A user can post a message several times, say k times, 
// in that case r->posts contain k entries corresponding to
// the message
// r->posts is a stack. The first node in the linked-list
// r->posts corresponds to the lasted post by the user
// A square node in trie also contains the history of post and delete
// events on the corresponding message. The history is stored in
// a linked list n->history, where n is "struct trie_node*"
// n->history is also a stack. The first node in the linked
// list corresponds to the most recent event. The linked list
// node is of type struct event the contains the type of event
// and a reference to the user's record that posted or deleted
// the message
struct trie_node* create_trie_node(char val, struct trie_node* parent) {
	struct trie_node* node = allocate_memory(sizeof(struct trie_node));
	node->val = val;
	node->parent = parent;
	node->child = NULL;
	node->next = NULL;
	node->history = NULL;
	return node;
}

void post_a_msg(char msg[MAX_MSG_LEN], struct record* r)
{
	if (trie_root == NULL) {
		trie_root = create_trie_node('\0', NULL);
	}

	int length_message = strlen(msg);
	struct trie_node* current = trie_root;

	for (int i = 0; i < length_message; i++) {
		struct trie_node* child = current->child;
		struct trie_node* prev = NULL;

		while (child != NULL && child->val != msg[i]) {
		prev = child;
		child = child->next;
		}

		if (child == NULL) {
		struct trie_node* new_node = create_trie_node(msg[i], current);
		if (prev == NULL) {
			current->child = new_node;
		} else {
			prev->next = new_node;
		}
		current = new_node;
		} else {
		current = child;
		}

		if (i == length_message - 1) {
		struct list_events* temp = allocate_memory(sizeof(struct list_events));
		temp->action = 2;
		temp->record = r;
		temp->next = current->history;
		current->history = temp; 
		} else {
		current->history = NULL;
		}
	}

	struct list_posts* travel = allocate_memory(sizeof(struct list_posts));
	travel->node = current;
	travel->next = r->posts;
	r->posts = travel;
	num_msgs++;
}

// delete the latest post by a given user
// the first node in linked list r->posts corresponds to
// the latest post
// To delete the latest post the first node in the r->posts
// is removed
// In addition the history of the corresponding square node
// in the trie also gets updated
// a square node also contains the history of post and delete
// events on that message. The history is stored in
// a linked list n->history, where n is "struct trie_node*"
// n->history is also a stack. The first node in the linked
// list corresponds to the most recent event. The linked list
// node is of type struct event the contains the type of event
// and a reference to the user's record that posted or deleted
// the message
// return 1 if a message was deleted
// return 0 id there is nothing to delete
int delete_latest_post(struct record *r)
{
	if (r->posts == NULL || r->posts->node == NULL)
	{
		return 0;
	}
	struct list_events *temp = (struct list_events *)allocate_memory(sizeof(struct list_events));
	temp->action = 1;
	temp->record = r;
	temp->next = r->posts->node->history;
	r->posts->node->history = temp;
	struct list_posts *posts_tmp = r->posts->next;
	free_memory(r->posts);
	r->posts = posts_tmp;
	return 1;
}

void delete_all_posts(struct record *r)
{
	struct list_posts* temp = r->posts;
	while (r->posts != NULL)
	{
		temp = r->posts->next;
		free_memory(r->posts);
		r->posts = temp;
	}
}

// store the latest post (null terminated) by the user (r) in msg
// return 0 if there is no post by r
// return 1 if msg contains the latest post
// "struct record" contains references to square nodes of
// trie in the linked list posts
// To facilitate the finding of corresponding message from 
// a trie node, a trie node contains a reference to its
// parent node, that is updated when a message is inserted
// in the trie
int read_latest_post(struct record *r, char msg[MAX_MSG_LEN])
{
	if (r->posts == NULL)
	{
		return 0;
	}
	char str[MAX_MSG_LEN];
	int i = 0;
	struct trie_node* curr = r->posts->node;
	while (curr)
	{
		char tmp = curr->val;
		str[i] = tmp;
		i++;
		curr = curr->parent;
		if (curr == NULL)
		{
			break;
		}
	}
	for (i = strlen(str) - 1; i >= 0; i--)
	{
		*msg = str[i];
		*msg++;
	}
	*msg = '\0';
	return 1;
}

// Delete all the memory allocated for the trie and
// the linked list that contains the history
void del_history(struct list_events* root)
{
	while (root != NULL)
	{
		struct list_events* temp = root;
		root = root->next;
		free_memory(temp);
	}
}

void del_trie(struct trie_node* root)
{
	if (root == NULL)
	{
		return;
	}
	del_trie(root->child);
	del_trie(root->next);
	del_history(root->history);
	free_memory(root);
}

void destroy_trie()
{
	del_trie(trie_root);
}
