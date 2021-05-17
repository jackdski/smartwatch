#ifndef WATCHDOG_H_
#define WATCHDOG_H_

void init_watchdog(void);
void watchdog_kick(void);
void watchdog_handler(void);

#endif /* WATCHDOG_H_ */