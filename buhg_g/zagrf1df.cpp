/* $Id: zagrf1df.c,v 1.4 2013/08/13 06:09:59 sasa Exp $ */
/*19.06.2019	12.07.2000	Белых А.И. 	zagrf1df.c
Загрузка dbf файла с формой 8ДР в архив
Измененная форма. Наказ ДПА від 08.04.2003 N 164
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg_g.h"

extern short	mmm;
extern SQL_baza bd;

void zagrf1df(const char *nomd, //Номер документа*
int kolst, //Количество строк уже введенных в документ*/
int kvrt, //Квартал
GtkWidget *wpredok)
{
SQL_str         row;
char		strsql[4096];
char		str[1024];
char		bros[512];
char		imadbf[2048];
FILE		*ff;
int		i,i1;
char		inn[16];
double		sumad,suman;
double		sumadn,sumann;
short		d1,m1,g1,d2,m2,g2;
char		priz[4],lgota[4];
time_t		vrem;
class iceb_u_str fio("");
SQLCURSOR curr;

naz:;



imadbf[0]='\0';

sprintf(imadbf,"da0000%02d.%d",1,kvrt);
if(iceb_poldan("Имя DBF файла",str,"zarsdf1df.alx",wpredok) == 0)
  sprintf(imadbf,"%s%02d.%d",str,1,kvrt);


sprintf(strsql,"%s\n%s",gettext("Загрузка формы 1ДФ из файла"),
gettext("Введите имя dbf файла"));

if(iceb_menu_vvod1(strsql,imadbf,sizeof(imadbf),"",wpredok) != 0)
 return;

 
if(imadbf[0] == '\0')
  return;

iceb_refresh();

struct stat work;

if(stat(imadbf,&work) != 0)
 {
  sprintf(strsql,"%s: %s !",gettext("Не найден файл"),imadbf);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

if((ff = fopen(imadbf,"r")) == NULL)
 {
 if(errno == ENOENT)
  {
   sprintf(strsql,"%s: %s !",gettext("Не найден файл"),imadbf);
   iceb_menu_soob(strsql,wpredok);
   goto naz;   
  }
 else
  {
   sprintf(strsql,"%s %s: %d !",gettext("Ошибка открытия файла"),imadbf,errno);
   iceb_menu_soob(strsql,wpredok);
   goto naz;
  }
 }
if(kolst != 0)
 {
  if(iceb_menu_danet(gettext("Удалить уже введённую информацию ?"),2,wpredok) == 1)
   {
    sprintf(strsql,"delete from F8dr1 where nomd='%s'",nomd);
    if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
     return;     
    kolst=0;    

    sprintf(strsql,"delete from F1dfvs where nomd='%s'",nomd);
    iceb_sql_zapis(strsql,1,0,wpredok);
   }
 }

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);

time(&vrem);

float razmer=513;
fseek(ff,513L,SEEK_SET);


char vidz[2];

while(fgets(str,103,ff) != NULL)
 {
  printf("%s*\n",str);
//  razmer+=102;
  razmer+=103;
//  strzag(LINES-1,0,work.st_size,razmer);
  iceb_pbar(gdite.bar,work.st_size,razmer);

  if(str[1] == '9' && str[2] == '9' && str[3] == '9' && str[4] == '9')
    break;

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=6; i < 7; i++)
    bros[i1++]=str[i];
  kvrt=atoi(bros);
 // printw("bros=%s\n",bros);    

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=7; i < 11; i++)
    bros[i1++]=str[i];
//  god=atoi(bros);
//  printw("bros=%s\n",bros);    


  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=21; i < 22; i++)
    bros[i1++]=str[i];
//  tip=atoi(bros);

  memset(inn,'\0',sizeof(inn));
  i1=0;
  for(i=22; i < 32; i++)
    inn[i1++]=str[i];
