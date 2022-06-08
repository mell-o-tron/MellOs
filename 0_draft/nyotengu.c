// Nyotengu.c - a.k.a. 'SHETENGU' - the skydogess exact searcher, written by Kaze, 2020-Oct-10
// For contacts: sanmayce@sanmayce.com

#ifdef _N_HIGH_PRIORITY
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms686219.aspx
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#endif

void x64toaKAZE (      /* stdcall is faster and smaller... Might as well use it for the helper. */
        unsigned long long val,
        char *buf,
        unsigned radix,
        int is_neg
        )
{
        char *p;                /* pointer to traverse string */
        char *firstdig;         /* pointer to first digit */
        char temp;              /* temp char */
        unsigned digval;        /* value of digit */

        p = buf;

        if ( is_neg )
        {
            *p++ = '-';         /* negative, so output '-' and negate */
            val = (unsigned long long)(-(long long)val);
        }

        firstdig = p;           /* save pointer to first digit */

        do {
            digval = (unsigned) (val % radix);
            val /= radix;       /* get next digit */

            /* convert to ascii and store */
            if (digval > 9)
                *p++ = (char) (digval - 10 + 'a');  /* a letter */
            else
                *p++ = (char) (digval + '0');       /* a digit */
        } while (val > 0);

        /* We now have the digit of the number in the buffer, but in reverse
           order.  Thus we reverse them now. */

        *p-- = '\0';            /* terminate string; p points to last digit */

        do {
            temp = *p;
            *p = *firstdig;
            *firstdig = temp;   /* swap *p and *firstdig */
            --p;
            ++firstdig;         /* advance to next two digits */
        } while (firstdig < p); /* repeat until halfway */
}

/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

char * _i64toaKAZE (
        long long val,
        char *buf,
        int radix
        )
{
        x64toaKAZE((unsigned long long)val, buf, radix, (radix == 10 && val < 0));
        return buf;
}

char * _ui64toaKAZE (
        unsigned long long val,
        char *buf,
        int radix
        )
{
        x64toaKAZE(val, buf, radix, 0);
        return buf;
}

char * _ui64toaKAZEzerocomma (
        unsigned long long val,
        char *buf,
        int radix
        )
{
                        char *p;
                        char temp;
                        int txpman;
                        int pxnman;
        x64toaKAZE(val, buf, radix, 0);
                        p = buf;
                        do {
                        } while (*++p != '\0');
                        p--; // p points to last digit
                             // buf points to first digit
                        buf[26] = 0;
                        txpman = 1;
                        pxnman = 0;
                        do
                        { if (buf <= p)
                          { temp = *p;
                            buf[26-txpman] = temp; pxnman++;
                            p--;
                            if (pxnman % 3 == 0)
                            { txpman++;
                              buf[26-txpman] = (char) (',');
                            }
                          }
                          else
                          { buf[26-txpman] = (char) ('0'); pxnman++;
                            if (pxnman % 3 == 0)
                            { txpman++;
                              buf[26-txpman] = (char) (',');
                            }
                          }
                          txpman++;
                        } while (txpman <= 26);
        return buf;
}

char * _ui64toaKAZEcomma (
        unsigned long long val,
        char *buf,
        int radix
        )
{
                        char *p;
                        char temp;
                        int txpman;
                        int pxnman;
        x64toaKAZE(val, buf, radix, 0);
                        p = buf;
                        do {
                        } while (*++p != '\0');
                        p--; // p points to last digit
                             // buf points to first digit
                        buf[26] = 0;
                        txpman = 1;
                        pxnman = 0;
                        while (buf <= p)
                        { temp = *p;
                          buf[26-txpman] = temp; pxnman++;
                          p--;
                          if (pxnman % 3 == 0 && buf <= p)
                          { txpman++;
                            buf[26-txpman] = (char) (',');
                          }
                          txpman++;
                        } 
        return buf+26-(txpman-1);
}

char * _ui64toaKAZEzerocomma4 (
        unsigned long long val,
        char *buf,
        int radix
        )
{
                        char *p;
                        char temp;
                        int txpman;
                        int pxnman;
        x64toaKAZE(val, buf, radix, 0);
                        p = buf;
                        do {
                        } while (*++p != '\0');
                        p--; // p points to last digit
                             // buf points to first digit
                        buf[26] = 0;
                        txpman = 1;
                        pxnman = 0;
                        do
                        { if (buf <= p)
                          { temp = *p;
                            buf[26-txpman] = temp; pxnman++;
                            p--;
                            if (pxnman % 4 == 0)
                            { txpman++;
                              buf[26-txpman] = (char) (',');
                            }
                          }
                          else
                          { buf[26-txpman] = (char) ('0'); pxnman++;
                            if (pxnman % 4 == 0)
                            { txpman++;
                              buf[26-txpman] = (char) (',');
                            }
                          }
                          txpman++;
                        } while (txpman <= 26);
        return buf;
}

      char llTOaDigits[27]; // 9,223,372,036,854,775,807: 1(sign or carry)+19(digits)+1('\0')+6(,)
      // below duplicates are needed because of one_line_invoking need different buffers.
      char llTOaDigits2[27]; // 9,223,372,036,854,775,807: 1(sign or carry)+19(digits)+1('\0')+6(,)
      char llTOaDigits3[27]; // 9,223,372,036,854,775,807: 1(sign or carry)+19(digits)+1('\0')+6(,)
      char llTOaDigits4[27]; // 9,223,372,036,854,775,807: 1(sign or carry)+19(digits)+1('\0')+6(,)
      char llTOaDigits5[27]; // 9,223,372,036,854,775,807: 1(sign or carry)+19(digits)+1('\0')+6(,)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // Needed for uint32_t
#include <string.h>

//Only one must be uncommented:
//#define _WIN32_ENVIRONMENT_
//#define _POSIX_ENVIRONMENT_

#include <time.h>

clock_t clocks0, clocks1, clocks2;

#if defined(_WIN32_ENVIRONMENT_)
#include <io.h> // needed for Windows' 'lseeki64' and 'telli64'
//Above line must be commented in order to compile with Intel C compiler: an error "can't find io.h" occurs.
#include <fcntl.h> // 2020-Jan-13
#else
#endif /* defined(_WIN32_ENVIRONMENT_)  */

#ifdef XMMtengu
#include <emmintrin.h> // SSE2 intrinsics
#include <smmintrin.h> // SSE4.1 intrinsics
#endif
#ifdef YMMtengu
#include <emmintrin.h> // SSE2 intrinsics
#include <smmintrin.h> // SSE4.1 intrinsics
#include <immintrin.h> // AVX intrinsics
#endif
#ifdef ZMMtengu
#include <emmintrin.h> // SSE2 intrinsics
#include <smmintrin.h> // SSE4.1 intrinsics
#include <immintrin.h> // AVX intrinsics
#include <zmmintrin.h> // AVX2 intrinsics, definitions and declarations for use with 512-bit compiler intrinsics.
#endif

