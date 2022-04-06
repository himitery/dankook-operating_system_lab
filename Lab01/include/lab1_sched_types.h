/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32203643
*	    Student name : 이학진
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <random>

using namespace std;

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H

typedef struct process {
  char name;
  int arrival_time;
  int service_time;
} process;

typedef struct tickets {
  process p;
  int ticket;
} tickets;

std::vector<char> fcfs(std::list<process>);
std::vector<char> rr(int, std::list<process>);
std::vector<char> spn(std::list<process>);
std::vector<char> hrrn(std::list<process>);
std::vector<char> mlfq(int, std::list<process>);
std::vector<char> mlfq_squared(int, std::list<process>);
std::vector<char> lottery(std::list<tickets>);

#endif /* LAB1_HEADER_H*/


