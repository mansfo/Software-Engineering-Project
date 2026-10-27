# E-commerce Backend
A university software-engineering project that models the backend of an online musical-instrument shop. It is implemented in C++ and demonstrates a multi-process system in which customer, supplier, and carrier services coordinate orders through Redis Streams, persist business data in PostgreSQL, and record activity in a separate log database.
This repository contains the backend and an automated workload simulator; it does not include a web frontend.
## What it does
- **Customers** can search the catalog, manage a shopping cart, place or cancel orders, review order history, update account details, and add funds.
- **Suppliers** can view their products, add or remove products, and change prices.
- **Carriers** can view assigned and delivered orders and update delivery status as orders are picked up or delivered.
- **Monitoring processes** collect access, inventory-availability, customer-funds, delivery-time, and order-status information from the log database.
- **The workload simulator** uses sample users and products to exercise customer, supplier, and carrier interactions concurrently.
## Architecture
The top-level executable in `ecommerce_backend` starts the server processes, monitors, and workload simulator. The role-specific customer, supplier, and carrier server loops handle requests from their corresponding clients. Redis Streams carry those requests and responses; PostgreSQL stores the catalog, accounts, carriers, and orders. A second PostgreSQL database stores operational and order-related logs.
The source is organized as follows:
| Directory | Purpose |
| --- | --- |
| `client/` | C++ client models and operations for customers, suppliers, and carriers |
| `server/` | Role-specific request handlers and business operations |
| `databases/main-db-scripts/` | Main database schema, demo records, and setup scripts |
| `databases/logdb-scripts/` | Logging database schema and setup scripts |
| `databases/db_functions/` | PostgreSQL-backed business and logging functions |
| `con2db/` | PostgreSQL connection wrapper |
| `con2redis/` | Redis Streams communication helpers |
| `monitor/` | Monitoring processes |
| `test/` | Concurrent workload simulator and its fixture data |
| `ecommerce_backend/` | Application entry point and build orchestration |
## Requirements
The project is designed for Linux or a compatible Unix environment: process orchestration uses POSIX APIs such as `fork` and `exec`. You will need:
- GNU Make and a C++20 compiler (the makefiles use `g++`)
- PostgreSQL, its client tools, and development headers/libraries (`psql`, `libpq`)
- Redis Server and the hiredis development library
The application expects PostgreSQL at `localhost:5432` and Redis at `localhost:6379`. Its database connection settings are currently hard-coded in the source.
## Set up the databases
Start PostgreSQL, then run each setup script from its own directory so its relative SQL-file paths resolve:
```sh
cd databases/main-db-scripts
bash create.sh
cd ../logdb-scripts
bash create.sh
```
The main database script creates the `ecommerce_main_db` database and inserts demo users, suppliers, carriers, companies, and products. The log database script creates `ecommerce_logdb` and its log tables. Both scripts use `sudo -u postgres` and create the `ecommerce_server` database role.
**Warning:** the setup SQL drops and recreates its target database and role. Do not run these scripts against databases or roles containing data you need to keep.
## Build and run
From the repository root, build the application:
```sh
cd ecommerce_backend/src
make
```
The executable is written to `ecommerce_backend/bin/main`. Run it from the same `ecommerce_backend/src` directory because it launches the other components using paths relative to the current working directory:
```sh
../bin/main
```
The application starts the server processes, monitors, and simulator together. The simulator generates activity automatically; this is not an interactive customer or supplier command-line application. On startup, the server initializes its Redis Streams and deletes the named application streams, so use a dedicated Redis instance rather than one containing other application data.
## Tests and sample data
There is no standalone unit-test target in the repository. The `test/` program is a concurrent integration/workload simulator, and the top-level executable starts it as part of normal operation. Its fixture files include sample credentials, addresses, and products.
## Configuration notes
The PostgreSQL host, port, role, password, and database names, as well as the Redis host and port, are embedded in the current source. To use different local services, update those connection settings and the database setup scripts together.
