# Project-5

Algorithm-Related Answers

Conditions Leading to Deadlock and Starvation:

Simultaneous Actions: 
Deadlocks are most likely to happen when all philosophers try to pick up their left fork at the same time.

No Preemption:
Once a philosopher picks up a fork, they don't release it until they have both forks and have finished eating.

Resource Holding: 
Each philosopher holds onto their left fork while waiting for the right fork, preventing others from accessing it.

How Often Deadlocks Occur:
Deadlocks may not occur every time the program runs because it depends on the timing of thread execution, which is influenced by the operating system's scheduler.

Minimal Thinking Time: 
If philosophers spend little to no time thinking (for example, usleep(0)), they are more likely to attempt to pick up forks simultaneously.

Synchronized Start: 
If all philosopher threads start executing at the same moment, the chance of simultaneous fork requests increases.

Even Number of Philosophers: 
With an even number of philosophers, the symmetry seems to contribute to more deadlocks.

Algorithm Analysis:

Algorithm 1 is implemented in philosopher1.c, Algorithm 2 is implemented in philosopher2.c, and Algorithm 3 is implemented in philosopher3.c. You can use the format ./philosopher1 N [thinking_time] [eating_time] to specify the parameters in the command line, with N being the number of philosophers. I'm using usleep so that decimal values can be used for thinking_time and eating_time. Note that the default eating and thinking times are set to 0. For example, I was able to get Algorithm 1 to deadlock by running ./philosopher1 20, and interestingly, I was able to get it to run much longer with ./philosopher1 10 0.1

To keep track of how some philosophers are getting to dine more than others, I'm using an array called eat_count with a simulation that prints how much each philosopher got to eat when a deadlock occurs. The exact timing of when each philosopher picks up their left fork can vary slightly. The operating system uses thread scheduling, which decides which thread runs at any given moment. Due to small timing differences, some philosophers may reach their forks slightly before others. A philosopher who happens to get scheduled by the operating system more frequently may pick up both forks and eat more often.