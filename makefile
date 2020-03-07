#	Filename	:	makefile
#	Developer	:	Eyal Weizman
#	Last Update	:	2019-05-18
#	Description	:	makefile for watchdog directory

################# super-tools ###################
#killall a.out
#killall watchdog.out
#killall watchdog.out a.out
#export LD_LIBRARY_PATH=$LD_LIBRARY:~/git/eyal-waizmann/system_programming/watchdog

################# vairables ###################
flags = -pedantic-errors -Wall -Wextra -g -Og
#TODO: -ldl needed?
end_flag = -pthread -ldl
so_flag = -fPIC -shared

headers = \
	wd_api.h \
	wd_shared.h \
	scheduler/scheduler.h \
	scheduler/task/task.h \
	scheduler/task/uid/uid.h \
	scheduler/task/types.h \
	scheduler/pqueue/pqueue.h \
	scheduler/pqueue/heap/heap.h \
	scheduler/pqueue/heap/dynamic_vctor/dynamic_vector.h \
	utils/general_types.h
	

# static lib with scheduler
sched_src = \
	scheduler/scheduler.c \
	scheduler/task/task.c \
	scheduler/task/uid/uid.c \
	scheduler/pqueue/pqueue.c \
	scheduler/pqueue/heap/heap.c \
	scheduler/pqueue/heap/dynamic_vctor/dynamic_vector.c
sched_objects = $(sched_src:.c=.o)
sched_lib = libsched.a


# WD shared object
wd_shared_src = wd_shared.c
wd_shared_lib = libwd.a

# WD API lib
wd_api_src = wd_api.c
wd_api_lib = libwd.a

# WD outer program
wd_outer_src = wd_outer.c
wd_outer_out = wd_outer.out

# test
test_src = test.c
test_out = test.out

################ main commands ####################
.PHONY : release test clean

release : $(sched_lib)

# release : $(wd_api_lib) $(wd_shared_lib) $(wd_outer_out) 

# test : release $(test_out)

clean:
	rm -rf */*.o */*/*.o */*/*/*.o  */*/*/*/*.o *.so *.a *.out *.gch *.out
# rm -r -f -v *.o *.so *.a *.out *.gch *.out

################ secondary rules ####################

# for scheduler objects
%.o : %.c %.h
	gcc $(flags) -c $< -o $@ $(end_flag)

# scheduler static library
$(sched_lib) : $(sched_objects) $(headers)
	ar rcs $@ $(sched_objects)

# shared lib with common functions
$(wd_shared_lib) : $(wd_shared_src) $(headers)
	gcc $(flags) $(so_flag) $< -o $@ $(end_flag)







# sources = $(wildcard *.c)
# exes = $(sources:.c=.out)
# ds_lib = ~/git/eyal-waizmann/ds/libdebug.a

# ################ main commands ####################
# .PHONY : all clean

# all : watchdog.out a.out
	
# # cleans all
# clean:
# 	rm -rf *.so *.a *.o *.out *.d


# ################ secondary rules ####################

# a.out : watchdog_test.c libwatchdog.a watchdog_shared.so $(ds_lib)
# 	gcc $(flags) $^ -o $@ $(end_flag)
	
# watchdog.out : watchdog_main.c watchdog_shared.so $(ds_lib)
# 	gcc $(flags) $^ -o $@ $(end_flag)


# # shared lib for the user
# %_shared.so : %_shared.o
# 	gcc -shared $^ -o $@

# #for shared lib watchdog_shared.so
# %_shared.o :%_shared.c watchdog_shared.h
# 	gcc -c -fPIC $< -o $@ $(end_flag)

# # static lib for the user
# libwatchdog.a : watchdog.o
# 	ar rcs $@ $<

# # for the static libwatchdog.a
# watchdog.o : watchdog.c watchdog.h watchdog_shared.h
# 	gcc $(flags) -c $< -o $@ $(end_flag)
