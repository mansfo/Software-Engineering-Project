\c :dbname postgres

--  domains
CREATE DOMAIN string as VARCHAR(100);
CREATE DOMAIN bigstring as VARCHAR(500);
CREATE DOMAIN integerGZ as integer check (value > 0 and value is not null);
CREATE DOMAIN integerGEZ AS integer check (value >= 0 and value is not null);
CREATE DOMAIN realGZ AS real check (value > 0 and value is not null);
CREATE DOMAIN realGEZ AS real check (value >= 0 and value is not null);
CREATE DOMAIN integerBool AS integer check(value >= 0 and value <= 1 and value is not null);
CREATE DOMAIN ordStatus AS integer check(value >= 0 and value <= 3 and value is not null);

--  tables

CREATE TABLE IF NOT EXISTS LogTable(
    id serial primary key,
    pid integer not null,
    userid integer not null,
    usertype string not null,
    operation string not null,
    outcome string not null,
    description bigstring not null,
    tstamp timestamp not null,
    unixtime bigint not null
);

CREATE TABLE IF NOT EXISTS AccessLogTable(
    id serial primary key,
    email string not null,
    successfull integerBool not null,
    tstamp timestamp not null,
    unixtime bigint not null
);

CREATE TABLE IF NOT EXISTS OrderLogTable(
    id serial primary key,
    ord integerGZ not null,
    status ordStatus not null,
    tstamp timestamp not null,
    unixtime bigint not null
);

CREATE TABLE IF NOT EXISTS PurchasesLogTable(
    id serial primary key,
    price realGZ not null,
    avail integerGEZ not null,
    money realGEZ not null,
    quantity integerGZ not null,
    ord integerGZ not null,
    tstamp timestamp not null,
    unixtime bigint not null
);