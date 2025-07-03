semaphore eatingmutex = 1;
int eating = 0; // 在吃饭的人 
int mustwait = 0; // 满5人时必须等待 
semaphore waiting = 0;

function customer() {
	P(eatingmutex);
	if(eating == 5) {
		mustwait = 1;
		P(waiting);
	} else if(eating < 5) {
		eating += 1;
	}
	V(eatingmutex);
	
	// eating...
	
	P(eatingmutex);
	eating -= 1;
	if(mustwait && eating == 0) {
		V(waiting);
	}
	V(eatingmutex);
}
 
