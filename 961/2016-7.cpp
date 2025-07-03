semaphore empty1 = 1, empty2 = 3;
semaphore full1 = 0, full2 = 0;
semaphore mutex = 1;

main() {
	Cobegin
		A();
		B();
		B();
		B();
		C();
	Coend
}

A() {
	while(1) {
		P(empty1);
		P(mutex);
		// 生产并放入C
		V(mutex);
		V(full1); 
	}
}

B() {
	while(1) {
		P(empty2);
		P(mutex);
		// 生产并放入C
		V(mutex);
		V(full2); 
	}
}

C() {
	while(1) {
		for(int i = 0; i < 1; i++) {
			P(full1);
			P(mutex);
			// 取出 A
			V(mutex);
			V(empty1); 
		}
		
		for(int i = 0; i < 3; i++) {
			P(full2);
			P(mutex);
			// 取出 B
			V(mutex);
			V(empty2); 
		}
	}
}
