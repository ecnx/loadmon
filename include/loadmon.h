/* ------------------------------------------------------------------
 * Load Monitor - Shared Project Header
 * ------------------------------------------------------------------ */

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef LOADMON_H
#define LOADMON_H

#define MONITOR_REFRESH_MSEC 1000
#define BUFFER_SIZE 256
#define PATH_SIZE 256

/**
 * Describes cpu load stats
 */
struct cpu_stat_t
{
    unsigned int total_jiffies;
    unsigned int user_jiffies;
    unsigned int kernel_jiffies;
};

#endif
