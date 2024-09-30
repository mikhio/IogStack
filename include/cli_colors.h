#ifndef CLI_COLORS_H
#define CLI_COLORS_H

/** @file CliColors.h */

#define COLOR_RESET "\x1B[0m"     ///< Constant for ANSI reset color
#define COLOR_RED   "\x1B[1;31m"  ///< Constant for ANSI red bold color
#define COLOR_GRN   "\x1B[1;32m"  ///< Constant for ANSI green bold color
#define COLOR_YEL   "\x1B[1;33m"  ///< Constant for ANSI yellow bold color
#define COLOR_BLU   "\x1B[1;34m"  ///< Constant for ANSI blue bold color
#define COLOR_MAG   "\x1B[1;35m"  ///< Constant for ANSI magenta bold color
#define COLOR_WHT   "\x1B[1;37m"  ///< Constant for ANSI white bold color
#define COLOR_BLK   "\x1B[1m"     ///< Constant for ANSI black bold color

#define RED(str)     COLOR_RED str COLOR_RESET ///< Macros that set red color to str
#define GREEN(str)   COLOR_GRN str COLOR_RESET ///< Macros that set green color to str
#define MAGENTA(str) COLOR_MAG str COLOR_RESET ///< Macros that set magenta color to str
#define BLACK(str)   COLOR_BLK str COLOR_RESET ///< Macros that set black color to str
#define BLUE(str)    COLOR_BLU str COLOR_RESET ///< Macros that set blue color to str




#endif // CLI_COLORS_H