// Railgun_Trolldom (the successor of Railgun_Swampshine_BailOut - avoiding second pattern comparison in BMH2 and pseudo-BMH4), copyleft 2016-Aug-19, Kaze.
// Railgun_Swampshine_BailOut, copyleft 2016-Aug-10, Kaze.
// Internet "home" page: http://www.codeproject.com/Articles/250566/Fastest-strstr-like-function-in-C
// My homepage (homeserver, often down): http://www.sanmayce.com/Railgun/
/*
!!!!!!!!!!!!!!!!!!!!!!!! BENCHMARKING GNU's memmem vs Railgun !!!!!!!!!!!!!!!!!!!!!!!! [
Add-on: 2016-Aug-22

Two things.

First, the fix from the last time was buggy, my apologies, now fixed, quite embarrassing since it is a simple left/right boundary check. It doesn't affect the speed, it appears as rare pattern hit misses.
Since I don't believe in saying "sorry" but in making things right, here my attempt to further disgrace my amateurish work follows:
Two years ago, I didn't pay due attention to adding 'Swampwalker' heuristic to the Railgun_Ennearch, I mean, only quick test was done and no real proofing - this was due not to a blunder of mine, nor carelessness, but overconfidence in my ability to write "on the fly". Stupid, indeed, however, when a coder gets momentum in writing simple etudes he starts gaining false confidence of mastering the subject, not good for sure!
Hopefully, other coders will learn to avoid such full of neglect style.

Second, wanted to present the heaviest testbed for search i.e. memmem() functions: it benefits the benchmarking (speed in real application) as well as bug-control.

The benchmark is downloadable at my INTERNET drive:
https://1drv.ms/u/s!AmWWFXGMzDmEglwjlUtnMJrfhosK

The speed showdown has three facets:
- compares the 64bit code generated from GCC 5.10 versus Intel 15.0 compilers;
- compares four types of datasets - search speed through English texts versus genome ACGT-type data versus binary versus UTF8;
- compares the tweaked Two-Way algorithm (implemented by Eric Blake) and adopted by GLIBC as memmem() versus my Railgun_Swampshine.

Note1: The GLIBC memmem() was taken from latest (2016-08-05) glibc 2.24 tar:
https://www.gnu.org/software/libc/
Note2: Eric Blake says that he enhanced the linearity of Two-Way by adding some sublinear paths, well, Railgun is all about sublinearity, so feel free to experiment with your own testfiles (worst-case-scenarios), just make such a file feed the compressor with it, then we will see how the LINEAR Two-Way behaves versus Railgun_Swampshine.
Note3: Just copy-and-paste 'Railgun_Swampshine' or 'Railgun_Ennearch' from the benchmark's source.

So the result on Core 2 Q9550s @2.83GHz DDR2 @666MHz / i5-2430M @3.00GHz DDR3 @666MHz:
--------------------------------------------------------------------------------------------------------------------------------
| Searcher                                  | GNU/GLIBC memmem()        | Railgun_Swampshine       | Railgun_Trolldom          | 
|--------------------------------------------------------------------------------------------------|---------------------------|
| Testfile\Compiler                         | Intel 15.0 | GCC 5.10     | Intel 15.0 | GCC 5.10    | Intel 15.0  | GCC 5.10    |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 27,703 bytes                        |     4506/- |   5330/14725 |    13198/- | 11581/15171 | 19105/22449 | 15493/21642 |
| Name: An_Interview_with_Carlos_Castaneda.TXT                          |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 308,062               |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 3,242,492,648       |            |             |             |             |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 2,347,772 bytes                     |      190/- |      226/244 |     1654/- |   1729/1806 |   1794/1822 |   1743/1809 |
| Name: Gutenberg_EBook_Don_Quixote_996_(ANSI).txt                      |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 14,316,954            |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 6,663,594,719,173   |            |             |             |             |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 899,425 bytes                       |      582/- |      760/816 |     3094/- |   2898/3088 |   3255/3289 |   2915/3322 |
| Name: Gutenberg_EBook_Dokoe_by_Hakucho_Masamune_(Japanese_UTF8).txt   |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 3,465,806             |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 848,276,034,315     |            |             |             |             |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 4,487,433 bytes                     |      104/- |      109/116 |      445/- |     458/417 |     450/411 |     467/425 |
| Name: Dragonfly_genome_shotgun_sequence_(ACGT_alphabet).fasta         |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 20,540,375            |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 13,592,530,857,131  |            |             |             |             |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 954,035 bytes                       |       99/- |      144/144 |      629/- |     580/682 |     634/807 |     585/725 |
| Name: LAOTZU_Wu_Wei_(BINARY).pdf                                      |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 27,594,933            |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 8,702,455,122,519   |            |             |             |             |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 15,583,440 bytes                    |        -/- |          -/- |        -/- |     663/771 |     675/778 |     663/757 |
| Name: Arabian_Nights_complete.html                                    |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 72,313,262            |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 105,631,163,854,099 |            |             |             |             |
--------------------------------------------------------------------------------------------------------------------------------

Note0: Railgun_Trolldom is slightly faster (both with Intel & GCC) than Railgun_Swampshine, this is mostly due to adding a bitwise BMH order 2 (8KB table overhead instead of 64KB) path - for haystacks <77777 bytes long - the warm-up is faster.
Note1: The numbers represent the rate (bytes/s) at which patterns/needles 4,5,6,8,9,10,12,13,14,16,17,18,24 bytes long are memmemed into 4KB, 256KB, 1MB, 256MB long haystacks.
in fact, these numbers are the compression speed using LZSS and memmem() as matchfinder.
Note2: The Arabian Nights is downloadable at:
https://ebooks.adelaide.edu.au/b/burton/richard/b97b/complete.html
Note3: On i5-2430M, TW is catching up since this CPU crunches instructions faster while the RAM speed is almost the same, Railgun suffers from the slow RAM fetches - the prefetcher and such suck.
Note4: With a simple English text 'Tales of 1001 Nights', 15,583,440 bytes long, the cumulative size of traversed haystack data is nearly 100TB, 105,631,163,854,099 ~ 1024^4 = 1,099,511,627,776.
Note5: With a simple French text 'Agatha_Christie_85-ebooks_(French)_TXT.tar', 32,007,168 bytes long, the cumulative size of traversed haystack data is nearly 200TB ~ 234,427,099,834,376.

Just to see how faster is Yann's Zstd in decompression (its level 12 is 377-331 MB/s faster), on Core 2 Q9550s @2.83GHz DDR2 @666MHz:
[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>Nakamichi_Kintaro++_Intel_15.0_64bit.exe Agatha_Christie_85-ebooks_(French)_TXT.tar
Nakamichi 'Kintaro++', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced, muffinesque suggestion by Jim Dempsey enforced.
Note1: This compile can handle files up to 1711MB.
Note2: The matchfinder/memmem() is Railgun_Trolldom.
Current priority class is HIGH_PRIORITY_CLASS.
Compressing 32007168 bytes ...
|; Each rotation means 64KB are encoded; Done 100%; Compression Ratio: 3.53:1
NumberOfFullLiterals (lower-the-better): 164
NumberOfFlushLiteralsHeuristic (bigger-the-better): 184323
Legend: WindowSizes: 1/2/3/4=Tiny/Short/Medium/Long
NumberOf(Tiny)Matches[Short]Window (4)[2]: 226869
NumberOf(Short)Matches[Short]Window (8)[2]: 119810
NumberOf(Medium)Matches[Short]Window (12)[2]: 71202
NumberOf(Long)Matches[Short]Window (16)[2]: 31955
NumberOf(MaxLong)Matches[Short]Window (24)[2]: 7078
NumberOf(Tiny)Matches[Medium]Window (5)[3]: 257313
NumberOf(Short)Matches[Medium]Window (9)[3]: 526493
NumberOf(Medium)Matches[Medium]Window (13)[3]: 285579
NumberOf(Long)Matches[Medium]Window (17)[3]: 158873
NumberOf(MaxLong)Matches[Medium]Window (24)[3]: 51276
NumberOf(Tiny)Matches[Long]Window (6)[4]: 41075
NumberOf(Short)Matches[Long]Window (10)[4]: 240454
NumberOf(Medium)Matches[Long]Window (14)[4]: 258653
NumberOf(Long)Matches[Long]Window (18)[4]: 209007
NumberOf(MaxLong)Matches[Long]Window (24)[4]: 190929
RAM-to-RAM performance: 605 bytes/s.
Compressed to 9076876 bytes.
LATENCY-WISE: Number of 'memmem()' Invocations: 102,091,852
THROUGHPUT-WISE: Number of Total bytes Traversed: 234,427,099,834,376

D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>"Nakamichi_Kintaro++_Intel_15.0_64bit.exe" "Agatha_Christie_85-ebooks_(French)_TXT.tar.Nakamichi"
Nakamichi 'Kintaro++', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced, muffinesque suggestion by Jim Dempsey enforced.
Note1: This compile can handle files up to 1711MB.
Note2: The matchfinder/memmem() is Railgun_Trolldom.
Current priority class is HIGH_PRIORITY_CLASS.
Decompressing 9076876 bytes ...
RAM-to-RAM performance: 331 MB/s.
Compression Ratio (bigger-the-better): 3.53:1

D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>zstd-windows-v0.8.1_win64.exe -h
*** zstd command line interface 64-bits v0.8.1, by Yann Collet ***
...

D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>zstd-windows-v0.8.1_win64.exe -b12 "Agatha_Christie_85-ebooks_(French)_TXT.tar"
12#_(French)_TXT.tar :  32007168 ->   8965791 (3.570),   6.7 MB/s , 377.0 MB/s

D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>zstd-windows-v0.8.1_win64.exe -b22 "Agatha_Christie_85-ebooks_(French)_TXT.tar"
22#_(French)_TXT.tar :  32007168 ->   6802321 (4.705),   1.0 MB/s , 260.7 MB/s

D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>
]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

Two-Way is significantly slower than BMH Order 2, the speed-down is in range:
- for TEXTUAL ANSI alphabets: 1729/226= 7.6x
- for TEXTUAL UTF8 alphabets: 2898/760= 3.8x
- for TEXTUAL ACGT alphabets:  458/109= 4.2x
- for BINARY-esque alphabets:  580/144= 4.0x

For faster RAM, than mine @666MHz, and for haystacks multimegabytes long, the speedup goes beyond 8x.

The benchmark shows the real behavior (both latency and raw speed) of the memmem variants, I added also the Thierry Lecroq's Two-Way implementation:
http://www-igm.univ-mlv.fr/~lecroq/string/node26.html#SECTION00260
However, Eric Blake's one is faster, so it was chosen for the speed showdown.

Once I measured the total length of traversed haystacks, and for files 100+MB long, it went ... quintillion of bytes i.e. petabytes - good torture it is.

!!!!!!!!!!!!!!!!!!!!!!!! BENCHMARKING GNU's memmem vs Railgun !!!!!!!!!!!!!!!!!!!!!!!! ]
*/
// 2014-Apr-27: The nasty SIGNED/UNSIGNED bug in 'Swampshines' which I illustrated several months ago in my fuzzy search article now is fixed here too:
/*
The bug is this (the variables 'i' and 'PRIMALposition' are uint32_t):
Next line assumes -19 >= 0 is true:
if ( (i-(PRIMALposition-1)) >= 0) printf ("THE NASTY BUG AGAIN: %d >= 0\n", i-(PRIMALposition-1));
Next line assumes -19 >= 0 is false:
if ( (signed int)(i-(PRIMALposition-1)) >= 0) printf ("THE NASTY BUG AGAIN: %d >= 0\n", i-(PRIMALposition-1));
And the actual fix:
...
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
	//if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
	if ( ((signed int)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) {
		if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
			count = PRIMALlengthCANDIDATE-4+1; 
			while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
				count = count-4;
			if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
		}
	}
}
...
*/
// Railgun_Swampshine_BailOut, copyleft 2014-Jan-31, Kaze.
// Caution: For better speed the case 'if (cbPattern==1)' was removed, so Pattern must be longer than 1 char.
#define NeedleThreshold2vs4swampLITE 9+10 // Should be bigger than 9. BMH2 works up to this value (inclusive), if bigger then BMH4 takes over. Should be <=255 otherwise the 0|1 BMH2 should be used.
char * Railgun_Trolldom_64 (char * pbTarget, char * pbPattern, uint64_t cbTarget, uint32_t cbPattern) 
{
	char * pbTargetMax = pbTarget + cbTarget;
	register uint32_t ulHashPattern;
	//signed long count;
	signed long long count; // 2020-Jan-11

	unsigned char bm_Horspool_Order2[256*256]; // Bitwise soon...
	unsigned char bm_Horspool_Order2bitwise[(256*256)>>3]; // Bitwise soon...
	//uint32_t i, Gulliver;
	uint64_t i, Gulliver;

	//uint32_t PRIMALposition, PRIMALpositionCANDIDATE;
	//uint32_t PRIMALlength, PRIMALlengthCANDIDATE;
	//uint32_t j, FoundAtPosition;

	uint64_t PRIMALposition, PRIMALpositionCANDIDATE;
	uint64_t PRIMALlength, PRIMALlengthCANDIDATE;
	uint64_t j, FoundAtPosition;

// Quadruplet [
    //char * pbTargetMax = pbTarget + cbTarget;
    //register unsigned long  ulHashPattern;
    unsigned long ulHashTarget;
    //unsigned long count;
    unsigned long countSTATIC;
    unsigned char SINGLET;
    unsigned long Quadruplet2nd;
    unsigned long Quadruplet3rd;
    unsigned long Quadruplet4th;
    unsigned long  AdvanceHopperGrass;
// Quadruplet ]

// 2020-Jan-11 [
//	uint64_t A=3123123123, B=5123123123;
//if ((signed int)A > 0) printf("(signed int)3billion OK\n"); else printf("(signed int)3billion Bug\n");
//if ((signed int)B > 0) printf("(signed int)5billion OK\n"); else printf("(signed int)5billion Bug\n");
//if ((signed long long)A > 0) printf("(signed long long)3billion OK\n"); else printf("(signed long long)3billion Bug\n");
//if ((signed long long)B > 0) printf("(signed long long)5billion OK\n"); else printf("(signed long long)5billion Bug\n");

//(signed int)3billion Bug
//(signed int)5billion OK
//(signed long long)3billion OK
//(signed long long)5billion OK
// 2020-Jan-11 ]

//GLOBAL_Railgun_INVOCATIONS++; // 2020-Jan-29
//GLOBAL_Railgun_INVOCATIONS_ARRAY[cbPattern]++; // 2020-Jan-29

	if (cbPattern > cbTarget) return(NULL);

#ifdef LITE
return(NULL); // 2020-Feb-14
#endif

	if ( cbPattern<4 ) { 
		// SSE2 i.e. 128bit Assembly rules here, Mischa knows best:
		// ...
        	pbTarget = pbTarget+cbPattern;
		ulHashPattern = ( (*(char *)(pbPattern))<<8 ) + *(pbPattern+(cbPattern-1));
		if ( cbPattern==3 ) {
			for ( ;; ) {
				if ( ulHashPattern == ( (*(char *)(pbTarget-3))<<8 ) + *(pbTarget-1) ) {
					if ( *(char *)(pbPattern+1) == *(char *)(pbTarget-2) ) return((pbTarget-3));
				}
				if ( (char)(ulHashPattern>>8) != *(pbTarget-2) ) { 
					pbTarget++;
					if ( (char)(ulHashPattern>>8) != *(pbTarget-2) ) pbTarget++;
				}
				pbTarget++;
				if (pbTarget > pbTargetMax) return(NULL);
			}
		} else {
		}
		for ( ;; ) {
			if ( ulHashPattern == ( (*(char *)(pbTarget-2))<<8 ) + *(pbTarget-1) ) return((pbTarget-2));
			if ( (char)(ulHashPattern>>8) != *(pbTarget-1) ) pbTarget++;
			pbTarget++;
			if (pbTarget > pbTargetMax) return(NULL);
		}
	} else { //if ( cbPattern<4 )
		if ( cbPattern<=NeedleThreshold2vs4swampLITE ) { 

// This is the awesome 'Railgun_Quadruplet', it did outperform EVERYWHERE the fastest strstr (back in old GLIBCes ~2003, by the Dutch hacker Stephen R. van den Berg), suitable for short haystacks ~100bytes.
// Caution: For better speed the case 'if (cbPattern==1)' was removed, so Pattern must be longer than 1 char.
// char * Railgun_Quadruplet (char * pbTarget, char * pbPattern, unsigned long cbTarget, unsigned long cbPattern)
// ...
//    if (cbPattern > cbTarget) return(NULL);
//} else { //if ( cbPattern<4)
if (cbTarget<777) // This value is arbitrary(don't know how exactly), it ensures(at least must) better performance than 'Boyer_Moore_Horspool'.
{
        pbTarget = pbTarget+cbPattern;
        ulHashPattern = *(unsigned long *)(pbPattern);
//        countSTATIC = cbPattern-1;

    //SINGLET = *(char *)(pbPattern);
    SINGLET = ulHashPattern & 0xFF;
    Quadruplet2nd = SINGLET<<8;
    Quadruplet3rd = SINGLET<<16;
    Quadruplet4th = SINGLET<<24;

    for ( ;; )
    {
	AdvanceHopperGrass = 0;
	ulHashTarget = *(unsigned long *)(pbTarget-cbPattern);

        if ( ulHashPattern == ulHashTarget ) { // Three unnecessary comparisons here, but 'AdvanceHopperGrass' must be calculated - it has a higher priority.
//         count = countSTATIC;
//         while ( count && *(char *)(pbPattern+1+(countSTATIC-count)) == *(char *)(pbTarget-cbPattern+1+(countSTATIC-count)) ) {
//	       if ( countSTATIC==AdvanceHopperGrass+count && SINGLET != *(char *)(pbTarget-cbPattern+1+(countSTATIC-count)) ) AdvanceHopperGrass++;
//               count--;
//         }
         count = cbPattern-1;
         while ( count && *(char *)(pbPattern+(cbPattern-count)) == *(char *)(pbTarget-count) ) {
	       if ( cbPattern-1==AdvanceHopperGrass+count && SINGLET != *(char *)(pbTarget-count) ) AdvanceHopperGrass++;
               count--;
         }
         if ( count == 0) return((pbTarget-cbPattern));
        } else { // The goal here: to avoid memory accesses by stressing the registers.
    if ( Quadruplet2nd != (ulHashTarget & 0x0000FF00) ) {
         AdvanceHopperGrass++;
         if ( Quadruplet3rd != (ulHashTarget & 0x00FF0000) ) {
              AdvanceHopperGrass++;
              if ( Quadruplet4th != (ulHashTarget & 0xFF000000) ) AdvanceHopperGrass++;
         }
    }
	}

	AdvanceHopperGrass++;

	pbTarget = pbTarget + AdvanceHopperGrass;
        if (pbTarget > pbTargetMax)
            return(NULL);
    }
} else if (cbTarget<77777) { // The warmup/overhead is lowered from 64K down to 8K, however the bitwise additional instructions quickly start hurting the throughput/traversal.
// The below bitwise 0|1 BMH2 gives 1427 bytes/s for 'Don_Quixote' with Intel:
// The below bitwise 0|1 BMH2 gives 1242 bytes/s for 'Don_Quixote' with GCC:
//	} else { //if ( cbPattern<4 )
//		if ( cbPattern<=NeedleThreshold2vs4Decumanus ) { 
			// BMH order 2, needle should be >=4:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			//for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			for (i=0; i < (256*256)>>3; i++) {bm_Horspool_Order2bitwise[i]=0;}
			//for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
			for (i=0; i < cbPattern-2+1; i++) bm_Horspool_Order2bitwise[(*(unsigned short *)(pbPattern+i))>>3]= bm_Horspool_Order2bitwise[(*(unsigned short *)(pbPattern+i))>>3] | (1<<((*(unsigned short *)(pbPattern+i))&0x7));
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1; // 'Gulliver' is the skip
				//if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
				if ( ( bm_Horspool_Order2bitwise[(*(unsigned short *)&pbTarget[i+cbPattern-1-1])>>3] & (1<<((*(unsigned short *)&pbTarget[i+cbPattern-1-1])&0x7)) ) != 0 ) {
					//if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
					if ( ( bm_Horspool_Order2bitwise[(*(unsigned short *)&pbTarget[i+cbPattern-1-1-2])>>3] & (1<<((*(unsigned short *)&pbTarget[i+cbPattern-1-1-2])&0x7)) ) == 0 ) Gulliver = cbPattern-(2-1)-2; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
							count = cbPattern-4+1; 
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;
							if ( count <= 0 ) return(pbTarget+i);
						}
					}
				} else Gulliver = cbPattern-(2-1);
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);
//		} else { // if ( cbPattern<=NeedleThreshold2vs4Decumanus )
} else { //if (cbTarget<777)
			// BMH order 2, needle should be >=4:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1; // 'Gulliver' is the skip
				if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
					if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
							count = cbPattern-4+1; 
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;
							if ( count <= 0 ) return(pbTarget+i);
						}
					}
				} else Gulliver = cbPattern-(2-1);
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);

// Slower than Swampshine's simple 0|1 segment:
/*
PRIMALlength=0;
for (i=0+(1); i < cbPattern-2+1+(1)-(1); i++) { // -(1) because the last BB order 2 has no counterpart(s)
    FoundAtPosition = cbPattern;
    PRIMALpositionCANDIDATE=i;
    while ( PRIMALpositionCANDIDATE <= (FoundAtPosition-1) ) {
        j = PRIMALpositionCANDIDATE + 1;
        while ( j <= (FoundAtPosition-1) ) {
            if ( *(unsigned short *)(pbPattern+PRIMALpositionCANDIDATE-(1)) == *(unsigned short *)(pbPattern+j-(1)) ) FoundAtPosition = j;
            j++;
        }
        PRIMALpositionCANDIDATE++;
    }
    PRIMALlengthCANDIDATE = (FoundAtPosition-1)-i+(2);
    if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=i; PRIMALlength = PRIMALlengthCANDIDATE;}
}
PRIMALlengthCANDIDATE = cbPattern;
cbPattern = PRIMALlength;
pbPattern = pbPattern + (PRIMALposition-1);
if (cbPattern<4) {
	cbPattern = PRIMALlengthCANDIDATE;
	pbPattern = pbPattern - (PRIMALposition-1);
}
if (cbPattern == PRIMALlengthCANDIDATE) {
			// BMH order 2, needle should be >=4:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1; // 'Gulliver' is the skip
				if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
					if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
							count = cbPattern-4+1; 
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;
							if ( count <= 0 ) return(pbTarget+i);
						}
					}
				} else Gulliver = cbPattern-(2-1);
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);
} else { //if (cbPattern == PRIMALlengthCANDIDATE) {
// BMH Order 2 [
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]= cbPattern-1;} // cbPattern-(Order-1) for Horspool; 'memset' if not optimized
			// The above 'for' gives  1424 bytes/s for 'Don_Quixote' with Intel:
			// The above 'for' gives  1431 bytes/s for 'Don_Quixote' with GCC:
			// The below 'memset' gives  1389 bytes/s for 'Don_Quixote' with Intel:
			// The below 'memset' gives  1432 bytes/s for 'Don_Quixote' with GCC:
			//memset(&bm_Horspool_Order2[0], cbPattern-1, 256*256); // Why why? It is 1700:1000 slower than above 'for'!?
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=i; // Rightmost appearance/position is needed
			i=0;
			while (i <= cbTarget-cbPattern) { 
				Gulliver = bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]];
				if ( Gulliver != cbPattern-1 ) { // CASE #2: if equal means the pair (char order 2) is not found i.e. Gulliver remains intact, skip the whole pattern and fall back (Order-1) chars i.e. one char for Order 2
				if ( Gulliver == cbPattern-2 ) { // CASE #1: means the pair (char order 2) is found
					if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
						count = cbPattern-4+1; 
						while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
							count = count-4;
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
	//if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
	if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
		if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
			count = PRIMALlengthCANDIDATE-4+1; 
			while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
				count = count-4;
			if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
		}
	}
}
					}
					Gulliver = 1;
				} else
					Gulliver = cbPattern - Gulliver - 2; // CASE #3: the pair is found and not as suffix i.e. rightmost position
				}
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);
// BMH Order 2 ]
} //if (cbPattern == PRIMALlengthCANDIDATE) {
*/

