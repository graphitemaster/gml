#!/usr/bin/gml

a = [ [ 1, 2 ], [3, 4] ];
a = a + a;
a = a + a;
a = a + a;
a = a + a;
a = a + a;

a = filter(fn(x) => x == [1, 2], a);
a = reduce(fn(x, y) => x, a);

println(a);


// cubes produced from even numbers in the range (0, 10)
// when sumed = 800
