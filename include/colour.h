/** 
 * @file colour.h
 * @author lymslive
 * @date 2023-06-29
 * @brief Implement print colourful message from tinytast framework.
 * */
#ifndef COLOUR_H__
#define COLOUR_H__


namespace tast
{

/// Print message to stdout in colour mode.
void colour_print(const char* message);

/// Check whether stdout support colour print.
/// Usually true when print to terminal, but false if stdout redicret to file
/// or pipe.
bool colour_support();

} // end of namespace tast
#endif /* end of include guard: COLOUR_H__ */
