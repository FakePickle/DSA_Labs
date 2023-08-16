#include "../include/common.h"
#include "pa1.h"


// don't remove these globals
static struct record *record_arr = NULL;
static size_t max_capacity = 0;
static size_t num_records = 0;

// don't modify this function
struct record *get_record_arr()
{
  return record_arr;
}

// don't modify this function
// compare two uids
// return -1 if uid1 < uid2
// return 0 if uid1 == uid2
// return 1 if uid1 > uid2
static int cmp_uid(char *uid1, char *uid2)
{
  int i;

  for (i = 0; i < MAX_LEN; i++) {
    if (uid1[i] > uid2[i]) {
      return 1;
    }
    else if (uid1[i] < uid2[i]) {
      return -1;
    }
  }
  return 0;
}

// don't modify this function
// compare two records when uid is key
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

#define NOT_IMPLEMENTED { printf("%s still needs to be implemented!\n", __func__); abort(); }

// return the total number of records in the
// record_arr
size_t get_num_records()
{
	return num_records;
}

// insert record r at the end of the record array
// if the record array is full, allocate
// a new buffer of double size, copy data
// from old buffer to the new buffer
// delete the old buffer
// make record_arr point to the new buffer
void insert_record(struct record r)
{
  if (max_capacity == num_records){
    max_capacity = (max_capacity == 0) ? 1 : max_capacity * 2;
    struct record* newarr = (struct record*)allocate_memory(sizeof(struct record) * max_capacity);
    for (int i = 0; i < num_records; i++){
      newarr[i] = record_arr[i];
    }
    if (record_arr != NULL){
      free_memory(record_arr);
    }
    record_arr = newarr;
  }
  record_arr[num_records] = r;
  num_records++;
}

// return the record corresponding to the uid
// use the linear search algorithm
// you can also use cmp_uid routine defined earlier
// if none of the records contain the input uid
// return a dummy record with status == -1
struct record search_record_linear(char uid[MAX_LEN])
{
	struct record* records = get_record_arr();
  struct record dummy;
  for (int i = 0; i < num_records; i++){
    if (cmp_uid(uid,(records+i)->uid) == 0){
      return *(records+i);
    }
  }
  dummy.status = -1;
  return dummy;
}

// return the record corresponding to the uid
// use the binary search algorithm
// you can assume that the record_arr is alaredy sorted
// if none of the records contain the input uid
// return a dummy record with status == -1
struct record search_record_binary(char uid[MAX_LEN])
{
	struct record* records = get_record_arr();
  struct record dummy;
  int l = 0;
  int r = num_records - 1;
  // printf("Entering Loop");
  while (l <= r){
    // printf("Entered Loop");
    int m = (r + l)/2;
    if (cmp_uid(records[m].uid,uid) == 0){
      return records[m];
    } else if(cmp_uid(records[m].uid,uid) == -1){
      l = m + 1;
    } else {
      r = m - 1;
    }
  }
  dummy.status = -1;
  return dummy;
}

// delete record correspondig to the uid
// if the corresponding record doesn't exist
// return a dummy record with status == -1
// Otherwise, return a copy of the record to the caller

// resize the record array in the following manner
// let's say, x is the size of the array
// and after deletion only x/4 
// space in the array is occupied, in this
// case, allocate an array of size x/2,
// copy records from old array to the new
// array, and delete the old array,
// make record_arr point to the new array
// sort the record_arr using selection sort
// use cmp_record implementaion to compare two records
void swap(struct record *x, struct record *y){
  struct record temp = *x;
  *x = *y;
  *y = temp;
}
struct record delete_record(char uid[MAX_LEN])
{
	int toggle = 0;
  int newsize;
  int i;
  struct record dummy;
  for (i = 0; i < num_records; i++){
    if (cmp_uid(record_arr[i].uid,uid) == 0){
      toggle = 1;
      break;
    }
  }
  if (toggle){
    swap(&record_arr[i], &record_arr[num_records-1]);
    // for (int j = i; j < num_records; j++){
    //   record_arr[j] = record_arr[j + 1];
    // }
    newsize = num_records - 1;
    num_records--;
    if (newsize > 0 && newsize <= max_capacity/4){
      max_capacity = max_capacity / 2;
      struct record* newarr = (struct record*)allocate_memory(sizeof(struct record) * newsize);
      for (int i = 0; i < num_records; i++){
        newarr[i] = record_arr[i];
      }
      if (record_arr != NULL){
        free_memory(record_arr);
      }
      record_arr = newarr;
    }
    return record_arr[num_records+1];
  } else {
    dummy.status = -1;
    return dummy;
  }
}

// delete all records in the record_arr
// free record_arr
void delete_all_records()
{
	free_memory(record_arr);
  record_arr = NULL;
  max_capacity = 0; num_records = 0;
}

// sort the record array using quick sort
// use cmp_record implementaion to compare two records
void exchange(struct record *arr, int i, int j) {
  struct record temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
}