/*
So the result on Core 2 Q9550s @2.83GHz:
---------------------------------------------------------------------------------------------------------------------
| testfile\Searcher                         | GNU/GLIBC memmem()    | Railgun_Swampshine    | Railgun_Trolldom      | 
|-------------------------------------------------------------------------------------------|-----------------------|
| Compiler                                  | Intel 15.0 | GCC 5.10 | Intel 15.0 | GCC 5.10 | Intel 15.0 | GCC 5.10 |
|-------------------------------------------------------------------------------------------|-----------------------|
| The_Project_Gutenberg_EBook_of_Don        |        190 |      226 |       1654 |     1729 |       1147 |     1764 |
| _Quixote_996_(ANSI).txt                   |            |          |            |          |            |          |
| 2,347,772 bytes                           |            |          |            |          |            |          |
|-------------------------------------------------------------------------------------------|-----------------------|
| The_Project_Gutenberg_EBook_of_Dokoe      |        582 |      760 |       3094 |     2898 |       2410 |     3036 |
| _by_Hakucho_Masamune_(Japanese_UTF-8).txt |            |          |            |          |            |          |
| 899,425 bytes                             |            |          |            |          |            |          |
|-------------------------------------------------------------------------------------------|-----------------------|
| Dragonfly_genome_shotgun_sequence         |        104 |      109 |        445 |      458 |        484 |      553 |
| _(ACGT_alphabet).fasta                    |            |          |            |          |            |          |
| 4,487,433 bytes                           |            |          |            |          |            |          |
|-------------------------------------------------------------------------------------------|-----------------------|
| LAOTZU_Wu_Wei_(BINARY).pdf                |         99 |      144 |        629 |      580 |        185 |      570 |
| 954,035 bytes                             |            |          |            |          |            |          |
|-------------------------------------------------------------------------------------------|-----------------------|
Below segment (when compiled with Intel) is very slow, see Railgun_Trolldom two sub-columns above, compared to GCC:
*/
/*
// BMH Order 2 [
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]= (cbPattern-1);} // cbPattern-(Order-1) for Horspool; 'memset' if not optimized
			// The above 'for' is translated by Intel as:
//.B5.21::                        
//  0013f 83 c0 40         add eax, 64                            
//  00142 66 0f 7f 44 14 
//        60               movdqa XMMWORD PTR [96+rsp+rdx], xmm0  
//  00148 3d 00 00 01 00   cmp eax, 65536                         
//  0014d 66 0f 7f 44 14 
//        70               movdqa XMMWORD PTR [112+rsp+rdx], xmm0 
//  00153 66 0f 7f 84 14 
//        80 00 00 00      movdqa XMMWORD PTR [128+rsp+rdx], xmm0 
//  0015c 66 0f 7f 84 14 
//        90 00 00 00      movdqa XMMWORD PTR [144+rsp+rdx], xmm0 
//  00165 89 c2            mov edx, eax                           
//  00167 72 d6            jb .B5.21 
			//memset(&bm_Horspool_Order2[0], cbPattern-1, 256*256); // Why why? It is 1700:1000 slower than above 'for'!?
			// The above 'memset' is translated by Intel as:
//  00127 41 b8 00 00 01 
//        00               mov r8d, 65536                         
//  0012d 44 8b 26         mov r12d, DWORD PTR [rsi]              
//  00130 e8 fc ff ff ff   call _intel_fast_memset                
			// ! The problem is that 256*256, 64KB, is already too much, going bitwise i.e. 8KB is not that better, when 'cbPattern-1' is bigger than 255 - an unsigned char - then 
			// we must switch to 0|1 table i.e. present or not. Since we are in 'if ( cbPattern<=NeedleThreshold2vs4swampLITE ) {' branch and NeedleThreshold2vs4swampLITE, by default, is 19 - it is okay to use 'memset'. !
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=i; // Rightmost appearance/position is needed
			i=0;
			while (i <= cbTarget-cbPattern) { 
				Gulliver = bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]];
				if ( Gulliver != cbPattern-1 ) { // CASE #2: if equal means the pair (char order 2) is not found i.e. Gulliver remains intact, skip the whole pattern and fall back (Order-1) chars i.e. one char for Order 2
				if ( Gulliver == cbPattern-2 ) { // CASE #1: means the pair (char order 2) is found
					if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
						count = cbPattern-4+1; 
						while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
							count = count-4;
						if ( count <= 0 ) return(pbTarget+i);	
					}
					Gulliver = 1;
				} else
					Gulliver = cbPattern - Gulliver - 2; // CASE #3: the pair is found and not as suffix i.e. rightmost position
				}
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);
// BMH Order 2 ]
*/
// Above fragment in Assembly:
/*
; mark_description "Intel(R) C++ Intel(R) 64 Compiler XE for applications running on Intel(R) 64, Version 15.0.0.108 Build 20140";
; mark_description "-O3 -QxSSE2 -D_N_XMM -D_N_prefetch_4096 -D_N_Branchfull -D_N_HIGH_PRIORITY -FA";
       ALIGN     16
.B6.1::                         ; Preds .B6.0
        push      rbx                                           ;3435.1
        push      r13                                           ;3435.1
        push      r15                                           ;3435.1
        push      rbp                                           ;3435.1
        mov       eax, 65592                                    ;3435.1
        call      __chkstk                                      ;3435.1
        sub       rsp, 65592                                    ;3435.1
        cmp       r9d, r8d                                      ;3460.18
        ja        .B6.25        ; Prob 28%                      ;3460.18
                                ; LOE rdx rcx rbx rsi rdi r12 r14 r8d r9d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.3::                         ; Preds .B6.1
        mov       r13d, DWORD PTR [rdx]                         ;3491.33
        lea       ebp, DWORD PTR [-1+r9]                        ;3492.67
        movzx     eax, bpl                                      ;3492.67
        xor       r10d, r10d                                    ;3492.4
        movd      xmm0, eax                                     ;3492.67
        xor       eax, eax                                      ;3492.4
        punpcklbw xmm0, xmm0                                    ;3492.67
        punpcklwd xmm0, xmm0                                    ;3492.67
        punpckldq xmm0, xmm0                                    ;3492.67
        punpcklqdq xmm0, xmm0                                   ;3492.67
                                ; LOE rdx rcx rbx rsi rdi r10 r12 r14 eax ebp r8d r9d r13d xmm0 xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.4::                         ; Preds .B6.4 .B6.3
        add       eax, 64                                       ;3492.4
        movdqa    XMMWORD PTR [48+rsp+r10], xmm0                ;3492.33
        cmp       eax, 65536                                    ;3492.4
        movdqa    XMMWORD PTR [64+rsp+r10], xmm0                ;3492.33
        movdqa    XMMWORD PTR [80+rsp+r10], xmm0                ;3492.33
        movdqa    XMMWORD PTR [96+rsp+r10], xmm0                ;3492.33
        mov       r10d, eax                                     ;3492.4
        jb        .B6.4         ; Prob 99%                      ;3492.4
                                ; LOE rdx rcx rbx rsi rdi r10 r12 r14 eax ebp r8d r9d r13d xmm0 xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.5::                         ; Preds .B6.4
        test      ebp, ebp                                      ;3515.28
        je        .B6.12        ; Prob 50%                      ;3515.28
                                ; LOE rdx rcx rbx rsi rdi r12 r14 ebp r8d r9d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.6::                         ; Preds .B6.5
        mov       eax, 1                                        ;3515.4
        lea       r11d, DWORD PTR [-1+r9]                       ;3515.4
        mov       r15d, r11d                                    ;3515.4
        xor       r10d, r10d                                    ;3515.4
        shr       r15d, 1                                       ;3515.4
        test      r15d, r15d                                    ;3515.4
        jbe       .B6.10        ; Prob 15%                      ;3515.4
                                ; LOE rdx rcx rbx rsi rdi r12 r14 eax ebp r8d r9d r10d r11d r13d r15d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.8::                         ; Preds .B6.6 .B6.8
        lea       eax, DWORD PTR [r10+r10]                      ;3515.36
        movzx     ebx, WORD PTR [rax+rdx]                       ;3515.75
        mov       BYTE PTR [48+rsp+rbx], al                     ;3515.36
        lea       eax, DWORD PTR [1+r10+r10]                    ;3515.36
        inc       r10d                                          ;3515.4
        cmp       r10d, r15d                                    ;3515.4
        movzx     ebx, WORD PTR [rax+rdx]                       ;3515.75
        mov       BYTE PTR [48+rsp+rbx], al                     ;3515.36
        jb        .B6.8         ; Prob 64%                      ;3515.4
                                ; LOE rdx rcx rsi rdi r12 r14 ebp r8d r9d r10d r11d r13d r15d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.9::                         ; Preds .B6.8
        lea       eax, DWORD PTR [1+r10+r10]                    ;3515.4
                                ; LOE rdx rcx rbx rsi rdi r12 r14 eax ebp r8d r9d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.10::                        ; Preds .B6.9 .B6.6
        dec       eax                                           ;3515.36
        cmp       eax, r11d                                     ;3515.4
        jae       .B6.12        ; Prob 15%                      ;3515.4
                                ; LOE rax rdx rcx rbx rsi rdi r12 r14 ebp r8d r9d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.11::                        ; Preds .B6.10
        movzx     r10d, WORD PTR [rax+rdx]                      ;3515.75
        mov       BYTE PTR [48+rsp+r10], al                     ;3515.36
                                ; LOE rdx rcx rbx rsi rdi r12 r14 ebp r8d r9d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.12::                        ; Preds .B6.5 .B6.10 .B6.11
        xor       r10d, r10d                                    ;3516.4
        lea       r15d, DWORD PTR [-3+r9]                       ;3522.27
        movsxd    r15, r15d                                     ;3522.7
        sub       r8d, r9d                                      ;3517.16
        lea       r11d, DWORD PTR [-2+r9]                       ;3520.32
                                ; LOE rdx rcx rsi rdi r12 r14 r15 ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.13::                        ; Preds .B6.12 .B6.24
        lea       eax, DWORD PTR [-2+r9+r10]                    ;3518.78
        movzx     ebx, WORD PTR [rax+rcx]                       ;3518.55
        movzx     eax, BYTE PTR [48+rsp+rbx]                    ;3518.16
        cmp       eax, ebp                                      ;3519.32
        je        .B6.24        ; Prob 50%                      ;3519.32
                                ; LOE rdx rcx rsi rdi r12 r14 r15 eax ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.14::                        ; Preds .B6.13
        cmp       eax, r11d                                     ;3520.32
        jne       .B6.23        ; Prob 62%                      ;3520.32
                                ; LOE rdx rcx rsi rdi r12 r14 r15 eax ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.15::                        ; Preds .B6.14
        mov       eax, r10d                                     ;3521.25
        add       rax, rcx                                      ;3521.25
        cmp       r13d, DWORD PTR [rax]                         ;3521.40
        je        .B6.17        ; Prob 50%                      ;3521.40
                                ; LOE rax rdx rcx rsi rdi r12 r14 r15 ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.16::                        ; Preds .B6.26 .B6.15
        mov       eax, 1                                        ;3527.6
        jmp       .B6.24        ; Prob 100%                     ;3527.6
                                ; LOE rdx rcx rsi rdi r12 r14 r15 eax ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.17::                        ; Preds .B6.15
        mov       rbx, r15                                      ;3522.7
        test      r15, r15                                      ;3523.23
        jle       .B6.22        ; Prob 2%                       ;3523.23
                                ; LOE rax rdx rcx rbx rsi rdi r12 r14 r15 ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.18::                        ; Preds .B6.17
        mov       QWORD PTR [32+rsp], rsi                       ;
                                ; LOE rax rdx rcx rbx rdi r12 r14 r15 ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.19::                        ; Preds .B6.20 .B6.18
        mov       esi, DWORD PTR [-1+rbx+rdx]                   ;3523.58
        cmp       esi, DWORD PTR [-1+rbx+rax]                   ;3523.79
        jne       .B6.26        ; Prob 20%                      ;3523.79
                                ; LOE rax rdx rcx rbx rdi r12 r14 r15 ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.20::                        ; Preds .B6.19
        add       rbx, -4                                       ;3524.22
        test      rbx, rbx                                      ;3523.23
        jg        .B6.19        ; Prob 82%                      ;3523.23
                                ; LOE rax rdx rcx rbx rdi r12 r14 r15 ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.21::                        ; Preds .B6.20
        mov       rsi, QWORD PTR [32+rsp]                       ;
                                ; LOE rax rbx rsi rdi r12 r14 xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.22::                        ; Preds .B6.17 .B6.21
        add       rsp, 65592                                    ;3525.32
        pop       rbp                                           ;3525.32
        pop       r15                                           ;3525.32
        pop       r13                                           ;3525.32
        pop       rbx                                           ;3525.32
        ret                                                     ;3525.32
                                ; LOE
.B6.23::                        ; Preds .B6.14
        neg       eax                                           ;3529.17
        add       eax, r9d                                      ;3529.17
        add       eax, -2                                       ;3529.40
                                ; LOE rdx rcx rsi rdi r12 r14 r15 eax ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.24::                        ; Preds .B6.16 .B6.23 .B6.13
        add       r10d, eax                                     ;3531.13
        cmp       r10d, r8d                                     ;3517.25
        jbe       .B6.13        ; Prob 82%                      ;3517.25
                                ; LOE rdx rcx rsi rdi r12 r14 r15 ebp r8d r9d r10d r11d r13d xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15
.B6.25::                        ; Preds .B6.1 .B6.24
        xor       eax, eax                                      ;3534.10
        add       rsp, 65592                                    ;3534.10
        pop       rbp                                           ;3534.10
        pop       r15                                           ;3534.10
        pop       r13                                           ;3534.10
        pop       rbx                                           ;3534.10
        ret                                                     ;3534.10
                                ; LOE
.B6.26::                        ; Preds .B6.19                  ; Infreq
        mov       rsi, QWORD PTR [32+rsp]                       ;
        jmp       .B6.16        ; Prob 100%                     ;
*/

// GCC 5.10; >gcc -O3 -m64 -fomit-frame-pointer
/*
Railgun_Trolldom:
	pushq	%r15
	.seh_pushreg	%r15
	movl	$65592, %eax
	pushq	%r14
	.seh_pushreg	%r14
	pushq	%r13
	.seh_pushreg	%r13
	pushq	%r12
	.seh_pushreg	%r12
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	call	___chkstk_ms
	subq	%rax, %rsp
	.seh_stackalloc	65592
	.seh_endprologue
	cmpl	%r9d, %r8d
	movq	%rcx, %rbx
	movq	%rdx, %rdi
	movl	%r8d, %r12d
	movl	%r9d, %esi
	jb	.L118
	movl	(%rdx), %ebp
	leal	-1(%r9), %edx
	movl	$65536, %r8d
	leaq	48(%rsp), %rcx
	movzbl	%dl, %edx
	call	memset
	movl	%esi, %r11d
	subl	$1, %r11d
	je	.L119
	xorl	%eax, %eax
	.p2align 4,,10
.L113:
	movzwl	(%rdi,%rax), %edx
	movb	%al, 48(%rsp,%rdx)
	addq	$1, %rax
	cmpl	%eax, %r11d
	ja	.L113
.L112:
	leal	-4(%rsi), %r9d
	movl	%r12d, %r8d
	xorl	%edx, %edx
	leal	-3(%rsi), %eax
	shrl	$2, %r9d
	subl	%esi, %r8d
	leal	-2(%rsi), %r10d
	movslq	%eax, %r14
	negq	%r9
	movl	%eax, 44(%rsp)
	leaq	-1(%r14), %r15
	salq	$2, %r9
	leaq	(%rdi,%r14), %r13
	jmp	.L117
	.p2align 4,,10
.L130:
	movl	%r10d, %eax
	subl	%ecx, %eax
	cmpl	%r10d, %ecx
	je	.L129
.L114:
	addl	%eax, %edx
	cmpl	%r8d, %edx
	ja	.L118
.L117:
	leal	(%rdx,%r10), %eax
	movzwl	(%rbx,%rax), %eax
	movzbl	48(%rsp,%rax), %ecx
	cmpl	%r11d, %ecx
	jne	.L130
	movl	%r11d, %eax
	addl	%eax, %edx
	cmpl	%r8d, %edx
	jbe	.L117
.L118:
	xorl	%eax, %eax
	jmp	.L128
	.p2align 4,,10
.L129:
	movl	%edx, %ecx
	movl	$1, %eax
	leaq	(%rbx,%rcx), %r12
	cmpl	(%r12), %ebp
	jne	.L114
	movl	44(%rsp), %esi
	testl	%esi, %esi
	jle	.L124
	movl	(%r12,%r15), %esi
	cmpl	%esi, (%rdi,%r15)
	jne	.L114
	addq	%r14, %rcx
	xorl	%eax, %eax
	addq	%rbx, %rcx
	jmp	.L116
	.p2align 4,,10
.L132:
	movl	-5(%r13,%rax), %esi
	subq	$4, %rax
	cmpl	-1(%rcx,%rax), %esi
	jne	.L131
.L116:
	cmpq	%rax, %r9
	jne	.L132
.L124:
	movq	%r12, %rax
.L128:
	addq	$65592, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	ret
	.p2align 4,,10
.L131:
	movl	$1, %eax
	jmp	.L114
.L119:
	xorl	%r11d, %r11d
	jmp	.L112
*/

} //if (cbTarget<777)

		} else { // if ( cbPattern<=NeedleThreshold2vs4swampLITE )

// Swampwalker_BAILOUT heuristic order 4 (Needle should be bigger than 4) [
// Needle: 1234567890qwertyuiopasdfghjklzxcv            PRIMALposition=01 PRIMALlength=33  '1234567890qwertyuiopasdfghjklzxcv'
// Needle: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv            PRIMALposition=29 PRIMALlength=04  'vvvv'
// Needle: vvvvvvvvvvBOOMSHAKALAKAvvvvvvvvvv            PRIMALposition=08 PRIMALlength=20  'vvvBOOMSHAKALAKAvvvv'
// Needle: Trollland                                    PRIMALposition=01 PRIMALlength=09  'Trollland'
// Needle: Swampwalker                                  PRIMALposition=01 PRIMALlength=11  'Swampwalker'
// Needle: licenselessness                              PRIMALposition=01 PRIMALlength=15  'licenselessness'
// Needle: alfalfa                                      PRIMALposition=02 PRIMALlength=06  'lfalfa'
// Needle: Sandokan                                     PRIMALposition=01 PRIMALlength=08  'Sandokan'
// Needle: shazamish                                    PRIMALposition=01 PRIMALlength=09  'shazamish'
// Needle: Simplicius Simplicissimus                    PRIMALposition=06 PRIMALlength=20  'icius Simplicissimus'
// Needle: domilliaquadringenquattuorquinquagintillion  PRIMALposition=01 PRIMALlength=32  'domilliaquadringenquattuorquinqu'
// Needle: boom-boom                                    PRIMALposition=02 PRIMALlength=08  'oom-boom'
// Needle: vvvvv                                        PRIMALposition=01 PRIMALlength=04  'vvvv'
// Needle: 12345                                        PRIMALposition=01 PRIMALlength=05  '12345'
// Needle: likey-likey                                  PRIMALposition=03 PRIMALlength=09  'key-likey'
// Needle: BOOOOOM                                      PRIMALposition=03 PRIMALlength=05  'OOOOM'
// Needle: aaaaaBOOOOOM                                 PRIMALposition=02 PRIMALlength=09  'aaaaBOOOO'
// Needle: BOOOOOMaaaaa                                 PRIMALposition=03 PRIMALlength=09  'OOOOMaaaa'
PRIMALlength=0;
for (i=0+(1); i < cbPattern-((4)-1)+(1)-(1); i++) { // -(1) because the last BB (Building-Block) order 4 has no counterpart(s)
	FoundAtPosition = cbPattern - ((4)-1) + 1;
	PRIMALpositionCANDIDATE=i;
	while ( PRIMALpositionCANDIDATE <= (FoundAtPosition-1) ) {
		j = PRIMALpositionCANDIDATE + 1;
		while ( j <= (FoundAtPosition-1) ) {
			if ( *(uint32_t *)(pbPattern+PRIMALpositionCANDIDATE-(1)) == *(uint32_t *)(pbPattern+j-(1)) ) FoundAtPosition = j;
			j++;
		}
		PRIMALpositionCANDIDATE++;
	}
	PRIMALlengthCANDIDATE = (FoundAtPosition-1)-i+1 +((4)-1);
	if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=i; PRIMALlength = PRIMALlengthCANDIDATE;}
	if (cbPattern-i+1 <= PRIMALlength) break;
	if (PRIMALlength > 128) break; // Bail Out for 129[+]
}
// Swampwalker_BAILOUT heuristic order 4 (Needle should be bigger than 4) ]

