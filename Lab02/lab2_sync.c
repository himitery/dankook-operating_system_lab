/*
*   DKU Operating System Lab
*           Lab2 (Hash Queue Lock Problem)
*           Student id : 
*           Student name : 
*
*   lab2_sync.c :
*       - lab2 main file.
*		- Thread-safe Hash Queue Lock code
*		- Coarse-grained, fine-grained lock code
*       - Must contains Hash Queue Problem function's declations.
*
*   Implement thread-safe Hash Queue for coarse-grained version and fine-grained version.
*/

#include <aio.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
//#include <asm/unistd.h>

#include "lab2_sync_types.h"

pthread_mutex_t front_lock, rear_lock, target_lock, hashlist_lock;
pthread_mutex_t *hashlist_lock_list[HASH_SIZE];

/*
 *  Implement function which init queue nodes for front and rear
 *  ( refer to the ./include/lab2_sync_types.h for front and rear nodes)
 */
void init_queue() {
  front = rear = NULL;
}

/*
 *  Implement function which add new_node at next rear node
 *
 *  @param queue_node *new_node		: Node which you need to insert at queue.
 */
void enqueue(queue_node *new_node) {
  if (rear == NULL) {
	front = rear = new_node;
  } else {
	rear->next = new_node;
	new_node->prev = rear;
	rear = new_node;
  }
}

/*
 *  Implement function which add new_node at next rear node
 *
 *  @param queue_node *new_node		: Node which you need to insert at queue in coarse-grained manner.
 */
void enqueue_cg(queue_node *new_node) {
  pthread_mutex_lock(&front_lock);
  pthread_mutex_lock(&rear_lock);
  if (rear == NULL) {
	front = rear = new_node;
  } else {
	rear->next = new_node;
	new_node->prev = rear;
	rear = new_node;
  }
  pthread_mutex_unlock(&rear_lock);
  pthread_mutex_unlock(&front_lock);
}

/*
 *  Implement function which add new_node at next rear node
 *
 *  @param queue_node *new_node		: Node which you need to insert at queue in fine-grained manner.
 */
void enqueue_fg(queue_node *new_node) {
  pthread_mutex_lock(&rear_lock);
  if (rear == NULL) {
	pthread_mutex_lock(&front_lock);
	front = rear = new_node;
	pthread_mutex_unlock(&front_lock);
  } else {
	rear->next = new_node;
	new_node->prev = rear;
	rear = new_node;
  }
  pthread_mutex_unlock(&rear_lock);
}

/*
 *  Implement function which delete del_node at location that contains target key
 *
 *  @param queue_node *del_node		: Node which you need to delete at queue.
 */
void dequeue(queue_node *del_node) {
  del_node = front;

  if (front->next == NULL) {
	front = rear = NULL;
  } else {
	front->next->prev = NULL;
	front = front->next;
  }
}

/*
 *  Implement function which delete del_node at location that contains target key
 *
 *  @param queue_node *del_node		: Node which you need to delete at queue in coarse-grained manner.
 */
void dequeue_cg(queue_node *del_node) {
  pthread_mutex_lock(&front_lock);
  pthread_mutex_lock(&rear_lock);
  del_node = front;

  if (front->next == NULL) {
	front = rear = NULL;
  } else {
	front->next->prev = NULL;
	front = front->next;
  }
  pthread_mutex_unlock(&rear_lock);
  pthread_mutex_unlock(&front_lock);
}

/*
 *  Implement function which delete del_node at location that contains target key
 *
 *  @param queue_node *del_node		: Node which you need to delete at queue in fine-grained manner.
 */
void dequeue_fg(queue_node *del_node) {
  pthread_mutex_lock(&front_lock);
  del_node = front;

  if (front->next == NULL) {
	pthread_mutex_lock(&rear_lock);
	front = rear = NULL;
	pthread_mutex_unlock(&rear_lock);
  } else {
	front->next->prev = NULL;
	front = front->next;
  }
  pthread_mutex_unlock(&front_lock);
}

/*
 *  Implement function which init hashlist(same as hashtable) node.
 */
void init_hlist_node() {
  for (int i = 0; i < HASH_SIZE; i++) {
	hashlist[i] = NULL;
  }
}

/*
 *  Implement function which calculate hash value with modulo operation.
 */
int hash(int val) {
  return val % HASH_SIZE;
}

/*
 *  Implement function which insert the result of finding the location
 *  of the bucket using value to the entry and hashtable
 *
 *  @param hlist_node *hashtable		: A pointer variable containing the bucket
 *  @param int val						: Data to be stored in the queue node
 */
void hash_queue_add(hlist_node *hashtable, int val) {
  queue_node new_node;
  new_node.data = val;

  enqueue(&new_node);

  hlist_node next_hash;
  next_hash.q_loc = &new_node;
  next_hash.next = NULL;

  if (hashtable == NULL) {
	hashtable = &next_hash;
  } else {
	hlist_node *table = hashtable;
	while (table->next != NULL) {
	  table = table->next;
	}
	table->next = &next_hash;
  }
}

/*
 *  Implement function which insert the resilt of finding the location
 *  of the bucket using value to the entry and hashtable
 *
 *  @param hlist_node *hashtable		: A pointer variable containing the bucket
 *  @param int val						: Data to be stored in the queue node
 */
