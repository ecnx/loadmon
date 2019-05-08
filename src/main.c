/* ------------------------------------------------------------------
 * Load Monitor - Main Program File
 * ------------------------------------------------------------------ */

#include "loadmon.h"

/**
 * Show program usage message
 */
static void show_usage ( void )
{
    printf ( "usage: loadmon [-gh]\n"
        "\n"
        "symbols explanation:\n"
        "  A - total cpu usage for both modes\n"
        "  U or | - user mode cpu usage\n"
        "  K or # - kernel mode cpu usage\n\n");
}

/**
 * Read file content into memory
 */
static int readfile ( const char *path, char *buffer, size_t size )
{
    int fd;
    size_t len;

    /* Open file for reading */
    if ( ( fd = open ( path, O_RDONLY ) ) < 0 )
    {
        return -1;
    }

    /* Read data from file */
    if ( ( ssize_t ) ( len = read ( fd, buffer, size - 1 ) ) < 0 )
    {
        close ( fd );
        return -1;
    }

    /* Put string terminator */
    buffer[len] = '\0';

    /* Close input file fd */
    close ( fd );

    return 0;
}

/**
 * Parse cpu load data from procfs
 */
static int get_cpu_load ( struct cpu_stat_t *stat )
{
    unsigned int pos;
    const char *ptr;
    char buffer[4096];
    unsigned int values[7];

    /* Read /proc/stat file content */
    if ( readfile ( "/proc/stat", buffer, sizeof ( buffer ) ) < 0 )
    {
        return -1;
    }

    /* Parse first seven numbers after cpu tag */
    for ( pos = 0, ptr = buffer; pos < 7; pos++ )
    {
        if ( !( ptr = strchr ( ptr, '\x20' ) ) )
        {
            return -1;
        }

        while ( *ptr == '\x20' )
        {
            ptr++;
        }

        if ( sscanf ( ptr, "%u", &values[pos] ) <= 0 )
        {
            return -1;
        }
    }

    /* User mode time share (user+nice) */
    stat->user_jiffies = values[0] + values[1];
    /* Kernel mode time share */
    stat->kernel_jiffies = values[2];
    /* Total time */
    stat->total_jiffies =
        stat->user_jiffies + stat->kernel_jiffies + values[3] + values[4] + values[5] + values[6];

    return 0;
}

/**
 * Print results as graph
 */
static void print_numbers ( unsigned int total_period, unsigned int user_period,
    unsigned int kernel_period )
{
    unsigned int all_promile;
    unsigned int user_promile;
    unsigned int kernel_promile;

    all_promile = 1000 * ( user_period + kernel_period ) / total_period;
    user_promile = 1000 * user_period / total_period;
    kernel_promile = 1000 * kernel_period / total_period;

    printf ( "desc: A: %u.%u%% U: %u.%u%% K: %u.%u%%\n",
        all_promile / 10, all_promile % 10,
        user_promile / 10, user_promile % 10, kernel_promile / 10, kernel_promile % 10 );
}


/**
 * Print results as graph
 */
static void print_graph ( unsigned int user_period, unsigned int kernel_period )
{
    unsigned int i;

    kernel_period /= 20;
    user_period =user_period / 20 + kernel_period;

    for (i = 0; i < 100; i++)
    {
        if (i < kernel_period)
        {
            putchar('#');
        } else if (i <user_period)
        {        
            putchar('|');
        } else {
            putchar('.');
        }
    }

    putchar ( '\n' );
}


/**
 * Print cpu load
 */
static void print_cpu_load ( struct cpu_stat_t *prev, struct cpu_stat_t *next, int graph )
{
    unsigned int total_period;
    unsigned int user_period;
    unsigned int kernel_period;

    total_period = next->total_jiffies - prev->total_jiffies;
    user_period = next->user_jiffies - prev->user_jiffies;
    kernel_period = next->kernel_jiffies - prev->kernel_jiffies;


    if ( graph )
    {
        print_graph ( user_period, kernel_period );

    } else
    {
        print_numbers ( total_period, user_period, kernel_period );
    }
}

/**
 * Monitor cpu load task
 */
static void monitor_cpu_load ( int graph )
{
    struct cpu_stat_t prev;
    struct cpu_stat_t next;
    struct pollfd fds[1];

    /* Prepare poll fd */
    fds[0].fd = 0;
    fds[0].events = POLLIN;

    /* Get initial values */
    get_cpu_load ( &prev );

    /* Wait some delay */
    if ( poll (fds, sizeof(fds) / sizeof(struct pollfd), MONITOR_REFRESH_MSEC ) > 0)
    {
        return;
    }

    /* Monitor cpu usage as long as possible */
    while ( get_cpu_load ( &next ) >= 0 )
    {
        /* Wait some delay */
        if ( poll (fds, sizeof(fds) / sizeof(struct pollfd), MONITOR_REFRESH_MSEC ) > 0)
        {
            break;
        }
        print_cpu_load ( &prev, &next, graph );

        /* Prepare to next loop */
        prev.total_jiffies = next.total_jiffies;
        prev.user_jiffies = next.user_jiffies;
        prev.kernel_jiffies = next.kernel_jiffies;
    }
}

/**
 * Program entry point
 */
int main ( int argc, char *argv[] )
{
    /* Do not buffer stdout */
    setbuf(stdout, NULL);

    /* Monitor cpu or show help  */
    if ( argc != 2 )
    {
        monitor_cpu_load ( 0 );

    } else if ( !strcmp ( argv[1], "-h" ) )
    {
        show_usage (  );
        return 1;

    } else
    {
        monitor_cpu_load ( !strcmp ( argv[1], "-g" ) );
    }

    printf ("stopped monitoring.\n");

    return 0;
}
