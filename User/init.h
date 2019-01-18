#ifndef __INIT_H
#define	__INIT_H

#define INT_ENABLE()			__set_PRIMASK(0)
#define INT_DISABLE()			__set_PRIMASK(1)

void Init_System(void);
void IWDG_Feed(void);

#endif /* __INIT_H */


