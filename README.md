# 📦 CSV-Based HTTP Data Server

A lightweight C++ HTTP server that processes `GET` requests to **query** and **modify** data stored in a CSV file. It supports concurrent access using multithreading and ensures data consistency using read-write locks.

---

## 📁 Project Structure

### `server.cpp`
- Main function entry point.
- Starts two threads to listen on different ports:
  - **Query** port
  - **Modify** port
- Spawns a new thread for each incoming connection.
- Only handles `GET` requests.
- Extracts the `command` parameter from the HTTP request.
- Converts `%20` to space characters for proper parsing.

---

### `data.csv`
- Persistent data storage.
- Must follow fixed format:
```

Nation,Category,Entity

```
- For every request:
- **Read**: loads file into memory for filtering.
- **Write**: overwrites file with updated content.
- Uses **read-write locks**:
- Multiple threads can **read** concurrently.
- **Write** blocks all other threads until finished.

---

### `CommandParser.cpp/hpp`
- Contains two parser classes:
- `QueryParser`
- `ModifyParser`
- Utilizes OOP with polymorphism for flexibility.
- A `Spliter` base class has specialized subclasses:
- `InsertSpliter`
- `DeleteSpliter`
- `UpdateSpliter`
- `QuerySpliter`

#### 🛠 Modify Command Formats

- **INSERT**  
```

INSERT "val1", "val2", "val3"

```
- Values must be double-quoted.
- Escape inner quotes with `\"`.

- **DELETE**  
```

DELETE "val1"
DELETE "val1", "val2"
DELETE "val1", "val2", "val3"

```

- **UPDATE**  
```

UPDATE "val1", "val2", ColumnName, "new\_value"

```

#### 🔍 Query Command Syntax

- Supports boolean expressions with **`AND`** / **`OR`** (no parentheses).
- Comparison operators:
- `==`: Exact match
- `!=`: Not equal
- `$=`: Case-insensitive match
- `&=`: Substring match  
- Example:
```

Nation == "USA" AND Category \$= "tech"

````

---

### `CommandExecuter.cpp/hpp`
- Executes parsed commands and manages file I/O.

#### ✅ Query
- Loads CSV into memory.
- Filters matching rows.
- Optimization (optional): map-based indexing for faster search.

#### ✏️ Modify
- Executes insert/delete/update.
- Returns success or failure:
- Fails on inserting existing row.
- Fails if no row matches for delete/update.

---

### `test.sh`
- Test script using `curl` to send HTTP `GET` requests.
- Use `%20` to encode spaces in command strings.

```bash
curl "http://<ip>:<port>/?command=INSERT%20\"USA\",\"Tech\",\"Apple\""
````

---

## 🛠 Build & Run

### 1. Build

```bash
mkdir build
cd build
cmake ..
make
```

### 2. Initialize Data

```bash
mkdir data
echo "Nation,Category,Entity" > data/data.csv
```

### 3. Run Server

```bash
./bin/server
```

---

## ❗ Notes

* Column names are fixed: `Nation`, `Category`, `Entity`.
* All string values must be **enclosed in double quotes**.
* Encode **spaces** in URL using `%20`.
* Docker packaging not included due to time constraints.

---

## 📂 Other Files

* `CMakeLists.txt`: Build config.
* `client.cpp`, `test.cpp`: Test utilities.

---

## 🧪 Example Commands

Insert:

```
INSERT "USA", "Tech", "Apple"
```

Delete:

```
DELETE "USA", "Tech"
```

Update:

```
UPDATE "USA", "Tech", Category, "Finance"
```

Query:

```
Nation == "USA" AND Category &= "Tech"
```