int partition(struct record *arr, int lo, int hi) {
  struct record pivot = arr[lo];
  int left = lo + 1;
  int right = hi;

  while (left <= right) {
    while (left <= right && (cmp_uid(arr[left].uid, pivot.uid)) == -1) {
      left++;
    }
    while (left <= right && (cmp_uid(arr[right].uid, pivot.uid)) == 1) {
      right--;
    }

    if (left <= right) {
      exchange(arr, left, right);
      left++;
      right--;
    }
  }

  exchange(arr, lo, right);
  return right;
}

void quicksort(struct record *arr, int lo, int hi) {
  if (lo >= hi) return;

  int p = partition(arr, lo, hi);
  quicksort(arr, lo, p - 1);
  quicksort(arr, p + 1, hi);
}

void sort_records_quick() {
  struct record *rec = get_record_arr();
  int lo = 0;
  int hi = num_records - 1;
  quicksort(rec, lo, hi);
}

// sort the record_arr using merge sort
// use cmp_record implementaion to compare two records
void merge(struct record arr[], int l, int m, int h){
  int i, j, k;
  int n1 = m - l + 1;
  int n2 = h - m;
  struct record *L = (struct record*) allocate_memory(sizeof(struct record) * n1);
  struct record *H = (struct record*) allocate_memory(sizeof(struct record) * n2);
  for (i = 0; i < n1; i++)
    L[i] = arr[l + i];
  for (j = 0; j < n2; j++)
    H[j] = arr[m + 1 + j];
  
  i = 0; j = 0; k = l;

  while (i < n1 && j < n2){
    if (cmp_record(&L[i],&H[j]) == -1 || cmp_record(&L[i],&H[j]) == 0){
      arr[k] = L[i];
      i++;
    }
    else{
      arr[k] = H[j];
      j++;
    }
    k++;
  }
  while (i < n1){
    arr[k] = L[i];
    i++;
    k++;
  }
  while(j < n2){
    arr[k] = H[j];
    j++;
    k++;
  }
  free_memory(L);
  free_memory(H);
}

void mergesort(struct record* arr, int l, int h){
  if (l < h){
    int m = l + (h - l) / 2;
    mergesort(record_arr, l, m);
    mergesort(record_arr, m + 1, h);
    merge(arr, l, m, h);
  }
}

void sort_records_merge()
{
  int l = 0, h = num_records - 1;
  struct record* arr = get_record_arr();
  mergesort(arr,l,h);
}



void sort_records_selection()
{
  int i, j, min_ind;
  for (i = 0; i < num_records ; i++){
    min_ind = i;
    for (j = i + 1; j < num_records; j++){
      if (cmp_record(&record_arr[j],&record_arr[min_ind]) == -1){
        min_ind = j;
      }
    }
    if (min_ind != i){
      swap(&record_arr[min_ind], &record_arr[i]);
    }
  }
}

// return the number of records corresponding to the input name
// use linear search to obtain the results
size_t get_num_records_with_name_linear(char name[MAX_LEN])
{
  int num = 0;
  for (int i = 0; i < num_records; i++){
    if (strcmp(record_arr[i].name,name) == 0){
      num++;
    }
  }
  return num;
}

// implement quick sort algorithm to sort the 
// record_arr using name as key
int partition1(struct record* arr, int lo, int hi) {
  struct record pivot = arr[lo];
  int left = lo + 1;
  int right = hi;

  while (left <= right) {
    while (left <= right && (strcmp(arr[left].name, pivot.name)) < 0) {
      left++;
    }
    while (left <= right && (strcmp(arr[right].name, pivot.name)) > 0) {
      right--;
    }

    if (left <= right) {
      exchange(arr, left, right);
      left++;
      right--;
    }
  }

  exchange(arr, lo, right);
  return right;
}

void quicksort1(struct record *arr, int lo, int hi) {
  if (lo >= hi) return;

  int p = partition1(arr, lo, hi);
  quicksort1(arr, lo, p - 1);
  quicksort1(arr, p + 1, hi);
}

void rearrange_data()
{
  // int l = 0, h = num_records - 1;
  // struct record* arr = get_record_arr();
  // quicksort1(arr,l,h);

  struct record *rec = get_record_arr();
  int lo = 0;
  int hi = num_records - 1;
  quicksort1(rec, lo, hi);
}

// return the number of records corresponding to the input name
// use binary search to obtain the results
// you can assume that the caller has sorted
// data using name as key before making these queries
size_t get_num_records_with_name_binary(char name[MAX_LEN])
{
	struct record* records = get_record_arr();
  int l = 0;
  int count = 0;
  int r = num_records - 1;
  // printf("Entering Loop");
  while (l <= r){
    // printf("Entered Loop");
    int m = (r + l)/2;
    printf("%d %d %d %d\n", l, r, m, count);
    if (strcmp(records[m].name,name) == 0){
      count++;
    } else if(strcmp(records[m].name,name) < 0){
      l = m + 1;
    } else {
      r = m - 1;
    }
  }
  return count;
}