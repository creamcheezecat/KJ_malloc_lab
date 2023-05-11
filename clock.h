/* Routines for using cycle counter 
   주기 카운터 사용 루틴*/

/* Start the counter */
void start_counter();

/* Get # cycles since counter started */
double get_counter();

/* Measure overhead for counter */
/* 카운터의 오버헤드 측정 */
double ovhd();

/* Determine clock rate of processor (using a default sleeptime) */
/* 프로세서의 클럭 속도 결정(기본 휴면 시간 사용) */
double mhz(int verbose);

/* Determine clock rate of processor, having more control over accuracy */
/* 프로세서의 클럭 속도를 결정하여 정확도를 더 많이 제어할 수 있음 */
double mhz_full(int verbose, int sleeptime);

/** Special counters that compensate for timer interrupt overhead */\
/** 타이머 인터럽트 오버헤드를 보상하는 특수 카운터 */
void start_comp_counter();

double get_comp_counter();
