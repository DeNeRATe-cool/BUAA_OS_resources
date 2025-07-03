int cntq = 0;
semaphore qm = 1;

int ticket = 0;
semaphore mutex;

semaphore w = 1;

main() {
	int qcnt = 100, bcnt = 100;
	Cobegin
		for(int i = 0; i < qcnt; i++)
			Q();
		for(int i = 0; i < bcnt; i++)
			B();
	Coend
} 

Q() {
	P(w);
	P(qm);
	cntq += 1;
	if(cntq == 1) {
		P(mutex);
	}
	V(qm);
	V(w);
	// ²éÑ¯
	P(qm);
	cntq -= 1;
	if(cntq == 0) {
		V(mutex);
	} 
	V(qm);
}

B() {
	P(w);
	P(mutex);
	if(ticket > 0) {
		ticket -= 1;
		// ¹ºÆ± 
	}
	V(mutex);
	V(w);
}
