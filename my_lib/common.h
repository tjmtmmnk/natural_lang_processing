#ifndef NATURAL_LANG_PROCESS_COMMON_H
#define NATURAL_LANG_PROCESS_COMMON_H

#define rep(i, s, n) int i; for(i = s;i<n;i++)

extern int error(int line_num, char *mes);
extern int errorWithReturn(int line_num, char *mes);

#endif //NATURAL_LANG_PROCESS_COMMON_H
