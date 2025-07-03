semaphore empty = 1;
semaphore full = 0;
semaphore pg = 0;
semaphore jz = 0;

function dad() {
	P(empty);
	V(pg);
	V(full);
}

function mom() {
	P(empty);
	V(jz);
	V(full);
}

function daughter() {
	P(pg);
	P(full);
	
	V(empty);
}

function son() {
	P(jz);
	P(full);
	
	V(empty);	
}
