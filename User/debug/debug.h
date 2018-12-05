#ifndef __DEBUG_H
#define	__DEBUG_H

void debug_puts(const uint8_t *str);

/**************_PUTS_ **************/
#ifdef DEBUG_EN
	#define DEBUG_PUTS_(expr)  debug_puts(expr)
#else
	#define	DEBUG_PUTS_(expr)
#endif

#endif /* __DEBUG_H */

