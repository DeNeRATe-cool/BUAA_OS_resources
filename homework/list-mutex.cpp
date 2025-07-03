semaphore c_m = 1; // 插入互斥 
semaphore mutex = 1; // 资源访问 

int sc_count = 0; // 插入和查询总量 
semaphore sc_m = 1; // 插入和查询总量变量互斥

function query() {
	P(sc_m);
	sc_count++;
	if(sc_count == 1)
		P(mutex);
	V(sc_m);
	
	// query...
	
	P(sc_m);
	sc_count--;
	if(sc_count == 0)
		V(mutex);
	V(sc_m);
} 

function insert() {
	P(c_m);
	 
	P(sc_m);
	sc_count++;
	if(sc_count == 1)
		P(mutex);
	V(sc_m);
	
	// insert..
	
	P(sc_m);
	sc_count--;
	if(sc_count == 0)
		V(mute);
	V(sc_m);
	
	V(c_m);
}

function delete() {
	P(mutex);
	
	// delete...
	
	V(mutex);
}
