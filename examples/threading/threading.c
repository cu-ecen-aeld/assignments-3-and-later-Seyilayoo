#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    
    //Initialise the thread parameters by type casting the void pointer
	struct thread_data* thread_func_args = (struct thread_data *) thread_param;
	
	int rc;
	//Initially set thread complete status as true
	thread_func_args->thread_complete_success=true;
	//Suspends the action for the microseconds as specified in the argument
	//Value in milliseconds converted to microseconds
	rc=usleep(thread_func_args->wait_ms_obtain*(CONVERT_MS_to_US));
	if(rc!=0)
	{
			thread_func_args->thread_complete_success=false;
	}
	//Lock the mutex
	rc=pthread_mutex_lock(thread_func_args->mutex_lock);
	if ( rc != 0 )
	{
		thread_func_args->thread_complete_success=true;
	}
	//Suspends action after locking mutex
	rc = usleep(thread_func_args->wait_ms_release*(CONVERT_MS_to_US));
	if(rc!=0)
	{
			thread_func_args->thread_complete_success=false;
	}
	//Release the mutex lock
	rc=pthread_mutex_unlock(thread_func_args->mutex_lock);
	if(rc!=0)
	{
			thread_func_args->thread_complete_success=false;
	}
    //Equivalent to pthread_exit(3) with the value supplied in the return statement
   
	return thread_param;
}



bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

struct thread_data *data = malloc(sizeof(*data));

    data->wait_to_obtain_ms = wait_to_obtain_ms;
    data->wait_to_release_ms = wait_to_release_ms;
    data->mutex = mutex;

    int rc = pthread_create(thread, NULL, threadfunc, data);

    if (rc != 0)
        return false;

    return true;

}

