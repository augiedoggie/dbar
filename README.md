## dbar for Haiku OS

A command line tool to help manipulate the contents of the Deskbar shelf.  Allows listing/adding/removing Deskbar replicants.

`dbar` help output:
```
Usage: dbar [-s] [-r] [-k] [-d id/name] [-a /path/to/replicant]
  -s: Scan deskbar items and print ID/Name.
  -r: Restart deskbar.
  -q: Quit deskbar.
  -k: Kill deskbar.
  -d: Delete deskbar item by either ID or Name.
  -a: Add deskbar item using path to the replicant application.
```

### Build Instructions

```
cmake .
make
```
