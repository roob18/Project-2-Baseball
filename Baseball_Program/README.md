# Baseball Program

A Qt-based application for managing and viewing MLB stadium information, with features for viewing team information, planning trips, and managing souvenirs through an admin interface.

## Prerequisites

- Qt 6.x (recommended) or Qt 5.15+
- Qt Creator IDE
- A C++17 compatible compiler
- Git for cloning the repository

## Required Files
The program comes with two important CSV files:
- `MLB Information.csv` - Main stadium database
- `MLB Information Expansion.csv` - Additional stadium information

These files should be in the root directory of the project.

## Setup Instructions

### Windows
1. Install Qt and Qt Creator from [Qt's official website](https://www.qt.io/download)
   - During installation, make sure to select:
     - Qt 6.x MSVC 64-bit (or MinGW 64-bit)
     - Qt Creator
     - CMake (should be included by default)
2. Clone this repository:
   ```bash
   git clone [your-repository-url]
   cd Baseball_Program
   ```
3. Open Qt Creator
4. Open the project:
   - Click "Open Project"
   - Navigate to where you cloned the repository
   - Select `Baseball_Program.pro`
5. Configure the project:
   - When prompted, select the appropriate kit (preferably Qt 6.x)
   - Click "Configure Project"
6. Build and run:
   - Press Ctrl+B to build
   - Press Ctrl+R to run

### macOS
1. Install Qt and Qt Creator:
   - Option 1: Download from [Qt's official website](https://www.qt.io/download)
   - Option 2: Install via Homebrew:
     ```bash
     brew install qt
     brew install qt-creator
     ```
2. Install Xcode command line tools:
   ```bash
   xcode-select --install
   ```
3. Clone this repository:
   ```bash
   git clone [your-repository-url]
   cd Baseball_Program
   ```
4. Open Qt Creator
5. Open and configure the project:
   - Open `Baseball_Program.pro`
   - Select the appropriate kit when prompted
6. Build and run:
   - Press Cmd+B to build
   - Press Cmd+R to run

## Features

- View and sort MLB stadium information
- Plan trips between stadiums
- View and purchase souvenirs
- Admin interface for managing stadium data and souvenirs
  - Default admin credentials:
    - Username: admin
    - Password: admin123

## Troubleshooting

### Common Issues
1. "Qt Creator can't find Qt installation":
   - Make sure Qt is properly installed
   - In Qt Creator, go to Tools > Options > Kits and verify your Qt version is detected

2. Build fails with missing files:
   - Clean the project (Build > Clean All)
   - Run qmake (Build > Run qmake)
   - Rebuild (Build > Rebuild All)

3. CSV files not found:
   - Ensure both CSV files are in the project root directory
   - Check file permissions

### For macOS Users
- If Qt Creator isn't finding your Qt installation from Homebrew:
  ```bash
  brew link qt --force
  ```
- Make sure you have full disk access enabled for Qt Creator in System Settings > Privacy & Security

### For Windows Users
- If using MSVC, make sure you have Visual Studio or the Visual Studio Build Tools installed
- If using MinGW, ensure it's properly set up in your Qt installation

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a new Pull Request 