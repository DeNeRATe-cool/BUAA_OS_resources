semaphore empty = 50;
semaphore cnta = 0;
semaphore cntb = 0;
semaphore mutex = 1;

function A() {
	P(empty);
	P(mutex);
	
	V(mutex);
	V(cnta);
}

function B() {
	P(empty);
	P(mutex);
	
	V(mutex);
	V(cntb);
}

function clientA() {
	P(cnta);
	P(mutex);
	
	V(mutex);
	V(empty);
}

function clientB() {
	P(cntb);
	P(mutex);
	
	V(mutex);
	V(empty);
}
