/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 * 
 * mm-naive.c - 가장 빠르지만 메모리 사용량이 적은 malloc 패키지입니다.
 * 이 단순한 방법에서는 블록은 brk 포인터를 증가시켜 할당됩니다.
 * 블록은 순수한 페이로드(block의 데이터)입니다. 헤더나 푸터가 없습니다.
 * 블록은 결합되거나 재사용되지 않습니다.
 * realloc은 mm_malloc과 mm_free를 직접 구현하여 사용됩니다.
 * 
 * NOTE TO STUDENTS: 이 헤더 주석을 자신의 솔루션에 대한 높은 수준의 
 * 설명을 제공하는 자신의 헤더 주석으로 대체하십시오.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 * 
 * NOTE TO STUDENTS: 무언가를 하기 전에 다음 구조체에 팀 정보를 제공하십시오.
 ********************************************************/
team_t team = {
    /* Team name */
    "Green 1 team",
    /* First member's full name */
    "Lee Yang Sang",
    /* First member's email address */
    "ysl960330@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))


/*
1<<12는 비트 연산자인 시프트(Shift) 연산자를 사용하여, 2의 12승을 계산하는 방법입니다.
<<는 좌측 시프트 연산자로, 왼쪽의 피연산자(1)를 2진수로 표현했을 때 각 비트를 지정한 수만큼 왼쪽으로 이동시키는 연산을 수행합니다.
따라서 1<<12는 0001을 왼쪽으로 12비트 이동시켜서 0001 0000 0000 0000을 만드는 것이며, 이진수로 표현하면 2의 12승인 4096이 됩니다.
따라서 #define CHUNKSIZE (1<<12)는 CHUNKSIZE를 4096으로 정의하는 것입니다.
*/

/* start */
/* 가용 리스트 조작을 위한 기본 상수 및 매크로 정의 */
/* 기본 상수와 매크로*/
#define WSIZE 4     /*워드 크기*/
#define DSIZE 8     /*더블 워드 크기*/

#define CHUNKSIZE (1<<12)/*초기 가용 블록과 힙 확장을 위한 기본 크기*/

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/*크기와 할당된 비트를 하나의 워드(word)에 패킹
크기와 할당 비트를 통합해 헤더와 풋터에 저장할 수 있는 값을 리턴한다.*/
#define PACK(size, alloc) ((size) | (alloc))


/*
p는 대개 (void*) 이므로 직접적으로 역참조가 불가능 하다
(unsigned = 부호 비트를 제거해 저장 가능한 양수 범위를 두배로 늘이는 역할)
*/
/*인자 p가 참조하는 워드를 읽어서 리턴한다.*/
#define GET(p)          (*(unsigned int *)(p))
/*인자 p가 가리키는 워드에 val을 저장한다.*/
#define PUT(p, val)     (*(unsigned int *)(p) = (val))

/*p 주소에서 크기와 할당된 필드를 읽어들입니다.
각각 주소 p에 있는 헤더 또는 풋터의 size를 리턴한다*/
#define GET_SIZE(p)     (GET(p) & ~0x7)
/*p 주소에서 크기와 할당된 필드를 읽어들입니다.
각각 주소 p에 있는 헤더 또는 풋터의 할당 비트를 리턴한다*/
#define GET_ALLOC(p)    (GET(p) & 0x1)

/*주어진 블록 포인터 bp에 대해, 해당 블록의 헤더의 주소를 계산*/
#define HDRP(bp)        ((char *)(bp) - WSIZE)
/*주어진 블록 포인터 bp에 대해, 해당 블록의 풋터의 주소를 계산*/
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
/*주어진 블록 포인터 bp에 대해, 해당 블록의 이전 블록 주소를 계산*/
#define PRED(bp)        ((char *)(bp))
/*주어진 블록 포인터 bp에 대해, 해당 블록의 다음 블록 주소를 계산*/
#define SUCC(bp)        ((char *)(bp) + WSIZE)
/*주어진 블록 포인터(bp)를 이용하여 다음 블록의 주소를 계산*/
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
/*주어진 블록 포인터(bp)를 이용하여 이전 블록의 주소를 계산*/
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

// 블록 최소 크기인 2**4부터 최대 크기인 2**32를 위한 리스트 29개
#define LIST_NUM 29
// 분리 가용 리스트 생성
void *seg_list[LIST_NUM];

void delete_block(char* bp);

void add_free_block(char* bp);

int get_seg_list_num(size_t size);
/* 
heap_listp 변수는 포인터 변수로, 할당된 힙 메모리 블록의 시작 주소를 가리켜야 합니다. 
따라서, void* 자료형을 사용하여 선언하는 것이 적절합니다.

void* 자료형은 어떤 타입의 주소도 가리킬 수 있는 범용 포인터이기 때문에 
heap_listp 변수가 가리키는 메모리 블록이 어떤 타입인지에 대해서는 신경쓰지 않아도 됩니다. 
따라서 heap_listp 변수를 void* 자료형으로 선언하여
포인터가 가리키는 메모리 블록의 주소를 저장하도록 하는 것이 좋습니다.

전역 변수로 사용 해야하는 이유
find_fit()함수에서 힙을 불러와야하는데 find_fit()을 사용하는 mm_malloc()에서 힙을 안 갖고있기때문에
인자로 넘겨 받을 수가 없다. mm_malloc()은 할당해야하는 사이즈만 인자로 가지고 있다.

위 이유는 그냥 갖다 붙이는 이유고 
책에서는 프롤로그와 에필로그 블록들은 연결과정 동안에 가장자리 조건을 없애주기 위한 속임수로 
할당기는 한개의 정적(static) 전역변수를 사용하며 이것은 항상 프롤로그 블록을 가르킨다
 */
static void *heap_listp;

static void *extend_heap(size_t words);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void *coalesce(void *bp);
/* end */

/* 새 가용 블록으로 힙 확장하기 */
static void *extend_heap(size_t words)
{
    char * bp;
    size_t size;

    /*정렬을 유지하기 위해 단어 수를 짝수로 할당*/
    size = (words % 2) ? (words + 1) * DSIZE : words * DSIZE;
    if((long)(bp = mem_sbrk(size)) == -1){
        return NULL;
    }

    /*프리 블록의 헤더(header)와 푸터(footer) 및 에필로그(epilogue) 헤더를 초기화*/
    PUT(HDRP(bp), PACK(size, 0));           /* Free block header */ /* 새 가용 블록 헤더 */
    PUT(FTRP(bp), PACK(size, 0));           /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0, 1));    /* New epilogue header */ /* 새 에필로그 블록 헤더*/

    /*이전 블록이 프리 블록(free block)인 경우, 병합(coalesce)을 수행*/
    return coalesce(bp);
}



