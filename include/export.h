#pragma once

#ifdef _WIN32
#ifdef MARKDOWNTOHTML_EXPORTS
#define MARKDOWNPARSER_API __declspec(dllexport)
#else
#define MARKDOWNPARSER_API __declspec(dllimport)
#endif
#else
#define MARKDOWNPARSER_API
#endif