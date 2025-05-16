\c :dbname postgres

--  domains
CREATE DOMAIN string AS VARCHAR(200) check (value is not null);
CREATE DOMAIN integerGZ AS integer check (value > 0 and value is not null);
CREATE DOMAIN integerGEZ AS integer check (value >= 0 and value is not null);
CREATE DOMAIN realGZ AS real check (value > 0 and value is not null);
CREATE DOMAIN realGEZ AS real check (value >= 0 and value is not null);
CREATE DOMAIN address AS VARCHAR(200);
CREATE DOMAIN integerBool AS integer check (value >= 0 and value <= 1 and value is not null);
CREATE DOMAIN carrStatus AS integer check (value >= 0 and value <= 3 and value is not null);
CREATE TYPE prod_category AS ENUM('guitar', 'bass', 'keyboard', 'piano', 'percussions', 'wind', 'other');

--  tables
CREATE TABLE IF NOT EXISTS Customer(
    id serial primary key,
    name string not null,
    surname string not null,
    addr address not null,
    email string not null,
    password string not null,
    money realGEZ not null,
    readstream string not null,
    writestream string not null,
    unique (email)
);

CREATE TABLE IF NOT EXISTS Supplier(
    id serial primary key,
    name string unique,
    addr address not null,
    email string unique,
    password string not null,
    readstream string not null,
    writestream string not null,
    unique (email)
);

CREATE TABLE IF NOT EXISTS Company(
    name string primary key,
    addr address not null
);

CREATE TABLE IF NOT EXISTS Carrier(
    id serial,
    name string not null,
    surname string not null,
    company string not null,
    status carrStatus not null,
    readstream string not null,
    writestream string not null,
    primary key (id),
    constraint company_fk foreign key (company) references Company(name)
);

CREATE TABLE IF NOT EXISTS Product(
    id serial primary key,
    name string not null,
    price realGZ not null,
    avail integerGEZ not null,
    supplier integer not null,
    category string not null,
    removed integerBool not null,
    constraint suppl_prod_fk foreign key (supplier) references Supplier(id)
);

CREATE TABLE IF NOT EXISTS Orders(
    id serial primary key,
    prod integer not null,
    cust integer not null, 
    suppl integer not null,
    carr integer,
    status integer not null,
    money realGZ not null,
    quant integerGZ not null,
    constraint prod_ord_fk foreign key (prod) references Product(id),
    constraint cust_ord_fk foreign key (cust) references Customer(id),
    constraint suppl_ord_fk foreign key (suppl) references Supplier(id),
    constraint carr_ord_fk foreign key (carr) references Carrier(id)
);