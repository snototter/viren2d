#ifndef __VIVI_STRING_UTILS_H__
#define __VIVI_STRING_UTILS_H__

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace vivi
{
namespace strings
{
/**
 * @brief Checks if the given string ends with another string
 */
inline bool EndsWith(const std::string &s, const std::string &end)
{
  if (s.length() >= end.length())
    return (s.compare(s.length() - end.length(), end.length(), end) == 0);
  return false;
}


/**
 * @brief Checks if the given string ends with the given character
 */
inline bool EndsWith(const std::string &s, char end)
{
  if (s.length() > 0)
    return (s.at(s.length()-1) == end);
  return false;
}


/** @brief Checks if the given string starts with the prefix. */
inline bool StartsWith(const std::string &s, const std::string &prefix)
{
  if (s.length() >= prefix.length())
    return s.compare(0, prefix.length(), prefix) == 0;
  return false;
}


/**
 * @brief Convert string to lower case
 */
inline void ToLower(std::string &s)
{
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}


/** @brief Returns the string converted to lower case letters. */
inline std::string Lower(const std::string &s)
{
  std::string tmp(s);
  ToLower(tmp);
  return tmp;
}


/**
 * @brief Convert string to upper case
 */
inline void ToUpper(std::string &s)
{
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}


/** @brief Returns the string converted to upper case letters. */
std::string Upper(const std::string &s)
{
  std::string tmp(s);
  ToUpper(tmp);
  return tmp;
}


/**
 * @brief Remove leading white space
 */
inline std::string LTrim(const std::string &totrim)
{
  std::string s(totrim);
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}


/**
 * @brief Remove trailing white space
 */
inline std::string RTrim(const std::string &totrim)
{
  std::string s(totrim);
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}


/**
 * @brief Remove leading and trailing white space
 */
inline std::string Trim(const std::string &s)
{
  return LTrim(RTrim(s));
}


/**
 * @brief Tokenize string by given delimiter
 */
inline void Split(const std::string &s, char delim, std::vector<std::string> &elems)
{
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim))
    elems.push_back(item);
}


/**
 * @brief Tokenize string by given delimiter
 */
inline std::vector<std::string> Split(const std::string &s, char delim)
{
  std::vector<std::string> elems;
  Split(s, delim, elems);
  return elems;
}

//TODO maybe include other vcp string utils

} // namespace strings
} // namespace vivi

#endif // __VIVI_STRING_UTILS_H__
