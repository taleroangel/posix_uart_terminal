#include "uart.h"
#include <shared.h>
#include <sys/types.h>
#include <threads.h>

#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *recv_thread(void *params) {

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  while (1) {
    // Create the message buffer
    message_t message_buffer;
    memset(message_buffer, 0, sizeof(message_buffer));

    // Recieve bytes
    ssize_t bytes_recv =
        mq_receive(terminal_to_uart_mq, message_buffer, MESSAGE_MAX_SIZE, NULL);

    if (bytes_recv > 0L) {
      // Print contents
      printf("[UART: RecvTerminal]\tRecieved: `%s`\n", message_buffer);

      // Send to UART
      ssize_t bytes_sent =
          write(serial_port, message_buffer, sizeof(message_buffer));

      printf("[UART: SendToSerial]\tSent %zu bytes of data\n", bytes_sent);
      fflush(stdout);
    }

    pthread_testcancel();
  }

  pthread_exit(NULL);
}

void *send_thread(void *params) {
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  while (1) {

    // Create the message buffer
    message_t message_buffer;
    memset(message_buffer, 0, sizeof(message_buffer));

    // Recieve from UART
    ssize_t read_bytes =
        read(serial_port, message_buffer, sizeof(message_buffer));

    if (read_bytes > 0L) {
      // Print contents
      printf("[UART: RecvSerial\tRecieved: `%s`\n", message_buffer);
      fflush(stdout);

      // Send to UART
      mq_send(uart_to_terminal_mq, message_buffer, MESSAGE_MAX_SIZE,
              MESSAGE_PRIORITY);
    }

    pthread_testcancel();
  }

  pthread_exit(NULL);
}
