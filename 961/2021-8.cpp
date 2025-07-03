semaphore f1 = 0, f2 = 0;
semaphore e1 = 10, e2 = 10;
semaphore m1 = 1, m2 = 1;

main() {
	Cobegin
		A();
		B();
		C();
	Coend;
}

A() {
	while(1) {
		P(e1);
		P(m1);
		
		produceA();
		
		V(m1);
		V(f1);
	}
}

B() {
	while(1) {
		P(e2);
		P(m2);
		
		produceB();
		
		V(m2);
		V(f2);
	}
}

C() {
	while(1) {
		for(int i = 0; i < 2; i++) {
			P(f1);
			P(m1);
			
			getA();
			
			V(m1);
			V(e1);
		}
		
		for(int i = 0; i < 3; i++) {
			P(f2);
			P(m2);
			
			getB();
			
			V(m2);
			V(e2);
		}
		
		integrate();
	}
}
