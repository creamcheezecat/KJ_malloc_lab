/* 
 * clock.c - Routines for using the cycle counters on x86, 
 *           Alpha, and Sparc boxes.
 * clock.c - x86에서 주기 카운터를 사용하기 위한 루틴,
 *            Alpha 및 Sparc 상자.
 * 
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * May not be used, modified, or copied without permission.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include "clock.h"


/******************************************************* 
 * Machine dependent functions 
 *
 * Note: the constants __i386__ and  __alpha
 * are set by GCC when it calls the C preprocessor
 * You can verify this for yourself using gcc -v.
 *
 * 기계 종속 기능
 * 
 * 참고: 상수 __i386__ 및 __alpha
 * C 전처리기를 호출할 때 GCC에 의해 설정됨
 * gcc -v를 사용하여 직접 확인할 수 있습니다.
 *******************************************************/

#if defined(__i386__)  
/*******************************************************
 * Pentium versions of start_counter() and get_counter()
 * start_counter() 및 get_counter()의 펜티엄 버전
 *******************************************************/


/* $begin x86cyclecounter */
/* Initialize the cycle counter */
/* $begin x86주기 카운터 */
/* 주기 카운터 초기화 */
static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;


/* Set *hi and *lo to the high and low order bits  of the cycle counter.  
   Implementation requires assembly code to use the rdtsc instruction. */
/* *hi 및 *lo를 사이클 카운터의 상위 및 하위 비트로 설정합니다.
   구현에는 rdtsc 명령어를 사용하기 위한 어셈블리 코드가 필요합니다. */
void access_counter(unsigned *hi, unsigned *lo)
{
    asm("rdtsc; movl %%edx,%0; movl %%eax,%1"   /* Read cycle counter */
	: "=r" (*hi), "=r" (*lo)                /* and move results to */
	: /* No input */                        /* the two outputs */
	: "%edx", "%eax");
}

/* Record the current value of the cycle counter. */
/* 사이클 카운터의 현재 값을 기록합니다. */
void start_counter()
{
    access_counter(&cyc_hi, &cyc_lo);
}

/* Return the number of cycles since the last call to start_counter. */
/* start_counter에 대한 마지막 호출 이후 사이클 수를 반환합니다. */
double get_counter()
{
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    /* Get cycle counter */
    access_counter(&ncyc_hi, &ncyc_lo);

    /* Do double precision subtraction */
    /* 배 정밀도 빼기 수행 */
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double) hi * (1 << 30) * 4 + lo;
    if (result < 0) {
	fprintf(stderr, "Error: counter returns neg value: %.0f\n", result);
    }
    return result;
}
/* $end x86cyclecounter */

#elif defined(__alpha)

/****************************************************
 * Alpha versions of start_counter() and get_counter()
 * start_counter() 및 get_counter()의 알파 버전
 ***************************************************/

/* Initialize the cycle counter */
static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;


/* Use Alpha cycle timer to compute cycles.  Then use
   measured clock speed to compute seconds 
   Alpha 주기 타이머를 사용하여 주기를 계산합니다. 그런 다음 사용
   초를 계산하기 위해 측정된 클럭 속도
*/

/*
 * counterRoutine is an array of Alpha instructions to access 
 * the Alpha's processor cycle counter. It uses the rpcc 
 * instruction to access the counter. This 64 bit register is 
 * divided into two parts. The lower 32 bits are the cycles 
 * used by the current process. The upper 32 bits are wall 
 * clock cycles. These instructions read the counter, and 
 * convert the lower 32 bits into an unsigned int - this is the 
 * user space counter value.
 * NOTE: The counter has a very limited time span. With a 
 * 450MhZ clock the counter can time things for about 9 
 * seconds. */
/*
 * counterRoutine은 액세스하기 위한 Alpha 명령의 배열입니다.
 * Alpha의 프로세서 사이클 카운터. rpcc를 사용합니다.
 * 카운터에 액세스하라는 지시. 이 64비트 레지스터는
 * 두 부분으로 나뉩니다. 하위 32비트는 주기입니다.
 * 현재 프로세스에서 사용됩니다. 상위 32비트는 벽
 * 클럭 사이클. 이 명령은 카운터를 읽고
 * 하위 32비트를 unsigned int로 변환 - 이것은
 * 사용자 공간 카운터 값.
 * 참고: 카운터의 시간 범위는 매우 제한되어 있습니다. 로
 * 450MhZ 시계 카운터는 약 9분 동안 시간을 ​​측정할 수 있습니다.
 * 초 */
static unsigned int counterRoutine[] =
{
    0x601fc000u,
    0x401f0000u,
    0x6bfa8001u
};

/* Cast the above instructions into a function. */\
/* 위의 명령을 함수로 변환합니다. */
static unsigned int (*counter)(void)= (void *)counterRoutine;


