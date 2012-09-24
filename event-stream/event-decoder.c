#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <endian.h>

#include "events.h"

char* event_to_str(int type) {
  switch (type) {
  case EVENT_CONTEXT_SWITCH:
    return "Context Switch";
  case EVENT_FORK:
    return "Fork";
  case EVENT_DATAGRAM_BLOCK:
    return "Block (datagram)";
  case EVENT_DATAGRAM_RESUME:
    return "Resume (datagram)";
  case EVENT_STREAM_BLOCK:
    return "Block (stream)";
  case EVENT_STREAM_RESUME:
    return "Resume (stream)";
  case EVENT_SOCK_BLOCK:
    return "Block (socket)";
  case EVENT_SOCK_RESUME:
    return "Resume (socket)";
  case EVENT_MUTEX_LOCK:
    return "Lock (mutex)";
  case EVENT_MUTEX_WAIT:
    return "Wait (mutex)";
  case EVENT_SEMAPHORE_LOCK:
    return "Lock (sem)";
  case EVENT_SEMAPHORE_WAIT:
    return "Wait (sem)";
  case EVENT_IO_BLOCK:
    return "Block (IO)";
  case EVENT_IO_RESUME:
    return "Resume (IO)";
  default:
    return "Unknown";
  }
}

void print_event_header(struct event_hdr* header) {

  time_t time = header->tv_sec;
  struct tm *time_tm = localtime(&time);

  char tmbuf[64], buf[64];
  strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", time_tm);
  snprintf(buf, sizeof buf, "%s.%06d", tmbuf, header->tv_usec);

  printf("[%s] <%d> (%5d) %s ", buf, header->cpu, header->pid, event_to_str(header->event_type));
}

void process_simple_event(struct event_hdr* event) {
  print_event_header(event);
  printf("\n");
}

void process_context_switch_event(struct event_hdr* header) {
  struct context_switch_event* event = (struct context_switch_event*) header;
  fread(&event->old_pid, 4, 1, stdin);
  print_event_header(header);
  printf("%5d => %5d\n", event->old_pid, event->new_pid);
}

void process_fork_event(struct event_hdr* header) {
  struct fork_event* event = (struct fork_event*) header;
  fread(&event->pid, 4, 1, stdin);
  print_event_header(header);
  printf("pid:%d tgid:%d\n", event->pid, event->tgid);
}						   

void process_mutex_lock_event(struct event_hdr* header) {
  struct mutex_lock_event* event = (struct mutex_lock_event*) header;
  fread(&event->lock, 4, 1, stdin);
  print_event_header(header);
  printf(" [%0x]\n", event->lock);
}

void process_mutex_wait_event(struct event_hdr* header) {
  struct mutex_wait_event* event = (struct mutex_wait_event*) header;
  fread(&event->lock, 4, 1, stdin);
  print_event_header(header);
  printf(" [%0x]\n", event->lock);
}

void process_sem_lock_event(struct event_hdr* header) {
  struct sem_lock_event* event = (struct sem_lock_event*) header;
  fread(&event->lock, 4, 1, stdin);
  print_event_header(header);
  printf(" [%0x]\n", event->lock);
}

void process_sem_wait_event(struct event_hdr* header) {
  struct sem_wait_event* event = (struct sem_wait_event*) header;
  fread(&event->lock, 4, 1, stdin);
  print_event_header(header);
  printf(" [%0x]\n", event->lock);
}

int main() {
  char raw[100];
  struct event_hdr* event = (struct event_hdr*) &raw;
  
  while (fread(event, sizeof(struct event_hdr), 1, stdin) && !feof(stdin)) {
    switch (event->event_type) {
    case EVENT_CONTEXT_SWITCH:
      process_context_switch_event(event);
      break;
    case EVENT_FORK:
      process_fork_event(event);
      break;
    case EVENT_MUTEX_LOCK:
      process_mutex_lock_event(event);
      break;
    case EVENT_MUTEX_WAIT:
      process_mutex_wait_event(event);
      break;
    case EVENT_SEMAPHORE_LOCK:
      process_sem_lock_event(event);
      break;
    case EVENT_SEMAPHORE_WAIT:
      process_sem_wait_event(event);
      break;
    case EVENT_DATAGRAM_BLOCK:
    case EVENT_DATAGRAM_RESUME:
    case EVENT_STREAM_BLOCK:
    case EVENT_STREAM_RESUME:
    case EVENT_SOCK_BLOCK:
    case EVENT_SOCK_RESUME:
    case EVENT_IO_BLOCK:
    case EVENT_IO_RESUME:
      process_simple_event(event);
      break;
    default:
      printf("Unknown event type received: %d\n", event->event_type);
    }
  }
}
