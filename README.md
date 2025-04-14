# MLB Stadium Information Program

A Qt-based application for viewing and analyzing MLB stadium information.

## Requirements
- Qt 6.x or later
- C++ compiler (gcc, clang, or MSVC)
- CMake 3.16 or later

## Setup Instructions

1. Install Qt:
   - Download and install Qt from https://www.qt.io/download
   - Make sure to install the Qt Creator IDE and the Qt framework

2. Install a C++ compiler:
   - Windows: Install Visual Studio with C++ support
   - macOS: Install Xcode Command Line Tools
   - Linux: Install gcc/g++ using your package manager

3. Clone the repository:
   ```bash
   git clone [your-repository-url]
   cd Baseball_Program
   ```

4. Build the project:
   - Open the project in Qt Creator
   - Click "Configure Project"
   - Click the build button (hammer icon) or press Ctrl+B

## Project Structure
- `src/` - Contains all source code files
- `data/` - Contains data files (CSV, etc.)
- `include/` - Contains header files
- `build/` - Build directory (do not commit this)

## Features
- View MLB team information
- Sort stadiums by various criteria
- Filter teams by league
- Import CSV data 