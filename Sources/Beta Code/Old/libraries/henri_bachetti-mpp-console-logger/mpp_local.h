
#ifndef _MPP_LOCAL_H_
#define _MPP_LOCAL_H_

#ifdef LOG_DEBUG
#define debug_print     console_printf
#else
#define debug_print
#endif

#define klog            console_printf

#endif
