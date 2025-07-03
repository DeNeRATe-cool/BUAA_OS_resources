semaphore yunxing = 0;

semaphore waiting = 0;

int cnt = 0;
semaphore cc = 1;

semaphore xiache = 0;

semaphore zaike = 0;

main() {
	Cobegin
		for(int i = 0; i < n; i++)
			Passenger();
		Car();
	Coend
}

Passenger() {
	while(1) {
		P(zaike); // 是载客状态 
		P(cc);
		// 上车 
		cnt += 1;
		if(cnt == C) {
			V(yunxing) 
		} else {
			V(zaike); // 通知下一个上车 
		}
		V(cc);
		
		P(xiache); // 是卸客状态 
		P(cc);
		// 下车 
		cnt -= 1;
		if(cnt == 0) {
			V(waiting);
		} else {
			V(xiache);
		}
		V(cc);
	}
}

Car() {
	while(1) {
		// 允许上车 
		V(zaike);
		// 等待所有人上车 
		P(yunxing);
		// 运行
		V(xiache);
		// 等待所有人下车 
		P(waiting);
	}
}
