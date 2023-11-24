#include "file.h"
#include <shared.h>
#include <threads.h>

#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

void *recv_thread(void *params) {
  ssize_t bytes_recv;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  while (1) {
    /* Create the message buffer */
    message_t message_buffer;
    memset(message_buffer, 0, sizeof(message_buffer));

    /* Recieve the message */
    bytes_recv =
        mq_receive(uart_to_terminal_mq, message_buffer, MESSAGE_MAX_SIZE, NULL);

    if (bytes_recv > 0L) {
      /* Print the message */
      printf("[Terminal: RecvUart]\tRecieved `%s`\n", message_buffer);
      fflush(stdout);

      /* Store it in output file */
      write(output_file, message_buffer, strlen(message_buffer));
    }

    pthread_testcancel();
  }

  pthread_exit(NULL);
}

static void send_cleanup_routine(void *terminal_param) {
  struct termios *terminal_prev = (struct termios *)terminal_param;
  tcsetattr(STDIN_FILENO, TCSANOW, terminal_prev);
  printf("[Terminal: SendCleanup]\ttcsetattr done\n");
  fflush(stdout);
}

void *send_thread(void *params) {

  struct termios terminal_prev;
  struct termios terminal_conf;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  /* Get copy of the terminal configuration */
  tcgetattr(STDIN_FILENO, &terminal_prev);
  /* Set cleanup routine */
  pthread_cleanup_push(send_cleanup_routine, &terminal_prev);

  /* Get new terminal configuration */
  terminal_conf = terminal_prev;
  terminal_conf.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &terminal_conf);

  while (1) {
    /* Prepare buffer */
    message_t terminal_buffer;
    memset(terminal_buffer, 0, sizeof(terminal_buffer));

    /* Read character for terminal */
    if (read(STDIN_FILENO, terminal_buffer, sizeof(terminal_buffer))) {
      /* On character arrival */
      printf("[Terminal: ConsoleRead]\tRead `%s`\n", terminal_buffer);

      /* Send to queue */
      mq_send(terminal_to_uart_mq, terminal_buffer, sizeof(terminal_buffer),
              MESSAGE_PRIORITY);
    }

    pthread_testcancel();
  }

  /* Return to previous state */
  pthread_cleanup_pop(NULL);
  pthread_exit(NULL);
}
