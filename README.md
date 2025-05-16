### E-commerce site Backend

This repository contains a university project for the Software Engineering course in the Computer Science Bachelor's degree at Sapienza. The project features an E-commerce site backend written in C++ based on three main servers, two PostgreSQL databases and a Redis stream-based communication system. There is also a test generator that emulates the interaction between the system and the various types of users.

Insturctions:
To execute the code, it's needed to start by running ./databases/main-db-script/create.sh and ./databases/logdb-script/create.sh to create the databases
Then, to compilate everything it's possible to run make inside ./ecommerce_backend/src/, the makefile will compile recursively every script in the project.
After that, it's possible to run the program by executing ./ecommerce_backend/bin/main
