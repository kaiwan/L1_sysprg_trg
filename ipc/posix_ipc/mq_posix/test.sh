#!/bin/bash

SENDER=./sender_mq
RECEIVER=./recv_mq

[[ ! -f ${SENDER} || ! -f ${RECEIVER} ]] && make

mount |grep mqueue > /dev/null 2>&1 || {
  mkdir /dev/mq 2>/dev/null
  mount -t mqueue none /dev/mq
}

# kill off any stale instances
pkill ${SENDER}
pkill ${RECEIVER}

${SENDER} "i ate a banana" 8
${SENDER} "i ate an apple" 3
${SENDER} "i ate nothing; i\'m hungry\!" 300
${SENDER} "i ate curry rice, (msg 1 prio 10) :)" 10
${SENDER} "he ate curry rice, (msg 2 prio 10) :)" 10
${SENDER} "she ate curry rice, (msg 3 prio 10) :)" 10
${SENDER} "i ate only health food :(" 2

# Notice how the receiver gets messages in priority order (max->min)
${RECEIVER}

