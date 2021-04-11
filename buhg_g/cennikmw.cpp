/*$Id: cennikmw.c,v 1.18 2014/02/28 05:20:58 sasa Exp $*/
/*10.07.2015	08.5.2003	Белых А.И.	cennikmw.c
Распечатать ценники на отмеченные материалы
*/
#include        <errno.h>
#include	"buhg_g.h"


extern double	okrg1;  /*Округление суммы*/
extern SQL_baza  bd;
extern int kol_strok_na_liste;
void		cennikmw(char *mas,GtkWidget *wpredok)
{
char 	strsql[512];
SQL_str	row1;
SQLCURSOR cur;
class iceb_u_str naim("");
double cena=0.;
class iceb_u_str strihkod("");
class iceb_u_str kodm("");
time_t          tmm;
struct  tm      *bf;

if(mas == NULL)
 return;

time(&tmm);
bf=localtime(&tmm);

int kp=iceb_u_pole2(mas,','); //Количество полей в массиве
if(kp == 0)
 kp=1;



FILE *ff;
char imaf[64];

sprintf(imaf,"cenm%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

for(int i=0; i< kp; i++)
 {
  if(i == 0 && iceb_u_pole(mas,&kodm,i+1,',') != 0)
     kodm.new_plus(mas);
  if(i != 0)
     iceb_u_pole(mas,&kodm,i+1,',');
  cena=0.;
  naim.new_plus("");
  strihkod.new_plus("");

  sprintf(strsql,"select naimat,strihkod,cenapr from Material where kodm=%s",kodm.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur) == 1)
   {
    naim.new_plus(row1[0]);
    strihkod.new_plus(row1[1]);

    cena=atof(row1[2]);
    cena=iceb_u_okrug(cena,okrg1);
   }  

  cennikw(bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,kodm.ravno(),naim.ravno(),cena,strihkod.ravno(),iceb_get_pnk("00",0,wpredok),ff);

 }
fclose(ff);

cenniksw(imaf,wpredok); //Преобразование в колонки

iceb_u_spisok imafils;
iceb_u_spisok naimf;

imafils.plus(imaf);
naimf.plus(gettext("Распечатка ценников для товаров"));

iceb_rabfil(&imafils,&naimf,wpredok);
}
/********************************************/
/*Складывание распечатки в несколько колонок*/
/********************************************/

void	cenniksw(char *imaf,GtkWidget *wpredok)
{
FILE	*ff,*fftmp;
char    imaftmp[64];
char	stroka[1024];
char    masiv[10][512];
short	kol=2; //Количество колонок в распечатке
short   kolstrlist=0;

sprintf(imaftmp,"cen%d.tmp",getpid());
//printw("\nimaftmp=%s\n",imaftmp);
//refresh();

if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }
iceb_u_startfil(fftmp);
fprintf(fftmp,"\x1B\x4D"); /*12-знаков*/

if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

short shethik=0;
short kolstr=0;
memset(&masiv,'\0',sizeof(masiv));
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  stroka[strlen(stroka)-1]='\0';

  if(iceb_u_strstrm(stroka,"┌") == 1)
    kolstr=0;

  if(shethik == 0)
   {
    strcpy(masiv[kolstr],stroka);
   }    
  else
   {
    strcat(masiv[kolstr]," ");
    strcat(masiv[kolstr],stroka);
   }

  kolstr++;

  if(iceb_u_strstrm(stroka,"└") == 1)
    shethik++;

  if(shethik == kol)
   {
    if(kolstrlist+kolstr >= kol_strok_na_liste)
     {
      fprintf(fftmp,"\f");
      kolstrlist=kolstr;
     }
    else
     kolstrlist+=kolstr;
     
    for(int i=0; i < kolstr; i++)
      fprintf(fftmp,"%s\n",masiv[i]);
    memset(&masiv,'\0',sizeof(masiv));

    shethik=0;
   }
  
 }

if(masiv[0][0] != '\0')
  for(int i=0; i < kolstr; i++)
     fprintf(fftmp,"%s\n",masiv[i]);

fprintf(fftmp,"\x1B\x50"); /*10-знаков*/

fclose(ff);
fclose(fftmp);

rename(imaftmp,imaf);

}
