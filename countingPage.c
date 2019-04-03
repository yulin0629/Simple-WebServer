#include <stdio.h>

static int countingPageCounter = 0;

void renderCountingPage(char *buf, int sizeOfBuf, int number) {

  snprintf(buf, sizeOfBuf,
           "<html>\r\n"
           "<a href='/countingPage'>\r\n"
           "<button type='button'.>Count = %d</button>\r\n"
           "</html>\r\n",
           number);
}