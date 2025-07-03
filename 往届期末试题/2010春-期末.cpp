procedure Reader {
	while(true) {
		// 输入数据data1;
		P(empty);
		P(mutex);
			buffer(inR) := data1;
			inR := (inR+1) mod (k);
		V(mutex);
		V(full);
	}
}

procedure Handler {
	while(true) {
		P(full);
		P(mutex);
			data2 := buffer(outR);
			outR:= (outR+1) mod (k);
		V(mutex);
		// 对data2加工;
		P(mutex);
			buffer(inP) := data2;
			inP:= (inP+1) mod (k);
		V(mutex);
		V(ok);
	}
}

procedure Printer {
	while(true) {
		P(ok);
		P(mutex);
			data3 := buffer(outP);
			outP := (outP+1) mod (k);
		V(mutex);
		V(empty);
		// 打印data3;
	}
}
