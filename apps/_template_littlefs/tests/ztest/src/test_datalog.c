#include <zephyr/ztest.h>
#include <zephyr/fs/fs.h>
#include "datalog.h"

FS_FSTAB_DECLARE_ENTRY(DT_NODELABEL(lfs1));

static void before(void *f)
{
	ARG_UNUSED(f);
	fs_unlink(DATALOG_DIR "/log.bin");
	fs_unlink(DATALOG_DIR "/log.old");
}

ZTEST_SUITE(datalog, NULL, NULL, before, NULL, NULL);

ZTEST(datalog, test_append_and_read)
{
	struct record r;

	for (int i = 0; i < 5; i++) {
		zassert_ok(datalog_append(i * 100));
	}
	zassert_equal(datalog_count(), 5);
	zassert_ok(datalog_read(3, &r));
	zassert_equal(r.value, 300);
	zassert_equal(datalog_read(5, &r), -ENOENT);
}

ZTEST(datalog, test_rotation)
{
	struct fs_dirent st;
	uint32_t rot = datalog_rotations();

	for (int i = 0; i < DATALOG_MAX_RECORDS + 3; i++) {
		zassert_ok(datalog_append(i), "append %d", i);
	}
	zassert_equal(datalog_rotations(), rot + 1);
	zassert_equal(datalog_count(), 3);
	zassert_ok(fs_stat(DATALOG_DIR "/log.old", &st));
	zassert_equal(st.size, DATALOG_MAX_RECORDS * sizeof(struct record));
}

ZTEST(datalog, test_survives_remount)
{
	struct fs_mount_t *mp = &FS_FSTAB_ENTRY(DT_NODELABEL(lfs1));
	struct record r;

	zassert_ok(datalog_append(777));
	zassert_ok(fs_unmount(mp));
	zassert_ok(fs_mount(mp));
	zassert_equal(datalog_count(), 1);
	zassert_ok(datalog_read(0, &r));
	zassert_equal(r.value, 777);
}
