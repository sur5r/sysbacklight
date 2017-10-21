# sysbacklight

`sysbacklight` is a tiny tool meant to be used on configurations where `xbacklight` does not work.

It changes the display brightness on the first sysfs backlight device of type raw.

It is written in C using `libsysfs` so it is possible to install it SUID-root.

## License
`sysbacklight` is free software under the GNU AGPL version 3. See `LICENSE` for details.

## Dependencies
`sysbacklight` needs libsysfs.

On Debian, you can install them with:

`# apt install libsysfs-dev`

## Usage

### Report the current brightness level in percent

`sysbacklight` or `sysbacklight ?`

### Set brightness to 42%

`sysbacklight = 42`

### Increase brightness by 10%
`sysbacklight + 10`

### Increase brightness by 10%
`sysbacklight - 10`

