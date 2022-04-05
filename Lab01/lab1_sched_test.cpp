/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32203643
*	    Student name : 이학진
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm test code.
*
*/

#include "include/lab1_sched_types.h"

// testing
int test();
bool equal(const vector<char> &, const vector<char> &);
void test_fcfs(const list<process> &);
void test_rr_q1(const list<process> &);
void test_rr_q3(const list<process> &);
void test_spn(const list<process> &);
void test_hrrn(const list<process> &);
void test_mlfq(const list<process> &);
void test_mlfq_squared(const list<process> &);

// utils
int get_scheduling_policy();
list<process> get_workload();
void show(const string &, const vector<char> &);

int main() {
  system("clear");

  list<process> processes = list<process>();
  int arrival_time_arr[] = {0, 2, 4, 6, 8};
  int service_time_arr[] = {3, 6, 4, 5, 2};
  for (int i = 0; i < 5; i++) {
	process p;
	p.name = char(int('A') + i);
	p.arrival_time = arrival_time_arr[i];
	p.service_time = service_time_arr[i];
	processes.push_back(p);
  }

  cout << "Default Processes" << endl;
  cout << "------------------------------------" << endl;
  for (process p : processes) {
	cout << p.name << " (" << "arrival_time: " << p.arrival_time << ", service_time: " << p.service_time << ")" << endl;
  }
  cout << "------------------------------------\n" << endl;

  test_fcfs(processes);
  test_rr_q1(processes);
  test_rr_q3(processes);
  test_spn(processes);
  test_hrrn(processes);
  test_mlfq(processes);
  test_mlfq_squared(processes);

  cout << "Would you like to run a new test?" << " (o | x) > ";
  char answer;
  cin >> answer;
  switch (answer) {
	case 'o': {
	  cout << endl;
	  test();
	  break;
	}
	case 'x': {
	  return 0;
	}
	default: {
	  cout << "Invalid input" << endl;
	  return -1;
	}
  }

  return 0;
}

int test() {
  int policy = get_scheduling_policy();
  list<process> processes = get_workload();
  processes.sort([](process prev, process next) -> bool {
	return prev.arrival_time < next.arrival_time;
  });

  switch (policy) {
	case 1: {
	  show("FCFS", fcfs(processes));
	  break;
	}
	case 2: {
	  int q;
	  cout << "q > ";
	  cin >> q;
	  system("clear");

	  string scheduler = "RR";
	  scheduler.append("(");
	  scheduler.append(to_string(q));
	  scheduler.append(")");

	  show(scheduler, rr(q, processes));
	  break;
	}
	default: {
	  cout << "Invalid input" << endl;
	  return -1;
	}
  }
  return 0;
}

bool equal(const vector<char> &target, const vector<char> &value) {
  if (target.size() != value.size()) return false;
  for (int i = 0; i < target.size(); i++) {
	if (target[i] != value[i]) return false;
  }
  return true;
}

void test_fcfs(const list<process> &processes) {
  char result_arr[] =
	  {'A', 'A', 'A', 'B', 'B', 'B', 'B', 'B', 'B', 'C', 'C', 'C', 'C', 'D', 'D', 'D', 'D', 'D', 'E', 'E'};
  vector<char> result(begin(result_arr), end(result_arr));
  vector<char> expect = fcfs(processes);

  show("FCFS", expect);
  assert(equal(expect, result));
}

void test_rr_q1(const list<process> &processes) {
  char result_arr[] =
	  {'A', 'A', 'B', 'A', 'B', 'C', 'B', 'D', 'C', 'B', 'E', 'D', 'C', 'B', 'E', 'D', 'C', 'B', 'D', 'D'};
  vector<char> result(begin(result_arr), end(result_arr));
  vector<char> expect = rr(1, processes);

  show("RR(q=1)", expect);
  assert(equal(expect, result));
}

void test_rr_q3(const list<process> &processes) {
  char result_arr[] =
	  {'A', 'A', 'A', 'B', 'B', 'B', 'B', 'C', 'C', 'C', 'C', 'D', 'D', 'D', 'D', 'B', 'B', 'E', 'E', 'D'};
  vector<char> result(begin(result_arr), end(result_arr));
  vector<char> expect = rr(4, processes);

  show("RR(q=4)", expect);
  assert(equal(expect, result));
}