void hash_queue_add_cg(hlist_node *hashtable, int val) {
  pthread_mutex_lock(&hashlist_lock);
  queue_node new_node;
  new_node.data = val;

  enqueue_cg(&new_node);

  hlist_node next_hash;
  next_hash.q_loc = &new_node;
  next_hash.next = NULL;

  if (hashtable == NULL) {
	hashtable = &next_hash;
  } else {
	hlist_node *table = hashtable;
	while (table->next != NULL) {
	  table = table->next;
	}
	table->next = &next_hash;
  }
  pthread_mutex_unlock(&hashlist_lock);
}

/*
 *  Implement function which insert the resilt of finding the location
 *  of the bucket using value to the entry and hashtable
 *
 *  @param hlist_node *hashtable		: A pointer variable containing the bucket
 *  @param int val						: Data to be stored in the queue node
 */
void hash_queue_add_fg(hlist_node *hashtable, int val) {
  queue_node new_node;
  new_node.data = val;

  enqueue_fg(&new_node);

  hlist_node next_hash;
  next_hash.q_loc = &new_node;
  next_hash.next = NULL;

  pthread_mutex_lock(hashlist_lock_list[hash(val)]);
  if (hashtable == NULL) {
	hashtable = &next_hash;
  } else {
	hlist_node *table = hashtable;
	while (table->next != NULL) {
	  table = table->next;
	}
	table->next = &next_hash;
  }
  pthread_mutex_unlock(hashlist_lock_list[hash(val)]);
}

/*
 *  Implement function which check if the data(value) to be stored is in the hashtable
 *
 *  @param int val						: variable needed to check if data exists
 *  @return								: status (success or fail)
 */
int value_exist(int val) {
  int hash_value = hash(val);

  hlist_node *table = hashlist[hash_value];
  while (table->q_loc->data != val && table->next != NULL) {
	table = table->next;
  }

  return table->q_loc->data == val;
}

/*
 *  Implement function which find the bucket location using target
 */
void hash_queue_insert_by_target() {
  hash_queue_add(hashlist[hash(target)], target);
}

/*
 *  Implement function which find the bucket location using target
 */
void hash_queue_insert_by_target_cg() {
  pthread_mutex_lock(&target_lock);
  pthread_mutex_lock(&hashlist_lock);
  hash_queue_add_cg(hashlist[hash(target)], target);
  pthread_mutex_unlock(&hashlist_lock);
  pthread_mutex_unlock(&target_lock);
}

/*
 *  Implement function which find the bucket location using target
 */
void hash_queue_insert_by_target_fg() {
  pthread_mutex_lock(&target_lock);
  pthread_mutex_lock(hashlist_lock_list[hash(target)]);
  hash_queue_add_fg(hashlist[hash(target)], target);
  pthread_mutex_unlock(hashlist_lock_list[hash(target)]);
  pthread_mutex_unlock(&target_lock);
}

/*
 *  Implement function which find the bucket location and stored data
 *  using target and delete node that contains target
 */
void hash_queue_delete_by_target() {
  int is_exist = value_exist(target);

  if (is_exist) {
	queue_node delete_node;
	do {
	  dequeue(&delete_node);
	  delete_node = *delete_node.next;
	} while (delete_node.data != target);
	dequeue(&delete_node);

	hlist_node *hashtable = hashlist[hash(target)];
	while (hashtable->q_loc->data != target) {
	  hashtable = hashtable->next;
	}
	hashtable = hashtable->next;
  }
}

/*
 *  Implement function which find the bucket location and stored data
 *  using target and delete node that contains target
 */
void hash_queue_delete_by_target_cg() {
  pthread_mutex_lock(&target_lock);
  pthread_mutex_lock(&hashlist_lock);
  int is_exist = value_exist(target);

  if (is_exist) {
	queue_node delete_node;
	do {
	  dequeue_cg(&delete_node);
	  delete_node = *delete_node.next;
	} while (delete_node.data != target);
	dequeue_cg(&delete_node);

	hlist_node *hashtable = hashlist[hash(target)];
	while (hashtable->q_loc->data != target) {
	  hashtable = hashtable->next;
	}
	hashtable = hashtable->next;
  }
  pthread_mutex_unlock(&hashlist_lock);
  pthread_mutex_unlock(&target_lock);
}

/*
 *  Implement function which find the bucket location and stored data
 *  using target and delete node that contains target
 */
void hash_queue_delete_by_target_fg() {
  pthread_mutex_lock(&target_lock);
  int is_exist = value_exist(target);

  if (is_exist) {
	queue_node delete_node;
	do {
	  dequeue_cg(&delete_node);
	  delete_node = *delete_node.next;
	} while (delete_node.data != target);
	dequeue_cg(&delete_node);

	pthread_mutex_lock(hashlist_lock_list[hash(target)]);
	hlist_node *hashtable = hashlist[hash(target)];
	while (hashtable->q_loc->data != target) {
	  hashtable = hashtable->next;
	}
	hashtable = hashtable->next;
	pthread_mutex_unlock(hashlist_lock_list[hash(target)]);
  }
  pthread_mutex_unlock(&target_lock);
}

