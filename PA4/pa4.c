#include "../include/common.h"
#include "pa4.h"

// don't remove these globals
struct heap_elem *min_heap_arr = NULL;

// don't modify this function
void initialize_min_heap_arr(struct heap_elem *arr)
{
  min_heap_arr = arr;
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

// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// return r->friends.
struct list_records* get_friends_list(struct record *r)
{
  return r->friends;
}

// Make r1 and r2 friends of each other if they aren't already friends.
// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// Return 1 if r1 and r2 are already friends before this call.
// Otherwise, make r1 a friend of r2 and return 0.
// To make r1 a friend of r2, 
// insert r1 into the list r2->friends and insert r2 into the list r1->friends.
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

int make_friends(struct record *r1, struct record *r2)
{
  // Check if the users are already friends
  struct list_records *curr = r1->friends;
  while (curr != NULL)
  {
    if (cmp_uid(curr->record->uid, r2->uid) == 0)
    {
      return 1;
    }
    curr = curr->next;
  }

  // Add user1 to user2's list of friends
  struct list_records *new_friend = allocate_memory(sizeof(struct list_records));
  new_friend->record = r1;
  new_friend->next = r2->friends;
  r2->friends = new_friend;

  // Add user2 to user1's list of friends
  struct list_records *new_friend2 = allocate_memory(sizeof(struct list_records));
  new_friend2->record = r2;
  new_friend2->next = r1->friends;
  r1->friends = new_friend2;
  return 0;
}

// remove all nodes from r->friends and release the corresponding memory
void delete_friends_list(struct record *r)
{
  struct list_records* friends = r->friends;
  while (friends != NULL)
  {
    struct list_records* temp = friends;
    friends = friends->next;
    free_memory(temp);
  }
  friends = NULL;
}

// Each friends f of r is connected via an undirected
// edge whose weight is the distance between r and f.
// compute the shortest distance and a shortest path to 
// all other vertices reachable from r.
// initially, the status fields in all records are set to zero.
// distance and pred fields may contain grabage values.
// you need to use the distance function to compute the distance
// between two records.
// you can use min_heap_arr to implement a min heap or
// a linear scan to find a vertex with the minimum distance.
// In addition to distance and pred, you can also update the
// status field in your implementation.
// Don't modify other fields in struct record.
// At the end of this procedure, distance field in each record
// reachable via r, must contain the minimum distance from r;
// and the pred field must contain the predecessor on a shorest
// path.
struct list_records* shortest_path(struct record* root, struct list_records* friends, struct record* r)
{
  if (friends == NULL)
  {
    return friends;
  }
  struct list_records* friends_record = friends;
  while (friends_record != NULL)
  {
    if (friends_record->record->pred != NULL && friends_record->record == root)
    {
      double dist = r->distance + distance(&friends_record->record->loc, &root->loc);
      if (dist <= root->distance)
      {
        root->distance = dist;
        root->pred = friends_record->record;
        root->verify ++;
      }
    }
    friends_record = friends_record->next;
  }
  return shortest_path(root, friends->next, r);
}

void compute_sssp(struct record* r)
{
  struct list_records *friends = r->friends;
  r->status = 0;
  r->distance = 0;
  r->pred = NULL;
  while (friends != NULL)
  {
    double dist = r->distance + distance(&r->loc, &friends->record->loc);
    friends->record->distance = dist;
    friends->record->pred = r;
    printf("Status is %d\n",friends->record->verify);
    friends->record->verify ++;
    friends = friends->next;
  }
  struct list_records *temp = r->friends;
  while (temp != NULL)
  {
    struct record* tmp = temp->record;
    shortest_path(tmp, temp->record->friends, r);
    temp = temp->next;
  }
}
