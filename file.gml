#!/usr/bin/gml

fn test(a, b) {
    fn() {
        if (a <= b) {
            r = a;
            a = a + 1;
            r;
        }
    };
}

t1 = test(5, 10);
print(t1());
print(t1());
print(t1());
