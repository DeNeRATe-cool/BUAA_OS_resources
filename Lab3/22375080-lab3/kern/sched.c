#include <env.h>
#include <pmap.h>
#include <printk.h>

/* Overview:
 *   Implement a round-robin scheduling to select a runnable env and schedule it using 'env_run'.
 *
 * Post-Condition:
 *   If 'yield' is set (non-zero), 'curenv' should not be scheduled again unless it is the only
 *   runnable env.
 *
 * Hints:
 *   1. The variable 'count' used for counting slices should be defined as 'static'.
 *   2. Use variable 'env_sched_list', which contains and only contains all runnable envs.
 *   3. You shouldn't use any 'return' statement because this function is 'noreturn'.
 */
void schedule(int yield) {
	static int system_tick = 0; // remaining time slices of current env
	struct Env *e = curenv;

	struct Env *env_i;
	u_int mn = -1;
	TAILQ_FOREACH(env_i, &env_sched_list, env_sched_link) {
		if(system_tick >= env_i -> begin_time && 
			system_tick < env_i -> begin_time + env_i -> period &&
			env_i -> run_time != 0) {
				if(env_i -> begin_time + env_i -> period < mn) {
					e = env_i;
					mn = env_i -> begin_time + env_i -> period;
				}
			}
	}

	env_run(e);
	e -> run_time -= 1;
	if(e -> run_time == 0) e -> begin_time += e -> period;
	system_tick += 1;
}
