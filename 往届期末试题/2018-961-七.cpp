semaphore xiache = 0;
semaphore shangche = 0;
semaphore chufa = 0;
semaphore empty = 0;
int cnt = 0;
semaphore cnt_mutex = 1;

/*
卸客 - 装客 - 出发 
*/ 

function bus() {
	V(shangche);
	P(chufa);
	V(xiache);
	P(empty);
}

function passenger() {
	P(shangche);
	P(cnt_mutex);
	cnt += 1;
	if(cnt == C) {
		V(chufa);
	} else {
		V(shangche);
	}
	V(cnt_mutex);
	P(xiache);
	P(cnt_mutex);
	cnt -= 1;
	if(cnt == 0) {
		V(empty);
	} else {
		V(xaiche);
	}
	V(cnt_mutex);
}