/* 
 * mm_init - initialize the malloc package.
 * mm_init - malloc 패키지를 초기화합니다.
 */
int mm_init(void)
{
    /* 최초 가용 블록으로 힙 생성 */
    for(int i = 0; i < LIST_NUM ; i++){
        seg_list[i] = NULL;
    }

    if ((heap_listp = mem_sbrk(4 * WSIZE))==(void*)-1){
        return -1;
    }

    PUT(heap_listp, 0);                             /* Alignment padding */
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE,1));   /* Prologue header */
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE,1));   /* Prologue footer */
    PUT(heap_listp + (3 * WSIZE), PACK(0,1));       /* Epilogue header */

    /*빈 힙에 CHUNKSIZE 바이트의 빈 블록을 추가*/
    if(extend_heap(CHUNKSIZE/DSIZE)== NULL){
        return -1;
    }

    return 0;
}

/*
size 바이트의 메모리 블록을 요청할떄 추가적인 요청들을 체크한 후에 
할당기는 요청한 블록 크기를 조절해서 헤더와 풋터를 위한 공간을 확보하고,
 더블 워드 요건을 만족시킨다. 
 */

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 * mm_malloc - 블록을 할당하기 위해 brk 포인터를 증가시킵니다
 *      항상 정렬의 배수인 크기의 블록을 할당합니다.
 */
