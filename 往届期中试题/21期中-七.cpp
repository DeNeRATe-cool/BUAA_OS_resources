semaphore odd = 0;
semaphore even = 0;
semaphore emp = N;
semaphore mutex = 1;

main() {
	Cobegin
		P1();
		P2();
		P3();
	Coend
}

P1() {
	while(true) {
		int x = produce();
		P(emp);
		P(mutex);
		
		put();
		if(x % 2) {
			V(odd);
		} else {
			V(even);
		}
				
		V(mutex);
	}
}

P2() {
	while(true) {
		P(odd);
		P(mutex);
		
		getodd();
		countodd();
		
		V(mutex);
		V(emp);
	}
}

P3() {
	while(true) {
		P(even);
		P(mutex);
		
		geteven();
		counteven();
		
		V(mutex);
		V(emp);
	}
}
