# Utility Tool for Updating Archives

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
3. [Installation](#installation)
4. [Usage](#usage)
    - [Command-Line Arguments](#command-line-arguments)
    - [Example Usage](#example-usage)
5. [Logging](#logging)
6. [Troubleshooting](#troubleshooting)
7. [Contributing](#contributing)
8. [License](#license)

## Introduction
The Utility Tool for Updating Archives is a powerful command-line tool designed to simplify the process of updating archives. It provides a comprehensive set of features to extract, modify, and re-create archives with updated file modification dates.

## Features
- Supports multiple archive types (e.g., ZIP, TAR, etc.)
- Extracts archives to a temporary directory
- Changes the modification dates of files within the temporary directory
- Creates an updated archive from the modified files
- Provides logging functionality for debugging and troubleshooting

## Installation
To install the Utility Tool for Updating Archives, follow these steps:

1. Clone the repository from GitHub:
   ```
   git clone https://github.com/your-username/utility-tool.git
   ```
2. Navigate to the project directory:
   ```
   cd utility-tool
   ```
3. Compile the source code:
   ```
   gcc -o util main.c
   ```

## Usage
The Utility Tool for Updating Archives can be used from the command line with the following arguments:

### Command-Line Arguments
- `working_dir`: The directory where the archive is located and where the updated archive will be created.
- `file_extension`: (Optional) The file extension of the files to be updated within the archive. If not provided, the tool will process all files in the archive.

### Example Usage
To update an archive with the default file extension:
```
./util /path/to/archive.zip
```

To update an archive with a specific file extension:
```
./util /path/to/archive.tar.gz .cpp
```

## Logging
The Utility Tool for Updating Archives provides logging functionality to help with debugging and troubleshooting. The log file is created in the same directory as the tool and is named `util.log`. You can adjust the log level by modifying the `current_log_level` variable in the source code.

## Troubleshooting
If you encounter any issues while using the Utility Tool for Updating Archives, please check the log file for more information. If the issue persists, feel free to open an issue on the project's GitHub repository.

## Contributing
Contributions to the Utility Tool for Updating Archives are welcome! If you would like to contribute, please follow these steps:

1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Make your changes and commit them.
4. Push your changes to your forked repository.
5. Create a pull request to the main repository.

## License
The Utility Tool for Updating Archives is licensed under the [MIT License](LICENSE). 