#	Filename	:	makefile
#	Developer	:	Eyal Weizman
#	Last Update	:	2020-03-08
#	Description	:	makefile for watchdog directory

################# vairables ###################
flags = -pedantic-errors -Wall -Wextra -g -Og
end_flag = -pthread -ldl
so_flag = -fPIC -shared

# all headers
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
	
# scheduler objects
sched_src = \
	scheduler/scheduler.c \
	scheduler/task/task.c \
	scheduler/task/uid/uid.c \
	scheduler/pqueue/pqueue.c \
	scheduler/pqueue/heap/heap.c \
	scheduler/pqueue/heap/dynamic_vctor/dynamic_vector.c
sched_objects = $(sched_src:.c=.o)

# WD shared object
wd_shared_src = wd_shared.c
wd_shared_lib = libshared.so

# WD outer program
wd_outer_src = wd_outer.c
wd_outer_out = wd_outer.out

# WD API lib
wd_api_src = wd_api.c
wd_api_obj = wd_api.o
wd_api_lib = libwd.a

# test
test_src = test.c
test_out = test.out

################ main commands ####################
.PHONY : release test clean

release : $(wd_outer_out) $(wd_api_lib)

test : release $(test_out)

clean:
	rm -rf -v nosuchfile `find . -name "*.o"` *.so *.a *.out *.gch *.out

################ secondary rules ####################

# for scheduler objects
%.o : %.c %.h
	gcc $(flags) -fPIC -c $< -o $@ $(end_flag)

# shared lib with common functions
$(wd_shared_lib) : $(wd_shared_src) $(sched_objects) $(headers)
	gcc $(flags) $(so_flag) $(wd_shared_src) $(sched_objects) -o $@ $(end_flag)

# creating the WD outer program
$(wd_outer_out) : $(wd_outer_src) $(wd_shared_lib)
	gcc -L. -Wl,-rpath=. -o $@ $< -lshared

# static lib with API functions
$(wd_api_lib) : $(wd_api_obj) $(wd_shared_lib)
	ar rcs $@ $<

# make test
$(test_out) : $(test_src) $(wd_api_lib)
	gcc -L. -Wl,-rpath=. $< -o $@ -lwd -lshared $(end_flag)

