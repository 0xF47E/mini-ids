#ifndef LOG_H
#define LOG_H

extern int debug;

#define DEBUG_PRINT(fmt, args...)                                              \
  if (debug)                                                                   \
  fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt "\n", __FILE__, __LINE__,          \
          __func__, ##args)

#define ERROR_PRINT(fmt, args...)                                              \
  fprintf(stderr, "ERROR: %s:%d:%s(): " fmt "\n", __FILE__, __LINE__,          \
          __func__, ##args)

#endif // LOG_H
