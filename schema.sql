/* TABLE DEFINITION */
-- DEFAULT CHARSET=utf8 COLLATE utf8_general_ci;
-- account Table Create SQL

-- account Table Create SQL
CREATE TABLE account
(
    `id`      VARCHAR(45)    NOT NULL    COMMENT '아이디', 
    `pw`      VARCHAR(45)    NOT NULL    COMMENT '비밀번호', 
    `num`     VARCHAR(45)    NOT NULL    COMMENT '학번/교번', 
    `isprof`  INT            NOT NULL    COMMENT 'isprof', 
    PRIMARY KEY (id, num)
) DEFAULT CHARSET=utf8 COLLATE utf8_general_ci;

ALTER TABLE account COMMENT '계정';


-- professor Table Create SQL
CREATE TABLE professor
(
    `prof_num`    VARCHAR(45)    NOT NULL    COMMENT '교번', 
    `id`          VARCHAR(45)    NOT NULL    COMMENT '아이디', 
    `name`        VARCHAR(45)    NOT NULL    COMMENT '이름', 
    `department`  VARCHAR(45)    NOT NULL    COMMENT '학과', 
    `tel`         VARCHAR(45)    NULL        COMMENT '전화번호', 
    PRIMARY KEY (prof_num)
) DEFAULT CHARSET=utf8 COLLATE utf8_general_ci;

ALTER TABLE professor COMMENT '교수';

ALTER TABLE professor ADD CONSTRAINT FK_professor_id_account_id FOREIGN KEY (id)
 REFERENCES account (id)  ON DELETE RESTRICT ON UPDATE RESTRICT;


-- subject Table Create SQL
CREATE TABLE subject
(
    `prof_num`      VARCHAR(45)    NOT NULL    COMMENT '교번', 
    `subject_code`  VARCHAR(45)    NOT NULL    COMMENT '과목코드', 
    `name`          VARCHAR(45)    NOT NULL    COMMENT '과목명', 
    `max`           INT            NOT NULL    COMMENT '정원', 
    `credit`        INT            NOT NULL    COMMENT '학점', 
    PRIMARY KEY (subject_code)
) DEFAULT CHARSET=utf8 COLLATE utf8_general_ci;

ALTER TABLE subject COMMENT '과목';

ALTER TABLE subject ADD CONSTRAINT FK_subject_prof_num_professor_prof_num FOREIGN KEY (prof_num)
 REFERENCES professor (prof_num)  ON DELETE RESTRICT ON UPDATE RESTRICT;


-- student Table Create SQL
CREATE TABLE student
(
    `student_num`  VARCHAR(45)    NOT NULL    COMMENT '학번', 
    `id`           VARCHAR(45)    NULL        COMMENT '아이디', 
    `name`         VARCHAR(45)    NOT NULL    COMMENT '이름', 
    `department`   VARCHAR(45)    NOT NULL    COMMENT '학과', 
    `tel`          VARCHAR(45)    NULL        COMMENT '전화번호', 
    PRIMARY KEY (student_num)
) DEFAULT CHARSET=utf8 COLLATE utf8_general_ci;

ALTER TABLE student COMMENT '학생';

ALTER TABLE student ADD CONSTRAINT FK_student_id_account_id FOREIGN KEY (id)
 REFERENCES account (id)  ON DELETE RESTRICT ON UPDATE RESTRICT;


-- score Table Create SQL
CREATE TABLE score
(
    `subject_code`  VARCHAR(45)    NOT NULL    COMMENT '과목코드', 
    `student_num`   VARCHAR(45)    NOT NULL    COMMENT '학번', 
    `midterm`       DOUBLE         NOT NULL    COMMENT '중간', 
    `final`         DOUBLE         NOT NULL    COMMENT '기말', 
    `homework`      DOUBLE         NOT NULL    COMMENT '과제', 
    `attendance`    DOUBLE         NOT NULL    COMMENT '출석', 
    `grade`         VARCHAR(45)    NULL        COMMENT '등급'
) DEFAULT CHARSET=utf8 COLLATE utf8_general_ci;

ALTER TABLE score COMMENT '수강(성적)';

ALTER TABLE score ADD CONSTRAINT FK_score_subject_code_subject_subject_code FOREIGN KEY (subject_code)
 REFERENCES subject (subject_code)  ON DELETE RESTRICT ON UPDATE RESTRICT;

ALTER TABLE score ADD CONSTRAINT FK_score_student_num_student_student_num FOREIGN KEY (student_num)
 REFERENCES student (student_num)  ON DELETE RESTRICT ON UPDATE RESTRICT;


-- attendance Table Create SQL
CREATE TABLE attendance
(
    `subject_code`  VARCHAR(45)    NOT NULL    COMMENT '과목코드', 
    `student_num`   VARCHAR(45)    NOT NULL    COMMENT '학번', 
    `type`          VARCHAR(45)    NOT NULL    COMMENT '종류', 
    `date`          DATETIME       NOT NULL    COMMENT '날짜', 
    `reason`        TEXT           NULL        COMMENT '사유'
) DEFAULT CHARSET=utf8 COLLATE utf8_general_ci;

ALTER TABLE attendance COMMENT '출결';

-- ALTER TABLE attendance ADD CONSTRAINT FK_attendance_student_num_score_student_num FOREIGN KEY (student_num)
--  REFERENCES score (student_num)  ON DELETE RESTRICT ON UPDATE RESTRICT;

ALTER TABLE attendance ADD CONSTRAINT FK_attendance_subject_code_score_subject_code FOREIGN KEY (subject_code)
 REFERENCES score (subject_code)  ON DELETE RESTRICT ON UPDATE RESTRICT;


-- message Table Create SQL
CREATE TABLE message
(
    `from`      VARCHAR(45)    NULL        COMMENT '보낸이', 
    `to`        VARCHAR(45)    NULL        COMMENT '받는이', 
    `content`   TEXT           NULL        COMMENT '내용', 
    `sendtime`  DATETIME       NULL        COMMENT '보낸시각', 
    `recvtime`  DATETIME       NULL        COMMENT '받은시각', 
    `read`      INT            NULL        COMMENT '읽음'
) DEFAULT CHARSET=utf8 COLLATE utf8_general_ci;

ALTER TABLE message COMMENT '메시지';

/* INSERT DATA */
insert into account values('jskim', 'jskim','2014244106', 0);
insert into account values('test', 'test', '20101010', 1);

insert into professor values('20101010', 'test', '교수', '컴퓨터공학과', '0101010');
insert into student values('2014244106', 'jskim', '김지섭', '컴퓨터공학과', '01010101010');

insert into subject values('20101010', '01', 'C language', 10, 3);
insert into score values('01', '2014244106', 20,10,0,0, NULL);

insert into attendance values('01', '2014244106', 'absent', '2018-06-09 04:10:15','');