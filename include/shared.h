#ifndef _SHARED_H_
#define _SHARED_H_

#include <mqueue.h>

#define MESSAGE_MAX_SIZE 120
#define QUEUE_MAX_MESSAGES 10
#define MESSAGE_PRIORITY 1

/* Type to be sent from and into queues */
typedef char message_t[MESSAGE_MAX_SIZE];

/* From UART to Terminal */
extern mqd_t uart_to_terminal_mq;
#define UART_TO_TERMINAL_QUEUE_NAME "/mq_in"

/* From Terminal to UART */
extern mqd_t terminal_to_uart_mq;
#define TERMINAL_TO_UART_QUEUE_NAME "/mq_out"

#endif
