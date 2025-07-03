/*
	»¥³â£º 
	Í¬²½£º 
*/

semaphore mutex1 = 1, mutex2 = 1;
semaphore empty1 = 100, empty2 = 100;
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
		P(empty1);
		P(mutex1);
		
		Awritefrom1();
		
		V(mutex1);
		V(full1);
	}
}

B() {
	while(1) {
		P(full1);
		P(mutex1);
		
		Breadfrom1();
		
		V(mutex1);
		V(empty1);
		
		P(empty2);
		P(mutex2);
		
		Bwritefrom2();
		
		V(mutex2);
		V(full2);
	}
}

C() {
	while(1) {
		P(full2);
		P(mutex2);
		
		Creadfrom2();
		
		V(mutex2);
		V(empty2);
	}
}
