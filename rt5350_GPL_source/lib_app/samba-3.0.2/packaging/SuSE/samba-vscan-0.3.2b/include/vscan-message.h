#ifndef __VSCAN_MESSAGE_H_
#define __VSCAN_MESSAGE_H_

int vscan_send_warning_message(pstring filename, pstring virname, pstring ipaddr);
void send_message(pstring msg);

#endif /* __VSCAN_MESSAGE_H_ */
