int c_read_count = 0;
semaphore c_rc_mutex = 1;

semaphore w = 1; // 实现写优先 
semaphore bc_mutex = 1;

function B() {
	P(w);
	P(bc_mutex);
	// write...	
	V(bc_mutex);
	V(w);
}

function C() {
	P(w);
	P(c_rc_mutex);
	c_read_count += 1;
	if(c_read_count == 1)
		P(bc_mutex);
	V(c_rc_mutex);
	V(w);
	// read...
	P(c_rc_mutex)
	c_read_count -= 1;
	if(c_read_count == 0)
		V(bc_mutex);
	V(c_rc_mutex);
}
