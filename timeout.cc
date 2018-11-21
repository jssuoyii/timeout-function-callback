#include "timeout.h"
#include "phpx.h"
#include <iostream>
using namespace php;
using namespace std;

extern "C"{
	#include <setjmp.h>
	#include <stdarg.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <signal.h>
	#include <string.h>
	#include <errno.h>
	#include <sys/time.h>
}

sigjmp_buf invoke_env;//设置跳转点

void timeout_signal_handler(int);//信号处理函数
typedef void (*Sigfunc)(int);//
Sigfunc timeout_set_signal(int signo, Sigfunc func);//信号设置函数

void timeout_signal_handler(int){
	siglongjmp(invoke_env, 1);	//触发跳转信号，跳转到
}

Sigfunc timeout_set_signal(int signo, Sigfunc func) {
    /*
     *注册信号处理相关信息
     * */
	struct sigaction act, oact;
	memset(&act, '\0', sizeof(struct sigaction));
	memset(&oact, '\0', sizeof(struct sigaction));
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if(signo == SIGALRM) {
    //设置信号中断后的系统调用处理行为， 默认是SA_INTERRUPT
	#ifdef SA_INTERRUPT
		//act.sa_flags |= SA_INTERRUPT;//不会重新调用
	#endif
	}else{
	#ifdef SA_RESTART
		//act.sa_flags |= SA_RESTART;//重新调用
	#endif
	}
	if (sigaction(signo, &act, &oact) < 0) {
		return SIG_ERR;
	}

	return oact.sa_handler;
	
}

PHPX_METHOD(Timeout, exec){
	Variant interval = args[1];//注册函数的参数，允许执行的时间

    int func_argv = args.count() - 2;
    Array func_args;
    if(func_argv > 0){
        for(int i=2; i<args.count(); i++){
            func_args.append(args[i]);
        }
    }else{
        func_args = NULL;
    }
	
//    for(int i=0;i<func_args.count();i++){
//        cout<<"i:"<<i<<" value:"<<func_args[i].toCString();
//    }
	int call_ret;
	
	Sigfunc sf = timeout_set_signal(SIGALRM, timeout_signal_handler);//调用信号注册及信号处理句柄
	
	if(sf == SIG_ERR){
		call_ret = TT_SET_SIGNAL_ERROR;
		goto END;
	}	
	struct itimerval tick;
	struct itimerval oldtick;

    //注册时钟信息
	tick.it_value.tv_sec = interval.toInt()/1000;
	tick.it_value.tv_usec = (interval.toInt()%1000) * 1000;
	tick.it_interval.tv_sec = interval.toInt()/1000;
	tick.it_interval.tv_usec = (interval.toInt()%1000) * 1000;
	if (setitimer(ITIMER_REAL, &tick, &oldtick) < 0) {//真实的系统时间，发出SIGALRM
		call_ret = TT_SET_TIMER_ERROR;
		goto END;
	}


	if (sigsetjmp(invoke_env, SIGALRM) == 0) {//第一次调用，并没有触发SIGALRM
        //以下是执行注册的回调函数逻辑
		if(args[0].isString()){
            switch(func_args.count()){
                case 0:
			        retval = php::exec(args[0].toCString(), func_args);
                    break;
                case 1:
			        retval = php::exec(args[0].toCString(), func_args[0]);
                    break;
                case 2:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1]);
                    break;
                case 3:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2]);
                    break;
                case 4:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3]);
                    break;
                case 5:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4]);
                    break;
                case 6:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5]);
                    break;
                case 7:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6]);
                    break;
                case 8:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7]);
                    break;
                case 9:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7], func_args[8]);
                    break;
                case 10:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7], func_args[8], func_args[9]);
                    break;
                default:
                    call_ret = TT_FUNCTION_CALL_FAILURE;
                    goto END;
                    
            }
		}else if(args[0].isArray()){
			auto v1 = args[0];
			Array v2(v1);
			const char *className = v2[0].toCString();
			const char *funcName = v2[1].toCString();

			Object obj = php::newObject(className);

            switch(func_args.count()){
                case 0:
			        retval = obj.exec(funcName, func_args);
                    break;
                case 1:
			        retval = obj.exec(funcName, func_args[0]);
                    break;
                case 2:
			        retval = obj.exec(funcName, func_args[0], func_args[1]);
                    break;
                case 3:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2]);
                    break;
                case 4:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3]);
                    break;
                case 5:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4]);
                    break;
                case 6:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5]);
                    break;
                case 7:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6]);
                    break;
                case 8:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7]);
                    break;
                case 9:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7], func_args[8]);
                    break;
                case 10:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7], func_args[8], func_args[9]);
                    break;
                default:
                    call_ret = TT_FUNCTION_CALL_FAILURE;
                    goto END;
            }
		}
	
		call_ret = TT_CALL_FUNCTION_SUCC;
		if(call_ret != TT_CALL_FUNCTION_SUCC){
			goto END;
		}
	}else{//从siglongjmp中调用，即调用过了siglongjmp,说明触发了注册的信号，执行已超时
		//function execute timeout
		call_ret = TT_CALL_FUNCTION_TIMEOUT;
		goto END;
	}

	call_ret = TT_CALL_FUNCTION_SUCC;
	END:
		setitimer(ITIMER_REAL, &oldtick, NULL);//执行完了，需要取消之前设置的延时信号
		timeout_set_signal(SIGALRM, sf);
	_this.set("statusCode", call_ret);//把注册函数结果
}


