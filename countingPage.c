#include <stdio.h>

static int countingPageCounter = 0;

int renderCountingPage(char *buf, int sizeOfBuf) {
    countingPageCounter++;

    snprintf(buf, sizeOfBuf,
        "<html>\r\n"
        "<a href='/countingPage'>\r\n"
        "<button type='button'.>Count = %d</button>\r\n"
        "</html>\r\n"
        , countingPageCounter
    );

    return countingPageCounter;
}