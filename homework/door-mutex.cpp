int entered = 0;
semaphore entered_m = 1;

int wait = 0;
semaphore wait_m = 1;

semaphore barrier = 0;

function employee() {
	P(wait_m);
	wait++;
	if(wait == 5) {
		V(barrier);
	}
	V(wait_m);
	
	P(all);
	
	P(entered_m);
	entered++;
	if(entered === 5) {
		// close the door...
	}
	V(entered_m);
	
	// work...
}
