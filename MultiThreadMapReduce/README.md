
## Instruction
Write a program that will get you familiar with writing multi-threaded programs in Rust

MapReduce is a programming model and an associated implementation for processing and generating big data sets with a parallel, distributed algorithm on a cluster.

A MapReduce program is composed of a map procedure, which performs filtering and sorting (such as sorting students by first name into queues, one queue for each name), and a reduce method, which performs a summary operation (such as counting the number of students in each queue, yielding name frequencies).
## How to compile your code
```sh
rustc -o main main.rs
```
## Example of run program commands
```sh
./main 5 150
```
## sample output
```sh
./main 5 150
Number of partitions = 2
    size of partition 0 = 75
    size of partition 1 = 75
Intermediate sums = [2775, 8400]
Sum = 11175
Number of partitions = 5
    size of partition 0 = 30
    size of partition 1 = 30
    size of partition 2 = 30
    size of partition 3 = 30
    size of partition 4 = 30
Intermediate sums = [435, 1335, 2235, 3135, 4035]
Sum = 11175

```

```sh
./main 5 153
Number of partitions = 2
    size of partition 0 = 76
    size of partition 1 = 77
Intermediate sums = [2850, 8778]
Sum = 11628
Number of partitions = 5
    size of partition 0 = 31
    size of partition 1 = 31
    size of partition 2 = 31
    size of partition 3 = 30
    size of partition 4 = 30
Intermediate sums = [585, 1486, 2387, 3135, 4035]
Sum = 11628

```