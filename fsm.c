#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW 4
#define MAX_SEQ 8
#define FRAMES 10

int send_base = 0, next_seq = 0, recv_expected = 0;

int errorOccur() {
    return rand() % 10 < 2;   
}

void sendFrame(int frame) {
    printf("Sender: Frame %d transmitted\n", frame);
}

void retransmit() {
    printf("Sender: Timeout! Retransmitting...\n");
    for (int i = send_base; i != next_seq; i = (i + 1) % MAX_SEQ) {
        printf("   Retransmit Frame %d\n", i);
    }
}

void senderSide() {
    if (((next_seq - send_base + MAX_SEQ) % MAX_SEQ) < WINDOW) {
        sendFrame(next_seq);
        next_seq = (next_seq + 1) % MAX_SEQ;
    } else {
        printf("Sender: Window full, waiting...\n");
        retransmit();
    }
}

void receiverSide(int frame) {
    if (errorOccur()) {
        printf("   Receiver: Frame %d corrupted, discarded\n", frame);
        return;
    }
    if (frame == recv_expected) {
        printf("   Receiver: Frame %d received correctly\n", frame);
        recv_expected = (recv_expected + 1) % MAX_SEQ;
    } else {
        printf("   Receiver: Frame %d out of order, expected %d\n", frame, recv_expected);
    }
    printf("   Receiver: Sending ACK %d\n", recv_expected);
}

void processAck(int ack) {
    if (errorOccur()) {
        printf("   Sender: ACK corrupted, ignored\n");
        return;
    }
    if ((ack > send_base && ack <= next_seq) || (next_seq < send_base && (ack > send_base || ack <= next_seq))) {
        printf("   Sender: ACK %d received, sliding window\n", ack);
        send_base = ack;
    } else {
        printf("   Sender: Invalid ACK %d\n", ack);
    }
}

int main() {
    srand(time(NULL));
    int count = 0;
    while (count < FRAMES) {
        senderSide();
        receiverSide((next_seq - 1 + MAX_SEQ) % MAX_SEQ);
        processAck(recv_expected);
        count++;
    }
    return 0;
}
