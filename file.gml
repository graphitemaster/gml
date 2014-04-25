makething = fn(n) {
    y = n;
    while (y >= 0) {
        i = 0;
        while (i < y) {
            print(" ");
            i++;
        }
        x = 0;
        while (x + y < n) {
            if (x & y) {
                print("  ");
            } else {
                print("◢◣");
            }
            x++;
        }
        y--;
        print("\n");
    }
};

fun run(size) {
    print("Triangle", size);
    (makething(size))();
}

run(16);
run(32);
