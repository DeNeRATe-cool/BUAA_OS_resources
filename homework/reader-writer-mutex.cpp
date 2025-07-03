int writecount = 0;
int readcount = 0;
semaphore RWmutex = 1; // reader & writer data·ÃÎÊ»¥³â 
semaphore readermutex = 1; // »¥³â·ÃÎÊ readcount
semaphore writermutex = 1; // »¥³â·ÃÎÊ writecount o'ra 
semaphore WWmutex = 1; // writer data·ÃÎÊ»¥³â£¬ÓÅÏÈ»½ÐÑwriter 

function writer() {
	P(writermutex);
	writecount += 1;
	if(writecount == 1) {
		P(RWmutex);
	}
	P(WWmutex);
	V(writermutex);
	
	// write...
	
	P(writemutex)
	V(WWmutex);
	writecount -= 1;
	if(writecount == 0) {
		V(RWmutex);
	}
	V(writemutex);
	
}

function reader() {
	P(readermutex);
	readcount += 1;
	if(readcount == 1) {
		P(RWmutex);
	}
	V(readermutex);
	
	// read...
	
	P(readermutex);
	readcount -= 1;
	if(readcount == 0) {
		V(RWmutex);
	}
	V(readermutex);
} 

