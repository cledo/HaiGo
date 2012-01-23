#! /bin/bash

rm -f src/*.gcov
rm -f src/*.gcda
rm -f src/*.gcno
rm -f tests/*.gcov
rm -f tests/*.gcda
rm -f tests/*.gcno
lcov -d src/ -d tests/ -z

#FLAGS="-fprofile-arcs -ftest-coverage -g -O0"
FLAGS="--coverage -g -O0"

autoreconf --install && ./configure CFLAGS="$FLAGS" && prove


lcov -d src/ -d tests/ -c -o haigo.info
genhtml -o cover haigo.info

echo;
echo "Coverage:";
echo "=========";

cd src/
for i in *.c
do
    x=$(basename $i .c)
    #echo -n -e "$i:\t"
    printf "%20s: " $i
    PERC=$(gcov $i -o haigo-$x | egrep -o '[0-9]+\.[0-9]{2}%')
    printf "%8s\n" $PERC
    #gcov -b -c -f -o haigo-$x $i
done
cd ..

cd tests/
for i in *.c
do
    x=$(basename $i .c)
    #echo -n -e "$i:\t"
    printf "%20s: " $i
    PERC=$(gcov $i -o $x-$x | egrep -o '[0-9]+\.[0-9]{2}%')
    printf "%8s\n" $PERC
    #gcov -b -c -f -o haigo-$x $i
done
cd ..
