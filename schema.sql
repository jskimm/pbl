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

ALTER TABLE attendance ADD CONSTRAINT FK_attendance_student_num_score_student_num FOREIGN KEY (student_num)
 REFERENCES score (student_num);

ALTER TABLE attendance ADD CONSTRAINT FK_attendance_subject_code_score_subject_code FOREIGN KEY (subject_code)
 REFERENCES score (subject_code);


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






insert into account values
('chw001','password','2014244001','0');
insert into account values
('psw002','password','2014244002','0');
insert into account values
('kms003','password','2017244003','0');
insert into account values
('khm004','password','2018244004','0');
insert into account values
('kmc005','password','2013244005','0');
insert into account values
('kjk006','password','2014244006','0');
insert into account values
('nice007','password','2014244007','0');
insert into account values
('lsj008','password','2016244008','0');
insert into account values
('kjs009','password','2012244009','0');
insert into account values
('kyu010','password','2018244010','0');
insert into account values
('ksh011','password','2017244011','0');
insert into account values
('kht012','password','2015244012','0');
insert into account values
('gjh013','password','2015244013','0');
insert into account values
('sye014','password','2016244014','0');
insert into account values
('nmr015','password','2018244015','0');
insert into account values
('yhe016','password','2017244016','0');
insert into account values
('kjh017','password','2013244017','0');
insert into account values
('kdw018','password','2015244018','0');
insert into account values
('kkm019','password','2017244019','0');
insert into account values
('yjm020','password','2015244020','0');
insert into account values
('kyr021','password','2016244021','0');
insert into account values
('kjm022','password','2017244022','0');
insert into account values
('kjy023','password','2018244023','0');
insert into account values
('kjy024','password','2015244024','0');
insert into account values
('kkr025','password','2017244025','0');
insert into account values
('kkh026','password','2017244026','0');
insert into account values
('ksw027','password','2014244027','0');
insert into account values
('kyg028','password','2012244028','0');
insert into account values
('kyh029','password','2013244029','0');
insert into account values
('kws030','password','2013244030','0');
insert into account values
('kjh031','password','2015244031','0');
insert into account values
('kjh032','password','2016244032','0');
insert into account values
('kyh033','password','2012244033','0');
insert into account values
('njh034','password','2016244034','0');
insert into account values
('kjg033','password','2014244035','0');
insert into account values
('ljh034','password','2013244036','0');
insert into account values
('rcb035','password','19800402','1');
insert into account values
('kmj036','password','19900512','1');
insert into account values
('bjw037','password','19920623','1');
insert into account values
('syd038','password','19890725','1');
insert into account values
('sjh039','password','19870723','1');
insert into account values
('cjs040','password','19990923','1');
insert into account values
('ahs041','password','20000102','1');



insert into student values('2014244001','chw001','최현우','computer engineering','01031312424');
insert into student values('2014244002','psw002','박성우','computer engineering','01031312425');
insert into student values('2017244003','kms003','김명섭','computer engineering','01031312426');
insert into student values('2018244004','khm004','김형민','computer engineering','01031312427');
insert into student values('2013244005','kmc005','김민찬','computer engineering','01031312428');
insert into student values('2014244006','kjk006','김진겸','computer engineering','01031312429');
insert into student values('2014244007','nice007','나이수','computer engineering','01031312430');
insert into student values('2016244008','lsj008','이수진','computer engineering','01031312431');
insert into student values('2012244009','kjs009','김지수','computer engineering','01031312432');
insert into student values('2018244010','kyu010','김예은','computer engineering','01031312433');
insert into student values('2017244011','ksh011','김수현','smart car','01031312434');
insert into student values('2015244012','kht012','김현태','smart car','01031312435');
insert into student values('2015244013','gjh013','권지혜','smart car','01031312436');
insert into student values('2016244014','sye014','서연이','smart car','01031312437');
insert into student values('2018244015','nmr015','노미란','smart car','01031312438');
insert into student values('2017244016','yhe016','윤혜은','smart car','01031312439');
insert into student values('2013244017','kjh017','김장환','smart car','01031312440');
insert into student values('2015244018','kdw018','강동우','smart car','01031312441');
insert into student values('2017244019','kkm019','김경민','new material','01031312442');
insert into student values('2015244020','yjm020','유정민','new material','01031312443');
insert into student values('2016244021','kyr021','고예림','new material','01031312444');
insert into student values('2017244022','kjm022','구준모','new material','01031312445');
insert into student values('2018244023','kjy023','권준영','new material','01031312446');
insert into student values('2015244024','kjy024','강지영','new material','01031312447');
insert into student values('2017244025','kkr025','김규리','new material','01031312448');
insert into student values('2017244026','kkh026','김기현','electronic engineering','01031312449');
insert into student values('2014244027','ksw027','김서원','electronic engineering','01031312450');
insert into student values('2012244028','kyg028','김영규','electronic engineering','01031312451');
insert into student values('2013244029','kyh029','김영훈','electronic engineering','01031312452');
insert into student values('2013244030','kws030','김원섭','electronic engineering','01031312453');
insert into student values('2015244031','kjh031','김주환','electronic engineering','01031312454');
insert into student values('2016244032','kjh032','김재훈','electronic engineering','01031312455');
insert into student values('2012244033','kyh033','김용환','electronic engineering','01031312456');
insert into student values('2016244034','njh034','남진호','electronic engineering','01031312457');
insert into student values('2014244035','kjg033','김정균','electronic engineering','01031312458');
insert into student values('2013244036','ljh034','이정훈','electronic engineering','01031312459');



