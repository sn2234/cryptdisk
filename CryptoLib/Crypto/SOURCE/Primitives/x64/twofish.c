/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */

 /** 
   @file c
   Implementation of Twofish by Tom St Denis 
 */
#include "twofish_ltc.h"

#include "twofish_tab.h"

#define sbox(i, x) ((ulong32)SBOX[i][(x)&255])


/* computes [y0 y1 y2 y3] = MDS . [x0] */

#define mds_column_mult(x, i) mds_tab[i][x]

/* Computes [y0 y1 y2 y3] = MDS . [x0 x1 x2 x3] */
__inline void mds_mult(const unsigned char *in, unsigned char *out)
{
  int x;
  ulong32 tmp;
  for (tmp = x = 0; x < 4; x++) {
      tmp ^= mds_column_mult(in[x], x);
  }
  STORE32L(tmp, out);
}

/* computes [y0 y1 y2 y3] = RS . [x0 x1 x2 x3 x4 x5 x6 x7] */
__inline void rs_mult(const unsigned char *in, unsigned char *out)
{
   ulong32 tmp;
   tmp = rs_tab0[in[0]] ^ rs_tab1[in[1]] ^ rs_tab2[in[2]] ^ rs_tab3[in[3]] ^
         rs_tab4[in[4]] ^ rs_tab5[in[5]] ^ rs_tab6[in[6]] ^ rs_tab7[in[7]];
   STORE32L(tmp, out);
}

/* computes h(x) */
__inline void h_func(const unsigned char *in, unsigned char *out, unsigned char *M, int k, int offset)
{
  int x;
  unsigned char y[4];
  for (x = 0; x < 4; x++) {
      y[x] = in[x];
 }
  switch (k) {
     case 4:
            y[0] = (unsigned char)(sbox(1, (ulong32)y[0]) ^ M[4 * (6 + offset) + 0]);
            y[1] = (unsigned char)(sbox(0, (ulong32)y[1]) ^ M[4 * (6 + offset) + 1]);
            y[2] = (unsigned char)(sbox(0, (ulong32)y[2]) ^ M[4 * (6 + offset) + 2]);
            y[3] = (unsigned char)(sbox(1, (ulong32)y[3]) ^ M[4 * (6 + offset) + 3]);
     case 3:
            y[0] = (unsigned char)(sbox(1, (ulong32)y[0]) ^ M[4 * (4 + offset) + 0]);
            y[1] = (unsigned char)(sbox(1, (ulong32)y[1]) ^ M[4 * (4 + offset) + 1]);
            y[2] = (unsigned char)(sbox(0, (ulong32)y[2]) ^ M[4 * (4 + offset) + 2]);
            y[3] = (unsigned char)(sbox(0, (ulong32)y[3]) ^ M[4 * (4 + offset) + 3]);
     case 2:
            y[0] = (unsigned char)(sbox(1, sbox(0, sbox(0, (ulong32)y[0]) ^ M[4 * (2 + offset) + 0]) ^ M[4 * (0 + offset) + 0]));
            y[1] = (unsigned char)(sbox(0, sbox(0, sbox(1, (ulong32)y[1]) ^ M[4 * (2 + offset) + 1]) ^ M[4 * (0 + offset) + 1]));
            y[2] = (unsigned char)(sbox(1, sbox(1, sbox(0, (ulong32)y[2]) ^ M[4 * (2 + offset) + 2]) ^ M[4 * (0 + offset) + 2]));
            y[3] = (unsigned char)(sbox(0, sbox(1, sbox(1, (ulong32)y[3]) ^ M[4 * (2 + offset) + 3]) ^ M[4 * (0 + offset) + 3]));
  }
  mds_mult(y, out);
}

/* the G function */
#define g_func(x, dum)  (S1[byte(x,0)] ^ S2[byte(x,1)] ^ S3[byte(x,2)] ^ S4[byte(x,3)])
#define g1_func(x, dum) (S2[byte(x,0)] ^ S3[byte(x,1)] ^ S4[byte(x,2)] ^ S1[byte(x,3)])

