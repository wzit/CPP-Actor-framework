#ifndef __CHECK_ACTOR_STACK_H
#define __CHECK_ACTOR_STACK_H

#define kB	*1024

#ifdef WIN32
#define STACK_BLOCK_SIZE	(64 kB)
#elif __linux__
#define STACK_BLOCK_SIZE	(32 kB)
#endif

//Ĭ�϶�ջ
#define DEFAULT_STACKSIZE	(STACK_BLOCK_SIZE - STACK_RESERVED_SPACE_SIZE)

#define TRY_SIZE(__s__) (0x80000000 | (__s__))
#define IS_TRY_SIZE(__s__) (0x80000000 & (__s__))
#define GET_TRY_SIZE(__s__) (0x1FFFFF & (__s__))

#if (_DEBUG || DEBUG)
#define STACK_SIZE(__debug__, __release__) (__debug__)
#define STACK_SIZE_REL(__release__) DEFAULT_STACKSIZE
#else
#define STACK_SIZE(__debug__, __release__) (__release__)
#define STACK_SIZE_REL(__release__) (__release__)
#endif
//////////////////////////////////////////////////////////////////////////

#if (_DEBUG || DEBUG)
#	if (defined _WIN64) || (defined __x86_64__)
#define STACK_SIZE64(__debug32__, __debug64__, __release32, __release64__) (__debug64__)
#	else
#define STACK_SIZE64(__debug32__, __debug64__, __release32, __release64__) (__debug32__)
#	endif
#else
#	if (defined _WIN64) || (defined __x86_64__)
#define STACK_SIZE64(__debug32__, __debug64__, __release32, __release64__) (__release64__)
#define STACK_SIZE_REL64(__release32__, __release64__) (__release64__)
#	else
#define STACK_SIZE64(__debug32__, __debug64__, __release32, __release64__) (__release32)
#define STACK_SIZE_REL64(__release32__, __release64__) (__release32__)
#	endif
#endif
//////////////////////////////////////////////////////////////////////////

//��ջ��Ԥ���ռ䣬����ջ���
#if (_DEBUG || DEBUG)
#		if (defined _WIN64) || (defined __x86_64__)
#define STACK_RESERVED_SPACE_SIZE		(24 kB)
#		else
#define STACK_RESERVED_SPACE_SIZE		(16 kB)
#		endif
#	else
#		if (defined _WIN64) || (defined __x86_64__)
#define STACK_RESERVED_SPACE_SIZE		(16 kB)
#		else
#define STACK_RESERVED_SPACE_SIZE		(8 kB)
#		endif
#	endif

//ҳ���С
#define MEM_PAGE_SIZE					(4 kB)

//ջ״̬�����ռ�
#define CORO_CONTEXT_STATE_SPACE	(1 * MEM_PAGE_SIZE)

#endif