
# Ghost data

The dex files and info for record-holding ghosts were sourced from the [MK64 Ghost Database](https://docs.google.com/spreadsheets/u/0/d/1cgya3QHo3wLav4cNOn3JfJ1SMtc7CZ89sIflolzQj-w/htmlview#gid=1676616222).



## Ghost info

Each ghost's info is stored in `info.csv`.

```
<track_category>,<player>,<ntsc(pal)_time>
```


## Updating a dex file

First, edit the new ghost's respective entry in `info.csv`.

Next, replace the corresponding old dex file in `dex/` with the updated dex file. Note that you cannot create a new dex file category, as this would require changes to the code base.


### Creating, processing an mpk file

Before proceeding, it will be helpful to understand a little bit about the files that mupen64plus uses to save its data.

- .mpk: emulates saving to the N64 Controller Pak (dex file converted to this)
- .eep: emulates saving to the N64 EEPROM memory

Mupen64plus uses a ROM's "Goodname" value and the first 8 characters of its MD5 checksum separated by a hyphen when it auto-generate its eek and mpk files. For example...
```
Mario Kart 64 (U) [!]-3A67D998.eep
Mario Kart 64 (U) [!]-3A67D998.mpk
```
These values for your ROM can be found in mupen64plus' terminal output.

Now open `rom` and set the `bash` variable `ROM`, which will be used by all of the following scripts, to your MK64 ROM's path.

```bash
vim rom
ROM="$HOME/Games/N64/roms/mk64.n64"   # for example
```

Note that all of the following `bash` scripts use the `config/` directory when starting mupen64plus so as not to interfere with your personal configuration file(s). Our use of mupen64plus here will not require any special settings.

In order for mupen64plus to use a new mpk file, it must be renamed in the above fashion and be in the correct directory. To generate properly-named files, use the `init` script, which launches mupen64plus and stores the mpk and eep files in the `save/` directory. Let mupen64plus run until it reaches the MK64 menu before exiting.

```bash
./init
ls save   # verify presence of mpk,eep files
```

Now we can begin the dex-to-mpk conversion and ghost processing using the `process` script. You must first build the `src/sa_tool/dex_to_mpk.c` tool and put the resulting `dex_to_mpk` binary in your path. 

```bash
cd src/sa_tool
make dex_to_mpk
cd ../../bin
cp dex_to_mpk $HOME/.local/bin  # or wherever
```

Now run the `process` script, which will:
- Create an mpk file in `mpk/` from the dex file in `dex/`
- Run the mpk in mupen64plus, where you can...
  - Verify the record's ghost exists in the Time Trial data menu
  - Remove any ghost saves not related to the record
  - __Not__ do a race or anything else that will result in adding data
    - Use the `run_mpk` script discussed below if you want to test the ghost out
- Copy the mpk file in `save/` along with any changes you made to `mpk/`

The script's argument is the dex file's name without the extension.

```bash
./process LR_3lap
```

And we're done. You should now have an updated mpk file in `mpk/`. You can test it out with `run_mpk` (see below) before rebuilding `mk64tt` with your new ghost!


If you want to edit an existing mpk file, use `edit_mpk`.

```bash
./edit_mpk LR_3lap
```

If you want to run an mpk file without changing it or observe mupen64plus' output, use the `run_mpk` script.

```bash
./run_mpk LR_3lap
```
