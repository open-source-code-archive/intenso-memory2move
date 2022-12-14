#ifndef __VSCAN_FILEACCESSLOG_H_
#define __VSCAN_FILEACCESSLOG_H_

#include <includes.h>

#define MAX_LRUFILES 100
#define LRUFILES_INVALIDATE_TIME 10  

struct lrufiles_struct {
	struct lrufiles_struct *prev, *next;
	fstring fname;		/* the file name */
	time_t mtime;		/* mtime of file */
	BOOL infected;		/* infected? */
	time_t time_added;	/* time entry was added to list */
};
void lrufiles_init(int max_enties, time_t invalidate_time);
struct lrufiles_struct *lrufiles_add(fstring fname, time_t mtime, BOOL infected);
void lrufiles_destroy_all();
struct lrufiles_struct *lrufiles_search(fstring fname);
void lrufiles_delete(fstring fname);
int lrufiles_must_be_checked (fstring fname, time_t mtime);

#endif /* __VSCAN_FILEACCESSLOG_H_ */
