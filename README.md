# Archivedater

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
3. [Installation](#installation)
4. [Usage](#usage)
    - [Command-Line Arguments](#command-line-arguments)
    - [Example Usage](#example-usage)
5. [Troubleshooting](#troubleshooting)

## Introduction
Archivedater is a command-line tool designed to simplify the process of updating file dates in archives. It provides a comprehensive set of features to easily correct the modification dates of the archived files.

## Features
- Supports multiple archive types (e.g., ZIP, TAR.GZ, TAR.BZ2)
- Provides several option flags (e.g., --verbose, --outputdir, etc.)
- Provides logging functionality for debugging and troubleshooting

## Installation
To install Archivedater, follow these steps:

1. Clone the repository from GitHub:
   ```
   git clone https://github.com/Krame1S/KOLYANOV_OSISP_prj_2024
   ```
2. Navigate to the project directory:
   ```
   cd KOLYANOV_OSISP_prj_2024
   ```
3. Compile the source code with make:
   ```
   make all
   ```
4. Move the compiled binary to a directory in your PATH. For example, you can move it to `/usr/local/bin` which is commonly included in the PATH on many systems:
   ```
   sudo mv archivedater /usr/local/bin/
   ```

## Usage
Archivedater can be used from the command line with the following arguments:

### Command-Line Arguments
- **Verbose Option**
    - Short Form: `-v`
    - Long Form: `--verbose`
  
- **Quiet Option**
    - Short Form: `-q`
    - Long Form: `--quiet`
  
- **Filetype Option**
    - Short Form: `-f`
    - Long Form: `--filetype`
  
- **Loglevel Option**
    - Short Form: `-l`
    - Long Form: `--loglevel`
  
- **Output Directory Option**
    - Short Form: `-o`
    - Long Form: `--outputdir`
  
- **Help Option**
    - Short Form: `-h`
    - Long Form: `--help`
  
- **Version Option**
    - Long Form: `--version`

### Example Usage
To update all file modification dates within archive:
```
archivedater /path/to/archive.zip
```

To update the modification dates of files of the .cpp type within archive, outputing updated archive in output/dir/:
```
archivedater /path/to/archive.tar.gz .cpp -o output/dir/
```

To show help message:
```
archivedater --help
```

## Troubleshooting
If you encounter any issues while using the Utility Tool for Updating Archives, please check the log file for more information. If the issue persists, feel free to open an issue on the project's GitHub repository.