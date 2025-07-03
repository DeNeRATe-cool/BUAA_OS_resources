semaphore empty = 1;
semaphore apple = 0;
semaphore orange = 0;

function father() {
	P(empty);
	char x = rand();
	if(x == "apple")
		V(apple);
	else 
		V(orange);
}

function daughter() {
	P(apple);
	V(empty);
}

function son() {
	P(orange);
	V(empty);
}
