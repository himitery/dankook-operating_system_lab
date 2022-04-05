/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32203643
*	    Student name : 이학진
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
*
*/


#include "include/lab1_sched_types.h"

vector<char> fcfs(list<process> processes) {
  list<process> queue = list<process>();
  vector<char> result = vector<char>();

  int tick = 0;
  while (!queue.empty() || !processes.empty()) {
	while (processes.front().arrival_time == tick) {
	  queue.push_back(processes.front());
	  processes.pop_front();
	}

	if (!queue.empty()) {
	  result.push_back(queue.front().name);
	  queue.front().service_time -= 1;
	  if (queue.front().service_time == 0) queue.pop_front();
	} else {
	  result.push_back('\0');
	}
	tick++;
  }

  return result;
}

vector<char> rr(int q, list<process> processes) {
  list<process> queue = list<process>();
  vector<char> result = vector<char>();

  while (processes.front().arrival_time == 0) {
	queue.push_back(processes.front());
	processes.pop_front();
  }

  int tick = 0, q_count = 0;
  while (!queue.empty() || !processes.empty()) {
	if (!queue.empty()) {
	  result.push_back(queue.front().name);
	  queue.front().service_time -= 1;
	  q_count++;

	  while (processes.front().arrival_time == (tick + 1)) {
		queue.push_back(processes.front());
		processes.pop_front();
	  }

	  if (queue.front().service_time == 0) {
		queue.pop_front();
		q_count = 0;
	  } else if (q_count % q == 0) {
		queue.push_back(queue.front());
		queue.pop_front();
		q_count = 0;
	  }
	} else {
	  result.push_back('\0');
	}
	tick++;
  }

  return result;
}

vector<char> spn(list<process> processes) {
  list<process> queue = list<process>();
  vector<char> result = vector<char>();

  int tick = 0;
  while (!queue.empty() || !processes.empty()) {
	while (processes.front().arrival_time == tick) {
	  queue.push_back(processes.front());
	  processes.pop_front();
	}

	if (!queue.empty()) {
	  result.push_back(queue.front().name);
	  queue.front().service_time -= 1;

	  if (queue.front().service_time == 0) {
		queue.pop_front();

		queue.sort([](process prev, process next) -> bool {
		  return prev.service_time < next.service_time;
		});
	  }
	} else {
	  result.push_back('\0');
	}
	tick++;
  }

  return result;
}

vector<char> hrrn(list<process> processes) {
  typedef struct wait_process {
	process p;
	int wait;
  } wait_process;
  list<wait_process> queue = list<wait_process>();
  vector<char> result = vector<char>();

  int tick = 0;
  while (!queue.empty() || !processes.empty()) {
	while (processes.front().arrival_time == tick) {
	  wait_process wp;
	  wp.p = processes.front();
	  wp.wait = 0;
	  queue.push_back(wp);
	  processes.pop_front();
	}

	if (!queue.empty()) {
	  result.push_back(queue.front().p.name);
	  queue.front().p.service_time -= 1;
	  queue.front().wait = -1;

	  if (queue.front().p.service_time == 0) {
		queue.pop_front();

		queue.sort([](wait_process prev, wait_process next) -> bool {
		  return (prev.p.service_time + prev.wait) / prev.p.service_time
			  > (next.p.service_time + next.wait) / next.p.service_time;
		});
	  }

	  for (wait_process &wp : queue) {
		wp.wait += 1;
	  }
	} else {
	  result.push_back('\0');
	}
	tick++;
  }

  return result;
}

vector<char> mlfq(int queue_count, list<process> processes) {
  typedef struct depth_process {
	process p;
	int depth;
  } depth_process;
  list<depth_process> queue = list<depth_process>();
  vector<char> result = vector<char>();

  int tick = 0;
  while (!queue.empty() || !processes.empty()) {
	while (processes.front().arrival_time == tick) {
	  depth_process wp;
	  wp.p = processes.front();
	  wp.depth = 0;
	  queue.push_back(wp);
	  processes.pop_front();
	}

	queue.sort([](depth_process prev, depth_process next) {
	  return prev.depth < next.depth;
	});

	if (!queue.empty()) {
	  depth_process dp = queue.front();
	  result.push_back(dp.p.name);
	  queue.pop_front();
	  dp.p.service_time -= 1;

	  if (dp.p.service_time != 0) {
		if (dp.depth + 1 < queue_count && (processes.front().arrival_time == (tick + 1) ||
			any_of(queue.begin(), queue.end(), [dp](depth_process p) { return p.depth >= dp.depth; })
		)) {
		  dp.depth += 1;
		}
		queue.push_back(dp);
	  }
	} else {
	  result.push_back('\0');
	}
	tick++;
  }

  return result;
}

vector<char> mlfq_squared(int queue_count, list<process> processes) {
  typedef struct depth_process {
	process p;
	int depth;
  } depth_process;
  list<depth_process> queue = list<depth_process>();
  vector<char> result = vector<char>();

  int tick = 0, q_count = 0;
  while (!queue.empty() || !processes.empty()) {
	while (processes.front().arrival_time == tick) {
	  depth_process wp;
	  wp.p = processes.front();
	  wp.depth = 0;
	  queue.push_back(wp);
	  processes.pop_front();
	}

	if (q_count == 0)
	  queue.sort([](depth_process prev, depth_process next) {
		return prev.depth < next.depth;
	  });

	if (!queue.empty()) {
	  depth_process dp = queue.front();
	  if (q_count == 0) q_count = (int) pow(2, dp.depth);
	  result.push_back(dp.p.name);
	  queue.pop_front();
	  dp.p.service_time -= 1;
	  q_count -= 1;

	  if (dp.p.service_time != 0 && q_count == 0) {
		if (dp.depth + 1 < queue_count && (processes.front().arrival_time <= (tick + 1)
			|| any_of(queue.begin(), queue.end(), [dp](depth_process p) { return p.depth >= dp.depth; }))) {
		  dp.depth += 1;
		}
		queue.push_back(dp);
	  } else if (dp.p.service_time != 0) {
		queue.push_front(dp);
	  } else {
		q_count = 0;
	  }
	} else {
	  result.push_back('\0');
	}
	tick++;
  }

  return result;
}
