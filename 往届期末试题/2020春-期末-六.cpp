semaphore mutex = 1;
int wait = 0;
semaphore wait_mutex = 1;
semaphore seat = 0;

function passenger() {
	P(mutex);
	V(mutex);	
	P(wait_mutex) 
	wait += 1;
	V(wait_mutex);
	P(seat);
	boardBus();
}

function bus() {
	P(mutex);
	P(wait_mutex);
	
	for(int i = 1; i <= wait; i++)
		V(seat);
	wait = 0;
	
	V(wait_mutex);
	depart();
	V(mutex);
}