// Swampwalker_BAILOUT heuristic order 2 (Needle should be bigger than 2) [
// Needle: 1234567890qwertyuiopasdfghjklzxcv            PRIMALposition=01 PRIMALlength=33  '1234567890qwertyuiopasdfghjklzxcv'
// Needle: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv            PRIMALposition=31 PRIMALlength=02  'vv'
// Needle: vvvvvvvvvvBOOMSHAKALAKAvvvvvvvvvv            PRIMALposition=09 PRIMALlength=13  'vvBOOMSHAKALA'
// Needle: Trollland                                    PRIMALposition=05 PRIMALlength=05  'lland'
// Needle: Swampwalker                                  PRIMALposition=03 PRIMALlength=09  'ampwalker'
// Needle: licenselessness                              PRIMALposition=01 PRIMALlength=13  'licenselessne'
// Needle: alfalfa                                      PRIMALposition=04 PRIMALlength=04  'alfa'
// Needle: Sandokan                                     PRIMALposition=01 PRIMALlength=07  'Sandoka'
// Needle: shazamish                                    PRIMALposition=02 PRIMALlength=08  'hazamish'
// Needle: Simplicius Simplicissimus                    PRIMALposition=08 PRIMALlength=15  'ius Simplicissi'
// Needle: domilliaquadringenquattuorquinquagintillion  PRIMALposition=01 PRIMALlength=19  'domilliaquadringenq'
// Needle: DODO                                         PRIMALposition=02 PRIMALlength=03  'ODO'
// Needle: DODOD                                        PRIMALposition=03 PRIMALlength=03  'DOD'
// Needle: aaaDODO                                      PRIMALposition=02 PRIMALlength=05  'aaDOD'
// Needle: aaaDODOD                                     PRIMALposition=02 PRIMALlength=05  'aaDOD'
// Needle: DODOaaa                                      PRIMALposition=02 PRIMALlength=05  'ODOaa'
// Needle: DODODaaa                                     PRIMALposition=03 PRIMALlength=05  'DODaa'
/*
PRIMALlength=0;
for (i=0+(1); i < cbPattern-2+1+(1)-(1); i++) { // -(1) because the last BB order 2 has no counterpart(s)
    FoundAtPosition = cbPattern;
    PRIMALpositionCANDIDATE=i;
    while ( PRIMALpositionCANDIDATE <= (FoundAtPosition-1) ) {
        j = PRIMALpositionCANDIDATE + 1;
        while ( j <= (FoundAtPosition-1) ) {
            if ( *(unsigned short *)(pbPattern+PRIMALpositionCANDIDATE-(1)) == *(unsigned short *)(pbPattern+j-(1)) ) FoundAtPosition = j;
            j++;
        }
        PRIMALpositionCANDIDATE++;
    }
    PRIMALlengthCANDIDATE = (FoundAtPosition-1)-i+(2);
    if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=i; PRIMALlength = PRIMALlengthCANDIDATE;}
}
*/
// Swampwalker_BAILOUT heuristic order 2 (Needle should be bigger than 2) ]

/*
Legend:
'[]' points to BB forming left or right boundary;
'{}' points to BB being searched for;
'()' position of duplicate and new right boundary;

                       00000000011111111112222222222333
                       12345678901234567890123456789012
Example #1 for Needle: 1234567890qwertyuiopasdfghjklzxcv  NewNeedle = '1234567890qwertyuiopasdfghjklzxcv'
Example #2 for Needle: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv  NewNeedle = 'vv'
Example #3 for Needle: vvvvvvvvvvBOOMSHAKALAKAvvvvvvvvvv  NewNeedle = 'vvBOOMSHAKALA'


     PRIMALlength=00; FoundAtPosition=33; 
Step 01_00: {}[12]34567890qwertyuiopasdfghjklzxc[v?] ! For position #01 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=01, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 !
Step 01_01: [{12}]34567890qwertyuiopasdfghjklzxc[v?] ! Searching for '12', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 ! 
Step 01_02: [1{2]3}4567890qwertyuiopasdfghjklzxc[v?] ! Searching for '23', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 ! 
...
Step 01_30: [12]34567890qwertyuiopasdfghjkl{zx}c[v?] ! Searching for 'zx', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 ! 
Step 01_31: [12]34567890qwertyuiopasdfghjklz{xc}[v?] ! Searching for 'xc', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
Step 02_00: {}1[23]4567890qwertyuiopasdfghjklzxc[v?] ! For position #02 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=02, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 !
Step 02_01: 1[{23}]4567890qwertyuiopasdfghjklzxc[v?] ! Searching for '23', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 ! 
Step 02_02: 1[2{3]4}567890qwertyuiopasdfghjklzxc[v?] ! Searching for '34', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 ! 
...
Step 02_29: 1[23]4567890qwertyuiopasdfghjkl{zx}c[v?] ! Searching for 'zx', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 ! 
Step 02_30: 1[23]4567890qwertyuiopasdfghjklz{xc}[v?] ! Searching for 'xc', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
...
Step 31_00: {}1234567890qwertyuiopasdfghjklz[xc][v?] ! For position #31 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=31, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-31+(2)=03 !
Step 31_01: 1234567890qwertyuiopasdfghjklz[{xc}][v?] ! Searching for 'xc', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-31+(2)=03 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
     Result:
     PRIMALposition=01 PRIMALlength=33, NewNeedle = '1234567890qwertyuiopasdfghjklzxcv'


     PRIMALlength=00; FoundAtPosition=33; 
Step 01_00: {}[vv]vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv[v?] ! For position #01 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=01, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 !
Step 01_01: [{v(v}]v)vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv  ! Searching for 'vv', FoundAtPosition = 02, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(02-1)-01+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
Step 02_00: {}v[vv]vvvvvvvvvvvvvvvvvvvvvvvvvvvvv[v?] ! For position #02 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=02, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 !
Step 02_01: v[{v(v}]v)vvvvvvvvvvvvvvvvvvvvvvvvvvvvv  ! Searching for 'vv', FoundAtPosition = 03, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(03-1)-02+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
...
Step 31_00: {}vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv[vv][v?] ! For position #31 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=31, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-31+(2)=03 !
Step 31_01: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv[{v(v}]v)  ! Searching for 'vv', FoundAtPosition = 32, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(32-1)-31+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
     Result:
     PRIMALposition=31 PRIMALlength=02, NewNeedle = 'vv'


     PRIMALlength=00; FoundAtPosition=33; 
Step 01_00: {}[vv]vvvvvvvvBOOMSHAKALAKAvvvvvvvvv[v?] ! For position #01 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=01, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 !
Step 01_01: [{v(v}]v)vvvvvvvBOOMSHAKALAKAvvvvvvvvvv  ! Searching for 'vv', FoundAtPosition = 02, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(02-1)-01+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
Step 02_00: {}v[vv]vvvvvvvBOOMSHAKALAKAvvvvvvvvv[v?] ! For position #02 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=02, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 !
Step 02_01: v[{v(v}]v)vvvvvvBOOMSHAKALAKAvvvvvvvvvv  ! Searching for 'vv', FoundAtPosition = 03, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(03-1)-02+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
...
Step 09_00: {}vvvvvvvv[vv]BOOMSHAKALAKAvvvvvvvvv[v?] ! For position #09 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=09, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-09+(2)=25 !
Step 09_01: vvvvvvvv[{vv}]BOOMSHAKALAKA(vv)vvvvvvvv  ! Searching for 'vv', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_02: vvvvvvvv[v{v]B}OOMSHAKALAKA[vv]vvvvvvvv  ! Searching for 'vB', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_03: vvvvvvvv[vv]{BO}OMSHAKALAKA[vv]vvvvvvvv  ! Searching for 'BO', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_04: vvvvvvvv[vv]B{OO}MSHAKALAKA[vv]vvvvvvvv  ! Searching for 'OO', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_05: vvvvvvvv[vv]BO{OM}SHAKALAKA[vv]vvvvvvvv  ! Searching for 'OM', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_06: vvvvvvvv[vv]BOO{MS}HAKALAKA[vv]vvvvvvvv  ! Searching for 'MS', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_07: vvvvvvvv[vv]BOOM{SH}AKALAKA[vv]vvvvvvvv  ! Searching for 'SH', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_08: vvvvvvvv[vv]BOOMS{HA}KALAKA[vv]vvvvvvvv  ! Searching for 'HA', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_09: vvvvvvvv[vv]BOOMSH{AK}AL(AK)Avvvvvvvvvv  ! Searching for 'AK', FoundAtPosition = 21, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(21-1)-09+(2)=13 ! 
Step 09_10: vvvvvvvv[vv]BOOMSHA{KA}L[AK]Avvvvvvvvvv  ! Searching for 'KA', FoundAtPosition = 21, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(21-1)-09+(2)=13 ! 
Step 09_11: vvvvvvvv[vv]BOOMSHAK{AL}[AK]Avvvvvvvvvv  ! Searching for 'AL', FoundAtPosition = 21, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(21-1)-09+(2)=13 ! 
Step 09_12: vvvvvvvv[vv]BOOMSHAKA{L[A}K]Avvvvvvvvvv  ! Searching for 'LA', FoundAtPosition = 21, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(21-1)-09+(2)=13 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
...
Step 31_00: {}vvvvvvvv[vv]BOOMSHAKALAKAvvvvvvvvv[v?] ! For position #31 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=31, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-31+(2)=03 !
Step 31_01: vvvvvvvvvvBOOMSHAKALAKAvvvvvvv[{v(v}]v)  ! Searching for 'vv', FoundAtPosition = 32, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(32-1)-31+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
     Result:
     PRIMALposition=09 PRIMALlength=13, NewNeedle = 'vvBOOMSHAKALA'
*/

// Here we have 4 or bigger NewNeedle, apply order 2 for pbPattern[i+(PRIMALposition-1)] with length 'PRIMALlength' and compare the pbPattern[i] with length 'cbPattern':
PRIMALlengthCANDIDATE = cbPattern;
cbPattern = PRIMALlength;
pbPattern = pbPattern + (PRIMALposition-1);

// Revision 2 commented section [
/*
if (cbPattern-1 <= 255) {
// BMH Order 2 [
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]= cbPattern-1;} // cbPattern-(Order-1) for Horspool; 'memset' if not optimized
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=i; // Rightmost appearance/position is needed
			i=0;
			while (i <= cbTarget-cbPattern) { 
				Gulliver = bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]];
				if ( Gulliver != cbPattern-1 ) { // CASE #2: if equal means the pair (char order 2) is not found i.e. Gulliver remains intact, skip the whole pattern and fall back (Order-1) chars i.e. one char for Order 2
				if ( Gulliver == cbPattern-2 ) { // CASE #1: means the pair (char order 2) is found
					if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
						count = cbPattern-4+1; 
						while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
							count = count-4;
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
	//if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
	if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
		if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
			count = PRIMALlengthCANDIDATE-4+1; 
			while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
				count = count-4;
			if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
		}
	}
}
					}
					Gulliver = 1;
				} else
					Gulliver = cbPattern - Gulliver - 2; // CASE #3: the pair is found and not as suffix i.e. rightmost position
				}
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);
// BMH Order 2 ]
} else {
			// BMH order 2, needle should be >=4:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1; // 'Gulliver' is the skip
				if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
					if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
							count = cbPattern-4+1; 
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
	//if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
	if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
		if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
			count = PRIMALlengthCANDIDATE-4+1; 
			while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
				count = count-4;
			if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
		}
	}
}
						}
					}
				} else Gulliver = cbPattern-(2-1);
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);
}
*/
// Revision 2 commented section ]

		if ( cbPattern<=NeedleThreshold2vs4swampLITE ) { 

			// BMH order 2, needle should be >=4:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			// Above line is translated by Intel as:
//  0044c 41 b8 00 00 01 
//        00               mov r8d, 65536                         
//  00452 44 89 5c 24 20   mov DWORD PTR [32+rsp], r11d           
//  00457 44 89 54 24 60   mov DWORD PTR [96+rsp], r10d           
//  0045c e8 fc ff ff ff   call _intel_fast_memset                
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1; // 'Gulliver' is the skip
				if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
					if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
							count = cbPattern-4+1; 
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;

	if (cbPattern != PRIMALlengthCANDIDATE) { // No need of same comparison when Needle and NewNeedle are equal!
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
	//if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
	if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
		if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
			count = PRIMALlengthCANDIDATE-4+1; 
			while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
				count = count-4;
			if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
		}
	}
}
	} else { //if (cbPattern != PRIMALlengthCANDIDATE)
							if ( count <= 0 ) return(pbTarget+i);
	}
						}
					}
				} else Gulliver = cbPattern-(2-1);
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);

		} else { // if ( cbPattern<=NeedleThreshold2vs4swampLITE )

			// BMH pseudo-order 4, needle should be >=8+2:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			// In line below we "hash" 4bytes to 2bytes i.e. 16bit table, how to compute TOTAL number of BBs, 'cbPattern - Order + 1' is the number of BBs for text 'cbPattern' bytes long, for example, for cbPattern=11 'fastest fox' and Order=4 we have BBs = 11-4+1=8:
			//"fast"
			//"aste"
			//"stes"
			//"test"
			//"est "
			//"st f"
			//"t fo"
			//" fox"
			//for (i=0; i < cbPattern-4+1; i++) bm_Horspool_Order2[( *(unsigned short *)(pbPattern+i+0) + *(unsigned short *)(pbPattern+i+2) ) & ( (1<<16)-1 )]=1;
			//for (i=0; i < cbPattern-4+1; i++) bm_Horspool_Order2[( (*(uint32_t *)(pbPattern+i+0)>>16)+(*(uint32_t *)(pbPattern+i+0)&0xFFFF) ) & ( (1<<16)-1 )]=1;
			// Above line is replaced by next one with better hashing:
			for (i=0; i < cbPattern-4+1; i++) bm_Horspool_Order2[( (*(uint32_t *)(pbPattern+i+0)>>(16-1))+(*(uint32_t *)(pbPattern+i+0)&0xFFFF) ) & ( (1<<16)-1 )]=1;
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1;
				//if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]&0xFFFF) ) & ( (1<<16)-1 )] != 0 ) { // DWORD #1
				// Above line is replaced by next one with better hashing:
				if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]&0xFFFF) ) & ( (1<<16)-1 )] != 0 ) { // DWORD #1
					//if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] == 0 ) Gulliver = cbPattern-(2-1)-2-4; else {
					// Above line is replaced in order to strengthen the skip by checking the middle DWORD,if the two DWORDs are 'ab' and 'cd' i.e. [2x][2a][2b][2c][2d] then the middle DWORD is 'bc'.
					// The respective offsets (backwards) are: -10/-8/-6/-4 for 'xa'/'ab'/'bc'/'cd'.
					//if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) ) & ( (1<<16)-1 )] ) < 3 ) Gulliver = cbPattern-(2-1)-2-4-2; else {
					// Above line is replaced by next one with better hashing:
					// When using (16-1) right shifting instead of 16 we will have two different pairs (if they are equal), the highest bit being lost do the job especialy for ASCII texts with no symbols in range 128-255.
					// Example for genomesque pair TT+TT being shifted by (16-1):
					// T            = 01010100
					// TT           = 01010100 01010100
					// TTTT         = 01010100 01010100 01010100 01010100
					// TTTT>>16     = 00000000 00000000 01010100 01010100
					// TTTT>>(16-1) = 00000000 00000000 10101000 10101000 <--- Due to the left shift by 1, the 8th bits of 1st and 2nd bytes are populated - usually they are 0 for English texts & 'ACGT' data.
					//if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) ) & ( (1<<16)-1 )] ) < 3 ) Gulliver = cbPattern-(2-1)-2-4-2; else {
					// 'Maximus' uses branched 'if', again.
					if ( \
					( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6 +1]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6 +1]&0xFFFF) ) & ( (1<<16)-1 )] ) == 0 \
					|| ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4 +1]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4 +1]&0xFFFF) ) & ( (1<<16)-1 )] ) == 0 \
					) Gulliver = cbPattern-(2-1)-2-4-2 +1; else {
					// Above line is not optimized (several a SHR are used), we have 5 non-overlapping WORDs, or 3 overlapping WORDs, within 4 overlapping DWORDs so:
// [2x][2a][2b][2c][2d]
// DWORD #4
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]>>16) =     !SHR to be avoided! <--
// [2x] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) =                        |
//     DWORD #3                                                                       |
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16) =     !SHR to be avoided!   |<--
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) = ------------------------  |
//         DWORD #2                                                                      |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]>>16) =     !SHR to be avoided!      |<--
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) = ---------------------------  |
//             DWORD #1                                                                     |
// [2d] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]>>16) =                                 |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF) = ------------------------------
//
// So in order to remove 3 SHR instructions the equal extractions are:
// DWORD #4
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) =  !SHR to be avoided! <--
// [2x] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) =                        |
//     DWORD #3                                                                       |
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) =  !SHR to be avoided!   |<--
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) = ------------------------  |
//         DWORD #2                                                                      |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF) =  !SHR to be avoided!      |<--
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) = ---------------------------  |
//             DWORD #1                                                                     |
// [2d] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]>>16) =                                 |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF) = ------------------------------
					//if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) ) & ( (1<<16)-1 )] ) < 3 ) Gulliver = cbPattern-(2-1)-2-6; else {
