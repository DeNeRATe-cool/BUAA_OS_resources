semaphore m1 = 0, m2 = 0;

T1() {
	A();
	V(m1);
	P(m2);
	E();
	
	F();
}

T2() {
	B();
	P(m1);
	C();
	V(m2);
	D();
}
