int cntx = 0, cnty = 0;
semaphore mutex = 1;

semaphore xyfull = 0, zok = 0;

semaphore emx = 1, emy = 1;
semaphore emp = 10;

main() {
	Cobegin
		R1();
		R2();
		R3();
		R4();
	Coend
}

R1() {
	produceX();
	P(emx); // 暂存区中没有 x 
	P(emp); // 暂存区有位置
	
	P(mutex); // 锁定 x,y 尝试放 x
	putX();
	cntx += 1;
	if(cntx == 1 && cnty == 1) {
		V(emx);
		V(emy);
		V(emp);
		V(emp); 
		V(xyfull); // xy都有了，可以取出进行加工了 
	}
	V(mutex); 
}

R2() {
	produceY();
	P(emy);
	P(emp); 
	
	P(mutex);
	putY();
	cnty += 1;
	if(cntx == 1 && cnty == 1) {
		V(xyfull); // xy都有了，可以取出进行加工了 
	}
	V(mutex); 	
}

R3() {
	P(xyfull);
	P(mutex);
	
	V(emx);
	V(empty);
	cntx -= 1;
	getX();
	V(emy);
	V(empty);
	cnty -= 1;
	getY();
	
	produceZ();
	P(empty);
	putZ();
	V(zok);
	
	V(mutex);
	
}

R4() {
	P(zok);
	V(empty);
	getZ();
	produceP();
}