insert into professor values
('19800402','rcb035','류창빈','computer engineering','01021213131');
insert into professor values
('19900512','kmj036','김민지','computer engineering','01021213132');
insert into professor values
('19920623','bjw037','배지웅','computer engineering','01021213132');
insert into professor values
('19890725','syd038','서윤덕','computer engineering','01021213133');
insert into professor values
('19870723','sjh039','서정훈','computer engineering','01021213134');
insert into professor values
('19990923','cjs040','최재성','computer engineering','01021213135');
insert into professor values
('20000102','ahs041','안혜선','computer engineering','01021213136');


insert into subject values('19800402','01','data structure',10,3);
insert into subject values('19900512','02','iot',10,3);
insert into subject values('19920623','03','java foundation',10,3);
insert into subject values('19890725','04','introduction to computer',10,3);
insert into subject values('19870723','05','reading english',10,3);
insert into subject values('19990923','06','general physics',10,3);
insert into subject values('20000102','07','pbl',10,3);


insert into score values
('01','2017244003','0','0','0','0','F');
insert into score values
('01','2018244004','0','0','0','0','F');
insert into score values
('01','2013244005','0','0','0','0','F');
insert into score values
('01','2017244011','0','0','0','0','F');
insert into score values
('01','2015244012','0','0','0','0','F');
insert into score values
('01','2015244024','0','0','0','0','F');
insert into score values
('01','2017244025','0','0','0','0','F');
insert into score values
('01','2017244026','0','0','0','0','F');
insert into score values
('01','2014244035','0','0','0','0','F');
insert into score values
('01','2013244036','0','0','0','0','F');

insert into score values
('02','2014244006','0','0','0','0','F');
insert into score values
('02','2014244007','0','0','0','0','F');
insert into score values
('02','2016244008','0','0','0','0','F');
insert into score values
('02','2012244009','0','0','0','0','F');
insert into score values
('02','2018244010','0','0','0','0','F');
insert into score values
('02','2017244011','0','0','0','0','F');
insert into score values
('02','2015244012','0','0','0','0','F');
insert into score values
('02','2013244017','0','0','0','0','F');
insert into score values
('02','2015244018','0','0','0','0','F');
insert into score values
('02','2015244013','0','0','0','0','F');

insert into score values
('03','2016244034','0','0','0','0','F');
insert into score values
('03','2014244035','0','0','0','0','F');
insert into score values
('03','2013244036','0','0','0','0','F');
insert into score values
('03','2016244008','0','0','0','0','F');
insert into score values
('03','2012244009','0','0','0','0','F');
insert into score values
('03','2018244010','0','0','0','0','F');
insert into score values
('03','2018244023','0','0','0','0','F');
insert into score values
('03','2015244024','0','0','0','0','F');
insert into score values
('03','2017244025','0','0','0','0','F');
insert into score values
('03','2018244015','0','0','0','0','F');

