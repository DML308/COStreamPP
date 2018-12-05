#ifndef _BASICS_H_
#define _BASICS_H_
#define MAX_SCOPE_DEPTH 100 //定义最大嵌套深度为100
/*level表示当前嵌套深度，version表示嵌套域计数器 */
extern int Level;
extern int current_version[MAX_SCOPE_DEPTH];

#endif