#ifndef SYSCALL_MEMORY_TOOL_H
#define SYSCALL_MEMORY_TOOL_H

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <locale>
#include <set>
#include <map>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <fstream>
#include <ctime>
#include <fcntl.h>
#include <dlfcn.h>
#include <pthread.h>

#ifdef __ANDROID__
#include <android/log.h>  // Only include this for Android
#endif

#ifdef __linux__
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <linux/memfd.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <linux/ptrace.h>
#endif

// Use Android.mk Docking : LOCAL_SRC_FILES += syscall.s
// syscall.s [isCanRead] -> MemoryTools.h
extern "C" long raw_syscall(long __number, ...);

static int random_fd = -1;
bool isMemoryTrap(uintptr_t addr) {
    if (addr < 0x10000000 || addr > 0xFFFFFFFFFF) {
        return false;
    }
    if (random_fd <= 0) {
        random_fd = raw_syscall(__NR_memfd_create, "jit-cache", MFD_CLOEXEC | MFD_ALLOW_SEALING);
    }
    return raw_syscall(__NR_write, random_fd, reinterpret_cast<void *>(addr), 4) >= 0;
}

bool memoryRead(uintptr_t address, void *buffer, int size) {
    memset(buffer, 0, size);
    if (isMemoryTrap(address)) {
    	return memmove(&buffer, (void*)address, sizeof(size)) != nullptr;
    } else {
    	return false;
    }
}

// ROOT : Write some pointers to set SuLnux to Permissive Mode
bool memoryWrite(uintptr_t addr, void *buffer, size_t length) {
    unsigned long page_size = sysconf(_SC_PAGESIZE);
    unsigned long size = page_size * sizeof(uintptr_t);
    return mprotect((void *) (addr - (addr % page_size) - page_size), (size_t) size, PROT_EXEC | PROT_READ | PROT_WRITE) == 0 && memcpy(reinterpret_cast<void*>(addr), buffer, length) != 0;
}

template<class T> T Read(uintptr_t ptr)
{
	T buff = {};
	if (isMemoryTrap(ptr)) {
		memmove(&buff, (void*)ptr, sizeof(T));
	}
	return buff;
}

template <typename T> void Write(uintptr_t addr, T var)
{
    memoryWrite(addr, &var, sizeof(var));
}
#endif // SYSCALL_MEMORY_TOOL_H

uintptr_t GetModule(const char* module_name) {
    FILE *fp;
    uintptr_t addr = 0;
    char filename[32];
    char line[1024];

    snprintf(filename, sizeof(filename), "/proc/%d/maps", getpid());
    fp = fopen(filename, "r");
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, module_name)) {
                addr = strtoul(strtok(line, "-"), nullptr, 16);
                if (addr == 0x8000) addr = 0;
                break;
            }
        }
        fclose(fp);
    }
    return addr;
}
