/* 
 * Function timers 
 */
typedef void (*ftimer_test_funct)(void *); 

/* Estimate the running time of f(argp) using the Unix interval timer.
   Return the average of n runs */
/* 유닉스 interval 타이머를 이용하여 f(argp)의 실행 시간을 추정합니다.
n 번 실행의 평균을 반환합니다. */
double ftimer_itimer(ftimer_test_funct f, void *argp, int n);


/* Estimate the running time of f(argp) using gettimeofday 
   Return the average of n runs */
/* gettimeofday를 이용하여 f(argp)의 실행 시간을 추정합니다.
n 번 실행의 평균을 반환합니다. */
double ftimer_gettod(ftimer_test_funct f, void *argp, int n);

