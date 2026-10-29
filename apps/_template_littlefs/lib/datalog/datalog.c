#include <zephyr/kernel.h>
#include <zephyr/fs/fs.h>
#include "datalog.h"

#define CUR DATALOG_DIR "/log.bin"
#define OLD DATALOG_DIR "/log.old"

static uint32_t seq;
static uint32_t rotations;

int datalog_count(void)
{
	struct fs_dirent st;

	return fs_stat(CUR, &st) == 0 ? (int)(st.size / sizeof(struct record)) : 0;
}

static int rotate(void)
{
	fs_unlink(OLD); /* may not exist */
	rotations++;
	return fs_rename(CUR, OLD);
}

int datalog_append(int32_t value)
{
	struct record r = {.seq = seq++, .uptime_ms = k_uptime_get_32(), .value = value};
	struct fs_file_t f;
	int err;

	if (datalog_count() >= DATALOG_MAX_RECORDS) {
		err = rotate();
		if (err) {
			return err;
		}
	}
	fs_file_t_init(&f);
	err = fs_open(&f, CUR, FS_O_CREATE | FS_O_APPEND | FS_O_WRITE);
	if (err) {
		return err;
	}
	err = fs_write(&f, &r, sizeof(r)) == sizeof(r) ? 0 : -EIO;
	fs_close(&f); /* close = commit: a reset after this keeps the record */
	return err;
}

int datalog_read(int index, struct record *r)
{
	struct fs_file_t f;
	int err;

	fs_file_t_init(&f);
	err = fs_open(&f, CUR, FS_O_READ);
	if (err) {
		return err;
	}
	err = fs_seek(&f, (off_t)index * sizeof(*r), FS_SEEK_SET);
	if (!err) {
		err = fs_read(&f, r, sizeof(*r)) == sizeof(*r) ? 0 : -ENOENT;
	}
	fs_close(&f);
	return err;
}

uint32_t datalog_rotations(void)
{
	return rotations;
}