void *mm_malloc(size_t size)
{
    /* 가용 리스트에서 블록할당 하기 */
    size_t asize;
    size_t extendsize;
    char *bp;

    if(size == 0){
        return NULL;
    }
    /*
    최소 16 바이트 크기의 블록을 구성한다 
    : 8 바이트는 정렬 요건을 만족시키기 위해 추가적인 
    8바이트는 헤더와 풋터 오버헤드를 위해 8바이트를 넘기는 요청에 대해서는 
    else 문에 구성 처럼 일반적인 규칙은 오버헤드 바이트를 추가하고, 
    인접 8의 배수로 반올림 하는 것이다.
    */
    if(size <= DSIZE){
        asize = 2*DSIZE;
    }
    else{
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
    }

    /*
    할당기가 요청한 크기를 조정한 후에 적절한 가용 블록을 가용 리스트에서 검색한다
    만일 맞는 블록을 찾으면 할당기는 요청한 블록을 배치하고(if문 조건문 안에서 find_fit() 함수 부분), 
    옵션으로 초과부분을 분할라고(place 함수 부분), 새롭게 할당한 블록을 리턴한다.
    */
    if((bp = find_fit(asize)) != NULL){
        place(bp,asize);
        return bp;
    }

    /*
    할당기가 맞는 블록을 찾지 못하면, 힙을 새로운 가용 블록으로 확장하고,
    요청한 블록을 이 새 가용 블록에 배치하고, 필요한 경우에 블록을 분할하며(place 함수 부분),
    이후에 새롭게 할당한 블록의 포인터를 리턴한다.
    */
    extendsize = MAX(asize,CHUNKSIZE);
    if ((bp = extend_heap(extendsize/DSIZE)) == NULL){
        return NULL;
    }
    place(bp,asize);
    return bp;
    /* 
    // 기본으로 주어지는 코드 
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
    */
}

/* 동적 메모리 할당 시 메모리 블록을 찾는 함수 */
static void *find_fit(size_t asize)
{
    void *search_p;
    //asize가 들어갈 수 있는 seg_list 찾기
    int i = get_seg_list_num(asize);
    
    //리스트 내부의 블록들 중 가장 작은 블록 할당(best-fit)
    void *tmp = NULL;
    // 모든 seg_list에서 검색합니다.
    while (i < LIST_NUM){  
        for (search_p = seg_list[i]; search_p != NULL; search_p = (void*)GET(SUCC(search_p))){
            if (GET_SIZE(HDRP(search_p)) >= asize){// 요청한 크기(asize) 이상인 블록을 찾습니다.
                if (tmp == NULL){// 처음으로 찾은 블록이라면, tmp 변수에 할당합니다.
                    tmp = search_p;
                }/*이미 찾은 블록(tmp)이 있다면, 
                이번에 찾은 블록(search_p)과 비교하여 더 작은 블록을 tmp에 할당합니다.*/
                else {
                    if (GET_SIZE(tmp) > GET_SIZE(HDRP(search_p))){
                        tmp = search_p;
                    }
                }
            }
        }// 찾은 블록이 있다면, 해당 블록을 리턴합니다.
        if (tmp != NULL){
            return tmp;
        }
        i ++;
    }

    return NULL;
}

/* 가용 블록 중에서 요청한 크기(asize)에 따라 적절한 크기의 블록을 할당하는 함수 */
static void place(void *bp, size_t asize)
{
   delete_block(bp);
    size_t old_size = GET_SIZE(HDRP(bp));
    if (old_size >= asize + (2 * DSIZE)){
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        
        PUT(HDRP(NEXT_BLKP(bp)), PACK((old_size - asize), 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK((old_size - asize), 0));
        
        coalesce(NEXT_BLKP(bp));
    }
    else {
        PUT(HDRP(bp), PACK(old_size, 1));
        PUT(FTRP(bp), PACK(old_size, 1));
    }
}

/*
 * mm_free - Freeing a block does nothing.
 * mm_free - 블록을 해제하면 아무것도 하지 않습니다.
 */
void mm_free(void *bp)
{
    /* 블록을 반환하고 경계 태그 연결을 사용해서 
    상수 시간에 인접 가용 블록들과 통합 */
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    coalesce(bp);
}

static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && !next_alloc) {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        
        delete_block(NEXT_BLKP(bp));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc) {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));

        delete_block(PREV_BLKP(bp));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else if (!prev_alloc && !next_alloc){
        size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + GET_SIZE(HDRP(PREV_BLKP(bp)));

        delete_block(PREV_BLKP(bp));
        delete_block(NEXT_BLKP(bp));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    add_free_block(bp);
    return bp;
}

/*
1. 입력으로 받은 bp 주소를 oldptr에 저장하고, 
    새롭게 할당할 메모리 주소를 newptr에 저장합니다.
2. 만약 size가 0이면, 
    free 함수를 호출하여 oldptr을 free()하고 NULL을 반환합니다.???
3. bp가 NULL이면, 
    malloc 함수를 호출하여 size 크기의 메모리를 할당하고 해당 주소를 반환합니다.
4. newptr이 NULL을 반환하면, 
    할당 실패로 처리합니다.
5. 그 외의 경우, oldptr의 크기를 계산하고, 
    size와 비교하여 copySize에 저장합니다.
6. newptr로 copySize 만큼 데이터를 복사합니다.
7. oldptr을 free()하고 newptr을 반환합니다. ??
*/