// Since the above Decumanus mumbo-jumbo (3 overlapping lookups vs 2 non-overlapping lookups) is not fast enough we go DuoDecumanus or 3x4:
// [2y][2x][2a][2b][2c][2d]
// DWORD #3
//         DWORD #2
//                 DWORD #1
					//if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-8]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-8]&0xFFFF) ) & ( (1<<16)-1 )] ) < 2 ) Gulliver = cbPattern-(2-1)-2-8; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
							// Order 4 [
						// Let's try something "outrageous" like comparing with[out] overlap BBs 4bytes long instead of 1 byte back-to-back:
						// Inhere we are using order 4, 'cbPattern - Order + 1' is the number of BBs for text 'cbPattern' bytes long, for example, for cbPattern=11 'fastest fox' and Order=4 we have BBs = 11-4+1=8:
						//0:"fast" if the comparison failed here, 'count' is 1; 'Gulliver' is cbPattern-(4-1)-7
						//1:"aste" if the comparison failed here, 'count' is 2; 'Gulliver' is cbPattern-(4-1)-6
						//2:"stes" if the comparison failed here, 'count' is 3; 'Gulliver' is cbPattern-(4-1)-5
						//3:"test" if the comparison failed here, 'count' is 4; 'Gulliver' is cbPattern-(4-1)-4
						//4:"est " if the comparison failed here, 'count' is 5; 'Gulliver' is cbPattern-(4-1)-3
						//5:"st f" if the comparison failed here, 'count' is 6; 'Gulliver' is cbPattern-(4-1)-2
						//6:"t fo" if the comparison failed here, 'count' is 7; 'Gulliver' is cbPattern-(4-1)-1
						//7:" fox" if the comparison failed here, 'count' is 8; 'Gulliver' is cbPattern-(4-1)
							count = cbPattern-4+1; 
							// Below comparison is UNIdirectional:
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;

	if (cbPattern != PRIMALlengthCANDIDATE) { // No need of same comparison when Needle and NewNeedle are equal!
// count = cbPattern-4+1 = 23-4+1 = 20
// boomshakalakaZZZZZZ[ZZZZ] 20
// boomshakalakaZZ[ZZZZ]ZZZZ 20-4
// boomshakala[kaZZ]ZZZZZZZZ 20-8 = 12
// boomsha[kala]kaZZZZZZZZZZ 20-12 = 8
// boo[msha]kalakaZZZZZZZZZZ 20-16 = 4

// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
	//if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
	if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
		if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
			count = PRIMALlengthCANDIDATE-4+1; 
			while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
				count = count-4;
			if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
		}
	}
}
	} else { //if (cbPattern != PRIMALlengthCANDIDATE)
							if ( count <= 0 ) return(pbTarget+i);
	}

							// In order to avoid only-left or only-right WCS the memcmp should be done as left-to-right and right-to-left AT THE SAME TIME.
							// Below comparison is BIdirectional. It pays off when needle is 8+++ long:
//							for (count = cbPattern-4+1; count > 0; count = count-4) {
//								if ( *(uint32_t *)(pbPattern+count-1) != *(uint32_t *)(&pbTarget[i]+(count-1)) ) {break;};
//								if ( *(uint32_t *)(pbPattern+(cbPattern-4+1)-count) != *(uint32_t *)(&pbTarget[i]+(cbPattern-4+1)-count) ) {count = (cbPattern-4+1)-count +(1); break;} // +(1) because two lookups are implemented as one, also no danger of 'count' being 0 because of the fast check outwith the 'while': if ( *(uint32_t *)&pbTarget[i] == ulHashPattern)
//							}
//							if ( count <= 0 ) return(pbTarget+i);
								// Checking the order 2 pairs in mismatched DWORD, all the 3:
								//if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1]] == 0 ) Gulliver = count; // 1 or bigger, as it should
								//if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1]] == 0 ) Gulliver = count+1; // 1 or bigger, as it should
								//if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1+1]] == 0 ) Gulliver = count+1+1; // 1 or bigger, as it should
							//	if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1]] + bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1]] + bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1+1]] < 3 ) Gulliver = count; // 1 or bigger, as it should, THE MIN(count,count+1,count+1+1)
								// Above compound 'if' guarantees not that Gulliver > 1, an example:
								// Needle:    fastest tax
								// Window: ...fastast tax...
								// After matching ' tax' vs ' tax' and 'fast' vs 'fast' the mismathced DWORD is 'test' vs 'tast':
								// 'tast' when factorized down to order 2 yields: 'ta','as','st' - all the three when summed give 1+1+1=3 i.e. Gulliver remains 1.
								// Roughly speaking, this attempt maybe has its place in worst-case scenarios but not in English text and even not in ACGT data, that's why I commented it in original 'Shockeroo'.
								//if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+count-1]>>16)+(*(uint32_t *)&pbTarget[i+count-1]&0xFFFF) ) & ( (1<<16)-1 )] == 0 ) Gulliver = count; // 1 or bigger, as it should
								// Above line is replaced by next one with better hashing:
//								if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+count-1]>>(16-1))+(*(uint32_t *)&pbTarget[i+count-1]&0xFFFF) ) & ( (1<<16)-1 )] == 0 ) Gulliver = count; // 1 or bigger, as it should
							// Order 4 ]
						}
					}
				} else Gulliver = cbPattern-(2-1)-2; // -2 because we check the 4 rightmost bytes not 2.
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);

		} // if ( cbPattern<=NeedleThreshold2vs4swampLITE )
		} // if ( cbPattern<=NeedleThreshold2vs4swampLITE )
	} //if ( cbPattern<4 )
}

// For short needles, and mainly haystacks, 'Doublet' is quite effective. Consider it or 'Quadruplet'.
// Fixed version from 2012-Feb-27.
// Caution: For better speed the case 'if (cbPattern==1)' was removed, so Pattern must be longer than 1 char.
char * Railgun_Doublet (char * pbTarget, char * pbPattern, uint32_t cbTarget, uint32_t cbPattern)
{
	char * pbTargetMax = pbTarget + cbTarget;
	register uint32_t ulHashPattern;
	uint32_t ulHashTarget, count, countSTATIC;

	if (cbPattern > cbTarget) return(NULL);

	countSTATIC = cbPattern-2;

	pbTarget = pbTarget+cbPattern;
	ulHashPattern = (*(uint16_t *)(pbPattern));

	for ( ;; ) {
		if ( ulHashPattern == (*(uint16_t *)(pbTarget-cbPattern)) ) {
			count = countSTATIC;
			while ( count && *(char *)(pbPattern+2+(countSTATIC-count)) == *(char *)(pbTarget-cbPattern+2+(countSTATIC-count)) ) {
				count--;
			}
			if ( count == 0 ) return((pbTarget-cbPattern));
		}
		pbTarget++;
		if (pbTarget > pbTargetMax) return(NULL);
	}
}


// Little I did know, the fastest 'memmem' (not benchmarked against Railgun, yet) appears to be:
// https://web.itu.edu.tr/kulekci/2018/08/25/paper-at-13th-international-meeting-high-performance-computing-for-computational-science-vecpar18-optimizing-packed-string-matching-on-avx2-platform/
// https://link.springer.com/chapter/10.1007/978-3-030-15996-2_4

