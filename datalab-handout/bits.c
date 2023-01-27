/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* We do not support C11 <threads.h>.  */
/* 
 * bitNot - ~x without using ~
 *   Example: bitNot(0) = 0xffffffff
 *   Legal ops: ! & ^ | + << >>
 *   Max ops: 12
 *   Rating: 1
 */
int bitNot(int x)
{
  int mask = 0xff;
  mask = mask + (mask << 8);
  mask = mask + (mask << 16);
  return x ^ mask;
}
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y)
{
  return ((~(x & y)) & (~((~x) & (~y))));
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x)
{
  int re = 0;
  int mask = (0xaa << 8) + 0xaa;
  mask = (mask << 16) + mask;
  re = (mask & x) ^ mask;
  return !re;
}
/* 
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateRight(int x, int n)
{
  int mask = x >> 31;
  int re = 0;
  n = 32 + (~n + 1); //the example seems to be wrong...
  mask = mask << n;
  re = (x >> (31 + (~n + 1)));
  re = re >> 1;
  re = re ^ mask;
  return (x << n) | re;
}
/* 
 * palindrome - return 1 if x is palindrome in binary form,
 *   return 0 otherwise
 *   A number is palindrome if it is the same when reversed
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   Example: palindrome(0xff0000ff) = 1,
 *            palindrome(0xff00ff00) = 0
 *   Legal ops: ~ ! | ^ & << >> +
 *   Max ops: 40
 *   Rating: 4

 */
int palindrome(int x)
{
  int tar = x;
  x = ((x >> 16) & 0x0000ffff) | (x << 16);
  x = ((x >> 8) & 0x00ff00ff) | ((x << 8) & 0xff00ff00);
  x = ((x >> 4) & 0x0f0f0f0f) | ((x << 4) & 0xf0f0f0f0);
  x = ((x >> 2) & 0x33333333) | ((x << 2) & 0xcccccccc);
  x = ((x >> 1) & 0x55555555) | ((x << 1) & 0xaaaaaaaa);
  return !(tar + (~x + 1));
}
/*
 * countConsecutive1 - return the number of consecutive 1‘s
 *   in the binary form of x
 *   Examples: countConsecutive1(0xff00ff00) = 2,
 *             countConsecutive1(0xf070f070) = 4,
 *             countConsecutive1(0b00101010001111110101110101110101) = 10
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Rating: 4
 */
int countConsecutive1(int x)
{
  int mask1, mask2, mask3, mask4, mask5, y;
  int tmp = (0x55 << 8) + 0x55;
  int xLeft1 = x << 1;
  y = xLeft1 & x;
  x = x + (~y + 1);
  mask1 = (tmp << 16) + tmp; //0x55555555;
  tmp = (0x33 << 8) + 0x33;
  mask2 = (tmp << 16) + tmp;
  tmp = (0x0f << 8) + 0x0f;
  mask3 = (tmp << 16) + tmp;
  mask4 = (0xff << 16) + 0xff;
  mask5 = (0xff << 8) + 0xff;
  x = (x & mask1) + ((x >> 1) & mask1);
  x = (x & mask2) + ((x >> 2) & mask2);
  x = (x & mask3) + ((x >> 4) & mask3);
  x = (x & mask4) + ((x >> 8) & mask4);
  x = (x & mask5) + ((x >> 16) & mask5);
  return x;
}
/* 
 * counter1To5 - return 1 + x if x < 5, return 1 otherwise, we ensure that 1<=x<=5
 *   Examples counter1To5(2) = 3,  counter1To5(5) = 1
 *   Legal ops: ~ & ! | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int counter1To5(int x)
{
  int flag = !!(x + (~5 + 1));
  flag = ~flag + 1;
  return (x & flag) + 1;
}
/*
 * fullSub - 4-bits sub using bit-wise operations only.
 *   (0 <= x, y < 16) 
 *   Example: fullSub(12, 7) = 0x5,
 *            fullSub(7, 8) = 0xf,
 *   Legal ops: ~ | ^ & << >>
 *   Max ops: 35
 *   Rating: 2
 * 
 */
