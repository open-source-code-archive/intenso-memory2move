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

//--------------------------------useful definitions----------------------------
#define	VERBOSE		0
#define	PATH		0
#define	SILENT		0x01
#define	SHMEM		0x04


/*****************************************************************************
*		  Kaspersky Anti-Virus client C library			     *
*	(an implementation of Kaspersky Anti-Virus daemon interface)	     *
******************************************************************************/
//KAVConnect()- create a socket and connect it to KAVDaemon is running
//  path_to daemon - path to AvpCtl file
//  possible flags -{VERBOSE/SILENT}
int KAVConnect(char *path_to_daemon_ctl,char flags);

/*KAVRequestShmem() - send request string to KAVDaemon via created socket
   using shared memory
- "kav_socket" - descriptor of an open socket, connected to kavdaemon
- "path" may point to a single file;
- "keyname" is a filename to use in shmem key construction
- "chSHM" is a character	---	""	---
- "flags" can be a disjunction of elements of the following groups:
	{VERBOSE/SILENT}- client writes error messages and over info in stderr
		{VERBOSE,default} or remains silent while working{SILENT}
 "default" means that flags has 0 in certain position in this case, and since
 flags is equal to 0 by default, this flag is default;*/
int KAVRequestShmem(int kav_socket, char *path, char *keyname, char chSHM,char flags);

/*KAVRequestPath() - send simple one-target request, transmitting a path
  to the target.
- "kav_socket" - descriptor of an open socket, connected to kavdaemon
- "path" may point to a single file or to a directory
- "flags" - {VERBOSE/SILENT}*/
int KAVRequestPath(int kav_socket, char *path, char flags);

/*KAVRequestMulti() - send multi-target request, transmitting a path
to the target(s), a set of dirs, allowed to scan in and keys for KAVdaemon
- "kav_socket" - descriptor of an open socket, connected to kavdaemon
flags - {VERBOSE/SILENT}
- "keys" is an array of keys to KAVDaemon for current request ending with 0;
- "paths" is an array of paths, in which KAVDaemon can perform scanning/curing,
	ending with 0 (NULL);
- "flags" can be {VERBOSE/SILENT}
*/
int KAVRequestMulti(int kav_socket, char** keys,char** paths,char flags);

/*KAVResponse() - receive response from KAVDaemon on a previously sent request,
fills account buffer if account info is provided and returns a pointer to it.
by KAVDaemon. Returns 0 in case of an error
- "kav_socket" - descriptor of an open socket, connected to kavdaemon
- "exit code is a pointer to an int variable to store kavdaemon's exit code
- "file" argument is used to write cured file to if SHMEM flag is set;
- possible flags -{VERBOSE/SILENT}|{PATH/SHMEM}
	{PATH/SHMEM}	- file was transferred to daemon as its path
	{PATH,default} or in a shared memory{SHMEM}*/
char* KAVResponse(int kav_socket,int* exit_code,char flags,char* file);

/*KAVVersion() - sends enquiry for version returns a ointer to a buffer
with version info or 0 in case of an error
- "kav_socket" - descriptor of an open socket, connected to kavdaemon
- "flags" can be {VERBOSE/SILENT}
*/
char* KAVVersion(int kav_socket, char flags);

/*KAVClose() - close a session with KAV Daemon
- "kav_socket" - descriptor of an open socket, connected to kavdaemon
possible flags -{VERBOSE/SILENT}*/
int KAVClose(int kav_socket,char flags);


//All functions except KAVResponse and KAVVersion return <0 in case of error.
//------------------------------------------------------------------------------
//-------------------------------helpers----------------------------------------

/*isadir() - returns 0 in case of "path" is a file,
  1 in case of "path" is a directory
 -1 in case of error*/
int isadir(const char* path,char flags);

//------------------------------------------------------------------------------
