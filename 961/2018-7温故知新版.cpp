Car() {
	while(1) {
		
		P(yxzt);
		
		V(xkzt);
		
		P(xkwb);
		
		V(zkzt);
	}
}

Passenger() {
	while(1) {
		P(zkzt);
		V(zkzt);
		
		P(mc);
		// …œ≥µ 
		cnt += 1;
		if(cnt == C) {
			P(zkzt);
			V(yxzt);
		}
		V(mc);
		
		P(xkzt);
		V(xkzt);
		
		P(mc);
		cnt -= 1;
		if(cnt == 0) {
			P(xkzt);
			V(xkwb);
		}
		V(mc);
	}
}
