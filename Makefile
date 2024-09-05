# Set up paths
ImageProducer_DIR := imageProducer
ImageConsumer_DIR := imageConsumer
BUILD_DIR := $(ImageProducer_DIR)/build

.PHONY: all clean imageProducer_build imageConsumer_build run

all: imageProducer_build imageConsumer_build run

# ---- ImageProducer(C++) Build ----
imageProducer_build:
	@echo "Building the imageProducer..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. && make
	@if [ $$? -ne 0 ]; then \
	  echo "Build failed for imageProducer."; \
	  exit 1; \
	fi
	@echo "ImageProducer built successfully."

# ---- ImageConsumer(Java) Build ----
imageConsumer_build:
	@echo "Building the imageConsumer..."
	@cd $(ImageConsumer_DIR) && gradle build
	@if [ $$? -ne 0 ]; then \
	  echo "Gradle build failed for imageConsumer."; \
	  exit 1; \
	fi
	@echo "ImageConsumer built successfully."

# ---- Run Both Peer Applications ----
run: imageProducer_build imageConsumer_build
	@echo "Running the imageConsumer..."
	@gnome-terminal -- bash -c "cd '$(ImageConsumer_DIR)' && gradle run; exec bash"
	@sleep 3
	@JAVA_PID=$$!
	@echo "Running the imageProducer..."
	@cd $(BUILD_DIR) && ./ImageProducer
	@if [ $$? -ne 0 ]; then \
	  echo "Failed to run the imageProducer."; \
	  kill $$JAVA_PID; \
	  exit 1; \
	fi
	@wait $$JAVA_PID
	@echo "Both ran successfully."

# ---- Clean Up ----
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR)
	@cd $(ImageConsumer_DIR) && gradle clean
	@echo "Cleanup completed."
