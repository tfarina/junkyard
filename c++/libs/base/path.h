#ifndef PATH_H_
#define PATH_H_

#include <string>

std::string MakeAbsolute(const char *path);

bool PathExists(const std::string& path);

bool GetRealPath(const std::string& path, std::string* out);

#endif  // PATH_H_