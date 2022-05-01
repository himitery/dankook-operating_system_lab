/*
*   DKU Operating System Lab
*           Lab2 (Hash Queue Lock Problem)
*           Student id : 32203643
*           Student name : 이학진
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

#include "lab2_sync_types.h"

/**
 * init mutex variables
 *
 * @param lock_type int             : 0(no lock), 1(cg lock), 2(fg lock)
 * */
void init_mutex(int lock_type) {
  if (lock_type != 0) {
	pthread_mutex_init(&target_lock, NULL);
	if (lock_type == 1) {
	  pthread_mutex_init(&cg_queue_lock, NULL);
	  pthread_mutex_init(&cg_hashlist_lock, NULL);
	} else if (lock_type == 2) {
	  pthread_mutex_init(&fg_queue_front_lock, NULL);
	  pthread_mutex_init(&fg_queue_rear_lock, NULL);
	  pthread_mutex_init(&cg_hashlist_lock, NULL);
	  for (int i = 0; i < HASH_SIZE; i++) {
		pthread_mutex_init(&fg_hashlist_lock_list[i], NULL);
	  }
	}
  }
}

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
  pthread_mutex_lock(&cg_queue_lock);
  if (rear == NULL) {
	front = rear = new_node;
  } else {
	rear->next = new_node;
	new_node->prev = rear;
	rear = new_node;
  }
  pthread_mutex_unlock(&cg_queue_lock);
}

/*
 *  Implement function which add new_node at next rear node
 *
 *  @param queue_node *new_node		: Node which you need to insert at queue in fine-grained manner.
 */
void enqueue_fg(queue_node *new_node) {
  pthread_mutex_lock(&fg_queue_rear_lock);
  if (rear == NULL) {
	pthread_mutex_lock(&fg_queue_front_lock);
	front = rear = new_node;
	pthread_mutex_unlock(&fg_queue_front_lock);
  } else {
	rear->next = new_node;
	new_node->prev = rear;
	rear = new_node;
  }
  pthread_mutex_unlock(&fg_queue_rear_lock);
}

/*
 *  Implement function which delete del_node at location that contains target key
 *
 *  @param queue_node *del_node		: Node which you need to delete at queue.
 */
void dequeue(queue_node *del_node) {
  if (del_node->next != NULL) {
	del_node->next->prev = del_node->prev;
  } else {
	rear = del_node->prev;
  }

  if (del_node->prev != NULL) {
	del_node->prev->next = del_node->next;
  } else {
	front = del_node->next;
  };
}

/*
 *  Implement function which delete del_node at location that contains target key
 *
 *  @param queue_node *del_node		: Node which you need to delete at queue in coarse-grained manner.
 */
void dequeue_cg(queue_node *del_node) {
  pthread_mutex_lock(&cg_queue_lock);
  if (del_node->next != NULL) {
	del_node->next->prev = del_node->prev;
  } else {
	rear = del_node->prev;
  }

  if (del_node->prev != NULL) {
	del_node->prev->next = del_node->next;
  } else {
	front = del_node->next;
  };
  pthread_mutex_unlock(&cg_queue_lock);
}

/*
 *  Implement function which delete del_node at location that contains target key
 *
 *  @param queue_node *del_node		: Node which you need to delete at queue in fine-grained manner.
 */
void dequeue_fg(queue_node *del_node) {
  if (del_node->next != NULL) {
	del_node->next->prev = del_node->prev;
  } else {
	pthread_mutex_lock(&fg_queue_rear_lock);
	rear = del_node->prev;
	pthread_mutex_unlock(&fg_queue_rear_lock);
  }

  if (del_node->prev != NULL) {
	del_node->prev->next = del_node->next;
  } else {
	pthread_mutex_lock(&fg_queue_front_lock);
	front = del_node->next;
	pthread_mutex_unlock(&fg_queue_front_lock);
  };
}

/*
 *  Implement function which init hashlist(same as hashtable) node.
 */
