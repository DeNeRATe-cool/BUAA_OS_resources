/*
	同步：员工到时刷完卡需要等待；所有员工到了才能进入
	互斥：互斥访问刷卡机 
*/

semaphore barrier = 0;
semaphore mutex = 1;
int cnt = 0;

main() {
	Cobegin
		for(int i = 0; i < 5; i++)
			emp();
	Coend
}

emp() {
	while(1) {
		P(mutex);
		cnt += 1;
		if(cnt == 5) {
			V(barrier);
		}
		V(mutex);
		
		P(barrier);
		V(barrier);
		
		P(mutex);
		cnt -= 1;
		if(cnt == 0) {
			close_door();
		}
		V(mutex);
	}
}
