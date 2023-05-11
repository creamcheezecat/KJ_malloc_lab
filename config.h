#ifndef __CONFIG_H_
#define __CONFIG_H_

/*
 * config.h - malloc lab configuration file
 *                          구성
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * May not be used, modified, or copied without permission.
 */

/*
 * This is the default path where the driver will look for the
 * default tracefiles. You can override it at runtime with the -t flag.
 * 이것은 드라이버가 기본 추적 파일을 찾을 위치입니다.
 * -t 플래그로 런타임에 재정의할 수 있습니다.
 */
#define TRACEDIR "./traces/"

/*
 * This is the list of default tracefiles in TRACEDIR that the driver
 * will use for testing. Modify this if you want to add or delete
 * traces from the driver's test suite. For example, if you don't want
 * your students to implement realloc, you can delete the last two
 * traces.
 * 드라이버가 테스트에 사용할 기본 추적 파일 목록입니다.
 * 드라이버의 테스트 스위트에서 추적 파일을 추가하거나 삭제하려면
 * 이것을 수정하십시오. 예를 들어, realloc을 구현하지 않도록 하려면
 * 마지막 두 추적 파일을 삭제할 수 있습니다.
 */
#define DEFAULT_TRACEFILES \
  "amptjp-bal.rep",\
  "cccp-bal.rep",\
  "cp-decl-bal.rep",\
  "expr-bal.rep",\
  "coalescing-bal.rep",\
  "random-bal.rep",\
  "random2-bal.rep",\
  "binary-bal.rep",\
  "binary2-bal.rep",\
  "realloc-bal.rep",\
  "realloc2-bal.rep"

/*
 * This constant gives the estimated performance of the libc malloc
 * package using our traces on some reference system, typically the
 * same kind of system the students use. Its purpose is to cap the
 * contribution of throughput to the performance index. Once the
 * students surpass the AVG_LIBC_THRUPUT, they get no further benefit
 * to their score.  This deters students from building extremely fast,
 * but extremely stupid malloc packages.
 * 이 상수는 libc malloc 패키지의 성능을 추정합니다.
 * 참조 시스템 (일반적으로 학생이 사용하는 시스템과 동일한 종류)에서
 * 우리의 추적에 대해 사용됩니다. 이 상수는 처리량 기여도를 제한합니다.
 * 학생이 AVG_LIBC_THRUPUT을 초과하면 더 이상 점수가 상승하지 않습니다.
 * 이것은 학생이 극단적으로 빠르지만 극단적으로 어리석은 malloc 패키지를 만드는 것을 방지합니다.
 */
#define AVG_LIBC_THRUPUT      600E3  /* 600 Kops/sec */

 /* 
  * This constant determines the contributions of space utilization
  * (UTIL_WEIGHT) and throughput (1 - UTIL_WEIGHT) to the performance
  * index. 
  * 이 상수는 공간 활용 (UTIL_WEIGHT)과 처리량 (1 - UTIL_WEIGHT)의 기여도를 결정합니다.
  * 성능 지수에 대한 영향을 미칩니다.
  */
#define UTIL_WEIGHT .60

/* 
 * Alignment requirement in bytes (either 4 or 8) 
 */
#define ALIGNMENT 8  

/* 
 * Maximum heap size in bytes 
 * 바이트 단위의 정렬 요구 사항 (4 또는 8 중 하나)
 */
#define MAX_HEAP (20*(1<<20))  /* 20 MB */

/*****************************************************************************
 * Set exactly one of these USE_xxx constants to "1" to select a timing method
 * 다음 USE_xxx 상수 중 하나만 "1"로 설정하여 타이밍 방법을 선택하십시오.
 *****************************************************************************/
#define USE_FCYC   0   /* cycle counter w/K-best scheme (x86 & Alpha only) */
#define USE_ITIMER 0   /* interval timer (any Unix box) */
#define USE_GETTOD 1   /* gettimeofday (any Unix box) */

#endif /* __CONFIG_H */
