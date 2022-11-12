# 系统调用
## glic
```c
int open(const char *pathname, int flags, mode_t mode)
源码:
syscall.list 文件  列举着 glibc 对应的系统调用
# File name	Caller	Syscall name	Args	Strong name	Weak names
    open		-	open		Ci:siv	__libc_open __open open

make-syscall.sh 对每个封装好系统调用生成一个文件

sysdep/unix/sysdep.h

#define	SYSCALL(name, args)	PSEUDO (name, name, args)

sysdep/unix/syscall.S

#include <sysdep.h>

#ifndef SYS_syscall
#define SYS_syscall	0
#endif


SYSCALL (syscall, 1)
	ret
PSEUDO_END(syscall)


//
SYSCALL (syscall, 1)
	ret
PSEUDO_END(syscall)

相当于

PSEUDO (syscall, syscall, args)
	ret
PSEUDO_END(syscall)



// PSEUDO
//i386
#undef	PSEUDO
#define	PSEUDO(name, syscall_name, args)				      \
  .text;								      \
  ENTRY (name)								      \
    DO_CALL (syscall_name, args);					      \
    cmpl $-4095, %eax;							      \
    jae SYSCALL_ERROR_LABEL;						      \
  L(pseudo_end):


//ia64
#undef PSEUDO
#define	PSEUDO(name, syscall_name, args)	\
  ENTRY(name)					\
    DO_CALL (SYS_ify(syscall_name));		\
	cmp.eq p6,p0=-1,r10;			\
(p6)	br.cond.spnt.few __syscall_error;


```
### 32 位的系统调用 DO_CALL
```c
/* Linux takes system call arguments in registers:

	syscall number	%eax	     call-clobbered 系统调用号码
	arg 1		%ebx	     call-saved
	arg 2		%ecx	     call-clobbered
	arg 3		%edx	     call-clobbered
	arg 4		%esi	     call-saved
	arg 5		%edi	     call-saved

   The stack layout upon entering the function is:

	20(%esp)	Arg# 5
	16(%esp)	Arg# 4
	12(%esp)	Arg# 3
	 8(%esp)	Arg# 2
	 4(%esp)	Arg# 1
	  (%esp)	Return address
*/
// linux i386
#undef	DO_CALL
#define DO_CALL(syscall_name, args)			      		      \
    PUSHARGS_##args							      \
    DOARGS_##args							      \
    movl $SYS_ify (syscall_name), %eax;					      \
    ENTER_KERNEL							      \
    POPARGS_##args

// linux i386 最初越大时 int 0x80 软中断号
#ifdef I386_USE_SYSENTER
# ifdef SHARED
#  define ENTER_KERNEL call *%gs:SYSINFO_OFFSET
# else
#  define ENTER_KERNEL call *_dl_sysinfo
# endif
#else
# define ENTER_KERNEL int $0x80 //
#endif



# define ENTER_KERNEL int $0x80 //
int $0x80 
软中断使当前线程由内核接管

```