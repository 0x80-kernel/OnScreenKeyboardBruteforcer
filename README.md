# PIN Automation Tool

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

A utility for automating PIN entry by simulating mouse clicks on a virtual keypad. This tool is particularly useful for testing PIN-based security systems or automating data entry tasks that require repetitive PIN input.

## Features

- **Calibration System**: Record the position of each digit (0-9) on any virtual keypad
- **Save/Load Configurations**: Save calibration settings for reuse
- **Customizable Delays**: Set delays between individual digits and between different PINs
- **Hotkey Control**: Use F12 to start/stop automation during runtime
- **PIN Batching**: Process multiple PINs from a text file

## Requirements

- Linux/Unix-based system with X11
- X11 development libraries
- C++ compiler supporting C++11 or later
- CMake for building (3.10+)

## Dependencies

- X11 libraries
- XTest extension

## Installation

### Install Dependencies

On Debian/Ubuntu:
```bash
sudo apt-get install libx11-dev libxtst-dev
```

On Fedora/CentOS:
```bash
sudo dnf install libX11-devel libXtst-devel
```

### Build the Project

```bash
# Clone the repository
git clone https://github.com/yourusername/pin-automation.git
cd pin-automation

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make
```

## Usage

### Running the Program

```bash
./pin_automation
```

### Basic Workflow

1. **Calibrate the Virtual Keypad**:
   - Select option 1 from the menu
   - Click on each digit (0-9) when prompted
   - The program will record the position of each digit

2. **Configure Delays**:
   - Select option 4 from the menu
   - Set delays between PINs (default: 1000ms)
   - Set delays between digits (default: 100ms)

3. **Save Calibration**:
   - Select option 3 from the menu
   - Enter a filename (default: calibration.dat)

4. **Load Previous Calibration**:
   - Select option 2 from the menu
   - Enter the filename of a saved calibration

5. **Start Automation**:
   - Select option 5 from the menu
   - Provide the path to a text file containing PINs (one per line)
   - Press F12 to start/stop the automation

### PIN File Format

Create a text file with one PIN per line:
```
1234
5678
9012
```

The program ignores empty lines and lines containing non-numeric characters.

## Safety Features

- **Emergency Stop**: Press F12 at any time to pause the automation
- **Button Release Detection**: Automatically detects and fixes "stuck" mouse buttons
- **Input Validation**: Verifies PIN data before processing

### Debug Logs

The program outputs detailed logs with timestamps for troubleshooting:
```
[2023-05-07 10:15:32] Starting PIN automation program
[2023-05-07 10:15:35] Calibration complete!
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## TODO / Work in Progress

### Current Issues
- Fix button release detection issue
- Improve error handling for X11 connection failures
- Address memory leak in event thread handling

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Disclaimer

This tool is intended for legitimate testing and automation purposes only. Users are responsible for ensuring they have appropriate authorization to use this tool on any system. The authors are not responsible for any misuse of this software.
