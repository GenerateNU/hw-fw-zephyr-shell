# Zephyr Shell Template

A starting point for a Zephyr app with the [Zephyr shell](https://docs.zephyrproject.org/latest/services/shell/index.html) turned on and every general purpose built-in shell command enabled (kernel, device, gpio, i2c, spi, sensor, flash, adc, pwm, log, ...). Flash it to a board, open a serial terminal, and you can poke at the hardware without writing any code.

# Repo layout

```c
zephyr-shell-template/
	| - west.yml            // which zephyr version + modules (HALs) west downloads
	| - app/
		| - CMakeLists.txt  // lists the app's source files
		| - Kconfig         // turns on all the built-in shell command modules
		| - prj.conf        // shell settings (history, tab complete, colors, logging, ...)
		| - src/main.c      // your application code, empty except for a startup log
```

# Using this repo

1. Create a new GitHub repo from this template ("Use this template" on GitHub).

2. Make sure your vendor's HAL is in `west.yml`. Only the modules listed under `name-allowlist` get downloaded. `hal_nordic` and `hal_espressif` are already there. For other vendors, add theirs (for example `hal_stm32`, `hal_nxp`, `hal_rpi_pico`; search `zephyr/west.yml` for your vendor), then run `west update` again.

3. Build from inside the repo folder. Run `west boards` to find your board's name.
```c
cd zephyr-shell-template
west build -p always -b nrf52840dk/nrf52840 app          // nRF52840 DK
west build -p always -b nrf54l15dk/nrf54l15/cpuapp app   // nRF54L15 DK
west build -p always -b esp32_devkitc/esp32/procpu app   // ESP32 DevKitC (needs: pip install esptool)
```
`-p always` does a clean build. Use it when you switch boards or change `prj.conf`/`Kconfig`. Otherwise plain `west build` is faster.

4. Flash
```c
west flash
```
Nordic boards need [nrfutil](https://www.nordicsemi.com/Products/Development-tools/nRF-Util) or SEGGER J-Link installed for this.

# Opening the shell

The shell starts on its own when the board boots, on the same UART as the console (`zephyr,shell-uart` in the board's devicetree). You just need a serial terminal at **115200 baud**.

1. Find the port
```c
ls /dev/tty.usbmodem*   // macOS (Nordic DKs, most dev kits)
ls /dev/ttyACM* /dev/ttyUSB*   // Linux / WSL
```
Some boards, like the nRF54L15 DK, show up as two ports. The shell is usually on the lower-numbered one.

2. Connect
```c
screen /dev/tty.usbmodem<...> 115200   // quit with ctrl+a, k, y
west espressif monitor                  // espressif boards
```
`minicom`, `picocom`, PuTTY, the nRF Connect Serial Terminal, or VS Code's Serial Monitor extension also work.

3. Press **Enter** and you should see `zephyr:~$ `. If nothing shows, press the board's reset button, and check that no other program has the port open.

# Using the shell

- `tab` lists or completes commands, subcommands **and device names**. Use it a lot.
- `up`/`down` arrows scroll through history.
- Add `-h` to any command for its help, e.g. `i2c -h` or `i2c scan -h`.
- `select <cmd>` stays "inside" a command so you can skip retyping its name. Press `alt+r` to go back.
- `ctrl+a`/`ctrl+e` (start/end of line), `ctrl+w` (delete word), `ctrl+l` (clear screen).

Device arguments are the device's devicetree name, e.g. `i2c@40003000` or `gpio@50000000`. Run `device list` to see them, or press `tab` where a device goes.

## Built-in commands

Which ones show up depends on your board. A command only exists if the SoC has that peripheral and the driver is supported.

| Command | Examples | What it does |
| --- | --- | --- |
| `help` | `help` | list every command |
| `kernel` | `kernel uptime`, `kernel version`, `kernel thread list`, `kernel thread stacks`, `kernel heap`, `kernel reboot cold` | system info, threads and their CPU/stack usage, reboot |
| `device` | `device list` | every device and whether its driver initialized |
| `log` | `log status`, `log enable dbg app`, `log disable i2c_nrfx_twim` | change log levels per module at runtime |
| `gpio` | `gpio info`, `gpio get gpio@50000000 13`, `gpio conf gpio@50000000 13 o`, `gpio set gpio@50000000 13 1`, `gpio blink gpio@50000000 13` | read/write/configure any pin |
| `i2c` | `i2c scan i2c@40003000`, `i2c read_byte i2c@40003000 0x76 0xD0`, `i2c write_byte <dev> <addr> <reg> <val>` | find devices on an I2C bus, read/write registers |
| `spi` | `spi conf <dev> 1000000`, `spi transceive <dev> 0x9f 0x00 0x00` | configure a bus and send/receive bytes |
| `sensor` | `sensor get <dev>`, `sensor info` | read any sensor that has a Zephyr driver |
| `adc` | `adc <dev> read <channel> [period_ms]` | read an ADC channel |
| `pwm` | `pwm cycles <dev> <channel> <period> <pulse>`, `pwm usec ...` | drive a PWM output |
| `flash` | `flash read <dev> 0x0 16`, `flash erase ...`, `flash write ...` | read/erase/write flash (careful, you can erase your firmware) |
| `flash_map` | `flash_map list` | show the flash partitions |
| `uart` | `uart write <dev> hello`, `uart read <dev> 5`, `uart baudrate <dev> 9600` | talk to another UART |
| `hwinfo` | `hwinfo devid`, `hwinfo reset_cause` | unique chip ID and why it last reset |
| `date` | `date get`, `date set 2026-01-01 12:00:00` | wall clock time (resets on reboot without an RTC) |
| `devmem` | `devmem 0x40000000 32` | read/write raw memory or registers (careful) |
| `counter`, `wdt`, `rtc`, `eeprom`, `led`, `regulator`, `dac`, `can`, `i2s`, `comp`, `input`, `crc`, `stats` | press `tab` after the name | drivers for those peripherals, shown only if the board has them |
| `history`, `clear`, `resize`, `retval`, `shell` | `shell colors off`, `shell stats show` | shell utilities |

## Turning commands on/off

- Every built-in command module is enabled in `app/Kconfig` with `imply`. That turns an option on only when the board supports it, so the template builds on any board without warnings.
- To remove one, set it off in `app/prj.conf`, e.g. `CONFIG_CAN_SHELL=n`. Each module costs flash, so turn off the ones you don't need when your app gets bigger.
- Shell behaviour (history size, prompt text, colors, stack size, logging) is set in `app/prj.conf`. Each line has a comment.
- Bluetooth (`bt`), networking (`net`), Wi-Fi (`wifi`), and filesystem (`fs`) shells are off. They also turn on large subsystems, so enable them only if your app uses them: `CONFIG_BT_SHELL`, `CONFIG_NET_SHELL`, `CONFIG_WIFI_SHELL`/`CONFIG_NET_L2_WIFI_SHELL`, `CONFIG_FILE_SYSTEM_SHELL`.

## Adding your own command

```c
#include <zephyr/shell/shell.h>

static int cmd_mycmd(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "got %zu args", argc - 1);   // shell_warn / shell_error print in color
    return 0;                                    // negative errno on failure, shows in "retval"
}

// name, subcommands (NULL if none), help text, handler, mandatory args (incl. the name), optional args
SHELL_CMD_ARG_REGISTER(mycmd, NULL, "My command", cmd_mycmd, 1, 2);
```

For a command with subcommands (like `mycmd on` / `mycmd off`), use `SHELL_STATIC_SUBCMD_SET_CREATE`. See the [shell docs](https://docs.zephyrproject.org/latest/services/shell/index.html#commands).

Note: the shell owns the console UART, so use `LOG_INF()`/`LOG_ERR()` instead of `printk()`. Logs then print through the shell without breaking the prompt.
