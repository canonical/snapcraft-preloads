# snapcraft-preloads
Individual libraries that can be preloaded in snaps to ensure behavior is
consistent with confinement rules.

## Available Libraries
### semaphores
Rewrite paths for `sem_open` and `sem_unlink` for them to write in the
appropriate paths available to a given snap.

## Building
To build all projects, just run `make` at the root of this project.

Each subdirectory can be individually built using `make -C <dir>`.

A resulting `libpreload-<dir>.so` is the output of each `Makefile` which can be
install with `make install` to its default target of `$DESTDIR/libpreload-<dir>.so`

## Using
In the given snap where it is to be used, add the library to the list of
`LD_PRELOAD` libraries (`:` separated list).