void twofish_setup(const unsigned char *key, int keylen, TWOFISH_KEY_CTX *skey)
{
   unsigned char S[4*4], tmpx0, tmpx1;
   int k, x, y;
   unsigned char tmp[4], tmp2[4], M[8*4];
   ulong32 A, B;

   /* k = keysize/64 [but since our keysize is in bytes...] */
   k = keylen / 8;

   /* copy the key into M */
   for (x = 0; x < keylen; x++) {
       M[x] = key[x] & 255;
   }

   /* create the S[..] words */
   for (x = 0; x < k; x++) {
       rs_mult(M+(x*8), S+(x*4));
   }
   for (x = 0; x < k; x++) {
       rs_mult(M+(x*8), (unsigned char*)skey->S+(x*4));
   }

   /* make subkeys */
   for (x = 0; x < 20; x++) {
       /* A = h(p * 2x, Me) */
       for (y = 0; y < 4; y++) {
           tmp[y] = x+x;
       }
       h_func(tmp, tmp2, M, k, 0);
       LOAD32L(A, tmp2);

       /* B = ROL(h(p * (2x + 1), Mo), 8) */
       for (y = 0; y < 4; y++) {
           tmp[y] = (unsigned char)(x+x+1);
       }
       h_func(tmp, tmp2, M, k, 1);
       LOAD32L(B, tmp2);
       B = ROLc(B, 8);

       /* K[2i]   = A + B */
       skey->K[x+x] = (A + B) & 0xFFFFFFFFUL;

       /* K[2i+1] = (A + 2B) <<< 9 */
       skey->K[x+x+1] = ROLc(B + B + A, 9);
   }

   /* make the sboxes (large ram variant) */
   if (k == 2) {
        for (x = 0; x < 256; x++) {
           tmpx0 = (unsigned char)sbox(0, x);
           tmpx1 = (unsigned char)sbox(1, x);
           skey->S[0][x] = mds_column_mult(sbox(1, (sbox(0, tmpx0 ^ S[0]) ^ S[4])),0);
           skey->S[1][x] = mds_column_mult(sbox(0, (sbox(0, tmpx1 ^ S[1]) ^ S[5])),1);
           skey->S[2][x] = mds_column_mult(sbox(1, (sbox(1, tmpx0 ^ S[2]) ^ S[6])),2);
           skey->S[3][x] = mds_column_mult(sbox(0, (sbox(1, tmpx1 ^ S[3]) ^ S[7])),3);
        }
   } else if (k == 3) {
        for (x = 0; x < 256; x++) {
           tmpx0 = (unsigned char)sbox(0, x);
           tmpx1 = (unsigned char)sbox(1, x);
           skey->S[0][x] = mds_column_mult(sbox(1, (sbox(0, sbox(0, tmpx1 ^ S[0]) ^ S[4]) ^ S[8])),0);
           skey->S[1][x] = mds_column_mult(sbox(0, (sbox(0, sbox(1, tmpx1 ^ S[1]) ^ S[5]) ^ S[9])),1);
           skey->S[2][x] = mds_column_mult(sbox(1, (sbox(1, sbox(0, tmpx0 ^ S[2]) ^ S[6]) ^ S[10])),2);
           skey->S[3][x] = mds_column_mult(sbox(0, (sbox(1, sbox(1, tmpx0 ^ S[3]) ^ S[7]) ^ S[11])),3);
        }
   } else {
        for (x = 0; x < 256; x++) {
           tmpx0 = (unsigned char)sbox(0, x);
           tmpx1 = (unsigned char)sbox(1, x);
           skey->S[0][x] = mds_column_mult(sbox(1, (sbox(0, sbox(0, sbox(1, tmpx1 ^ S[0]) ^ S[4]) ^ S[8]) ^ S[12])),0);
           skey->S[1][x] = mds_column_mult(sbox(0, (sbox(0, sbox(1, sbox(1, tmpx0 ^ S[1]) ^ S[5]) ^ S[9]) ^ S[13])),1);
           skey->S[2][x] = mds_column_mult(sbox(1, (sbox(1, sbox(0, sbox(0, tmpx0 ^ S[2]) ^ S[6]) ^ S[10]) ^ S[14])),2);
           skey->S[3][x] = mds_column_mult(sbox(0, (sbox(1, sbox(1, sbox(0, tmpx1 ^ S[3]) ^ S[7]) ^ S[11]) ^ S[15])),3);
        }
   }
}

