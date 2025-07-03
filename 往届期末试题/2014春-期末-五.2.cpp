int cnt1 = 0, cnt2 = 0;
semaphore cnt1m = 1, cnt2m = 1;
semaphore m = 1;

function p1() {
	P(cnt1m);
	cnt1 += 1;
	if(cnt1 == 1) {
		P(m);
	}
	V(cnt1m);
	
	P(cnt1m);
	cnt1m -= 1;
	if(cnt1m == 0) {
		V(m);
	}
	V(cnt1m);
}

function p2() {
	P(cnt2m);
	cnt2 += 1;
	if(cnt2 == 1) {
		P(m);
	}
	V(cnt2m);
	
	P(cnt2m);
	cnt2m -= 1;
	if(cnt2m == 0) {
		V(m);
	}
	V(cnt2m);
}
