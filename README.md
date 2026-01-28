# Logan - Log Aggregation & Query Service

A high-performance, multithreaded backend service written in C++17 for collecting, persisting, and querying application logs via HTTP.

## Overview

Logan is a production-style log aggregation system that handles concurrent HTTP requests, provides persistent file-based storage, and supports filtered log queries. Built with performance and thread safety in mind, it simulates real-world backend infrastructure used for monitoring, debugging, and operational visibility.

## Features

- **HTTP API** for log ingestion and querying
- **Thread-safe** concurrent request handling
- **Persistent storage** with append-only log files
- **Flexible querying** by time range, service name, and severity level
- **Crash-safe** file format for durability
- **Stream-based architecture** - queries read directly from disk (no memory overhead)

## Technology Stack

- **Language:** C++17
- **HTTP Server:** cpp-httplib
- **JSON Parsing:** nlohmann/json
- **Concurrency:** std::thread, std::mutex
- **Storage:** File-based append-only logs
- **Build System:** CMake with Ninja

## Data Model

### Log Entry Structure
```json
{
  "timestamp": 1700000000,
  "service": "auth",
  "level": "ERROR",
  "message": "Invalid token"
}
```

| Field     | Type   | Description                         |
|-----------|--------|-------------------------------------|
| timestamp | int64  | Unix timestamp (seconds)            |
| service   | string | Source service name                 |
| level     | string | INFO / WARN / ERROR / DEBUG / FATAL |
| message   | string | Log message content                 |

## API Specification

### Health Check

Check if the service is running.

**Endpoint:** `GET /health`

**Response:**
```json
{
  "status": "ok"
}
```

---

### Submit Log

Ingest a new log entry.

**Endpoint:** `POST /log`

**Request Body:**
```json
{
  "service": "auth",
  "level": "ERROR",
  "message": "Invalid token",
  "timestamp": 1700000000
}
```

**Response:**
```json
{
  "success": "true"
}
```

**Error Response (400):**
```json
{
  "error": "Missing required field: service"
}
```

---

### Query Logs

Retrieve logs with optional filtering.

**Endpoint:** `GET /log`

**Query Parameters:**

| Parameter | Required | Type   | Description                    |
|-----------|----------|--------|--------------------------------|
| service   | No       | string | Filter by service name         |
| level     | No       | string | Filter by severity (INFO/WARN/ERROR/DEBUG/FATAL) |
| from      | No       | int64  | Start timestamp (inclusive)    |
| to        | No       | int64  | End timestamp (inclusive)      |

**Example Requests:**
```bash
# Get all logs
GET /log

# Get all ERROR logs
GET /log?level=ERROR

# Get logs from auth service
GET /log?service=auth

# Get logs in time range
GET /log?from=1700000000&to=1700001000

# Combined filters
GET /log?service=auth&level=ERROR&from=1700000000
```

**Response:**
```json
{
  "success": true,
  "logs": [
    {
      "timestamp": 1700000000,
      "service": "auth",
      "level": "ERROR",
      "message": "Invalid token"
    }
  ],
  "count": 1
}
```

## Persistence Design

- **Append-only log file** - New logs are appended to `logs.txt`
- **Asynchronous logging** - Logs are buffered in memory and periodically flushed to disk by a dedicated worker thread
- **Stream-based queries** - Logs are read from disk on-demand (no memory loading)
- **Crash-safe** - Simple newline-delimited JSON format ensures partial writes are detectable
- **No startup overhead** - Service starts instantly without loading logs into memory

## Concurrency Design

- **Parallel request handling** - HTTP server handles multiple concurrent connections
- **Mutex-protected storage** - Write operations are serialized with mutex locks
- **Thread-safe reads** - Query operations safely read from disk while writes occur
- **No race conditions** - Proper synchronization ensures data consistency

## Error Handling

- **Invalid JSON** → HTTP 400 with descriptive error message
- **Missing required fields** → HTTP 400 with field name
- **I/O failures** → Graceful error handling with logging

## Performance Considerations

- **No memory overhead** - Logs are never loaded into memory
- **Direct disk I/O** - Queries stream results from disk
- **Efficient filtering** - Sequential scan with early termination
- **Write performance** - Append-only writes are fast and simple
- **Scalability tradeoff** - Optimized for write throughput; queries perform full scans

## Build & Run

### Prerequisites

- C++17 compatible compiler (g++, clang++)
- CMake 3.10+
- Ninja 1.10.0+
- cpp-httplib library
- nlohmann/json library

### Quick Start
```bash
# Make the run script executable
chmod +x run.sh

# Build and run the service
./run.sh
```

The service will start on `http://localhost:8080`

### Manual Build
```bash
# Create build directory
mkdir -p build
cd build

# Configure with Ninja generator
cmake -G Ninja ..

# Build with Ninja
ninja

# Run the service
./logan
```

## Usage Examples

### Submit a log
```bash
curl -X POST http://localhost:8080/log \
  -H "Content-Type: application/json" \
  -d '{
    "service": "api-gateway",
    "level": "INFO",
    "message": "Request processed successfully",
    "timestamp": 1700000000
  }'
```

### Query all logs
```bash
curl http://localhost:8080/log
```

### Query with filters
```bash
# Get all ERROR logs from auth service
curl "http://localhost:8080/log?service=auth&level=ERROR"

# Get logs in a specific time range
curl "http://localhost:8080/log?from=1700000000&to=1700010000"
```

### Check service health
```bash
curl http://localhost:8080/health
```
