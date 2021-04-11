/* $Id: libsql.h,v 1.12 2012-05-20 08:54:14 sasa Exp $ */
/*16.05.2012	28.09.1999	Белых А.И.	libsql.h
*/
#include	<stdio.h>

#include	<mysql.h>
#include	<mysqld_error.h>

typedef MYSQL     SQL_baza;
typedef MYSQL_RES SQL_cursor;
typedef MYSQL_ROW SQL_str;
typedef MYSQL_ROW_OFFSET SQL_smesh;


class SQLCURSOR
 {
private:
  SQL_cursor    *cursor;
  short		metka;  //0-курсор не создан 1-создан
public:

  SQLCURSOR(); //Конструктор
  ~SQLCURSOR();         //Деструктор

  long make_cursor(SQL_baza*,const char*); //Создать курсор
  SQL_str read_cursor(SQL_str*);      //Прочитать следующюю строку курсора
  void poz_cursor(int);          //Стать на нужную строку в курсоре
  void delete_cursor(void);      //Уничтожить курсор
  SQL_smesh pozv_cursor(void); //Получить смещение текущей строки в курсоре
  SQL_smesh readv_cursor(SQL_smesh); //Прочитать строку по заданному смещению
  
 };



const char *sql_error(SQL_baza*);
unsigned int	sql_nerror(SQL_baza*);
int	sql_zap(SQL_baza*,const char*);
void    	sql_closebaz(SQL_baza*);
int	sql_openbaz(SQL_baza*,const char*,const char*,const char*,const char*);
void	sqlfiltr(char*,int);
int 	sql_readkey(SQL_baza*,const char*);
int 	sql_readkey(SQL_baza*,const char*,SQL_str*,SQLCURSOR*);
int	sql_flag(SQL_baza*,const char*,int,int);
int	sql_rows(SQL_baza*);
void	msql_error(SQL_baza*,const char*,const char*);
const char *sql_escape_string(const char *str_from,int razmer);