insert into score values
('04','2015244018','0','0','0','0','F');
insert into score values
('04','2017244019','0','0','0','0','F');
insert into score values
('04','2017244026','0','0','0','0','F');
insert into score values
('04','2014244027','0','0','0','0','F');
insert into score values
('04','2018244015','0','0','0','0','F');
insert into score values
('04','2015244024','0','0','0','0','F');
insert into score values
('04','2017244025','0','0','0','0','F');
insert into score values
('04','2017244022','0','0','0','0','F');
insert into score values
('04','2018244023','0','0','0','0','F');
insert into score values
('04','2014244001','0','0','0','0','F');

insert into score values
('05','2017244016','0','0','0','0','F');
insert into score values
('05','2013244017','0','0','0','0','F');
insert into score values
('05','2012244028','0','0','0','0','F');
insert into score values
('05','2013244029','0','0','0','0','F');
insert into score values
('05','2015244031','0','0','0','0','F');
insert into score values
('05','2016244032','0','0','0','0','F');
insert into score values
('05','2012244033','0','0','0','0','F');
insert into score values
('05','2016244034','0','0','0','0','F');
insert into score values
('05','2014244035','0','0','0','0','F');
insert into score values
('05','2013244036','0','0','0','0','F');

insert into score values
('06','2017244011','0','0','0','0','F');
insert into score values
('06','2015244012','0','0','0','0','F');
insert into score values
('06','2017244019','0','0','0','0','F');
insert into score values
('06','2015244020','0','0','0','0','F');
insert into score values
('06','2012244028','0','0','0','0','F');
insert into score values
('06','2013244029','0','0','0','0','F');
insert into score values
('06','2017244003','0','0','0','0','F');
insert into score values
('06','2018244004','0','0','0','0','F');
insert into score values
('06','2015244013','0','0','0','0','F');
insert into score values
('06','2016244014','0','0','0','0','F');


insert into score values
('07','2014244001','0','0','0','0','F');
insert into score values
('07','2014244002','0','0','0','0','F');
insert into score values
('07','2017244003','0','0','0','0','F');
insert into score values
('07','2018244004','0','0','0','0','F');
insert into score values
('07','2013244005','0','0','0','0','F');
insert into score values
('07','2014244006','0','0','0','0','F');
insert into score values
('07','2014244007','0','0','0','0','F');
insert into score values
('07','2016244008','0','0','0','0','F');
insert into score values
('07','2012244009','0','0','0','0','F');
insert into score values
('07','2018244010','0','0','0','0','F');



insert into attendance values('01', '2017244003', 'late', '2018-06-09 04:50:15','');
insert into attendance values('01', '2018244004', 'absent', '2018-06-09 04:10:15','');
insert into attendance values('01', '2013244005', 'sick', '2018-06-09 04:10:15','');
insert into attendance values('01', '2017244011', 'absent', '2018-06-09 04:10:15','');
insert into attendance values('01', '2015244012', 'early', '2018-06-09 04:10:15','');
insert into attendance values('01', '2015244024', 'absent', '2018-06-09 04:10:15','');
insert into attendance values('01', '2017244025', 'absent', '2018-06-09 04:10:15','');
insert into attendance values('01', '2017244026', 'late', '2018-06-09 04:10:15','');
insert into attendance values('01', '2014244035', 'late', '2018-06-09 04:28:15','');
insert into attendance values('01', '2013244036', 'late', '2018-06-09 04:18:15','');

insert into attendance values('02', '2014244006', 'absent', '2018-06-09 06:18:15','');
insert into attendance values('02', '2014244007', 'absent', '2018-06-09 06:18:15','');
insert into attendance values('02', '2016244008', 'late', '2018-06-09 06:18:15','');
insert into attendance values('02', '2012244009', 'absent', '2018-06-09 06:18:15','');
insert into attendance values('02', '2018244010', 'absent', '2018-06-09 06:18:15','');
insert into attendance values('02', '2017244011', 'sick', '2018-06-09 06:18:15','');
insert into attendance values('02', '2015244012', 'late', '2018-06-09 06:18:15','');
insert into attendance values('02', '2013244017', 'late', '2018-06-09 06:18:15','');
insert into attendance values('02', '2015244018', 'late', '2018-06-09 06:35:15','');
insert into attendance values('02', '2015244013', 'late', '2018-06-09 06:50:15','');

