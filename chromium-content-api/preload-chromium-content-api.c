/* Copyright (C) 2020 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define __USE_GNU

#include <dlfcn.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static int (*original_open)(const char *, int, ...);
static int (*original_unlink)(const char *);


void debug(char *fmt, ...)
{
	if (getenv("SHM_CHROMIUM_DEBUG")) {
		va_list va;
		va_start(va, fmt);
		fprintf(stderr, "SHM_CHROMIUM_WRAP: ");
		vfprintf(stderr, fmt, va);
		va_end(va);
		fprintf(stderr, "\n");
	}
}

const char *get_snap_name(void)
{
	const char *snapname = getenv("SNAP_INSTANCE_NAME");
	if (snapname == NULL) {
		snapname = getenv("SNAP_NAME");
	}
	if (snapname == NULL) {
		debug("SNAP_NAME and SNAP_INSTANCE_NAME not set");
	}
	return snapname;
}


static char * redirect_shm(const char *snapname, const char *pathname)
{
	size_t max_len = PATH_MAX + 1;
	/* New path includes snap.<SNAP_INSTANCE_NAME> and NULL.  == 6 + 1 */
	size_t rewritten_len = strlen(snapname) + strlen(pathname) + 6 + 1;
	
	if (rewritten_len >= max_len) {
		debug("path too long, ignoring: %s", pathname);
		return NULL;
	}

	/* /dev/shm/shm- == 13  */
	char remainder[rewritten_len - 13];
	int filled = sscanf(pathname, "/dev/shm/shm-%s", remainder);
	if (filled < 1) {
		return NULL;
	}

	char *rewritten = (char *)malloc(rewritten_len);
	if (rewritten == NULL) {
		return NULL;
	}

	int n = snprintf(rewritten, rewritten_len, "/dev/shm/snap.%s.shm-%s", snapname, remainder);
	if (n < 0 || n >= rewritten_len) {
		fprintf(stderr, "rewrite truncated for %s (%s)\n", pathname, rewritten);
	}

	// Ensure null-terminated.
	rewritten[rewritten_len - 1] = '\0';

	return rewritten;
}

int open(const char *pathname, int flags, ...)
{
	mode_t mode = 0;

	va_list args;
	va_start(args, flags);

	if (flags & (O_CREAT | O_TMPFILE)) {
		mode = va_arg(args, mode_t);
	}

	va_end(args);

	if (original_open == NULL) {
		original_open = (int (*)(const char *, int, ...)) dlsym(RTLD_NEXT, "open");
	}

	const char *snapname = get_snap_name();
	if (snapname == NULL) {
		return original_open(pathname, flags, mode);
	}

	char *redirected_path = redirect_shm(snapname, pathname);
	if (redirected_path == NULL) {
		return original_open(pathname, flags, mode);
	}

	debug("[open pre-call] redirected_path=%s (from path=%s)", redirected_path, pathname);
	int result = original_open(redirected_path, flags, mode);
	debug("[open post-call] redirected_path=%s ret=0x%x", redirected_path, result);

	free(redirected_path);
	return result;
}

int unlink(const char *pathname)
{
	if (original_unlink == NULL) {
		original_unlink = (int (*)(const char *)) dlsym(RTLD_NEXT, "unlink");
	}

	const char *snapname = get_snap_name();
	if (snapname == NULL) {
		return original_unlink(pathname);
	}

	char *redirected_path = redirect_shm(snapname, pathname);
	if (redirected_path == NULL) {
		return original_unlink(pathname);
	}

	debug("[unlink pre-call] redirected_path=%s (from path=%s)", redirected_path, pathname);
	int result = original_unlink(redirected_path);
	debug("[unlink post-call] redirected_path=%s ret=0x%x", redirected_path, result);

	free(redirected_path);
	return result;
}
