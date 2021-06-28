CREATE SCHEMA `LinuxProgrammingPracticum` ;

CREATE TABLE `LinuxProgrammingPracticum`.`friend` (
  `username` VARCHAR(45) NOT NULL,
  `targetname` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`username`, `targetname`));
  
CREATE TABLE `LinuxProgrammingPracticum`.`groupchat` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `username` VARCHAR(45) NOT NULL,
  `groupname` VARCHAR(45) NOT NULL,
  `time` DATETIME NOT NULL,
  `content` VARCHAR(200) NOT NULL,
  PRIMARY KEY (`id`));
  
CREATE TABLE `LinuxProgrammingPracticum`.`groupmember` (
  `username` VARCHAR(45) NOT NULL,
  `groupname` VARCHAR(45) NOT NULL,
  `isadmin` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`username`, `groupname`));

CREATE TABLE `LinuxProgrammingPracticum`.`groupt` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`id`));
  
CREATE TABLE `LinuxProgrammingPracticum`.`sock` (
  `sock` INT NOT NULL,
  `name` VARCHAR(45) NULL,
  PRIMARY KEY (`sock`));

CREATE TABLE `LinuxProgrammingPracticum`.`user` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(45) NOT NULL,
  `pwd` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`id`));

CREATE TABLE `LinuxProgrammingPracticum`.`userchat` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `username` VARCHAR(45) NOT NULL,
  `targetname` VARCHAR(45) NOT NULL,
  `time` DATETIME NOT NULL,
  `content` VARCHAR(200) NOT NULL,
  PRIMARY KEY (`id`));