PHPX_FUNCTION(call_function_with_timeout){
	Variant interval = args[1];//注册函数的参数，允许执行的时间

    int func_argv = args.count() - 2;
    Array func_args;
    if(func_argv > 0){
        for(int i=2; i<args.count(); i++){
            func_args.append(args[i]);
        }
    }else{
        func_args = NULL;
    }
	
//    for(int i=0;i<func_args.count();i++){
//        cout<<"i:"<<i<<" value:"<<func_args[i].toCString();
//    }
	int call_ret;
	
	Sigfunc sf = timeout_set_signal(SIGALRM, timeout_signal_handler);//调用信号注册及信号处理句柄
	
	if(sf == SIG_ERR){
		call_ret = TT_SET_SIGNAL_ERROR;
		goto END;
	}	
	struct itimerval tick;
	struct itimerval oldtick;

    //注册时钟信息
	tick.it_value.tv_sec = interval.toInt()/1000;
	tick.it_value.tv_usec = (interval.toInt()%1000) * 1000;
	tick.it_interval.tv_sec = interval.toInt()/1000;
	tick.it_interval.tv_usec = (interval.toInt()%1000) * 1000;
	if (setitimer(ITIMER_REAL, &tick, &oldtick) < 0) {//真实的系统时间，发出SIGALRM
		call_ret = TT_SET_TIMER_ERROR;
		goto END;
	}


	if (sigsetjmp(invoke_env, SIGALRM) == 0) {//第一次调用，并没有触发SIGALRM
        //以下是执行注册的回调函数逻辑
		if(args[0].isString()){
            switch(func_args.count()){
                case 0:
			        retval = php::exec(args[0].toCString(), func_args);
                    break;
                case 1:
			        retval = php::exec(args[0].toCString(), func_args[0]);
                    break;
                case 2:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1]);
                    break;
                case 3:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2]);
                    break;
                case 4:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3]);
                    break;
                case 5:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4]);
                    break;
                case 6:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5]);
                    break;
                case 7:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6]);
                    break;
                case 8:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7]);
                    break;
                case 9:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7], func_args[8]);
                    break;
                case 10:
			        retval = php::exec(args[0].toCString(), func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7], func_args[8], func_args[9]);
                    break;
                default:
                    call_ret = TT_FUNCTION_CALL_FAILURE;
                    goto END;
                    
            }
		}else if(args[0].isArray()){
			auto v1 = args[0];
			Array v2(v1);
			const char *className = v2[0].toCString();
			const char *funcName = v2[1].toCString();

			Object obj = php::newObject(className);

            switch(func_args.count()){
                case 0:
			        retval = obj.exec(funcName, func_args);
                    break;
                case 1:
			        retval = obj.exec(funcName, func_args[0]);
                    break;
                case 2:
			        retval = obj.exec(funcName, func_args[0], func_args[1]);
                    break;
                case 3:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2]);
                    break;
                case 4:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3]);
                    break;
                case 5:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4]);
                    break;
                case 6:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5]);
                    break;
                case 7:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6]);
                    break;
                case 8:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7]);
                    break;
                case 9:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7], func_args[8]);
                    break;
                case 10:
			        retval = obj.exec(funcName, func_args[0], func_args[1], func_args[2], func_args[3], func_args[4], func_args[5], func_args[6], func_args[7], func_args[8], func_args[9]);
                    break;
                default:
                    call_ret = TT_FUNCTION_CALL_FAILURE;
                    goto END;
            }
		}
	
		call_ret = TT_CALL_FUNCTION_SUCC;
		if(call_ret != TT_CALL_FUNCTION_SUCC){
			goto END;
		}
	}else{//从siglongjmp中调用，即调用过了siglongjmp,说明触发了注册的信号，执行已超时
		//function execute timeout
		call_ret = TT_CALL_FUNCTION_TIMEOUT;
		goto END;
	}

	call_ret = TT_CALL_FUNCTION_SUCC;
	END:
		setitimer(ITIMER_REAL, &oldtick, NULL);//执行完了，需要取消之前设置的延时信号
		timeout_set_signal(SIGALRM, sf);
    
    if(call_ret == TT_CALL_FUNCTION_TIMEOUT) {
        //php::throwException(NULL, "CALL_FUNC_TIMEOUT_EXCEPT",TT_FUNCTION_CALL_FAILURE );
        zend_throw_exception(NULL, "call function timeout", TT_CALL_FUNCTION_TIMEOUT);
    }else if(call_ret == TT_FUNCTION_CALL_FAILURE){
        zend_throw_exception(NULL, "call function failure", TT_FUNCTION_CALL_FAILURE);
    }

    //php::error(E_NOTICE, "hello world!");
}

