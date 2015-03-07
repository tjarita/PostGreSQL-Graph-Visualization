
CREATE TABLE Country
(Name VARCHAR(35) NOT NULL UNIQUE,
 Code VARCHAR(4) CONSTRAINT CountryKey PRIMARY KEY,
 Capital VARCHAR(35),
 Province VARCHAR(35),
 Area NUMERIC CONSTRAINT CountryArea
   CHECK (Area >= 0),
 Population NUMERIC CONSTRAINT CountryPop
   CHECK (Population >= 0));

CREATE TABLE City
(Name VARCHAR(35),
 Country VARCHAR(4),
 Province VARCHAR(35),
 Population NUMERIC CONSTRAINT CityPop
   CHECK (Population >= 0),
 Longitude NUMERIC CONSTRAINT CityLon
   CHECK ((Longitude >= -180) AND (Longitude <= 180)) ,
 Latitude NUMERIC CONSTRAINT CityLat
   CHECK ((Latitude >= -90) AND (Latitude <= 90)) ,
 CONSTRAINT CityKey PRIMARY KEY (Name, Country, Province));

CREATE TABLE Province
(Name VARCHAR(35) CONSTRAINT PrName NOT NULL ,
 Country  VARCHAR(4) CONSTRAINT PrCountry NOT NULL REFERENCES Country DEFERRABLE INITIALLY DEFERRED,
 Population NUMERIC CONSTRAINT PrPop
   CHECK (Population >= 0),
 Area NUMERIC CONSTRAINT PrAr
   CHECK (Area >= 0),
 Capital VARCHAR(35),
 CapProv VARCHAR(35),
 CONSTRAINT PrKey PRIMARY KEY (Name, Country));

ALTER TABLE Country
  ADD CONSTRAINT Capital_FKey FOREIGN KEY (Capital, Code, Province) REFERENCES City(Name, Country, Province) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE City
  ADD CONSTRAINT Province_FKey FOREIGN KEY (Province, Country) REFERENCES Province(Name, Country) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE Province
  ADD CONSTRAINT Capital_FKey FOREIGN KEY (Capital, Country, CapProv) REFERENCES City(Name, Country, Province) DEFERRABLE INITIALLY DEFERRED;