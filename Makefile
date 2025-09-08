# Default project = current directory name (if inside a project)
PROJ ?= $(notdir $(CURDIR))

# Full project path (always under projects/)
PROJ_PATH := projects/$(PROJ)

# Build directory lives inside the project
BUILD_DIR := $(PROJ_PATH)/build

.PHONY: build clean flash

build:
	@echo "Building $(PROJ)..."
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. -G Ninja && ninja

clean:
	@echo "Cleaning $(PROJ)..."
	rm -rf $(BUILD_DIR)

flash:
	@echo "Flashing $(PROJ)..."
	cd $(BUILD_DIR) && ninja flash
