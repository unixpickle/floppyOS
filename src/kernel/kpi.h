/**
 * puts a character at the current cursor position
 */
void kputc (char c);

/**
 * sets the cursor index on the screen
 */
void kcurmove (unsigned short cursorIndex);

/**
 * gets the cursor index on the screen
 */
int kcurpos ();

/**
 * scrolls down by one line
 */
void kscroll();

void kgettime (unsigned char * seconds, unsigned char * minutes, unsigned char * hours, unsigned char * weekday, unsigned char * monthday, unsigned char * month, unsigned char * year, unsigned char * century);

unsigned int kontime ();
void kaddtime (unsigned int numMS);
void ksettime (unsigned int numMS);