void twofish_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const TWOFISH_KEY_CTX *skey)
{
    ulong32 a,b,c,d,ta,tb,tc,td,t1,t2;
    int r;
    const ulong32 *S1, *S2, *S3, *S4, *k;

    S1 = skey->S[0];
    S2 = skey->S[1];
    S3 = skey->S[2];
    S4 = skey->S[3];

    LOAD32L(a,&pt[0]); LOAD32L(b,&pt[4]);
    LOAD32L(c,&pt[8]); LOAD32L(d,&pt[12]);
    a ^= skey->K[0];
    b ^= skey->K[1];
    c ^= skey->K[2];
    d ^= skey->K[3];
    
    k  = skey->K + 8;
    for (r = 8; r != 0; --r) {
        t2 = g1_func(b, skey);
        t1 = g_func(a, skey) + t2;
        c  = RORc(c ^ (t1 + k[0]), 1);
        d  = ROLc(d, 1) ^ (t2 + t1 + k[1]);
        
        t2 = g1_func(d, skey);
        t1 = g_func(c, skey) + t2;
        a  = RORc(a ^ (t1 + k[2]), 1);
        b  = ROLc(b, 1) ^ (t2 + t1 + k[3]);
        k += 4;
   }

    /* output with "undo last swap" */
    ta = c ^ skey->K[4];
    tb = d ^ skey->K[5];
    tc = a ^ skey->K[6];
    td = b ^ skey->K[7];

    /* store output */
    STORE32L(ta,&ct[0]); STORE32L(tb,&ct[4]);
    STORE32L(tc,&ct[8]); STORE32L(td,&ct[12]);
}


void twofish_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const TWOFISH_KEY_CTX *skey)
{
    ulong32 a,b,c,d,ta,tb,tc,td,t1,t2;
    int r;
    const ulong32 *S1, *S2, *S3, *S4, *k;

    S1 = skey->S[0];
    S2 = skey->S[1];
    S3 = skey->S[2];
    S4 = skey->S[3];

    /* load input */
    LOAD32L(ta,&ct[0]); LOAD32L(tb,&ct[4]);
    LOAD32L(tc,&ct[8]); LOAD32L(td,&ct[12]);

    /* undo undo final swap */
    a = tc ^ skey->K[6];
    b = td ^ skey->K[7];
    c = ta ^ skey->K[4];
    d = tb ^ skey->K[5];

    k = skey->K + 36;
    for (r = 8; r != 0; --r) {
        t2 = g1_func(d, skey);
        t1 = g_func(c, skey) + t2;
        a = ROLc(a, 1) ^ (t1 + k[2]);
        b = RORc(b ^ (t2 + t1 + k[3]), 1);

        t2 = g1_func(b, skey);
        t1 = g_func(a, skey) + t2;
        c = ROLc(c, 1) ^ (t1 + k[0]);
        d = RORc(d ^ (t2 +  t1 + k[1]), 1);
        k -= 4;
    }

    /* pre-white */
    a ^= skey->K[0];
    b ^= skey->K[1];
    c ^= skey->K[2];
    d ^= skey->K[3];
    
    /* store */
    STORE32L(a, &pt[0]); STORE32L(b, &pt[4]);
    STORE32L(c, &pt[8]); STORE32L(d, &pt[12]);
}
