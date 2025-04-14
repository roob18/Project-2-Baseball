# Baseball Program

A Qt-based application for managing and viewing MLB stadium information.

## Prerequisites

- Qt 6.x (recommended) or Qt 5.15+
- Qt Creator IDE
- A C++17 compatible compiler

## Setup Instructions

### Windows
1. Install Qt and Qt Creator from [Qt's official website](https://www.qt.io/download)
2. Clone this repository:
   ```bash
   git clone [your-repository-url]
   ```
3. Open Qt Creator
4. Open the project by selecting `Baseball_Program.pro`
5. Configure the project for your kit when prompted
6. Build and run the project (Ctrl+R)

### macOS
1. Install Qt and Qt Creator from [Qt's official website](https://www.qt.io/download)
   - Alternatively, install via Homebrew:
     ```bash
     brew install qt
     brew install qt-creator
     ```
2. Clone this repository:
   ```bash
   git clone [your-repository-url]
   ```
3. Open Qt Creator
4. Open the project by selecting `Baseball_Program.pro`
5. Configure the project for your kit when prompted
6. Build and run the project (Cmd+R)

## Usage

1. Launch the application
2. Use the "Import CSV" option to load the MLB stadium data
3. Use the various tabs to view and sort the data:
   - Show Team Info
   - Sort by Team Name
   - Sort by Stadium Name
   - View American/National League Teams
   - View Teams by Stadium Type
   - And more...

## Common Issues

- If you get "database not found" errors, make sure the baseball.db file is in the correct directory
- If Qt Creator can't find Qt, make sure your Qt installation is properly set up in the IDE
- For Windows users: Make sure you have the MSVC compiler or MinGW installed
- For macOS users: Make sure Xcode command line tools are installed

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a new Pull Request 