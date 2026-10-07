// astralia-open key...  (no shell: commands split on whitespace)
#include <fcntl.h>
#include <spawn.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "main.h"

extern char **environ;

bool readConf(char *buf, size_t cap) {
    char path[512];
    const char *e = getenv("ASTRALIA_CONF");
    if (e) {
        snprintf(path, sizeof path, "%s", e);
    } else {
        char host[256] = "";
        gethostname(host, sizeof host - 1);
        const char *h = getenv("HOME");
        snprintf(path, sizeof path, "%s/.config/astralia-open/%s.conf", h ? h : "", host);
    }
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) { fprintf(stderr, "cannot open %s\n", path); return false; }
    ssize_t n = read(fd, buf, cap - 1);
    close(fd);
    if (n < 0) return false;
    buf[n] = 0;
    return true;
}

bool findCmd(const char *buf, const char *key, char *out, size_t cap) {
    size_t klen = strlen(key);
    for (const char *line = buf; *line;) {
        const char *end = strchr(line, '\n');
        if (!end) end = line + strlen(line);
        const char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (!strncmp(p, key, klen)) {
            p += klen;
            while (*p == ' ' || *p == '\t') p++;
            if (*p == '=' && p < end) {
                size_t n = end - (p + 1);
                if (n >= cap) n = cap - 1;
                memcpy(out, p + 1, n);
                out[n] = 0;
                return true;
            }
        }
        line = *end ? end + 1 : end;
    }
    return false;
}

int split(char *s, char **argv, int maxArgs) {
    int n = 0;
    for (char *tok = strtok(s, " \t\r"); tok && n < maxArgs; tok = strtok(nullptr, " \t\r")) argv[n++] = tok;
    return n;
}

bool launch(char **argv) {
    posix_spawnattr_t attr;
    posix_spawnattr_init(&attr);
    posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSID);
    int rc = posix_spawnp(nullptr, argv[0], nullptr, &attr, argv, environ);
    posix_spawnattr_destroy(&attr);
    return rc == 0;
}

int main(int argc, char **argv) {
    if (argc < 2) { fputs("usage: astralia-open key [args...]\n", stderr); return 1; }

    static char buf[16384];
    if (!readConf(buf, sizeof buf)) return 1;

    char val[512], tval[512], *cmd[64];
    if (!findCmd(buf, argv[1], val, sizeof val)) { fprintf(stderr, "unknown app: %s\n", argv[1]); return 1; }

    // terminal:<cmd> runs as `<terminal> -e <cmd>`
    char *s = val;
    while (*s == ' ' || *s == '\t') s++;
    int n = 0;
    if (!strncmp(s, "terminal:", 9)) {
        s += 9;
        if (!findCmd(buf, "terminal", tval, sizeof tval)) { fputs("no `terminal` entry\n", stderr); return 1; }
        n = split(tval, cmd, 60);
        cmd[n++] = (char *)"-e";
    }
    n += split(s, cmd + n, 63 - n);
    for (int i = 2; i < argc && n < 63; i++) cmd[n++] = argv[i];
    cmd[n] = nullptr;
    if (n == 0 || !launch(cmd)) { fprintf(stderr, "cannot launch: %s\n", argv[1]); return 1; }
    return 0;
}
