#!/bin/bash
assert() {
	expected="$1"
	input="$2"

	./abcc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		mv tmp.s tmp_fail.s
		exit 1
	fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 "12 + 34 - 5;"
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4  "(3+5)/2;"
assert 10 "-10+20;"
assert 10 "- -10;"
assert 10 "- -  +10;"
# 比較Trueテスト
assert 1 "5 >= 2;"
assert 1 "5 == 5;"
assert 1 "5 != 2;"
assert 1 "2 <= 5;"
assert 1 "5 > 2;"
assert 1 "2 < 5;"
# Falseテスト
assert 0 "2 >= 5;"
assert 0 "5 == 2;"
assert 0 "5 != 5;"
assert 0 "5 <= 2;"
assert 0 "2 > 5;"
assert 0 "5 < 2;"
assert 1 "(5==5) == (2==2);"
assert 1 "(5 > 2) != (2!=2);"

# assignテスト
assert 5 "a=3+2;a;"
assert 5 "foo = 3+2;foo;"

assert 8 "a=3; b=5; a + b;"
assert 8 "foo=3; bar = 5; foo + bar;"

assert 5 "a = 3;b = 5 * 6 - 8;(a + b) / 5;"
assert 5 "foo = 3; bar = 5 * 6 - 8; ( foo + bar) / 5;"

# return テスト
assert 14 "a=3; b = 5 * 6 - 8; return a + b /2;"

assert 0 "a = 3; a ; return 0;"

assert 33 "a=3; a; return a + 10 * a;"

assert 1 "a = 0; a = a + 1; return a;"
assert 43 "a = 42; a = a + 1; return a;"

assert 0 "a=10; a < 10;"

assert 10 "a = 0; while (a < 10) a=a+1; return a;"
assert 10 "a = 10; while (a < 10) a=a+1; return a;"
assert 4 "a = 5; if (a == 5) 4;"
assert 4 "a = 5; if (a == 5) return 4; 12;"
assert 12 "a = 5; if (a == 1) return 4; 12;"
assert 12 "a = 5; if (a != 5) return 4; 12;"
assert 4 "a = 5; if (a == 5) return 4; else return 12;" 
assert 12 "a = 9; if (a == 5) return 4; else return 12;" 
assert 4 "a = 5; if (a == 5) return 4; else return 12;"
assert 10 "for(a=0;a < 10; a=a+1) a; return a;"
assert 10 "a=0;for(;a < 10; a=a+1) a; return a;"
assert 10 "a=0;for(;; a=a+1) if (a == 10) return a;" # たまにテストが落ちる。
assert 10 "a=0;for(;;) if (a == 10) return a; else a = a + 1;" # たまにテストが落ちる。

echo OK