PHPX_EXTENSION(){
	Extension *ext = new Extension("timeout", "0.1.0");
	ext->onStart = [ext](){
		//注册扩展类
		Class *c = new Class("Timeout");
		c->addMethod(PHPX_ME(Timeout, exec));
		//c->addProperty("errno", TT_CALL_FUNCTION_SUCC);
		c->addConstant("TT_CALL_FUNCTION_SUCC", TT_CALL_FUNCTION_SUCC);
		c->addConstant("TT_SET_FUNCTION_CALL_INFO_ERROR", TT_SET_FUNCTION_CALL_INFO_ERROR);
		c->addConstant("TT_FUNCTION_CALL_FAILURE", TT_FUNCTION_CALL_FAILURE);
		c->addConstant("TT_SET_SIGNAL_ERROR", TT_SET_SIGNAL_ERROR);
		c->addConstant("TT_SET_TIMER_ERROR", TT_SET_TIMER_ERROR);
		c->addConstant("TT_CALL_FUNCTION_TIMEOUT", TT_CALL_FUNCTION_TIMEOUT);
		ext->registerClass(c);

		ext->registerConstant("TT_CALL_FUNCTION_SUCC", TT_CALL_FUNCTION_SUCC);
        ext->registerConstant("TT_SET_FUNCTION_CALL_INFO_ERROR", TT_SET_FUNCTION_CALL_INFO_ERROR);
        ext->registerConstant("TT_FUNCTION_CALL_FAILURE", TT_FUNCTION_CALL_FAILURE);
        ext->registerConstant("TT_SET_SIGNAL_ERROR", TT_SET_SIGNAL_ERROR);
        ext->registerConstant("TT_SET_TIMER_ERROR", TT_SET_TIMER_ERROR);
        ext->registerConstant("TT_CALL_FUNCTION_TIMEOUT", TT_CALL_FUNCTION_TIMEOUT);
	
		return SUCCESS;
	};
	ext->onBeforeRequest = [ext](){
		return SUCCESS;
	};
	ext->onShutdown = [ext](){
		return SUCCESS;
	};
	ext->onAfterRequest = [ext](){
		return SUCCESS;
	};
    
    ext->registerFunction(PHPX_FN(call_function_with_timeout));

	return ext;
}