void test_spn(const list<process> &processes) {
  char result_arr[] =
	  {'A', 'A', 'A', 'B', 'B', 'B', 'B', 'B', 'B', 'E', 'E', 'C', 'C', 'C', 'C', 'D', 'D', 'D', 'D', 'D'};
  vector<char> result(begin(result_arr), end(result_arr));
  vector<char> expect = spn(processes);

  show("SPN", expect);
  assert(equal(expect, result));
}

void test_hrrn(const list<process> &processes) {
  char result_arr[] =
	  {'A', 'A', 'A', 'B', 'B', 'B', 'B', 'B', 'B', 'C', 'C', 'C', 'C', 'E', 'E', 'D', 'D', 'D', 'D', 'D'};
  vector<char> result(begin(result_arr), end(result_arr));
  vector<char> expect = hrrn(processes);

  show("HRRN", expect);
  assert(equal(expect, result));
}

void test_mlfq(const list<process> &processes) {
  char result_arr[] =
	  {'A', 'A', 'B', 'A', 'C', 'B', 'D', 'C', 'E', 'D', 'E', 'B', 'C', 'D', 'B', 'C', 'D', 'B', 'D', 'B'};
  vector<char> result(begin(result_arr), end(result_arr));
  vector<char> expect = mlfq(3, processes);

  show("MLFQ (q=1)", expect);
  assert(equal(expect, result));
}

void test_mlfq_squared(const list<process> &processes) {
  char result_arr[] =
	  {'A', 'A', 'B', 'A', 'C', 'B', 'B', 'D', 'E', 'C', 'C', 'D', 'D', 'E', 'B', 'B', 'B', 'C', 'D', 'D'};
  vector<char> result(begin(result_arr), end(result_arr));
  vector<char> expect = mlfq_squared(3, processes);

  show("MLFQ (q=2^i)", expect);
  assert(equal(expect, result));
}

int get_scheduling_policy() {
  int input;

  cout << "Choose a scheduling policies" << endl;
  cout << "----------------------------" << endl;
  cout << "1. FCFS" << endl;
  cout << "2. RR" << endl;
  cout << "3. SPN" << endl;
  cout << "4. HRRN" << endl;
  cout << "5. MLFQ (q=1)" << endl;
  cout << "6. MLFQ (q=2^i)" << endl;
  cout << "----------------------------" << endl;
  cout << "> ";
  cin >> input;
  cout << "----------------------------\n" << endl;

  return input;
}

list<process> get_workload() {
  int count;
  list<process> processes;

  cout << "Select number of processes" << endl;
  cout << "--------------------------" << endl;
  cout << "> ";
  cin >> count;
  cout << "--------------------------" << endl;

  for (int i = 0; i < count; i++) {
	process p;
	p.name = char(int('A') + i);

	cout << p.name << " | arrival_time > ";
	cin >> p.arrival_time;

	cout << p.name << " | service_time > ";
	cin >> p.service_time;

	cout << "--------------------------" << endl;
	processes.push_back(p);
  }
  cout << endl;

  return processes;
}

void show(const string &scheduler, const vector<char> &result) {
  vector<char> process_name = vector<char>();
  for (char name : result) {
	if (name != '\0' && find(process_name.begin(), process_name.end(), name) == process_name.end()) {
	  process_name.push_back(name);
	}
  }
  sort(process_name.begin(), process_name.end(), [](char prev, char next) -> bool {
	return prev < next;
  });

  cout << scheduler << " Scheduling" << endl;
  for (int i = 0; i < result.size() + 3; i++) cout << ((i == result.size() + 2) ? "-\n" : "-");

  for (char name : process_name) {
	cout << name << " |";
	for (char r : result) {
	  cout << (name == r ? "#" : " ");
	}
	cout << endl;
  }

  for (int i = 0; i < result.size() + 3; i++) cout << ((i == result.size() + 2) ? "-\n\n" : "-");
}