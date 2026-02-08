# Use an existing base image
FROM python:3.12-slim

# Set the working directory in the container
WORKDIR /app

# Copy application files from host to container
COPY . .

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Set the default command to execute when the container starts
RUN make -C scanner

CMD ["python3", "controller/cli.py"]
