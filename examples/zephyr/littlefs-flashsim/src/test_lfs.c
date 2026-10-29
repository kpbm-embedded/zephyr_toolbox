#include <stdio.h>
#include <string.h>
#include <zephyr/ztest.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>

FS_FSTAB_DECLARE_ENTRY(DT_NODELABEL(lfs1));
#define MNT "/lfs"
static struct fs_mount_t *mp = &FS_FSTAB_ENTRY(DT_NODELABEL(lfs1));

static int write_file(const char *path, const void *data, size_t len, fs_mode_t mode)
{
	struct fs_file_t f;
	ssize_t n;

	fs_file_t_init(&f);
	if (fs_open(&f, path, FS_O_CREATE | FS_O_WRITE | mode) < 0) {
		return -1;
	}
	n = fs_write(&f, data, len);
	fs_close(&f);
	return n == (ssize_t)len ? 0 : -1;
}

static ssize_t read_file(const char *path, void *buf, size_t len)
{
	struct fs_file_t f;
	ssize_t n;

	fs_file_t_init(&f);
	if (fs_open(&f, path, FS_O_READ) < 0) {
		return -1;
	}
	n = fs_read(&f, buf, len);
	fs_close(&f);
	return n;
}

ZTEST_SUITE(littlefs, NULL, NULL, NULL, NULL, NULL);

ZTEST(littlefs, test_automounted)
{
	struct fs_statvfs st;

	zassert_ok(fs_statvfs(MNT, &st), "fstab automount did not mount " MNT);
	zassert_equal(st.f_frsize, 1024, "block = erase block of the simulated flash");
	zassert_equal(st.f_blocks, 32);
	zassert_true(st.f_bfree > 20);
}

ZTEST(littlefs, test_write_read)
{
	char buf[32] = {0};

	zassert_ok(write_file(MNT "/hello.txt", "hello flash", 11, FS_O_TRUNC));
	zassert_equal(read_file(MNT "/hello.txt", buf, sizeof(buf)), 11);
	zassert_str_equal(buf, "hello flash");
}

ZTEST(littlefs, test_append_and_stat)
{
	struct fs_dirent st;

	zassert_ok(write_file(MNT "/log.txt", "a", 1, FS_O_TRUNC));
	zassert_ok(write_file(MNT "/log.txt", "bc", 2, FS_O_APPEND));
	zassert_ok(fs_stat(MNT "/log.txt", &st));
	zassert_equal(st.type, FS_DIR_ENTRY_FILE);
	zassert_equal(st.size, 3);
}

ZTEST(littlefs, test_dirs_rename_unlink)
{
	struct fs_dir_t dir;
	struct fs_dirent e;
	int files = 0;

	fs_mkdir(MNT "/data"); /* may exist from an earlier case */
	zassert_ok(write_file(MNT "/data/a.bin", "1", 1, FS_O_TRUNC));
	zassert_ok(write_file(MNT "/data/b.bin", "2", 1, FS_O_TRUNC));
	fs_unlink(MNT "/data/c.bin");
	zassert_ok(fs_rename(MNT "/data/b.bin", MNT "/data/c.bin"));
	zassert_ok(fs_unlink(MNT "/data/a.bin"));

	fs_dir_t_init(&dir);
	zassert_ok(fs_opendir(&dir, MNT "/data"));
	while (fs_readdir(&dir, &e) == 0 && e.name[0] != '\0') {
		zassert_str_equal(e.name, "c.bin");
		files++;
	}
	fs_closedir(&dir);
	zassert_equal(files, 1);
}

ZTEST(littlefs, test_file_spanning_blocks)
{
	static uint8_t out[3000], in[3000];

	for (size_t i = 0; i < sizeof(out); i++) {
		out[i] = (uint8_t)(i * 7);
	}
	zassert_ok(write_file(MNT "/big.bin", out, sizeof(out), FS_O_TRUNC));
	zassert_equal(read_file(MNT "/big.bin", in, sizeof(in)), sizeof(in));
	zassert_mem_equal(in, out, sizeof(out));
}

ZTEST(littlefs, test_survives_remount)
{
	char buf[16] = {0};

	zassert_ok(write_file(MNT "/keep.txt", "kept", 4, FS_O_TRUNC));
	zassert_ok(fs_unmount(mp));
	zassert_not_equal(fs_statvfs(MNT, &(struct fs_statvfs){0}), 0, "unmounted");
	zassert_ok(fs_mount(mp));
	zassert_equal(read_file(MNT "/keep.txt", buf, sizeof(buf)), 4);
	zassert_str_equal(buf, "kept");
}
