/* wink signal shim: self-pipe trick for SIGWINCH + SIGINT.
 *
 * Blink (v0.40) has no FFI callback / function-pointer surface, so the
 * sigaction(2) handler must live here in C. The handler writes one byte
 * to a nonblocking pipe; Blink reads from the read end via the event
 * loop. Static state: the write fd, prior sigactions, install flag.
 *
 * Linux/glibc only — see ../src/wink/raw.bl header comment.
 */

#define _GNU_SOURCE
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

static volatile sig_atomic_t g_pipe_w = -1;
static int g_read_fd  = -1;
static int g_write_fd = -1;
static struct sigaction g_prev_winch;
static struct sigaction g_prev_int;
static int g_installed = 0;

static void wink_handler(int sig) {
    int saved_errno = errno;
    int fd = g_pipe_w;
    if (fd >= 0) {
        unsigned char byte = (unsigned char)sig;
        ssize_t _w = write(fd, &byte, 1);
        (void)_w; /* short writes / EAGAIN: drain dedups, ignore */
    }
    errno = saved_errno;
}

int wink_signal_install(void) {
    if (g_installed) return -1;

    int fds[2];
    if (pipe2(fds, O_NONBLOCK | O_CLOEXEC) != 0) return -1;

    g_read_fd  = fds[0];
    g_write_fd = fds[1];
    g_pipe_w   = fds[1];

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = wink_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGWINCH, &sa, &g_prev_winch) != 0) goto fail;
    if (sigaction(SIGINT,   &sa, &g_prev_int)   != 0) {
        /* roll back winch before bailing */
        sigaction(SIGWINCH, &g_prev_winch, NULL);
        goto fail;
    }

    g_installed = 1;
    return 0;

fail:
    g_pipe_w = -1;
    close(g_read_fd);
    close(g_write_fd);
    g_read_fd = -1;
    g_write_fd = -1;
    return -1;
}

int wink_signal_uninstall(void) {
    if (!g_installed) return -1;

    sigaction(SIGWINCH, &g_prev_winch, NULL);
    sigaction(SIGINT,   &g_prev_int,   NULL);

    g_pipe_w = -1;
    if (g_read_fd  >= 0) close(g_read_fd);
    if (g_write_fd >= 0) close(g_write_fd);
    g_read_fd  = -1;
    g_write_fd = -1;
    g_installed = 0;
    return 0;
}

int wink_signal_read_fd(void) { return g_read_fd; }

/* Read one pending signal byte from the self-pipe.
 * Returns 0..255 on success, -1 if no data available (EAGAIN/closed/other),
 * so callers loop until -1. Avoids needing Ptr[U8] offset arithmetic on
 * the Blink side. */
int wink_signal_read_one(void) {
    if (g_read_fd < 0) return -1;
    unsigned char byte;
    ssize_t n = read(g_read_fd, &byte, 1);
    if (n == 1) return (int)byte;
    return -1;
}

/* Pack cols/rows into a single int: (cols << 16) | rows. Both fit in u16
 * (terminals beyond 65535 cells don't exist). Returns -1 on ioctl failure;
 * the high bit distinguishes the sentinel from any valid pack. One syscall
 * per SIGWINCH instead of two. */
int wink_winsize_get(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0) return -1;
    return ((int)ws.ws_col << 16) | (int)ws.ws_row;
}
