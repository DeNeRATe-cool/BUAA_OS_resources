/*
	互斥：读者和写者 写者和写者 读者缓冲区 
	同步：有空闲入 读完释放缓冲区 
*/ 
semaphore mutex = 1;
semaphore empty = 10;
semaphore write = 1;
int cnt = 0; // 写者数量
semaphore mc = 1; // 互斥访问 cnt 

main() {
	int n1, n2; // 写/读者数量 
	Cobegin
		for(int i = 0; i < n1; i++)
			reader();
		for(int i = 0; i < n2; i++)
			writer();
	Coend
}

writer() {
	while(1) {
		P(empty);
		P(mutex);
		P(mc);
		cnt += 1;
		if(cnt == 1) {
			P(write);
		}
		V(mc);
		
		do_read();
		
		P(mc);
		cnt -= 1;
		if(cnt == 1) {
			V(write);
		}
		V(mc);
		
		V(mutex);
		V(empty);
	}
}

reader() {
	while(1) {
		P(mutex);
		P(write);
		
		do_write();
		
		V(write);
		V(mutex);
	}
}
