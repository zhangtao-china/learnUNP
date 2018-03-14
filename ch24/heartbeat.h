#ifndef HEART_BEAT_H
#define HEART_BEAT_H

void heartbeat_cli(int servfd_arg, int nsec_arg, int maxnprobes_arg);

void heartbeat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg);

#endif