# DisplayToggle

Quickly toggle between any 2 specified presentation modes on Windows. Basically a better version of the `Win`+`P` shortcut for users who frequently swap modes.

## Usage

`> DisplayToggle.exe <primary> <secondary>`

*DisplayToggle* allows the user to specify "primary" and "secondary" presentation modes on the command line. When the program is run, it swaps to the primary mode if the current mode is any other than the primary. It swaps to the secondary mode if the current mode is the primary.

Argument strings map to presentation modes as follows:

| argv         | `Win`+`P`          | [`DISPLAYCONFIG_TOPOLOGY_ID`](https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ne-wingdi-displayconfig_topology_id) |
| ------------ | ------------------ | --------------------------------- |
| `"internal"` | PC screen only     | `DISPLAYCONFIG_TOPOLOGY_INTERNAL` |
| `"clone"`    | Duplicate          | `DISPLAYCONFIG_TOPOLOGY_CLONE`    |
| `"extend"`   | Extend             | `DISPLAYCONFIG_TOPOLOGY_EXTEND`   |
| `"external"` | Second screen only | `DISPLAYCONFIG_TOPOLOGY_EXTERNAL` |

`<primary>` and `<secondary>` must each be one of the above 4 strings, or the program will display an error dialog and take no action. Argument strings are case-insentitive.

If *DisplayToggle* is run with no additional arguments, then the primary mode defaults to `"internal"` and the secondary mode defaults to `"extend"`. The program should always be run with 2 arguments, or none. If any other number of arguments are given, the program will display an error dialog and take no action.

### Usage examples:

`> DisplayToggle.exe` \
Swaps between *PC screen only* and *Extend* presentation modes.

`> DisplayToggle.exe clone internal` \
Swaps between *Duplicate* and *PC screen only* presentation modes.

`> DisplayToggle.exe clone internal extend` \
Error! Too many command line arguments.

`> DisplayToggle.exe internal asdf` \
Error! *"asdf"* is not a valid presentation mode.

### Note for users:

It is recommended to bind a specific combination of arguments to something like a Windows shortcut, batch file, or keyboard macro so the arguments will be the same each time the program is run. This will make the program act like a true *toggle*, allowing for easy single-click swapping between frequently used presentation modes.