void start_counter()
{
    /* Get cycle counter */
    cyc_hi = 0;
    cyc_lo = counter();
}

double get_counter()
{
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;
    ncyc_lo = counter();
    ncyc_hi = 0;
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double) hi * (1 << 30) * 4 + lo;
    if (result < 0) {
	fprintf(stderr, "Error: Cycle counter returning negative value: %.0f\n", result);
    }
    return result;
}

#else

/****************************************************************
 * All the other platforms for which we haven't implemented cycle
 * counter routines. Newer models of sparcs (v8plus) have cycle
 * counters that can be accessed from user programs, but since there
 * are still many sparc boxes out there that don't support this, we
 * haven't provided a Sparc version here.
 * 주기를 구현하지 않은 다른 모든 플랫폼
 * 카운터 루틴. sparcs(v8plus)의 최신 모델에는 주기가 있습니다.
 * 사용자 프로그램에서 액세스할 수 있는 카운터
 * 아직 이를 지원하지 않는 많은 sparc 상자가 있습니다.
 * 여기에 Sparc 버전을 제공하지 않았습니다.
 ***************************************************************/

void start_counter()
{
    printf("ERROR: You are trying to use a start_counter routine in clock.c\n");
    printf("that has not been implemented yet on this platform.\n");
    printf("Please choose another timing package in config.h.\n");
    exit(1);
}

double get_counter() 
{
    printf("ERROR: You are trying to use a get_counter routine in clock.c\n");
    printf("that has not been implemented yet on this platform.\n");
    printf("Please choose another timing package in config.h.\n");
    exit(1);
}
#endif




/*******************************
 * Machine-independent functions
 * 기계 독립적인 기능
 ******************************/
double ovhd()
{
    /* Do it twice to eliminate cache effects */
    /* 캐시 효과를 제거하기 위해 두 번 수행 */
    int i;
    double result;

    for (i = 0; i < 2; i++) {
	start_counter();
	result = get_counter();
    }
    return result;
}

/* $begin mhz */
/* Estimate the clock rate by measuring the cycles that elapse */ 
/* while sleeping for sleeptime seconds */
/* $시작 mhz */
/* 경과 주기를 측정하여 클럭 속도를 추정 */
/* sleeptime 초 동안 자는 동안 */
double mhz_full(int verbose, int sleeptime)
{
    double rate;

    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6*sleeptime);
    if (verbose) 
	printf("Processor clock rate ~= %.1f MHz\n", rate);
    return rate;
}
/* $end mhz */

/* Version using a default sleeptime */
/* 기본 sleeptime을 사용하는 버전 */
double mhz(int verbose)
{
    return mhz_full(verbose, 2);
}

/** Special counters that compensate for timer interrupt overhead */
/** 타이머 인터럽트 오버헤드를 보상하는 특수 카운터 */
static double cyc_per_tick = 0.0;

#define NEVENT 100
#define THRESHOLD 1000
#define RECORDTHRESH 3000

/* Attempt to see how much time is used by timer interrupt */
/* 타이머 인터럽트에 의해 얼마나 많은 시간이 사용되었는지 확인 시도 */
static void callibrate(int verbose)
{
    double oldt;
    struct tms t;
    clock_t oldc;
    int e = 0;

    times(&t);
    oldc = t.tms_utime;
    start_counter();
    oldt = get_counter();
    while (e <NEVENT) {
	double newt = get_counter();

	if (newt-oldt >= THRESHOLD) {
	    clock_t newc;
	    times(&t);
	    newc = t.tms_utime;
	    if (newc > oldc) {
		double cpt = (newt-oldt)/(newc-oldc);
		if ((cyc_per_tick == 0.0 || cyc_per_tick > cpt) && cpt > RECORDTHRESH)
		    cyc_per_tick = cpt;
		/*
		  if (verbose)
		  printf("Saw event lasting %.0f cycles and %d ticks.  Ratio = %f\n",
		  newt-oldt, (int) (newc-oldc), cpt);
		*/
		e++;
		oldc = newc;
	    }
	    oldt = newt;
	}
    }
    if (verbose)
	printf("Setting cyc_per_tick to %f\n", cyc_per_tick);
}

static clock_t start_tick = 0;

void start_comp_counter() 
{
    struct tms t;

    if (cyc_per_tick == 0.0)
	callibrate(0);
    times(&t);
    start_tick = t.tms_utime;
    start_counter();
}

double get_comp_counter() 
{
    double time = get_counter();
    double ctime;
    struct tms t;
    clock_t ticks;

    times(&t);
    ticks = t.tms_utime - start_tick;
    ctime = time - ticks*cyc_per_tick;
    /*
      printf("Measured %.0f cycles.  Ticks = %d.  Corrected %.0f cycles\n",
      time, (int) ticks, ctime);
    */
    return ctime;
}

