#pragma once
#include <cstddef>

bool readConf(char *buf, size_t cap);
bool findCmd(const char *buf, const char *key, char *out, size_t cap);
int split(char *s, char **argv, int maxArgs);
bool launch(char **argv);