insert into attendance values('03', '2016244034', 'absent', '2018-06-09 08:50:15','');
insert into attendance values('03', '2014244035', 'late', '2018-06-09 08:50:15','');
insert into attendance values('03', '2013244036', 'late', '2018-06-09 08:50:15','');
insert into attendance values('03', '2016244008', 'early', '2018-06-09 08:50:15','');
insert into attendance values('03', '2012244009', 'absent', '2018-06-09 08:50:15','');
insert into attendance values('03', '2018244010', 'absent', '2018-06-09 08:50:15','');
insert into attendance values('03', '2018244023', 'early', '2018-06-09 08:50:15','');
insert into attendance values('03', '2015244024', 'absent', '2018-06-09 08:50:15','');
insert into attendance values('03', '2017244025', 'absent', '2018-06-09 08:50:15','');
insert into attendance values('03', '2018244015', 'late', '2018-06-09 08:50:15','');

insert into attendance values('04', '2015244018', 'absent', '2018-06-09 10:50:15','');
insert into attendance values('04', '2017244019', 'absent', '2018-06-09 10:50:15','');
insert into attendance values('04', '2017244026', 'late', '2018-06-09 10:50:15','');
insert into attendance values('04', '2014244027', 'absent', '2018-06-09 10:50:15','');
insert into attendance values('04', '2018244015', 'sick', '2018-06-09 10:50:15','');
insert into attendance values('04', '2015244024', 'late', '2018-06-09 10:50:15','');
insert into attendance values('04', '2017244025', 'late', '2018-06-09 10:50:15','');
insert into attendance values('04', '2017244022', 'late', '2018-06-09 10:58:15','');
insert into attendance values('04', '2018244023', 'early', '2018-06-09 10:50:15','');
insert into attendance values('04', '2014244001', 'late', '2018-06-09 10:55:15','');

insert into attendance values('05', '2017244016', 'late', '2018-06-09 12:14:15','');
insert into attendance values('05', '2013244017', 'absent', '2018-06-09 12:00:15','');
insert into attendance values('05', '2012244028', 'late', '2018-06-09 12:35:15','');
insert into attendance values('05', '2013244029', 'absent', '2018-06-09 12:00:15','');
insert into attendance values('05', '2015244031', 'early', '2018-06-09 12:00:15','');
insert into attendance values('05', '2016244032', 'sick', '2018-06-09 12:00:15','');
insert into attendance values('05', '2012244033', 'absent', '2018-06-09 12:00:15','');
insert into attendance values('05', '2016244034', 'late', '2018-06-09 12:10:15','');
insert into attendance values('05', '2014244035', 'absent', '2018-06-09 12:00:15','');
insert into attendance values('05', '2013244036', 'late', '2018-06-09 12:00:15','');

insert into attendance values('06', '2017244011', 'absent', '2018-06-09 14:00:15','');
insert into attendance values('06', '2015244012', 'late', '2018-06-09 14:00:15','');
insert into attendance values('06', '2017244019', 'late', '2018-06-09 14:00:15','');
insert into attendance values('06', '2015244020', 'late', '2018-06-09 14:30:15','');
insert into attendance values('06', '2012244028', 'absent', '2018-06-09 14:00:15','');
insert into attendance values('06', '2013244029', 'early', '2018-06-09 14:00:15','');
insert into attendance values('06', '2017244003', 'early', '2018-06-09 14:00:15','');
insert into attendance values('06', '2018244004', 'absent', '2018-06-09 14:00:15','');
insert into attendance values('06', '2015244013', 'sick', '2018-06-09 14:00:15','');
insert into attendance values('06', '2016244014', 'late', '2018-06-09 14:50:15','');

insert into attendance values('07', '2014244001', 'absent', '2018-06-09 16:50:15','');
insert into attendance values('07', '2014244002', 'absent', '2018-06-09 16:50:15','');
insert into attendance values('07', '2017244003', 'absent', '2018-06-09 16:50:15','');
insert into attendance values('07', '2018244004', 'absent', '2018-06-09 16:50:15','');
insert into attendance values('07', '2013244005', 'absent', '2018-06-09 16:50:15','');
insert into attendance values('07', '2014244006', 'absent', '2018-06-09 16:50:15','');
insert into attendance values('07', '2014244007', 'late', '2018-06-09 16:50:15','');
insert into attendance values('07', '2016244008', 'late', '2018-06-09 16:50:15','');
insert into attendance values('07', '2012244009', 'late', '2018-06-09 16:50:15','');
insert into attendance values('07', '2018244010', 'late', '2018-06-09 16:50:15','');



