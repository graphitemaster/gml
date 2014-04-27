#!/usr/bin/gml

fn triangle(n) {
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
                print("**");
            }
            x++;
        }
        y--;
        print("\n");
    }
}

triangle(16);

fn join(x) => reduce(fn(x, y) => x + y, x)

print(join(["hello", " world"]));
