#!/bin/bash

### Performance testing for Lab2 by himit0131@gmail.com ###

### Variables ###
TARGET_FILE=lab2_sync
LOOP_COUNT=$1

### Functions
function print_green() {
  echo -e "\033[32m$1\033[0m"
}

function print_red() {
  echo -e "\033[31m$1\033[0m"
}

function print_yellow() {
  echo -e "\033[33m$1\033[0m"
}

function __is_float() {
  if [ -z $1 ]; then
    echo 0
  else
    if [[ "$1" = *[a-zA-Z]* ]]; then
      echo 0
    else
      if [[ "$1" = *.* ]]; then
        echo 1
      else
        echo 0
      fi
    fi
  fi
}

function __get_time_value() {
  for value in $@; do
    if [ $(__is_float $value) -eq 1 ]; then
      IFS=$'.'
      num=($value)
      echo ${num[0]}
      IFS=$' '
    fi
  done
}

function get_average_time() {
  test_threads=$1
  test_node_count=$2
  lock_type=$3

  if [ $lock_type -eq 0 ]; then
    printf "%-12s: %-s\n" "Lock Type" "No Lock"
  elif [ $lock_type -eq 1 ]; then
    printf "%-12s: %-s\n" "Lock Type" "Coarse Grained Lock"
  elif [ $lock_type -eq 2 ]; then
    printf "%-12s: %-s\n" "Lock Type" "Fine Grained Lock"
  fi
  printf "%-12s: %-s\n" "Threads" "$test_threads"
  printf "%-12s: %-s\n\n" "Node Count" "$test_node_count"

  echo "┌────────────┬────────────┬────────────┬────────────┐"
  echo "│    index   │   insert   │   delete   │    total   │"
  echo "├────────────┼────────────┼────────────┼────────────┤"

  total_insert_execution_time=0
  total_delete_execution_time=0
  total=0
  index=1
  IFS=$'\n'
  for index in $(seq 1 $LOOP_COUNT); do
    IFS=$'\n'
    lines=($(./$TARGET_FILE -t=$test_threads -c=$test_node_count -l=$lock_type))
    IFS=$' '
    insert_execution_time=$(__get_time_value ${lines[4]})
    delete_execution_time=$(__get_time_value ${lines[9]})
    total_execution_time=$(__get_time_value ${lines[10]})

    total_insert_execution_time=$(expr $total_insert_execution_time + $insert_execution_time)
    total_delete_execution_time=$(expr $total_delete_execution_time + $delete_execution_time)
    total=$(expr $total + $total_execution_time)

    printf "│   %8d │   %8.2f │   %8.2f │   %8.2f │\n" $index $insert_execution_time $delete_execution_time $total_execution_time
    echo "├────────────┼────────────┼────────────┼────────────┤"
  done

  average_insert_execution_time=$(echo "scale=2; $total_insert_execution_time/$LOOP_COUNT" | bc -l)
  average_delete_execution_time=$(echo "scale=2; $total_delete_execution_time/$LOOP_COUNT" | bc -l)
  average_total_execution_time=$(echo "scale=2; $total/$LOOP_COUNT" | bc -l)

  printf "│   %8s │   %8.2f │   %8.2f │   %8.2f │\n" "" $average_insert_execution_time $average_delete_execution_time $average_total_execution_time
  echo "└────────────┴────────────┴────────────┴────────────┘"
  echo ""
}

### Initialize ###
_=$(make new)
clear

re='^[0-9]+$'
if [[ $LOOP_COUNT =~ $re ]]; then
  _=()
else
  print_red "Error"
  print_yellow "\tExample) ./lab2_sync_performance_test 100"
  print_yellow "\tMeaning) ./lab2_sync_performance_test (number of loop count)"
  exit -1
fi

print_green "\nPerformance testing for Lab2"
echo ""

### No Lock ###
get_average_time 1 300000 0

### Coarse Grained Lock ###
get_average_time 1 300000 1
get_average_time 10 300000 1
get_average_time 100 300000 1

### Fine Grained Lock ###
get_average_time 1 300000 2
get_average_time 10 300000 2
get_average_time 100 300000 2

### Clean ###
_=$(make clean)
