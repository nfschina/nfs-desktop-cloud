/*
 * debug_new.cpp  1.11 2003/07/03
 *
 * Implementation of debug versions of new and delete to check leakage
 *
 * By Wu Yongwei
 *
 */

#include <new>
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#pragma warning(disable: 4073)
#pragma init_seg(lib)
#endif

#ifndef DEBUG_NEW_HASHTABLESIZE
#define DEBUG_NEW_HASHTABLESIZE 16384
#endif

#ifndef DEBUG_NEW_HASH
#define DEBUG_NEW_HASH(p) (((unsigned long)(p) >> 8) % DEBUG_NEW_HASHTABLESIZE)
#endif

// The default behaviour now is to copy the file name, because we found
// that the exit leakage check cannot access the address of the file
// name sometimes (in our case, a core dump will occur when trying to
// access the file name in a shared library after a SIGINT).
#ifndef DEBUG_NEW_FILENAME_LEN
#define DEBUG_NEW_FILENAME_LEN	20
#endif
#if DEBUG_NEW_FILENAME_LEN == 0 && !defined(DEBUG_NEW_NO_FILENAME_COPY)
#define DEBUG_NEW_NO_FILENAME_COPY
#endif
#ifndef DEBUG_NEW_NO_FILENAME_COPY
#include <string.h>
#endif

#include <QMutex>
#include <QStringList>

#define LOG_FILE_NAME ("LEAK.LOG")

struct new_ptr_list_t
{
	new_ptr_list_t*		next;
#ifdef DEBUG_NEW_NO_FILENAME_COPY
	const char*			file;
#else
	char				file[DEBUG_NEW_FILENAME_LEN];
#endif
	int					line;
	size_t				size;

    char **             trace_string;
    int                 n;
};

static new_ptr_list_t* new_ptr_list[DEBUG_NEW_HASHTABLESIZE];
static QMutex          g_mutex;

bool new_verbose_flag = false;
bool new_autocheck_flag = true;
bool is_stack_trace = true;

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

/* Obtain a backtrace and print it to @code{stdout}. */
char** alloc_trace(int *s)
{
  void *array[10];
  size_t size;
  char **strings;
  size_t i;
  size = backtrace(array, 10);
  strings = backtrace_symbols(array, size);
  if (NULL == strings)
  {
    perror("backtrace_synbols");
    exit(1);
  }

  *s = size;

  //pthread_t pid = pthread_self();
  //printf("pid ==== %d\n", pid);

  return strings;
}

bool check_leaks()
{
	bool fLeaked = false;

    char *home_dir = getenv("HOME");
    char log_dir[256] = {0};

    snprintf(log_dir, sizeof(log_dir), "%s/%s", home_dir, LOG_FILE_NAME);

    FILE *file = fopen(log_dir, "w");

    QMutexLocker locker(&g_mutex);

    int count = 0;

	for (int i = 0; i < DEBUG_NEW_HASHTABLESIZE; ++i)
	{
		new_ptr_list_t* ptr = new_ptr_list[i];
		if (ptr == NULL)
			continue;
		fLeaked = true;
		while (ptr)
		{
            fprintf(file, "--------------------------------------------%d------------------------------------------------\n", count);
            fprintf(file,"Leaked object at %p (size %u, %s:%d)\n",
					(char*)ptr + sizeof(new_ptr_list_t),
					ptr->size,
					ptr->file,
					ptr->line);

            if(ptr->trace_string != NULL)
            {
                /**/
                for (int i = 1; i < ptr->n; i++)
                {
                    fprintf(file,"%s\n", ptr->trace_string[i]);
                }

                //free(ptr->trace_string);
                //ptr->trace_string = NULL;
            }

            fprintf(file, "--------------------------------------------%d------------------------------------------------\n", count);
            count++;

			ptr = ptr->next;
		}
	}

    fclose(file);

    return fLeaked;
}

