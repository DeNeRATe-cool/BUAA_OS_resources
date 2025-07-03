semaphore ...

main() {
    Cobegin
        R1();
        ..
    Coend
}

R1() {
    while(1) {
        do()
        ...
        P();
        V();
    }
}