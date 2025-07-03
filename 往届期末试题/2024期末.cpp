/*
	互斥：写进程和读写进程互斥访问缓冲区
	同步：缓冲区有空才可以写 
*/

semaphore mutex = 1;

int cnt = 0; // 访问人数 
semaphore mc = 1;

semaphore full = N; // 系统访问阈值

int capacity = M; // 课程容量 

main() {
	Cobegin
		browse_course();
		select_course();
	Coend
} 

browse_course() {
	while(1) {
		P(full);
		P(mc);
		cnt += 1;
		if(cnt == 1)
			P(mutex);
		V(mc);
		
		browse_course(); // 查询课程容量
		
		P(mc);
		cnt -= 1;
		if(cnt == 0)
			V(mutex);
		V(mc);
		V(full); 
	}
}

selelt_course() {
	while(1) {
		P(full); // 可否访问网站 
		P(mutex); // 进行选课
		
		if(capacity > 0) {
			select_course(); // 选课
			capacity -= 1;
		}

		V(mutex);		
		V(full);
	}
}