void* operator new(size_t size, const char* file, int line)
{
    QMutexLocker locker(&g_mutex);

	size_t s = size + sizeof(new_ptr_list_t);
	new_ptr_list_t* ptr = (new_ptr_list_t*)malloc(s);
	if (ptr == NULL)
	{
		fprintf(stderr, "new:  out of memory when allocating %u bytes\n",
				size);
		abort();
	}
	void* pointer = (char*)ptr + sizeof(new_ptr_list_t);
	size_t hash_index = DEBUG_NEW_HASH(pointer);
	ptr->next = new_ptr_list[hash_index];
#ifdef DEBUG_NEW_NO_FILENAME_COPY
	ptr->file = file;
#else
	strncpy(ptr->file, file, DEBUG_NEW_FILENAME_LEN - 1);
	ptr->file[DEBUG_NEW_FILENAME_LEN - 1] = '\0';
#endif
	ptr->line = line;
	ptr->size = size;
	new_ptr_list[hash_index] = ptr;
	if (new_verbose_flag)
    {
		printf("new:  allocated  %p (size %u, %s:%d)\n",
				pointer, size, file, line);
    }

    if (is_stack_trace)
    {
        int n = 0;
        ptr->trace_string = alloc_trace(&n);
        ptr->n = n;
    }
    else
    {
        ptr->trace_string = NULL;
    }

	return pointer;
}

void* operator new[](size_t size, const char* file, int line)
{
	return operator new(size, file, line);
}

void* operator new(size_t size)
{
	return operator new(size, "<Unknown>", 0);
}

void* operator new[](size_t size)
{
	return operator new(size);
}

void* operator new(size_t size, const std::nothrow_t&) throw()
{
	return operator new(size);
}

void* operator new[](size_t size, const std::nothrow_t&) throw()
{
	return operator new[](size);
}

void operator delete(void* pointer)
{
    QMutexLocker locker(&g_mutex);

	if (pointer == NULL)
		return;
	size_t hash_index = DEBUG_NEW_HASH(pointer);
	new_ptr_list_t* ptr = new_ptr_list[hash_index];
	new_ptr_list_t* ptr_last = NULL;
	while (ptr)
	{
		if ((char*)ptr + sizeof(new_ptr_list_t) == pointer)
		{
			if (new_verbose_flag)
				printf("delete: freeing  %p (size %u)\n", pointer, ptr->size);
			if (ptr_last == NULL)
				new_ptr_list[hash_index] = ptr->next;
			else
				ptr_last->next = ptr->next;

            if(ptr->trace_string != NULL)
            {
                free(ptr->trace_string);
                ptr->trace_string = NULL;
            }

			free(ptr);
			return;
		}
		ptr_last = ptr;
		ptr = ptr->next;
	}

	fprintf(stderr, "delete: invalid pointer %p\n", pointer);
	abort();
}

void operator delete[](void* pointer)
{
	operator delete(pointer);
}

// Some older compilers like Borland C++ Compiler 5.5.1 and Digital Mars
// Compiler 8.29 do not support placement delete operators.
// NO_PLACEMENT_DELETE needs to be defined when using such compilers.
// Also note that in that case memory leakage will occur if an exception
// is thrown in the initialization (constructor) of a dynamically
// created object.
#ifndef NO_PLACEMENT_DELETE
void operator delete(void* pointer, const char* file, int line)
{
	if (new_verbose_flag)
		printf("info: exception thrown on initializing object at %p (%s:%d)\n",
				pointer, file, line);
	operator delete(pointer);
}

void operator delete[](void* pointer, const char* file, int line)
{
	operator delete(pointer, file, line);
}

void operator delete(void* pointer, const std::nothrow_t&)
{
	operator delete(pointer, "<Unknown>", 0);
}

void operator delete[](void* pointer, const std::nothrow_t&)
{
	operator delete(pointer, std::nothrow);
}
#endif // NO_PLACEMENT_DELETE

// Proxy class to automatically call check_leaks if new_autocheck_flag is set
class new_check_t
{
public:
    new_check_t() {}
	~new_check_t()
	{
		if (new_autocheck_flag)
		{
			// Check for leakage.
			// If any leaks are found, set new_verbose_flag so that any
			// delete operations in the destruction of global/static
			// objects will display information to compensate for
			// possible false leakage reports.
			if (check_leaks())
				new_verbose_flag = true;
		}
	}
};

static new_check_t new_check_object;
