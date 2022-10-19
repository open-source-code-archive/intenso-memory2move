// Copyright (c) 2002, Kaspersky Lab.
// All rights reserved.
//       
// Redistribution  and  use  in  source  and  binary  forms,  with  or  without
// modification, are permitted provided that the following conditions are met:
// 		  
//     - Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimer. 
//     - Redistributions in binary form  must  reproduce  the  above  copyright
//       notice, this list of conditions  and  the  following disclaimer in the
//       documentation and/or other materials provided with the distribution. 
//     - Neither  the  name  of  the  Kaspersky  Lab.  nor  the  names  of  its
//       contributors may be used to endorse or promote  products  derived from
//       this software without specific prior written permission. 
// 									
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND  CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT  LIMITED  TO,  THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  PARTICULAR  PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  OWNER  OR  CONTRIBUTORS  BE
// LIABLE  FOR  ANY   DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
// CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT  LIMITED  TO,  PROCUREMENT  OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA,  OR PROFITS;  OR  BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY  OF  LIABILITY,  WHETHER  IN
// CONTRACT, STRICT LIABILITY, OR  TORT  (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED  OF  THE
// POSSIBILITY OF SUCH DAMAGE.

/* 2003.02.15 kjw@rightsock.com - patched several memory and file descriptor leaks */

/* #ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

this is done via <includes.h> below - Rainer Link <rainer@openantivirus.org>

*/

#include "helpers.h"
#include <includes.h>

int timeoutread(int timeout,int fd,char *buf,int len)
{
  fd_set rfds;
  struct timeval tv;

  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  FD_ZERO(&rfds);
  FD_SET(fd,&rfds);

  if (select(fd + 1,&rfds,(fd_set *) 0,(fd_set *) 0,&tv) == -1) 
      return -1;
      
  if (FD_ISSET(fd,&rfds)) 
      return read(fd,buf,len);

  errno = ETIMEDOUT;
  return -1;
}

int timeoutwrite(int timeout , int fd, char *buf, int len)
{
  fd_set wfds;
  struct timeval tv;

  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  FD_ZERO(&wfds);
  FD_SET(fd,&wfds);

  if (select(fd + 1,(fd_set *) 0,&wfds,(fd_set *) 0,&tv) == -1) 
      return -1;
      
  if (FD_ISSET(fd,&wfds)) 
      return write(fd,buf,len);

  errno = ETIMEDOUT;
  return -1;
}

#define INT_BUF_LEN	512

static char int_buffer[INT_BUF_LEN];
static int int_buf_len = 0;
static int int_buf_pos = 1;

int get_timeout_ch(int fd, int timeout, char *ch)
{
    if (int_buf_pos > int_buf_len)
    {
	if ((int_buf_len = timeoutread(timeout, fd,int_buffer, INT_BUF_LEN)) <= 0 ) return -1;
	int_buf_pos = 0;
    }
    
    *ch = int_buffer[int_buf_pos];
    int_buf_pos++;
    
    return 1;
}

int out(int socket, int timeout, char *str)
{
    return timeoutwrite(timeout, socket, str, strlen(str));
}