void init_hlist_node() {
  for (int i = 0; i < HASH_SIZE; i++) {
	hashlist[i] = malloc(sizeof(hlist_node));
	hashlist[i]->next = NULL;
	hashlist[i]->q_loc = NULL;
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
  queue_node *new_node = malloc(sizeof(queue_node));
  new_node->data = val;
  hlist_node *new_hash = malloc(sizeof(hlist_node));
  new_hash->q_loc = new_node;

  enqueue(new_node);

  if (hashtable->q_loc == NULL) {
	hashtable->q_loc = new_node;
  } else {
	hlist_node *table = hashtable;
	while (table->next != NULL) {
	  table = table->next;
	}
	table->next = new_hash;
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
  queue_node *new_node = malloc(sizeof(queue_node));
  new_node->data = val;
  hlist_node *new_hash = malloc(sizeof(hlist_node));
  new_hash->q_loc = new_node;

  pthread_mutex_lock(&cg_hashlist_lock);
  enqueue_cg(new_node);

  if (hashtable->q_loc == NULL) {
	hashtable->q_loc = new_node;
  } else {
	hlist_node *table = hashtable;
	while (table->next != NULL) {
	  table = table->next;
	}
	table->next = new_hash;
  }
  pthread_mutex_unlock(&cg_hashlist_lock);
}

/*
 *  Implement function which insert the resilt of finding the location
 *  of the bucket using value to the entry and hashtable
 *
 *  @param hlist_node *hashtable		: A pointer variable containing the bucket
 *  @param int val						: Data to be stored in the queue node
 */
void hash_queue_add_fg(hlist_node *hashtable, int val) {
  queue_node *new_node = malloc(sizeof(queue_node));
  new_node->data = val;
  hlist_node *new_hash = malloc(sizeof(hlist_node));
  new_hash->q_loc = new_node;

  enqueue_fg(new_node);

  pthread_mutex_lock(&fg_hashlist_lock_list[hash(val)]);
  if (hashtable->q_loc == NULL) {
	hashtable->q_loc = new_node;
  } else {
	hlist_node *table = hashtable;
	while (table->next != NULL) {
	  table = table->next;
	}
	table->next = new_hash;
  }
  pthread_mutex_unlock(&fg_hashlist_lock_list[hash(val)]);
}

/*
 *  Implement function which check if the data(value) to be stored is in the hashtable
 *
 *  @param int val						: variable needed to check if data exists
 *  @return								: status (success or fail)
 */
int value_exist(int val) {
  hlist_node *table = hashlist[hash(val)];
  if (!table) return 0;

  while (1) {
	if (table->q_loc->data == val) return 1;
	if (table->next == NULL) return 0;
	table = table->next;
  }
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
  pthread_mutex_lock(&cg_hashlist_lock);
  int _target = target;
  hlist_node *_hashtable = hashlist[hash(_target)];
  pthread_mutex_unlock(&target_lock);
  pthread_mutex_unlock(&cg_hashlist_lock);

  hash_queue_add_cg(_hashtable, _target);
}

/*
 *  Implement function which find the bucket location using target
 */
void hash_queue_insert_by_target_fg() {
  pthread_mutex_lock(&target_lock);
  int _target = target;
  pthread_mutex_unlock(&target_lock);

  pthread_mutex_lock(&fg_hashlist_lock_list[hash(_target)]);
  hlist_node *_hashtable = hashlist[hash(_target)];
  pthread_mutex_unlock(&fg_hashlist_lock_list[hash(_target)]);

  hash_queue_add_fg(_hashtable, _target);
}

/*
 *  Implement function which find the bucket location and stored data
 *  using target and delete node that contains target
 */
void hash_queue_delete_by_target() {
  if (value_exist(target)) {
	hlist_node *table = hashlist[hash(target)];
	if (table->next == NULL) {
	  table->q_loc = NULL;
	} else {
	  while (table->next != NULL) {
		if (table->next->q_loc->data == target) {
		  queue_node *delete_node = table->next->q_loc;
		  table->next = table->next->next;
		  dequeue(delete_node);
		  break;
		}
		table = table->next;
	  }
	}
  }
}

/*
 *  Implement function which find the bucket location and stored data
 *  using target and delete node that contains target
 */
void hash_queue_delete_by_target_cg() {
  pthread_mutex_lock(&target_lock);
  int _target = target;
  pthread_mutex_unlock(&target_lock);

  pthread_mutex_lock(&cg_hashlist_lock);
  if (value_exist(_target)) {
	hlist_node *table = hashlist[hash(_target)];
	if (table->next == NULL) {
	  table->q_loc = NULL;
	} else {
	  while (table->next != NULL) {
		if (table->next->q_loc->data == _target) {
		  queue_node *delete_node = table->next->q_loc;
		  table->next = table->next->next;
		  dequeue_cg(delete_node);
		  break;
		}
		table = table->next;
	  }
	}
  }
  pthread_mutex_unlock(&cg_hashlist_lock);
}

/*
 *  Implement function which find the bucket location and stored data
 *  using target and delete node that contains target
 */
void hash_queue_delete_by_target_fg() {
  pthread_mutex_lock(&target_lock);
  int _target = target;
  pthread_mutex_unlock(&target_lock);

  pthread_mutex_lock(&fg_hashlist_lock_list[hash(_target)]);
  if (value_exist(_target)) {
	hlist_node *table = hashlist[hash(_target)];
	if (table->next == NULL) {
	  table->q_loc = NULL;
	} else {
	  while (table->next != NULL) {
		if (table->next->q_loc->data == _target) {
		  queue_node *delete_node = table->next->q_loc;
		  table->next = table->next->next;
		  dequeue_fg(delete_node);
		  break;
		}
		table = table->next;
	  }
	}
  }
  pthread_mutex_unlock(&fg_hashlist_lock_list[hash(_target)]);
}