//  printw("inn=%s*\n",inn);
  fio.new_plus("");
  /*Проверяем наличие в базе индентификационного кода*/
  //У частных предпринимателей нет карточки
  sprintf(strsql,"select inn from Kartb where inn='%s'",inn);
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) < 1)
   {
    sprintf(strsql,"select naikon from Kontragent where kod='%s'",inn);
    if(iceb_sql_readkey(strsql,&row,&curr,wpredok) < 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден индентификационный код"),inn);
      iceb_menu_soob(strsql,wpredok); 
      continue;
     }
    fio.new_plus(row[0]);
   }

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=32; i < 44; i++)
    bros[i1++]=str[i];
  sumadn=atof(bros);

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=44; i < 56; i++)
    bros[i1++]=str[i];
  sumad=atof(bros);
//  printw("sumad=%s*\n",bros);

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=56; i < 68; i++)
    bros[i1++]=str[i];
  sumann=atof(bros);
  sumann=iceb_u_okrug(sumann,0.01);
  
  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=68; i < 80; i++)
    bros[i1++]=str[i];
  suman=atof(bros);
  
  
//  printw("suman=%s*\n",bros);
  
  memset(priz,'\0',sizeof(priz));
  i1=0;
  for(i=80; i < 83; i++)
    priz[i1++]=str[i];

  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=83; i < 87; i++)
    bros[i1++]=str[i];
  d1=m1=g1=0;
  g1=atoi(bros);
  if(g1 != 0)
   {
    memset(bros,'\0',sizeof(bros));
    i1=0;
    for(i=87; i < 89; i++)
      bros[i1++]=str[i];
    m1=atoi(bros);
    memset(bros,'\0',sizeof(bros));
    i1=0;
    for(i=89; i < 91; i++)
      bros[i1++]=str[i];
    d1=atoi(bros);
   }
  memset(bros,'\0',sizeof(bros));
  i1=0;
  for(i=91; i < 95; i++)
    bros[i1++]=str[i];
  d2=m2=g2=0;
  g2=atoi(bros);

  if(g2 != 0)
   {
    memset(bros,'\0',sizeof(bros));
    i1=0;
    for(i=95; i < 97; i++)
      bros[i1++]=str[i];
    m2=atoi(bros);
    memset(bros,'\0',sizeof(bros));
    i1=0;
    for(i=97; i < 99; i++)
      bros[i1++]=str[i];
    d2=atoi(bros);
   }
  memset(lgota,'\0',sizeof(lgota));
  i1=0;
  for(i=99; i < 101; i++)
    lgota[i1++]=str[i];
  if(atoi(lgota) == 0)
    memset(lgota,'\0',sizeof(lgota));

  memset(vidz,'\0',sizeof(vidz));
  vidz[0]=str[101];    
/*
  printw("KOD=%s kvrt=%d god=%d tip=%d inn=%s %.2f %.2f %d.%d.%d %d.%d.%d lgota=%s\n",
  KOD,kvrt,god,tip,inn,sumad,suman,d1,m1,g1,d2,m2,g2,lgota);
  OSTANOV();
*/

  /*Делаем запись в базу*/
  sprintf(strsql,"insert into F8dr1 \
values ('%s','%s',%.2f,%.2f,'%s','%04d-%d-%d','%04d-%d-%d','%s',%d,%ld,'%s',%.2f,%.2f,%s)",
  nomd,inn,sumad,suman,priz,g1,m1,d1,g2,m2,d2,lgota,iceb_getuid(wpredok),vrem,fio.ravno_filtr(),sumadn,sumann,vidz);

zp:;

  if(sql_zap(&bd,strsql) != 0)
   {
    if(kolst != 0 && sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
     {
      printf("Повторяющаяся запись. Увеличивем значения в уже введенной !\n");
      sprintf(strsql,"update F8dr1 set sumad=sumad+%.2f, suman=suman+%.2f,\
sumadn=sumadn+%.2f, sumann=sumann+%.2f where nomd='%s' and inn='%s' and priz='%s' and lgota='%s'",
      sumad,suman,sumadn,sumann,nomd,inn,priz,lgota);
      printf("%s\n",strsql);
      goto zp;
     }
    iceb_msql_error(&bd,"Ошибка ввода записи !",strsql,wpredok);
   }
 }

fclose(ff);
gdite.close();


sprintf(strsql,"%s !",gettext("Загрузка завершена"));
iceb_menu_soob(strsql,wpredok);


}
