/*
	互斥：缓冲区（仓库）访问
	同步：连续获取2个 + 1个才可以进行生产 
*/ 

semaphore mutex = 1;
semaphore empty = 20;
semaphore full1 = 0, full2 = 0;

main() {
	Cobegin
		A();
		B();
		C();
	Coend
}

A() {
	while(1) {
		P(empty);
		P(mutex);
		// 生产A
		V(mutex);
		V(full1); 
	}
}

B() {
	while(1) {
		P(empty);
		P(mutex);
		// 生产B
		V(mutex);
		V(full2);
	}
}

C() {
	while(1) {
		for(int i = 0; i < 2; i++) {
			P(full1);
			P(mutex);
			// 取A
			V(mutex); 
			V(empty);	
		}
		
		for(int i = 0; i < 1; i++) {
			P(full2);
			P(mutex);
			// 取B
			V(mutex);
			V(empty); 
		}
			
	}
}
