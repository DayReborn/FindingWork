
#include <stdio.h>
#include <setjmp.h>


/**
 * @file setjmp_zzx.c
 * @brief Demonstrates the use of setjmp and longjmp for non-local jumps in C
 * This example shows how to use setjmp to save the current execution context
 * and longjmp to jump back to that context, effectively allowing for a form of
 * cooperative multitasking or error handling.
 */


jmp_buf env;  // 定义一个全局的跳转缓冲区

/** 
 * @brief 函数 func
 * @param arg 整数参数
 * @brief 该函数打印参数值，并使用 longjmp 跳转回 setjmp 保存的环境
*/
void func(int arg) {
    printf("func: %d\n", arg);
    longjmp(env, ++arg);  // 使用 longjmp 跳转回 setjmp 保存的环境
}

/**
 * @brief 主函数
 * @return 返回 0 表示程序正常结束
 * @brief 在主函数中调用 setjmp 保存当前执行环境，并根据返回值调用 func 函数
*/
int main() {


	int ret = setjmp(env); //
	if (ret == 0) {
		func(ret);
	} else if (ret == 1) {
		func(ret);
	} else if (ret == 2) {
		func(ret);
	} else if (ret == 3) {
		func(ret);
	}

	return 0;
}
