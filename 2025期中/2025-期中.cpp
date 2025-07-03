int cnt = 0; 
semaphore mc = 1;

semaphore mutex = 1;

int ticket = N;

main() {
	Cobegin
		Q();
		B();
	Coend
}

Q() {
	while(1) {
		P(mc);
		cnt += 1;
		if(cnt == 1) {
			P(mutex);
		}
		V(mc);
		
		query_ticket();
		
		P(mc);
		cnt -= 1;
		if(cnt == 0) {
			V(mutex);
		}
		V(mc);
	}
}

B() {
	while(1) {
		P(mutex);
		if(tikcet > 0) {
			tikcet -= 1;
			success_buy_tikcet();
		} else {
			fail_buy_ticket();
		}
		V(mutex);
	}
}