/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 * mm_realloc - 단순히 mm_malloc과 mm_free를 이용해 구현됩니다.
 */
void *mm_realloc(void *bp, size_t size)
{
    void *oldptr = bp;
    void *newptr;
    size_t copySize;

    /* size가 0이면 free와 같음 */
    if(size == 0){
        mm_free(bp);
        return NULL;
    }

    /* bp이 NULL이면 malloc과 같음 */
    if(bp == NULL){
        return mm_malloc(size);
    }

    newptr = mm_malloc(size);

    if(newptr == NULL){
        return NULL;
    }
    // 복사할 데이터 크기 결정
    copySize = GET_SIZE(HDRP(oldptr)) - DSIZE;
    if(size < copySize){
        copySize = size;
    }
    // 기존 블록에서 새로운 블록으로 데이터 복사
    memcpy(newptr, oldptr, copySize);
    // 기존 블록 free
    mm_free(oldptr);
    // 새로운 블록 주소 반환
    return newptr;

    /*
    // 기본으로 주어지는 코드
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
    */
}
/* free list에서 블록 포인터 bp를 삭제하는 함수 */
void delete_block(char* bp)
{ 
    // 블록이 들어가야 하는 seg_list 찾기
    int seg_list_num = get_seg_list_num(GET_SIZE(HDRP(bp)));
    // bp가 첫 번째 블록일 경우
    if (GET(PRED(bp)) == (unsigned int)NULL){
        // bp 다음 블록이 없는 경우
        if (GET(SUCC(bp)) == (unsigned int)NULL){
            // 해당 리스트의 첫 번째 블록을 NULL로 설정
            seg_list[seg_list_num] = NULL;
        } else {
            // 다음 블록의 이전 블록을 NULL로 설정
            PUT(PRED(GET(SUCC(bp))), (unsigned int)NULL);
            // 해당 리스트의 첫 번째 블록을 다음 블록으로 설정
            seg_list[seg_list_num] = (void *)GET(SUCC(bp));
        }
    }
    // bp가 첫 번째 블록이 아닌 경우
    else {
        // bp 다음 블록이 없는 경우
        if (GET(SUCC(bp)) == (unsigned int)NULL){
            // 이전 블록의 다음 블록을 NULL로 설정
            PUT(SUCC(GET(PRED(bp))), (unsigned int)NULL);
        } else {
            // 다음 블록의 이전 블록을 bp의 이전 블록으로 설정
            PUT(PRED(GET(SUCC(bp))), GET(PRED(bp)));
            // 이전 블록의 다음 블록을 bp의 다음 블록으로 설정
            PUT(SUCC(GET(PRED(bp))), GET(SUCC(bp)));
        }
    }
}
/* free list에 블록 포인터 bp를 추가하는 함수 */
void add_free_block(char* bp)
{
    //들어가야 하는 seg_list 찾고 그 seg_list에 추가
    int seg_list_num = get_seg_list_num(GET_SIZE(HDRP(bp)));
    // 해당 seg_list가 비어있는 경우
    if (seg_list[seg_list_num] == NULL){
        // bp의 이전 블록을 NULL로 설정
        PUT(PRED(bp), (unsigned int)NULL);
        PUT(SUCC(bp), (unsigned int)NULL);
    } else {
        // bp의 이전 블록을 NULL로 설정
        PUT(PRED(bp), (unsigned int)NULL);
        // bp의 다음 블록을 해당 seg_list의 첫 번째 블록으로 설정
        PUT(SUCC(bp), seg_list[seg_list_num]);
        // 해당 seg_list의 첫 번째 블록의 이전 블록을 bp로 설정
        PUT(PRED(seg_list[seg_list_num]), bp);
    }
    // 해당 seg_list의 첫 번째 블록을 bp로 설정
    seg_list[seg_list_num] = bp;
}
/* size에 맞는 seg_list의 인덱스를 반환하는 함수 */
int get_seg_list_num(size_t size)
{
    //seg_list[0]은 블록의 최소 크기인 2**4를 위한 리스트 
    int i = -4;
    while (size != 1){
        size = (size >> 1);
        i ++;
    }
    return i;
}