#include "uart.h"
#include <fcntl.h>
#include <shared.h>
#include <threads.h>

#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// Queue declaration
mqd_t uart_to_terminal_mq;
mqd_t terminal_to_uart_mq;

// Serial declaration
serial_t serial_port = (serial_t)-1;

static pthread_t recv_thread_handler = (pthread_t)0;
static pthread_t send_thread_handler = (pthread_t)0;

static void exit_signal_handler(int signum);

int main(int argc, char **argv) {

  if (argc != 2) {
    fprintf(stderr, "Bad usage: ./uart <serial_port_path>\n");
    return EXIT_FAILURE;
  }

  // Open UART port
  if ((serial_port = open(argv[1], O_RDWR)) <= 0) {
    perror("[UART: SerialError]\t");
    return EXIT_FAILURE;
  }

  // Read UART properties
  struct termios tty;
  if (tcgetattr(serial_port, &tty) != 0) {
    perror("[UART: SerialError]\t");
    close(serial_port);
    return EXIT_FAILURE;
  }

  // Set new properties
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    perror("[UART: SerialError]\t");
    close(serial_port);
    return EXIT_FAILURE;
  }

  // Interrupt signal handler
  if (signal(SIGINT, exit_signal_handler) == SIG_ERR) {
    perror("[UART: Error]\tFailed to set signal handler");
    return EXIT_FAILURE;
  }

  // Queue attributes
  struct mq_attr attr = {.mq_flags = 0,
                         .mq_maxmsg = QUEUE_MAX_MESSAGES,
                         .mq_msgsize = MESSAGE_MAX_SIZE,
                         .mq_curmsgs = 0};

  // Open UART to Terminal queue
  uart_to_terminal_mq =
      mq_open(UART_TO_TERMINAL_QUEUE_NAME, O_CREAT | O_WRONLY, 0666, &attr);
  if (uart_to_terminal_mq == (mqd_t)-1) {
    perror("[UART: Error]\tQueue creation");
    exit(EXIT_FAILURE);
  }

  terminal_to_uart_mq = mq_open(TERMINAL_TO_UART_QUEUE_NAME,
                                O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
  if (terminal_to_uart_mq == (mqd_t)-1) {
    perror("[UART: Error]\tQueue creation");
    mq_close(uart_to_terminal_mq);
    exit(EXIT_FAILURE);
  }

  printf("[UART]\tInitialized\n");
  fflush(stdout);

  // Create the threads
  pthread_create(&send_thread_handler, NULL, &send_thread, NULL);
  pthread_create(&recv_thread_handler, NULL, &recv_thread, NULL);

  printf("[UART]\tCreated threads\n");
  fflush(stdout);

  // Join the threads
  pthread_join(send_thread_handler, NULL);
  pthread_join(recv_thread_handler, NULL);

  printf("[UART]\tFinished with thread cancellation\n");
  fflush(stdout);

  // Close queues
  mq_close(uart_to_terminal_mq);
  mq_close(terminal_to_uart_mq);

  // Close serial
  close(serial_port);

  return EXIT_SUCCESS;
}

void exit_signal_handler(int signum) {
  printf("\n[UART]\tExecution interruption, cancelling threads\n");
  fflush(stdout);

  // Send termination signals
  pthread_cancel(send_thread_handler);
  pthread_cancel(recv_thread_handler);
}