int fullSub(int x, int y)
{
  int tmp1, tmp2, tmp3, tmp4;
  y = ~y;
  tmp1 = y ^ 1;
  tmp2 = (y & 1) << 1;
  tmp3 = tmp1 ^ tmp2;
  tmp4 = (tmp1 & tmp2) << 1;
  tmp1 = (tmp3 ^ tmp4);
  tmp2 = (tmp3 & tmp4) << 1;
  y = tmp1 ^ tmp2;
  tmp1 = x ^ y;
  tmp2 = (x & y) << 1;
  tmp3 = tmp1 ^ tmp2;
  tmp4 = (tmp1 & tmp2) << 1;
  tmp1 = (tmp3 ^ tmp4);
  tmp2 = (tmp3 & tmp4) << 1;
  return (tmp1 ^ tmp2) & 0xf;
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y)
{
  int Signx = (x >> 31) & 1;
  int Signy = (y >> 31) & 1;
  int DifSignBit = Signx ^ Signy; //same: 0;
  return (DifSignBit & Signx) | ((!DifSignBit) & (!(((y + (~x + 1)) >> 31) & 1)));
}
/* 
 * sm2tc - Convert from sign-magnitude to two's complement
 *   where the MSB is the sign bit
 *   Example: sm2tc(0x80000005) = -5.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int sm2tc(int x)
{
  int j = (x >> 31) & 1;
  int mask = ~(j << 31) + (!j);
  x = x ^ mask;
  return x + j;
}

/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y)
{
  int tmax = ~(1 << 31);
  int signx = (x >> 31) & 1;
  int signy = (y >> 31) & 1;
  int xysignDif = signx ^ signy;
  int res = x + y;
  int notOverflow = !((!(xysignDif)) & (signx ^ ((res >> 31) & 1)));
  notOverflow = (notOverflow << 31) >> 31;
  signx = (signx << 31) >> 31;
  res = (res & notOverflow) | ((((!notOverflow) & signx) + tmax) & (((!notOverflow) << 31) >> 31));
  return res;
}
/*
 * trueFiveEighths - multiplies by 5/8 rounding toward 0,
 *  avoiding errors due to overflow
 *  Examples: trueFiveEighths(11) = 6
 *            trueFiveEighths(-9) = -5u
 *            trueFiveEighths(0x30000000) = 0x1E000000 (no overflow)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 4
 */
int trueFiveEighths(int x)
{
  int bias8 = (x >> 31) & 0x7;
  int bias2 = (x >> 31) & 1;
  int bias = (((x >> 2) & 1) ^ bias2) & (x & 1);
  int xDiv8 = (x + bias8) >> 3;
  int xDiv2 = (x + bias2) >> 1;
  int res = xDiv8 + xDiv2 + (bias & (!bias2)) + (~(bias2 & bias) + 1);
  return res;
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf)
{
  int sign = ((uf >> 31) & 1) << 31;
  int exp = (uf >> 23) & 0xff;
  if (exp == 0xff)
    return uf;
  if (exp == 0)
    return (uf << 1) | (sign);
  ++exp;
  if (exp == 0xff)
    return (exp << 23) | (sign);
  return (exp << 23) | (uf & 0x807fffff);
}
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf)
{
  int sign = (uf >> 31) & 1;
  int exp = (uf >> 23) & 0xff;
  int bias = (uf >> 1) & (uf & 1);
  if (0xff == exp)
    return uf;
  if (exp == 0 || exp == 1)
    return (((uf & 0x7fffffff) >> 1) | (sign << 31)) + bias;
  --exp;
  return (exp << 23) | (uf & 0x807fffff);
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf)
{
  int E = ((uf >> 23) & 0xff) - 127;
  int sign = (uf >> 31) & 1;
  int frac = (uf & 0x7fffff) | 0x800000;
  if (E > 30)
    return 0x80000000u;
  if (E < 0)
    return 0;
  if (E >= 23)
    frac = frac << (E - 23);
  else if (E < 23)
    frac = frac >> (23 - E);
  if (!(((frac >> 31) & 1) ^ sign))
    return frac;
  else if (sign)
    return ~frac + 1;
  return 0x80000000u;
}
/* 
 * float_pwr2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned float_pwr2(int x)
{
  int exp;
  if (x >= 0x80)
    return (0xff << 23);
  exp = x + 127;
  if (exp <= -23)
    return 0;
  if (exp <= 0)
    return 1 << (exp + 22);
  return exp << 23;
}
