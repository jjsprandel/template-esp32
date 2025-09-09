
# ESP32 Project Template for ESP-IDF

This repository is a template for developing multiple ESP32 projects using the ESP-IDF framework. Each project lives in its own folder under `project/projects/`, and the workspace is designed for rapid prototyping, testing, and CI automation.

---

## Features

- Supports multiple independent ESP-IDF projects in `project/projects/`
- Modern dev container setup for VS Code
- Automated CI/CD with GitHub Actions:
  - **Unit tests** (Unity, CMock, etc.)
  - **Static analysis** (clang-tidy)
  - **Emulation** (QEMU)
  - **Virtual hardware simulation** (Wokwi)
  - **Per-project CI**: CI runs only for projects with changed files
- Example build, flash, and test tasks for each project
- Easy integration of new projects and components

---

## Project Structure

```
project/
├── projects/
│   ├── project1/
│   │   ├── CMakeLists.txt
│   │   ├── src/
│   │   ├── build/
│   │   └── ...
│   ├── project2/
│   │   ├── CMakeLists.txt
│   │   └── ...
│   └── ...
├── .devcontainer/
├── Dockerfile
├── entrypoint.sh
├── tools/
└── ...
```

---

## Getting Started

1. **Install Required Software**
   - [Docker Desktop](https://www.docker.com/products/docker-desktop) (for containerized builds)
   - [Visual Studio Code](https://code.visualstudio.com/) (recommended IDE)
   - [CP210x USB to UART Bridge VCP drivers](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads) (for Windows; Linux drivers are built-in)
2. **Clone the repository**
3. **Open in VS Code** (recommended: use the dev container)
4. **Connect your ESP32 device** (install drivers if needed)
5. **Add a new project** under `project/projects/` (copy an existing project as a template)
6. **Build, flash, and test** using ESP-IDF tasks or the provided scripts

### Example Commands

```bash
# Build a project
cd project/projects/project1
idf.py build

# Flash to device
idf.py flash

# Run unit tests
idf.py test
```

---

## Continuous Integration (CI)

GitHub Actions automatically run for each project when its files change:

- **Unit tests**: Runs ESP-IDF unit tests for the changed project
- **Clang-tidy**: Static analysis for C/C++ code
- **QEMU**: Emulates ESP32 firmware for headless testing
- **Wokwi**: Simulates hardware peripherals and logic

CI is configured to only run jobs for projects with changes, keeping builds fast and relevant.

---

## Adding a New Project

1. Create a new folder under `project/projects/` (e.g., `project3`)
2. Add a `CMakeLists.txt` and source files
3. Optionally add unit tests and CI configuration
4. Push your changes—CI will automatically pick up and test your new project

---

## Dependencies & Tools

* **ESP-IDF** (see `.devcontainer/devcontainer.json` for version)
* **Docker Desktop** (for reproducible builds)
* **Visual Studio Code** (with ESP-IDF extension)
* **CP210x USB to UART Bridge VCP drivers** (for Windows)
* **CMake**, **Ninja**, **clang-tidy**, **QEMU**, **Wokwi**

---


## Author & Origin

All content in this repository, including sections adapted from the [SCAN repository](https://github.com/jjsprandel/SCAN), was written by [Jonah Sprandel](https://github.com/jjsprandel). This template was adapted from the SCAN project to serve as a general ESP32 project template for ESP-IDF.

---

## License

See [LICENSE](LICENSE) for details.

