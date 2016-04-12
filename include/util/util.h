#ifndef SHADOW_UTIL_H
#define SHADOW_UTIL_H

#include <cmath>
#include <iostream>
#include <string>

static void inline error(std::string msg) {
  std::cerr << msg << std::endl;
  exit(1);
}

static void inline warn(std::string msg) {
  std::cout << msg << std::endl;
  exit(0);
}

static float inline rand_uniform(float min, float max) {
  return ((float)std::rand() / RAND_MAX) * (max - min) + min;
}

static float inline constrain(float min, float max, float value) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

static std::string find_replace(const std::string str, std::string oldstr,
                                std::string newstr) {
  std::string origin(str);
  size_t index = 0;
  while ((index = origin.find(oldstr, index)) != std::string::npos) {
    origin.replace(index, oldstr.length(), newstr);
    index += newstr.length();
  }
  return origin;
}

static std::string find_replace_last(const std::string str, std::string oldstr,
                                     std::string newstr) {
  std::string origin(str);
  size_t index = origin.find_last_of(oldstr);
  origin.replace(index, oldstr.length(), newstr);
  return origin;
}

#endif // SHADOW_UTIL_H