// Caution: It doesn't work for needles 1 byte long!
char * Railgun_Nyotengu_XMM_YMM_ZMM (char * pbTarget, char * pbPattern, uint32_t cbTarget, uint32_t cbPattern)
{
	char * pbTargetMax = pbTarget + cbTarget;
	register uint32_t ulHashPattern;
	register uint32_t ulHashTarget;
	signed long count;

	unsigned char SINGLET;
	uint32_t Quadruplet2nd;
	uint32_t Quadruplet3rd;
	uint32_t Quadruplet4th;

	uint32_t AdvanceHopperGrass;

	size_t i;
	size_t j;
	size_t VECTORchunks;
	uint32_t mask;
	//uint8_t mask;

#ifdef XMMtengu
	int SkipWholeVector=16;
	__m128i last4;
	__m128i last1;
	__m128i first2;
	__m128i first4;
	__m128i HaystackVector1;
	__m128i HaystackVector2;
	__m128i HaystackVector3;
	__m128i HaystackVector4;

	__m128i EQD1;
	__m128i EQD2;
	__m128i EQD3;
	__m128i EQD4;

	__m128i FinalVector12;
	__m128i FinalVector34;
#endif 

#ifdef YMMtengu
	int SkipWholeVector=32;
	__m256i last4;
	__m256i last1;
	__m256i first2;
	__m256i first4;
	__m256i HaystackVector1;
	__m256i HaystackVector2;
	__m256i HaystackVector3;
	__m256i HaystackVector4;

	__m256i EQD1;
	__m256i EQD2;
	__m256i EQD3;
	__m256i EQD4;

	__m256i FinalVector12;
	__m256i FinalVector34;
#endif 

#ifdef ZMMtengu
	int SkipWholeVector=64;
	__m512i last4;
	__m512i last1;
	__m512i first2;
	__m512i first4;
	__m512i HaystackVector1;
	__m512i HaystackVector2;
	__m512i HaystackVector3;
	__m512i HaystackVector4;

	uint16_t EQD1mask16;
	uint16_t EQD2mask16;
	uint16_t EQD3mask16;
	uint16_t EQD4mask16;

	uint16_t FinalVector12mask16;
	uint16_t FinalVector34mask16;
#endif 

	if (cbPattern > cbTarget) return(NULL);

	if ( cbPattern<4 ) { // needle 2..3; SCALAR

        	pbTarget = pbTarget+cbPattern;
		ulHashPattern = ( (*(char *)(pbPattern))<<8 ) + *(pbPattern+(cbPattern-1));
		if ( cbPattern==3 ) {
			for ( ;; ) {
				if ( ulHashPattern == ( (*(char *)(pbTarget-3))<<8 ) + *(pbTarget-1) ) {
					if ( *(char *)(pbPattern+1) == *(char *)(pbTarget-2) ) return((pbTarget-3));
				}
				if ( (char)(ulHashPattern>>8) != *(pbTarget-2) ) { 
					pbTarget++;
					if ( (char)(ulHashPattern>>8) != *(pbTarget-2) ) pbTarget++;
				}
				pbTarget++;
				if (pbTarget > pbTargetMax) return(NULL);
			}
		} else {
		}
		for ( ;; ) {
			if ( ulHashPattern == ( (*(char *)(pbTarget-2))<<8 ) + *(pbTarget-1) ) return((pbTarget-2));
			if ( (char)(ulHashPattern>>8) != *(pbTarget-1) ) pbTarget++;
			pbTarget++;
			if (pbTarget > pbTargetMax) return(NULL);
		}

	} else { // Below: haystack <128; needle >=4; SCALAR
		if (cbTarget<128) { // This value is arbitrary (don't know how exactly), it ensures (at least must) better performance than 'Boyer_Moore_Horspool'.

		pbTarget = pbTarget+cbPattern;
		ulHashPattern = *(uint32_t *)(pbPattern);
		SINGLET = ulHashPattern & 0xFF;
		Quadruplet2nd = SINGLET<<8;
		Quadruplet3rd = SINGLET<<16;
		Quadruplet4th = SINGLET<<24;
		for ( ;; ) {
			AdvanceHopperGrass = 0;
			ulHashTarget = *(uint32_t *)(pbTarget-cbPattern);
			if ( ulHashPattern == ulHashTarget ) { // Three unnecessary comparisons here, but 'AdvanceHopperGrass' must be calculated - it has a higher priority.
				count = cbPattern-1;
				while ( count && *(char *)(pbPattern+(cbPattern-count)) == *(char *)(pbTarget-count) ) {
					if ( cbPattern-1==AdvanceHopperGrass+count && SINGLET != *(char *)(pbTarget-count) ) AdvanceHopperGrass++;
					count--;
				}
				if ( count == 0) return((pbTarget-cbPattern));
			} else { // The goal here: to avoid memory accesses by stressing the registers.
				if ( Quadruplet2nd != (ulHashTarget & 0x0000FF00) ) {
					AdvanceHopperGrass++;
					if ( Quadruplet3rd != (ulHashTarget & 0x00FF0000) ) {
						AdvanceHopperGrass++;
						if ( Quadruplet4th != (ulHashTarget & 0xFF000000) ) AdvanceHopperGrass++;
					}
				}
			}
			AdvanceHopperGrass++;
			pbTarget = pbTarget + AdvanceHopperGrass;
			if (pbTarget > pbTargetMax) return(NULL);
		}
		} else { // Below: haystack >=128; needle >=4; VECTOR

		// Stage 1: SSE2 or AVX2 i.e. 16 or 32 strides.
		// Stage 2: Dealing with the eventual remainder.
		//          Careful! Remainder starts (overlapping with the last 32byte chunk, if Needle<32) at NEXT position to 32*(YMM_Chunks_Traversed)+Order4-Needle_Length = 2*32+4-14 = 54:
		//          Chunk #0                          Chunk #1                          Remainder
		//          [00000000001111111111222222222233][33333333444444444455555555556666][6666...
		//          [01234567890123456789012345678901][23456789012345678901234567890123][4567...
		//                                                                  Linus Torva  lds  ! The needle's postfix of order 4 was sought up to 63 (ensuring the 32 bytes skips).
		//                                                                   Linus Torv  alds  ! Then next possible hit is at 54 position, or suffix starting at next to 63 or 63+1=64.
		// The main idea: Stressing the registers as it was done in Quadruplet (the above fastest etude) - outperforms Stephen R. van den Berg's strstr at http://www.scs.stanford.edu/histar/src/pkg/uclibc/libc/string/generic/strstr.c
		// __m256i _mm256_cmpeq_epi32 (__m256i a, __m256i b) needs AVX2; the more attractive __mmask8 _mm256_cmpeq_epi32_mask (__m256i a, __m256i b) needs AVX512??
		
// Pattern: "Linus Torvalds" 
// Order4:            [    ] skip 32 if not a single occurrence of 'alds' within YMM + (Order - 1) = 32 + 3 = 35 bytes window:
// Haystack:                                 "otto.......................Torvalds" 
// YMM HaystackVector1:                      "otto.......................Torva" 
// YMM HaystackVector2:                      "tto.......................Torval" 
// YMM HaystackVector3:                      "to.......................Torvald" 
// YMM HaystackVector4:                      "o.......................Torvalds" 
// YMM Vector1:                              "aldsaldsaldsaldsaldsaldsaldsalds" 
// 
// Mask1=(HaystackVector1 eqd Vector1):       0   0   0   0   0   0   0   0     ! 8bit !
// Mask2=(HaystackVector2 eqd Vector1):       0   0   0   0   0   0   0   0     ! 8bit !
// Mask3=(HaystackVector3 eqd Vector1):       0   0   0   0   0   0   0   0     ! 8bit !
// Mask4=(HaystackVector4 eqd Vector1):       0   0   0   0   0   0   0   1     ! 8bit !
// Result=(Mask1 OR Mask2 OR Mask3 OR Mask4): 0   0   0   0   0   0   0   1     ! 8bit !

// printf("&pbPattern[cbPattern - 1 -3] = %s\n",&pbPattern[cbPattern - 1 -3]); //debug

#ifdef XMMtengu
	VECTORchunks = cbTarget/SkipWholeVector -1; // in here, ensured at least 7 chunks; in order to avoid past haystack XMM reads - decrease 1 chunk and finish with Scalar_Quadruplet
	// The preemptive search for the first char is slower SIGNIFICANTLY on i7 3rd gen?!
//	last1 = _mm_set1_epi8(*(uint8_t*)&pbPattern[0]); //cbPattern - 1
//	first2 = _mm_set1_epi16(*(uint16_t*)&pbPattern[0]); 
	first4 = _mm_set1_epi32(*(uint32_t*)&pbPattern[0]);
	for (i = 0; i < VECTORchunks*SkipWholeVector; i += SkipWholeVector) {
	HaystackVector1 = _mm_loadu_si128 ((const __m128i*)(pbTarget + i + 0));
//	EQD1 = _mm_cmpeq_epi8(HaystackVector1, last1);
//	mask = _mm_movemask_epi8( EQD1 );

//	if ( mask != 0 ) 
	{
	HaystackVector2 = _mm_loadu_si128 ((const __m128i*)(pbTarget + i + 1));
	EQD1 = _mm_cmpeq_epi32(HaystackVector1, first4);
	EQD2 = _mm_cmpeq_epi32(HaystackVector2, first4);
	HaystackVector3 = _mm_loadu_si128 ((const __m128i*)(pbTarget + i + 2));
	HaystackVector4 = _mm_loadu_si128 ((const __m128i*)(pbTarget + i + 3));
	EQD3 = _mm_cmpeq_epi32(HaystackVector3, first4);
	EQD4 = _mm_cmpeq_epi32(HaystackVector4, first4);

	FinalVector12 = _mm_or_si128(EQD1, EQD2);
	FinalVector34 = _mm_or_si128(EQD3, EQD4);

	mask = _mm_movemask_ps( _mm_castsi128_ps(_mm_or_si128(FinalVector12, FinalVector34)) );
	}

	_mm_prefetch((char*)(pbTarget + 64*64), _MM_HINT_T0);
#endif 
#ifdef YMMtengu
	VECTORchunks = cbTarget/SkipWholeVector -1; // in here, ensured at least 3 chunks; in order to avoid past haystack YMM reads - decrease 1 chunk and finish with Scalar_Quadruplet
//	last1 = _mm256_set1_epi8(*(uint8_t*)&pbPattern[0]); //cbPattern - 1
	first4 = _mm256_set1_epi32(*(uint32_t*)&pbPattern[0]);
	for (i = 0; i < VECTORchunks*SkipWholeVector; i += SkipWholeVector) {
	HaystackVector1 = _mm256_loadu_si256((const __m256i*)(pbTarget + i + 0));
//	EQD1 = _mm256_cmpeq_epi8(HaystackVector1, last1);
//	mask = _mm256_movemask_epi8( EQD1 );

//	if ( mask != 0 ) 
	{
	HaystackVector2 = _mm256_loadu_si256((const __m256i*)(pbTarget + i + 1));
	EQD1 = _mm256_cmpeq_epi32(HaystackVector1, first4);
	EQD2 = _mm256_cmpeq_epi32(HaystackVector2, first4);
	HaystackVector3 = _mm256_loadu_si256((const __m256i*)(pbTarget + i + 2));
	HaystackVector4 = _mm256_loadu_si256((const __m256i*)(pbTarget + i + 3));
	EQD3 = _mm256_cmpeq_epi32(HaystackVector3, first4);
	EQD4 = _mm256_cmpeq_epi32(HaystackVector4, first4);

	FinalVector12 = _mm256_or_si256(EQD1, EQD2);
	FinalVector34 = _mm256_or_si256(EQD3, EQD4);

	mask = _mm256_movemask_ps( _mm256_castsi256_ps(_mm256_or_si256(FinalVector12, FinalVector34)) );
	}
	_mm_prefetch((char*)(pbTarget + 64*64), _MM_HINT_T0);
#endif 
// The vector/main loop is (0012c-000fd+2)+(00175-0016d+2)= 59 bytes
/*
; mark_description "Intel(R) C++ Compiler XE for applications running on IA-32, Version 15.0.0.108 Build 20140726";
; mark_description "-O3 -DYMMtengu -D_WIN32_ENVIRONMENT_ -D_N_HIGH_PRIORITY -FeNyotengu_YMM_IntelV150_32bit -FAcs";

.B10.21:                        

;;; 	HaystackVector1 = _mm256_loadu_si256((const __m256i*)(pbTarget + i + 0));
;;; 	HaystackVector2 = _mm256_loadu_si256((const __m256i*)(pbTarget + i + 1));
;;; 	EQD1 = _mm256_cmpeq_epi32(HaystackVector1, first4);
;;; 	EQD2 = _mm256_cmpeq_epi32(HaystackVector2, first4);
;;; 	HaystackVector3 = _mm256_loadu_si256((const __m256i*)(pbTarget + i + 2));
;;; 	HaystackVector4 = _mm256_loadu_si256((const __m256i*)(pbTarget + i + 3));
;;; 	EQD3 = _mm256_cmpeq_epi32(HaystackVector3, first4);
;;; 	EQD4 = _mm256_cmpeq_epi32(HaystackVector4, first4);
;;; 	FinalVector12 = _mm256_or_si256(EQD1, EQD2);
;;; 	FinalVector34 = _mm256_or_si256(EQD3, EQD4);
;;; 	mask = _mm256_movemask_ps( _mm256_castsi256_ps(_mm256_or_si256(FinalVector12, FinalVector34)) );
;;; 	_mm_prefetch((char*)(pbTarget + 64*64), _MM_HINT_T0);

  000fd 0f 18 8e 00 10 
        00 00            prefetcht0 BYTE PTR [4096+esi]         
  00104 c5 fd 76 0b      vpcmpeqd ymm1, ymm0, YMMWORD PTR [ebx] 
  00108 c5 fd 76 54 32 
        01               vpcmpeqd ymm2, ymm0, YMMWORD PTR [1+edx+esi] 
  0010e c5 fd 76 5c 32 
        02               vpcmpeqd ymm3, ymm0, YMMWORD PTR [2+edx+esi] 
  00114 c5 fd 76 64 32 
        03               vpcmpeqd ymm4, ymm0, YMMWORD PTR [3+edx+esi] 
  0011a c5 f5 eb ea      vpor ymm5, ymm1, ymm2                  
  0011e c5 e5 eb f4      vpor ymm6, ymm3, ymm4                  
  00122 c5 d5 eb fe      vpor ymm7, ymm5, ymm6                  
  00126 c5 fc 50 cf      vmovmskps ecx, ymm7                    

;;; 	if ( mask != 0 ) {

  0012a 85 c9            test ecx, ecx                          
  0012c 74 3f            je .B10.26 

...

.B10.26:                        
  0016d 83 c2 20         add edx, 32                            
  00170 83 c3 20         add ebx, 32                            
  00173 3b d0            cmp edx, eax                           
  00175 72 86            jb .B10.21 

.B10.28:                        
*/

// __mmask16 _mm256_cmpeq_epi16_mask (__m256i a, __m256i b)
// __mmask16 _mm512_cmpeq_epi32_mask (__m512i a, __m512i b)
 // __mmask8 _mm256_cmpeq_epi32_mask (__m256i a, __m256i b)
#ifdef ZMMtengu
	VECTORchunks = cbTarget/SkipWholeVector -1; // in here, ensured at least 128/64-1=1 chunk; in order to avoid past haystack ZMM reads - decrease 1 chunk and finish with Scalar_Quadruplet
	first4 = _mm512_set1_epi32(*(uint32_t*)&pbPattern[0]);
	for (i = 0; i < VECTORchunks*SkipWholeVector; i += SkipWholeVector) {
	HaystackVector1 = _mm512_loadu_si512((const __m256i*)(pbTarget + i + 0));
	HaystackVector2 = _mm512_loadu_si512((const __m256i*)(pbTarget + i + 1));
	EQD1mask16 = _mm512_cmpeq_epi32_mask(HaystackVector1, first4);
	EQD2mask16 = _mm512_cmpeq_epi32_mask(HaystackVector2, first4);
	HaystackVector3 = _mm512_loadu_si512((const __m256i*)(pbTarget + i + 2));
	HaystackVector4 = _mm512_loadu_si512((const __m256i*)(pbTarget + i + 3));
	EQD3mask16 = _mm512_cmpeq_epi32_mask(HaystackVector3, first4);
	EQD4mask16 = _mm512_cmpeq_epi32_mask(HaystackVector4, first4);

	FinalVector12mask16 = (EQD1mask16 | EQD2mask16);
	FinalVector34mask16 = (EQD3mask16 | EQD4mask16);

	mask = (FinalVector12mask16 | FinalVector34mask16);
	
	_mm_prefetch((char*)(pbTarget + 64*64), _MM_HINT_T0);
#endif 
// The vector/main loop is (00143-000fc+2)+(00187-0017d+6)= 89 bytes
/*
; mark_description "Intel(R) C++ Compiler XE for applications running on IA-32, Version 15.0.0.108 Build 20140726";
; mark_description "-O3 -DZMMtengu -D_WIN32_ENVIRONMENT_ -D_N_HIGH_PRIORITY -FeNyotengu_ZMM_IntelV150_32bit -FAcs";

;;; #ifdef ZMMtengu
;;; 	VECTORchunks = cbTarget/SkipWholeVector -1; // in here, ensured at least 128/64-1=1 chunk; in order to avoid past haystack ZMM reads - decrease 1 chunk and finish with Scalar_Quadruplet
;;; 	first4 = _mm512_set1_epi32(*(uint32_t*)&pbPattern[0]);

  000df 8b 45 0c         mov eax, DWORD PTR [12+ebp]            
  000e2 8d 53 c0         lea edx, DWORD PTR [-64+ebx]           

;;; 	for (i = 0; i < VECTORchunks*SkipWholeVector; i += SkipWholeVector) {

  000e5 83 e2 c0         and edx, -64                           
  000e8 62 f2 7d 48 58 
        00               vpbroadcastd zmm0, DWORD PTR [eax]     
  000ee 0f 84 99 00 00 
        00               je .B10.28 

.B10.20:                        
  000f4 89 54 24 44      mov DWORD PTR [68+esp], edx            
  000f8 33 c0            xor eax, eax                           
  000fa 8b de            mov ebx, esi                           

.B10.21:                        

;;; 	HaystackVector1 = _mm512_loadu_si512((const __m256i*)(pbTarget + i + 0));
;;; 	HaystackVector2 = _mm512_loadu_si512((const __m256i*)(pbTarget + i + 1));
;;; 	EQD1mask16 = _mm512_cmpeq_epi32_mask(HaystackVector1, first4);
;;; 	EQD2mask16 = _mm512_cmpeq_epi32_mask(HaystackVector2, first4);
;;; 	HaystackVector3 = _mm512_loadu_si512((const __m256i*)(pbTarget + i + 2));
;;; 	HaystackVector4 = _mm512_loadu_si512((const __m256i*)(pbTarget + i + 3));
;;; 	EQD3mask16 = _mm512_cmpeq_epi32_mask(HaystackVector3, first4);
;;; 	EQD4mask16 = _mm512_cmpeq_epi32_mask(HaystackVector4, first4);
;;; 	FinalVector12mask16 = (EQD1mask16 | EQD2mask16);
;;; 	FinalVector34mask16 = (EQD3mask16 | EQD4mask16);
;;; 	mask = (FinalVector12mask16 | FinalVector34mask16);
;;; 	_mm_prefetch((char*)(pbTarget + 64*64), _MM_HINT_T0);

  000fc 8b 7d 08         mov edi, DWORD PTR [8+ebp]             
  000ff 0f 18 8f 00 10 
        00 00            prefetcht0 BYTE PTR [4096+edi]         
  00106 62 f1 7d 48 76 
        03               vpcmpeqd k0, zmm0, ZMMWORD PTR [ebx]   
  0010c 62 f1 7d 48 76 
        8c 38 01 00 00 
        00               vpcmpeqd k1, zmm0, ZMMWORD PTR [1+eax+edi] 
  00117 62 f1 7d 48 76 
        94 38 02 00 00 
        00               vpcmpeqd k2, zmm0, ZMMWORD PTR [2+eax+edi] 
  00122 62 f1 7d 48 76 
        9c 38 03 00 00 
        00               vpcmpeqd k3, zmm0, ZMMWORD PTR [3+eax+edi] 
  0012d c5 f8 93 f0      kmovw esi, k0                          
  00131 c5 f8 93 c9      kmovw ecx, k1                          
  00135 c5 f8 93 d2      kmovw edx, k2                          
  00139 c5 f8 93 fb      kmovw edi, k3                          
  0013d 0b f1            or esi, ecx                            
  0013f 0b d7            or edx, edi                            
  00141 0b f2            or esi, edx                            
;;; 	if ( mask != 0 ) {
  00143 74 38            je .B10.26 
                                
...
                                
.B10.26:                        
  0017d 83 c0 40         add eax, 64                            
  00180 83 c3 40         add ebx, 64                            
  00183 3b 44 24 44      cmp eax, DWORD PTR [68+esp]            
  00187 0f 82 6f ff ff 
        ff               jb .B10.21 
                                
.B10.28:                        
*/


//		printf("mask = %02x\n", mask); //debug
	if ( mask != 0 ) {
//		printf("_mm_popcnt_u32(mask) = %d\n", _mm_popcnt_u32(mask)); //debug

// For these two:
// char *Haystack = "CPU Benchmark: Linus Torvalds ................................................................................... Linus Torvalds"; // 128 bytes long
// char *Needle = "Linus Torvalds"; // 14 bytes long
// the 'debug' outcome is:
// &pbPattern[cbPattern - 1 -3] = alds
// mask = 40
// _mm_popcnt_u32(mask) = 1
// Okay, 0x40 is 0000 0010 (LSB first) i.e. 6th bit is set, it means 4 possible positions within Chunk #6 (4*6=24 offset) (in fact it is only 25th):
// DWORD #0DWORD #1DWORD #2DWORD #3DWORD #4DWORD #5DWORD #6DWORD #7
// [00..03][04..07][08..11][12..15][16..19][20..23][24..27][28..31]
//                                                  |      
//                                                  /
//                                                 /
//                                                \/
//                        000000000011111111112222[2222]2233
//                        012345678901234567890123[4567]8901
//                        CPU Benchmark: Linus Tor[vald]s ..

		// Manually find the first suffix position:
		//j = i; // somewhere in chunk #i lie possible POPCNT(mask) matches...
		// Okay, doing it dirty as a start - checking all the 16/32/64 positions one-by-one:
	for (j = 0; j < SkipWholeVector; j++)
		if (memcmp(pbTarget + i + j, &pbPattern[cbPattern - cbPattern], cbPattern) == 0) return( pbTarget + i + j ); //first4
	// pbTarget + i + j points to offset of DWORD/suffix so we have to repoint it to the start offset, namely, pbTarget + i + j - (cbPattern-4)
		//while (memcmp(pbTarget + j, &pbPattern[cbPattern - 1 -3], 4) != 0) j++;
		// Don't forget! Comparing the rest of the Needle (to the left) has to be boundary checked (to not go outside) - for speed, this check has to be done outside this loop.
/*
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
						// Order 4 [
					// Let's try something "outrageous" like comparing with[out] overlap BBs 4bytes long instead of 1 byte back-to-back:
					// Inhere we are using order 4, 'cbPattern - Order + 1' is the number of BBs for text 'cbPattern' bytes long, for example, for cbPattern=11 'fastest fox' and Order=4 we have BBs = 11-4+1=8:
					//0:"fast" if the comparison failed here, 'count' is 1; 'Gulliver' is cbPattern-(4-1)-7
					//1:"aste" if the comparison failed here, 'count' is 2; 'Gulliver' is cbPattern-(4-1)-6
					//2:"stes" if the comparison failed here, 'count' is 3; 'Gulliver' is cbPattern-(4-1)-5
					//3:"test" if the comparison failed here, 'count' is 4; 'Gulliver' is cbPattern-(4-1)-4
					//4:"est " if the comparison failed here, 'count' is 5; 'Gulliver' is cbPattern-(4-1)-3
					//5:"st f" if the comparison failed here, 'count' is 6; 'Gulliver' is cbPattern-(4-1)-2
					//6:"t fo" if the comparison failed here, 'count' is 7; 'Gulliver' is cbPattern-(4-1)-1
					//7:" fox" if the comparison failed here, 'count' is 8; 'Gulliver' is cbPattern-(4-1)
						count = cbPattern-4+1; 
						//count = count-4; // Double-beauty here of already being checked 'ulHashTarget' and not polluting/repeating the final lookup below.
						while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
							count = count-4; // - order, of course order 4 is much more SWEET&CHEAP - less loops
						if ( count <= 0 )
							return(pbTarget+i);
						// Order 4 ]
						}
*/

	} //if ( mask != 0 ) {
	} //for (i = 0; i <

// Deal with the remainder (starts right after the last chunk) with Scalar code [
pbTarget = pbTarget+ i; // 'i' has to be the traversed pool by the vector
	//if (cbPattern > cbTarget) return(NULL);
	// Above check precedes all Railguns, inhere 'cbTarget' is the HaystackLen-i i.e. the remainder
	if (cbPattern > cbTarget-(i)) return(NULL);

		pbTarget = pbTarget+cbPattern;
		ulHashPattern = *(uint32_t *)(pbPattern);
		SINGLET = ulHashPattern & 0xFF;
		Quadruplet2nd = SINGLET<<8;
		Quadruplet3rd = SINGLET<<16;
		Quadruplet4th = SINGLET<<24;
		for ( ;; ) {
			AdvanceHopperGrass = 0;
			ulHashTarget = *(uint32_t *)(pbTarget-cbPattern);
			if ( ulHashPattern == ulHashTarget ) { // Three unnecessary comparisons here, but 'AdvanceHopperGrass' must be calculated - it has a higher priority.
				count = cbPattern-1;
				while ( count && *(char *)(pbPattern+(cbPattern-count)) == *(char *)(pbTarget-count) ) {
					if ( cbPattern-1==AdvanceHopperGrass+count && SINGLET != *(char *)(pbTarget-count) ) AdvanceHopperGrass++;
					count--;
				}
				if ( count == 0) return((pbTarget-cbPattern));
			} else { // The goal here: to avoid memory accesses by stressing the registers.
				if ( Quadruplet2nd != (ulHashTarget & 0x0000FF00) ) {
					AdvanceHopperGrass++;
					if ( Quadruplet3rd != (ulHashTarget & 0x00FF0000) ) {
						AdvanceHopperGrass++;
						if ( Quadruplet4th != (ulHashTarget & 0xFF000000) ) AdvanceHopperGrass++;
					}
				}
			}
			AdvanceHopperGrass++;
			pbTarget = pbTarget + AdvanceHopperGrass;
			if (pbTarget > pbTargetMax) return(NULL);
		}
// Deal with the remainder (starts right after the last chunk) with Scalar code ]

		} //if (cbTarget<128) {
	} //if ( cbPattern<4 ) { needle 2..3; SCALAR

return(NULL);
}

int main(int argc, char** argv)
{
	FILE *fp, *fp2;
	const unsigned char BOM[3] = { '\xEF', '\xBB', '\xBF' };
	int ncnt;
	uint64_t SourceSize, size_inLINESIXFOUR;
	char* SourceBlock=NULL;
	size_t BewareOfMinGW;

	long double t;

#ifdef _N_HIGH_PRIORITY
   DWORD dwError, dwPriClass;
#endif

	char * ScanAllHits;
	char * ScanAllHitsNew;
	uint64_t AllHits;
	int64_t LenHaystack;
	uint32_t LenNeedle;
	char *Haystack = "CPU Benchmark: Linus Torvalds ................................................................................... Linus Torvalds";
//	char *Needle = "Linus Torvalds";
	char *Needle;
	char NeedleBuffer[320]={0};

	//                Linus Torvalds
	//                  |      <----|
	//                 \ /     [    ] ! comparison shouldn't read outside the Haystack
	//char *Haystack = "nus Torvalds .................................................................................................... Linus Torvalds";
	printf("NyoTengu a.k.a. 'SHETENGU' - the skydogess exact searcher, written by Kaze, 2020-Oct-10, for contacts: sanmayce@sanmayce.com\n"); 

	if (argc==1) {
		printf("Usage: NyoTengu Haystackfilename Needlefilename\n"); exit(13);
	}
	if ((fp = fopen(argv[1], "rb")) == NULL) { // fopen is buggy in MinGW it is problematic with big files being read in one go e.g. 3.3GB
		printf("NyoTengu: Can't open '%s' file.\n", argv[1]); exit(13);
	}
	if ((fp2 = fopen(argv[2], "rb")) == NULL) { // fopen is buggy in MinGW it is problematic with big files being read in one go e.g. 3.3GB
		printf("NyoTengu: Can't open '%s' file.\n", argv[2]); exit(13);
	}
	// trimBOM:
	ncnt = fread(NeedleBuffer, 1, 320, fp2);
	if (memcmp(NeedleBuffer, BOM, 3) != 0)
		Needle=NeedleBuffer;
	else {
		Needle=NeedleBuffer+3;
	}
	printf("Needle = %s\n", Needle);

#ifdef _N_HIGH_PRIORITY
   if(!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
   {
//      _tprintf(TEXT("Already REALTIME_PRIORITY.\n"));
//      goto Cleanup;
   } 
   if(!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
   {
//      _tprintf(TEXT("Already REALTIME_PRIORITY.\n"));
//      goto Cleanup;
   } 
   // Display priority class

   dwPriClass = GetPriorityClass(GetCurrentProcess());

   //_tprintf(TEXT("Current priority class is 0x%x\n"), dwPriClass);

   if (dwPriClass==0x00000080) printf("Current priority class is HIGH_PRIORITY_CLASS.\n");
   if (dwPriClass==0x00000100) printf("Current priority class is REALTIME_PRIORITY_CLASS.\n");
#endif

#ifdef _N_HIGH_PRIORITY
// https://habrahabr.ru/post/113682/
// Andrew Aksyonoff [
SetProcessAffinityMask(GetCurrentProcess(), 1);
	//volatile int zomg = 1;
	//for ( int i=1; i<1000000000; i++ )
	//	zomg *= i;
// Andrew Aksyonoff ]
#endif

#if defined(_WIN32_ENVIRONMENT_)
   // 64bit:
_lseeki64( fileno(fp), 0L, SEEK_END );
size_inLINESIXFOUR = _telli64( fileno(fp) );
_lseeki64( fileno(fp), 0L, SEEK_SET );
#else
   // 64bit:
fseeko( fp, 0L, SEEK_END );
size_inLINESIXFOUR = ftello( fp );
fseeko( fp, 0L, SEEK_SET );
#endif /* defined(_WIN32_ENVIRONMENT_)  */
SourceSize = (uint64_t)size_inLINESIXFOUR;

	printf("Allocating Source-Buffer %s bytes ... ", _ui64toaKAZEcomma(SourceSize, llTOaDigits2, 10) );
	SourceBlock = (char*)malloc(SourceSize+512);
	if( SourceBlock == NULL )
		{ printf("Failure!\nNyotengu: Needed memory allocation denied!\n"); exit(13); }
	else printf("OK\n");
	BewareOfMinGW = fread(SourceBlock, 1, SourceSize, fp);
	if (BewareOfMinGW != SourceSize)
		{ printf("Nyotengu: Unable to upload the whole file!\n"); exit(13); }

	//printf("Haystack Start / Found Suffix = %p / %p\n", Haystack, Railgun_Nyotengu_XMM_YMM_ZMM (Haystack, Needle, 128, 14)); 
//scalar:
	Haystack = SourceBlock;
	ScanAllHits = Haystack;
	//LenHaystack=strlen(Haystack);
	LenHaystack=SourceSize;
	LenNeedle=strlen(Needle);
	AllHits=0;
printf("Searching into Haystack (%s) for all occurrences of Needle (%s) with fastest (known to me) SCALAR memmem() - 'Railgun_Trolldom_64' ...\n", _ui64toaKAZEcomma(LenHaystack, llTOaDigits2, 10), _ui64toaKAZEcomma(LenNeedle, llTOaDigits3, 10));
//printf("Haystack size = %s \n", _ui64toaKAZEcomma(LenHaystack, llTOaDigits2, 10) );
//printf("Haystack first DWORD: %c%c%c%c\n",*(Haystack+0),*(Haystack+1),*(Haystack+2),*(Haystack+3));
//printf("Needle size = %s \n", _ui64toaKAZEcomma(LenNeedle, llTOaDigits2, 10) );
//printf("Needle first DWORD: %c%c%c%c\n",*(Needle+0),*(Needle+1),*(Needle+2),*(Needle+3));
		clocks1 = clock();
	while (!0) {
		//ScanAllHitsNew = Railgun_Doublet (ScanAllHits, Needle, LenHaystack - (uint64_t)(ScanAllHits-Haystack), LenNeedle);
		ScanAllHitsNew = Railgun_Trolldom_64 (ScanAllHits, Needle, LenHaystack - (uint64_t)(ScanAllHits-Haystack), LenNeedle);
		if (ScanAllHitsNew == NULL) break;
		ScanAllHits = ScanAllHitsNew + 1; 
		AllHits++;
	}
		clocks2 = clock();
	printf("Hits: %d\n", AllHits); 
	//printf("Search took %s milliseconds. \n", _ui64toaKAZEcomma((clocks2 - clocks1 + 1), llTOaDigits2, 10) );
	t = (long double)(clocks2 - clocks1 + 1)/CLOCKS_PER_SEC;
	//printf("Search took %Lg seconds.\n", t);
	printf("Search took %.3f seconds.\n", (float)t);
	printf("Pure Search Performance: %s bytes/second. \n", _ui64toaKAZEcomma( (uint64_t)((double)SourceSize/t), llTOaDigits2, 10) );
//vector:
	Haystack = SourceBlock;
	ScanAllHits = Haystack;
	//LenHaystack=strlen(Haystack);
	LenHaystack=SourceSize;
	LenNeedle=strlen(Needle);
	AllHits=0;
#ifdef XMMtengu
printf("Searching into Haystack (%s) for all occurrences of Needle (%s) with fastest (known to me) VECTOR memmem() - 'Railgun_Nyotengu_XMM' ...\n", _ui64toaKAZEcomma(LenHaystack, llTOaDigits2, 10), _ui64toaKAZEcomma(LenNeedle, llTOaDigits3, 10));
#endif 
#ifdef YMMtengu
printf("Searching into Haystack (%s) for all occurrences of Needle (%s) with fastest (known to me) VECTOR memmem() - 'Railgun_Nyotengu_YMM' ...\n", _ui64toaKAZEcomma(LenHaystack, llTOaDigits2, 10), _ui64toaKAZEcomma(LenNeedle, llTOaDigits3, 10));
#endif 
#ifdef ZMMtengu
printf("Searching into Haystack (%s) for all occurrences of Needle (%s) with fastest (known to me) VECTOR memmem() - 'Railgun_Nyotengu_ZMM' ...\n", _ui64toaKAZEcomma(LenHaystack, llTOaDigits2, 10), _ui64toaKAZEcomma(LenNeedle, llTOaDigits3, 10));
#endif 
		clocks1 = clock();
	while (!0) {
		//ScanAllHitsNew = Railgun_Doublet (ScanAllHits, Needle, LenHaystack - (uint64_t)(ScanAllHits-Haystack), LenNeedle);
		ScanAllHitsNew = Railgun_Nyotengu_XMM_YMM_ZMM (ScanAllHits, Needle, LenHaystack - (uint64_t)(ScanAllHits-Haystack), LenNeedle);
		if (ScanAllHitsNew == NULL) break;
		ScanAllHits = ScanAllHitsNew + 1; 
		AllHits++;
	}
		clocks2 = clock();
	printf("Hits: %d\n", AllHits); 
	//printf("Search took %s milliseconds. \n", _ui64toaKAZEcomma((clocks2 - clocks1 + 1), llTOaDigits2, 10) );
	t = (long double)(clocks2 - clocks1 + 1)/CLOCKS_PER_SEC;
	//printf("Search took %Lg seconds.\n", t);
	printf("Search took %.3f seconds.\n", (float)t);
	printf("Pure Search Performance: %s bytes/second. \n", _ui64toaKAZEcomma( (uint64_t)((double)SourceSize/t), llTOaDigits2, 10) );

	return 0;
}




















/*
Hi,
Johnny-come-lately, yet, have to ask what is the fastest of all variants shared here.
Wanna include it in my searcher and compare it with my variants...

In next days will try to write a SIMD memmem, just to see whether it is slower than already existing variants.

My idea is to emulate Boyer-Moore-Horspool of order 2, even order 3.


```
Pattern: "to" 
Haystack:                           "otto...........................toz" 
YMM HaystackVector1:                "otto...........................t" 
YMM HaystackVector2:                "tto...........................to" 
YMM Vector1:                        "tttttttttttttttttttttttttttttttt" 
YMM Vector2:                        "oooooooooooooooooooooooooooooooo" 
 
Mask1=(HaystackVector1 AND Vector1): 0110...........................1 
Mask2=(HaystackVector2 AND Vector2): 001............................1 
Result=(Mask1 AND Mask2):            0010...........................1 
```

So my question, have you tried this approach:

Instead of filling the pattern vector with `epi_8` (as in above scheme) but `epi_16`, as the manual shows the latter could be implemented either only with vectors or with scalar mask and vector:

```
Pattern: "to" 
Haystack:                           "otto...........................toz" 
YMM HaystackVector1:                "otto...........................t" 
YMM HaystackVector2:                "tto...........................to" 
YMM Vector1:                        "totototototototototototototototo" 
 
Mask1=(HaystackVector1 AND Vector1): 0 1 . . . . . . . . . . . . . 0    ! 16bit !
Mask2=(HaystackVector2 AND Vector1): 0 0 . . . . . . . . . . . . . 1    ! 16bit !
Result=(Mask1 OR Mask2):             0 1 . . . . . . . . . . . . . 1    ! 16bit !
```

Don't know how much latency hurts in a real etude:

```
__m256i _mm256_cmpeq_epi16 (__m256i a, __m256i b)
Architecture Latency  Throughput (CPI)
Skylake            1         0.5
```

```
__mmask16 _mm256_cmpeq_epi16_mask (__m256i a, __m256i b)
Architecture Latency Throughput (CPI)
Skylake            3          1
```

I will explore whether the mask (mixing scalars and vectors) variant stalls pipeline in practical searches with Linux kernel (1GB strong) used as a haystack... the needle will be "Linus Torvalds".
*/

/*
Right, even using a 8bit mask is interesting - `__mmask8 _mm256_cmpeq_epi32_mask (__m256i a, __m256i b)` case.

My wish is to benchmark fastest AVX2 functions (on i5-7200U), also to include the fastest grep - ripgrep which uses some SIMD variant called from RUST. Few days ago, thanks to overclock.net members, saw how 8 threads running my plain C Boyer-Moore-Horspool of order 2 function outperformed 1 thread of SIMDed RUST, only on 10th gen CPU (on 7th, 8th and 9th was slower), go figure.

Simply, I wonder how different variants are executed by different CPUs, last year I saw how changing "a trivial" uint64_t with size_t resulted in detectable speed up, the GCC generated 3 less instructions, didn't expect it, so benchmarking even small changes might be instrumental in tuning.
*/


/*
Good sources:
https://software.intel.com/sites/landingpage/IntrinsicsGuide/#expand=777,774,790,793,765,767,773&cats=Compare&text=_mm_cmpeq_epi
https://www.officedaytime.com/simd512e/

# # #

Synopsis
__m128i _mm_or_si128 (__m128i a, __m128i b)
#include <emmintrin.h>
Instruction: por xmm, xmm
CPUID Flags: SSE2

Description
Compute the bitwise OR of 128 bits (representing integer data) in a and b, and store the result in dst.

Operation
dst[127:0] := (a[127:0] OR b[127:0])

# # #

Synopsis
__m256i _mm256_or_si256 (__m256i a, __m256i b)
#include <immintrin.h>
Instruction: vpor ymm, ymm, ymm
CPUID Flags: AVX2

Description
Compute the bitwise OR of 256 bits (representing integer data) in a and b, and store the result in dst.

Operation
dst[255:0] := (a[255:0] OR b[255:0])
dst[MAX:256] := 0

# # #


@ @ @

Synopsis
int _mm_movemask_epi8 (__m128i a)
#include <emmintrin.h>
Instruction: pmovmskb r32, xmm
CPUID Flags: SSE2

Description
Create mask from the most significant bit of each 8-bit element in a, and store the result in dst.

Operation
FOR j := 0 to 15
	i := j*8
	dst[j] := a[i+7]
ENDFOR
dst[MAX:16] := 0

@ @ @

Synopsis
int _mm256_movemask_epi8 (__m256i a)
#include <immintrin.h>
Instruction: vpmovmskb r32, ymm
CPUID Flags: AVX2

Description
Create mask from the most significant bit of each 8-bit element in a, and store the result in dst.

Operation
FOR j := 0 to 31
	i := j*8
	dst[j] := a[i+7]
ENDFOR

@ @ @

Synopsis
int _mm256_movemask_ps (__m256 a)
#include <immintrin.h>
Instruction: vmovmskps r32, ymm
CPUID Flags: AVX

Description
Set each bit of mask dst based on the most significant bit of the corresponding packed single-precision (32-bit) floating-point element in a.

Operation
FOR j := 0 to 7
	i := j*32
	IF a[i+31]
		dst[j] := 1
	ELSE
		dst[j] := 0
	FI
ENDFOR
dst[MAX:8] := 0

@ @ @

~ ~ ~

Synopsis
__m128i _mm_cmpeq_epi16 (__m128i a, __m128i b)
#include <emmintrin.h>
Instruction: pcmpeqw xmm, xmm
CPUID Flags: SSE2

Description
Compare packed 16-bit integers in a and b for equality, and store the results in dst.

Operation
FOR j := 0 to 7
	i := j*16
	dst[i+15:i] := ( a[i+15:i] == b[i+15:i] ) ? 0xFFFF : 0
ENDFOR

~ ~ ~

Synopsis
__mmask8 _mm_cmpeq_epi16_mask (__m128i a, __m128i b)
#include <immintrin.h>
Instruction: vpcmpw k, xmm, xmm
CPUID Flags: AVX512VL + AVX512BW

Description
Compare packed signed 16-bit integers in a and b for equality, and store the results in mask vector k.

Operation
FOR j := 0 to 7
	i := j*16
	k[j] := ( a[i+15:i] == b[i+15:i] ) ? 1 : 0
ENDFOR
k[MAX:8] := 0

~ ~ ~ 

Synopsis
__m128i _mm_cmpeq_epi32 (__m128i a, __m128i b)
#include <emmintrin.h>
Instruction: pcmpeqd xmm, xmm
CPUID Flags: SSE2

Description
Compare packed 32-bit integers in a and b for equality, and store the results in dst.

Operation
FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ( a[i+31:i] == b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR

~ ~ ~

Synopsis
__mmask8 _mm_cmpeq_epi32_mask (__m128i a, __m128i b)
#include <immintrin.h>
Instruction: vpcmpd k, xmm, xmm
CPUID Flags: AVX512VL + AVX512F

Description
Compare packed signed 32-bit integers in a and b for equality, and store the results in mask vector k.

Operation
FOR j := 0 to 3
	i := j*32
	k[j] := ( a[i+31:i] == b[i+31:i] ) ? 1 : 0
ENDFOR
k[MAX:4] := 0

~ ~ ~

Synopsis
__m256i _mm256_cmpeq_epi8 (__m256i a, __m256i b)
#include <immintrin.h>
Instruction: vpcmpeqb ymm, ymm, ymm
CPUID Flags: AVX2

Description
Compare packed 8-bit integers in a and b for equality, and store the results in dst.

Operation
FOR j := 0 to 31
	i := j*8
	dst[i+7:i] := ( a[i+7:i] == b[i+7:i] ) ? 0xFF : 0
ENDFOR
dst[MAX:256] := 0

~ ~ ~

Synopsis
__mmask32 _mm256_cmpeq_epi8_mask (__m256i a, __m256i b)
#include <immintrin.h>
Instruction: vpcmpb k, ymm, ymm
CPUID Flags: AVX512VL + AVX512BW

Description
Compare packed signed 8-bit integers in a and b for equality, and store the results in mask vector k.

Operation
FOR j := 0 to 31
	i := j*8
	k[j] := ( a[i+7:i] == b[i+7:i] ) ? 1 : 0
ENDFOR
k[MAX:32] := 0

~ ~ ~

Synopsis
__m256i _mm256_cmpeq_epi16 (__m256i a, __m256i b)
#include <immintrin.h>
Instruction: vpcmpeqw ymm, ymm, ymm
CPUID Flags: AVX2

Description
Compare packed 16-bit integers in a and b for equality, and store the results in dst.

Operation
FOR j := 0 to 15
	i := j*16
	dst[i+15:i] := ( a[i+15:i] == b[i+15:i] ) ? 0xFFFF : 0
ENDFOR
dst[MAX:256] := 0

~ ~ ~

Synopsis
__mmask16 _mm256_cmpeq_epi16_mask (__m256i a, __m256i b)
#include <immintrin.h>
Instruction: vpcmpw k, ymm, ymm
CPUID Flags: AVX512VL + AVX512BW

Description
Compare packed signed 16-bit integers in a and b for equality, and store the results in mask vector k.

Operation
FOR j := 0 to 15
	i := j*16
	k[j] := ( a[i+15:i] == b[i+15:i] ) ? 1 : 0
ENDFOR
k[MAX:16] := 0

~ ~ ~

Synopsis
__m256i _mm256_cmpeq_epi32 (__m256i a, __m256i b)
#include <immintrin.h>
Instruction: vpcmpeqd ymm, ymm, ymm
CPUID Flags: AVX2

Description
Compare packed 32-bit integers in a and b for equality, and store the results in dst.

Operation
FOR j := 0 to 7
	i := j*32
	dst[i+31:i] := ( a[i+31:i] == b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR
dst[MAX:256] := 0

~ ~ ~

Synopsis
__mmask8 _mm256_cmpeq_epi32_mask (__m256i a, __m256i b)
#include <immintrin.h>
Instruction: vpcmpd k, ymm, ymm
CPUID Flags: AVX512VL + AVX512F

Description
Compare packed signed 32-bit integers in a and b for equality, and store the results in mask vector k.

Operation
FOR j := 0 to 7
	i := j*32
	k[j] := ( a[i+31:i] == b[i+31:i] ) ? 1 : 0
ENDFOR
k[MAX:8] := 0

~ ~ ~

Synopsis
__m256i _mm256_cmpeq_epi64 (__m256i a, __m256i b)
#include <immintrin.h>
Instruction: vpcmpeqq ymm, ymm, ymm
CPUID Flags: AVX2

Description
Compare packed 64-bit integers in a and b for equality, and store the results in dst.

Operation
FOR j := 0 to 3
	i := j*64
	dst[i+63:i] := ( a[i+63:i] == b[i+63:i] ) ? 0xFFFFFFFFFFFFFFFF : 0
ENDFOR
dst[MAX:256] := 0

~ ~ ~

Synopsis
__mmask8 _mm256_cmpeq_epi64_mask (__m256i a, __m256i b)
#include <immintrin.h>
Instruction: vpcmpq k, ymm, ymm
CPUID Flags: AVX512VL + AVX512F

Description
Compare packed signed 64-bit integers in a and b for equality, and store the results in mask vector k.

Operation
FOR j := 0 to 3
	i := j*64
	k[j] := ( a[i+63:i] == b[i+63:i] ) ? 1 : 0
ENDFOR
k[MAX:4] := 0

~ ~ ~

& & &

Synopsis
__m256i _mm256_set1_epi32 (int a)
#include <immintrin.h>
Instruction: Seqeuence
CPUID Flags: AVX

Description
Broadcast 32-bit integer a to all elements of dst. This intrinsic may generate the vpbroadcastd.

Operation
FOR j := 0 to 7
	i := j*32
	dst[i+31:i] := a[31:0]
ENDFOR
dst[MAX:256] := 0

& & &
*/


/*
"What Else Is There?" Lyrics
Röyksopp feat. Karin Dreijer Andersson

It was me on that road
But you couldn't see me
Too many lights out, but nowhere near here

It was me on that road
Still you couldn't see me
And then flashlights and explosions

Roads are getting nearer
We cover distance but not together

I am the storm and I am the wonder
And the flashlights nightmares
And sudden explosions

I don't know what more to ask for
I was given just one wish

It's about you and the sun
A morning run
The story of my maker
What I have and what I ache for

I've got a golden ear
I cut and I spear
And what else is there

Roads are getting nearer
We cover distance still not together

If I am the storm if I am the wonder
Will I have flashlights nightmares
And sudden explosion

There's no room where I can go and
You've got secrets too

I don't know what more to ask for
I was given just one wish
*/

// Tweet: Is "Road's end getting nearer" or "Roads are getting nearer" correct, the latter make no sense, to me.

// https://stackoverflow.com/questions/20775005/8-bit-shift-operation-in-avx2-with-shifting-in-zeros?noredirect=1
/*
template  <unsigned int N> __m256i _mm256_shift_left(__m256i a)
{
  __m256i mask =  _mm256_srli_si256(_mm256_permute2x128_si256(a, a, _MM_SHUFFLE(0,0,3,0)), 16-N);
  return _mm256_or_si256(_mm256_slli_si256(a,N),mask);
}
int main(int argc, char* argv[]) {
   __m256i reg =  _mm256_set_epi8(32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1);

   __m256i result = _mm256_shift_left<1>(reg);
   for(int i = 0; i < 32; i++)
     printf("%2d ",((unsigned char *)&result)[i]);
   printf("\n");
}
// The output is
// 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 0 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
*/

// https://stackoverflow.com/questions/25248766/emulating-shifts-on-32-bytes-with-avx?noredirect=1
// Here is a function to bit shift left a ymm register using avx2. I use it to shift left by one, though it looks like it works for up to 63 bit shifts.
/*
//----------------------------------------------------------------------------
// bit shift left a 256-bit value using ymm registers
//          __m256i *data - data to shift
//          int count     - number of bits to shift
// return:  __m256i       - carry out bit(s)

static __m256i bitShiftLeft256ymm (__m256i *data, int count)
   {
   __m256i innerCarry, carryOut, rotate;

   innerCarry = _mm256_srli_epi64 (*data, 64 - count);                        // carry outs in bit 0 of each qword
   rotate     = _mm256_permute4x64_epi64 (innerCarry, 0x93);                  // rotate ymm left 64 bits
   innerCarry = _mm256_blend_epi32 (_mm256_setzero_si256 (), rotate, 0xFC);   // clear lower qword
   *data      = _mm256_slli_epi64 (*data, count);                             // shift all qwords left
   *data      = _mm256_or_si256 (*data, innerCarry);                          // propagate carrys from low qwords
   carryOut   = _mm256_xor_si256 (innerCarry, rotate);                        // clear all except lower qword
   return carryOut;
   }

//----------------------------------------------------------------------------
*/





/*
__m128i _mm_set1_epi32 (int a)
#include <emmintrin.h>
Instruction: Seqeuence
CPUID Flags: SSE2
Description
Broadcast 32-bit integer a to all elements of dst. This intrinsic may generate vpbroadcastd.
Operation
FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := a[31:0]
ENDFOR


__m512i _mm512_set1_epi32 (int a)
#include <immintrin.h>
Instruction: vpbroadcastd zmm, r32
CPUID Flags: AVX512F
Description
Broadcast 32-bit integer a to all elements of dst.
Operation
FOR j := 0 to 15
	i := j*32
	dst[i+31:i] := a[31:0]
ENDFOR
dst[MAX:512] := 0


__m128i _mm_loadu_si128 (__m128i const* mem_addr)
#include <emmintrin.h>
Instruction: movdqu xmm, m128
CPUID Flags: SSE2
Description
Load 128-bits of integer data from memory into dst. mem_addr does not need to be aligned on any particular boundary.
Operation
dst[127:0] := MEM[mem_addr+127:mem_addr]


__m512i _mm512_loadu_si512 (void const* mem_addr)
#include <immintrin.h>
Instruction: vmovdqu32 zmm, m512
CPUID Flags: AVX512F
Description
Load 512-bits of integer data from memory into dst. mem_addr does not need to be aligned on any particular boundary.
Operation
dst[511:0] := MEM[mem_addr+511:mem_addr]
dst[MAX:512] := 0


__m128i _mm_cmpeq_epi32 (__m128i a, __m128i b)
#include <emmintrin.h>
Instruction: pcmpeqd xmm, xmm
CPUID Flags: SSE2
Description
Compare packed 32-bit integers in a and b for equality, and store the results in dst.
Operation
FOR j := 0 to 3
	i := j*32
	dst[i+31:i] := ( a[i+31:i] == b[i+31:i] ) ? 0xFFFFFFFF : 0
ENDFOR


__mmask16 _mm512_cmpeq_epi32_mask (__m512i a, __m512i b)
#include <immintrin.h>
Instruction: vpcmpeqd k, zmm, zmm
CPUID Flags: AVX512F for AVX-512, KNCNI for KNC
Description
Compare packed 32-bit integers in a and b for equality, and store the results in mask vector k.
Operation
FOR j := 0 to 15
	i := j*32
	k[j] := ( a[i+31:i] == b[i+31:i] ) ? 1 : 0
ENDFOR
k[MAX:16] := 0


__m128i _mm_or_si128 (__m128i a, __m128i b)
#include <emmintrin.h>
Instruction: por xmm, xmm
CPUID Flags: SSE2
Description
Compute the bitwise OR of 128 bits (representing integer data) in a and b, and store the result in dst.
Operation
dst[127:0] := (a[127:0] OR b[127:0])


__m512i _mm512_or_si512 (__m512i a, __m512i b)
#include <immintrin.h>
Instruction: vpord zmm, zmm, zmm
CPUID Flags: AVX512F for AVX-512, KNCNI for KNC
Description
Compute the bitwise OR of 512 bits (representing integer data) in a and b, and store the result in dst.
Operation
dst[511:0] := (a[511:0] OR b[511:0])
dst[MAX:512] := 0


int _mm_movemask_epi8 (__m128i a)
Synopsis
int _mm_movemask_epi8 (__m128i a)
#include <emmintrin.h>
Instruction: pmovmskb r32, xmm
CPUID Flags: SSE2
Description
Create mask from the most significant bit of each 8-bit element in a, and store the result in dst.
Operation
FOR j := 0 to 15
	i := j*8
	dst[j] := a[i+7]
ENDFOR
dst[MAX:16] := 0


! NO 512 counterpart !


int _mm_movemask_ps (__m128 a)
Synopsis
int _mm_movemask_ps (__m128 a)
#include <xmmintrin.h>
Instruction: movmskps r32, xmm
CPUID Flags: SSE
Description
Set each bit of mask dst based on the most significant bit of the corresponding packed single-precision (32-bit) floating-point element in a.
Operation
FOR j := 0 to 3
	i := j*32
	IF a[i+31]
		dst[j] := 1
	ELSE
		dst[j] := 0
	FI
ENDFOR
dst[MAX:4] := 0


! NO 512 counterpart !


__m128 _mm_castsi128_ps (__m128i a)
#include <emmintrin.h>
CPUID Flags: SSE2
Description
Cast vector of type __m128i to type __m128. This intrinsic is only used for compilation and does not generate any instructions, thus it has zero latency.


__m512 _mm512_castsi512_ps (__m512i a)
#include <immintrin.h>
CPUID Flags: AVX512F for AVX-512, KNCNI for KNC
Description
Cast vector of type __m512i to type __m512. This intrinsic is only used for compilation and does not generate any instructions, thus it has zero latency.


__m128i _mm_bsrli_si128 (__m128i a, int imm8)
#include <emmintrin.h>
Instruction: psrldq xmm, imm8
CPUID Flags: SSE2
Description
Shift a right by imm8 bytes while shifting in zeros, and store the results in dst.
Operation
tmp := imm8[7:0]
IF tmp > 15
	tmp := 16
FI
dst[127:0] := a[127:0] >> (tmp*8)


__m128i _mm_bslli_si128 (__m128i a, int imm8)
#include <emmintrin.h>
Instruction: pslldq xmm, imm8
CPUID Flags: SSE2
Description
Shift a left by imm8 bytes while shifting in zeros, and store the results in dst.
Operation
tmp := imm8[7:0]
IF tmp > 15
	tmp := 16
FI
dst[127:0] := a[127:0] << (tmp*8)


int _bit_scan_forward (int a)
#include <immintrin.h>
Instruction: bsf r32, r32
Description
Set dst to the index of the lowest set bit in 32-bit integer a. If no bits are set in a then dst is undefined.
Operation
tmp := 0
IF a == 0
	// dst is undefined
ELSE
	DO WHILE ((tmp < 32) AND a[tmp] == 0)
		tmp := tmp + 1
	OD
FI
dst := tmp
*/


// https://github.com/viralshahrf/DatabaseSystemImplementation/blob/master/searchSIMDTree.c
/*
#include <stdio.h>
#include <stdlib.h>
#include <x86intrin.h>
#include <xmmintrin.h> // SSE
#include <emmintrin.h> // SSE2
#include <pmmintrin.h> // SSE3
#include <tmmintrin.h> // SSSE3
#include <smmintrin.h> // SSE4.1
#include <nmmintrin.h> // SSE4.2
#include <ammintrin.h> // SSE4A

int searchSIMDTree(int32_t **tree, int *fanout, int levels, int32_t value) {
    int iLevel = 0;
    int lOffset = 0;
    int pOffset = 0;
    int32_t cmpmask = 0;
    int32_t eqmask = 0;

     __m128i key = _mm_cvtsi32_si128(value);
    key = _mm_shuffle_epi32(key, _MM_SHUFFLE(0,0,0,0));

    while (iLevel < levels) {
        int f = fanout[iLevel];
        pOffset = lOffset;
        lOffset *= f - 1;
        int iter = 0;
        int position = 0;
        while (iter < f/4) {
            __m128i delimiters = _mm_load_si128((__m128i const*)&tree[iLevel][lOffset + iter*4]);
            __m128i compare = _mm_cmpgt_epi32(key, delimiters);
            cmpmask = _mm_movemask_ps(_mm_castsi128_ps(compare));
            cmpmask ^= 0x0F;
            if (cmpmask) {
                position = _bit_scan_forward(cmpmask);
                break;
            }
            iter++;
        }
        int offset = lOffset + iter*4 + position;
        lOffset = offset + pOffset;
        iLevel++;
    }
    return lOffset;
}
*/

// https://patchwork.ozlabs.org/project/gcc/patch/20170407144431.GA1914@tucnak/
/*
+_mm512_mask2int (__mmask16 __M)
+{
+  return (int) __M;
+}

> +static void
> +avx512f_test (void)
> +{
> +  __m512i a
> +    = _mm512_set_epi32 (1, 17, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16);
> +  __m512i b
> +    = _mm512_set_epi32 (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16);
> +  __mmask16 c = _mm512_cmpgt_epu32_mask (a, b);
> +  if (_mm512_mask2int (c) != 0xc000)
> +    __builtin_abort ();
> +}
*/

