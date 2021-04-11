/* $Id: procentw.c,v 1.23 2013/09/26 09:46:53 sasa Exp $ */
/*13.11.2020    10.05.1995      Белых А.И.      procentw.c
Перерасчёт начисления по заданному табельному номеру и проценту
Если введён тариф то считается от тарифа и отработанных дней
Если тариф не введён то считается от суммы введенной в
указанное начисление
*/
#include <ctype.h>
#include        "buhg_g.h"

extern double   okrg;
extern short    *kodotrs;  /*Коды отдельного рачета доплаты до минимальной зарплаты*/
extern short	koddopmin; /*Код доплаты до минимальной зарплаты*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern SQL_baza bd;
#if 0
###########################################################
void procentw(int tn,short mp,short gp,int podr,const char *nah_only,GtkWidget *wpredok)
{
float    dnei; /*Количество рабочих дней в текущем месяце*/
float    hasov; /*Количество рабочих часов в текущем месяце*/
double		koefpv; /*Коэффициент полноты выплаты*/
double		doplata;
class iceb_u_str str("");
class iceb_u_str br("");
class iceb_u_str kn1(""); /*Перечень входных кодов*/
short           kn2=0;     /*Выходной код*/
short		kodud;   //Код удержания
double          proc;
class iceb_u_str tarif_str("");
int            tn1;
double          nah=0.;
double		nahb=0.;
double          tarif=0.;
double          sum,sumb;
int             i,i1;
short           d;
int             mhs=0; /*Метка часовой ставки*/
short		ktab=0;
class iceb_u_str shet("");
SQL_str         row,row_alx;
char		strsql[4096];
long		kolstr;
float		has=0.;
short		gg,mm;
short		nomzap;
float		odnei=0.;
class iceb_u_str den("");
class iceb_u_str uprp(""); /*Метка учета выполенения плана подразделением*/
class iceb_u_str kodpodr(""); /*Код подразделения план которого надо учесть*/
double		procentp=0.; /*Процент выполнения плана подразделением*/
double		procentpb=0.; /*Процент выполнения плана подразделением бюджет*/
float		dneir;    /*Колічество дней взятых для расчёта*/
float		hasovr;   /*Количество часов взятых для расчёта*/
int		iskkod;   /*Исключающий код*/
class iceb_u_str mesqc("");
class iceb_u_str shetbd(""); //Бюджетный счет
class iceb_u_str shetnbd(""); //Не бюджетный счет
class iceb_u_str shetuder(""); //Счёт удержания
class ZARP     zp;
double		bb;
SQLCURSOR curr,cur_alx;
/*
printw("proc-%d %d.%d %d\n",tn,mp,gp,podr);
refresh();
*/
redkalw(mp,gp,&dnei,&hasov,wpredok);
koefpv=proc=nah=tarif=sum=sumb=0.;
dneir=dnei;

sprintf(strsql,"select str from Alx where fil='zarproc.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдены настройки zarproc.alx\n");
  zar_pr_insw(strsql,wpredok);
  return;
 }

class SQLCURSOR cur;
 
iceb_u_dpm(&d,&mp,&gp,5);
zp.tabnom=tn;
zp.prn=1;
zp.knu=kn2;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
short metka_ras=0; //если равна 1-делать расчёт

while(cur_alx.read_cursor(&row_alx) != 0)
 {
//   printw("%s",row_alx[0]);
   if(row_alx[0][0] == '#')
     continue;
     
   hasovr=hasov;  //Иногда обнуляется
   has=odnei=0.;

   iceb_u_polen(row_alx[0],&br,1,'|');

   tn1=(short)br.ravno_atoi();
   metka_ras=0;

   if(iceb_u_strstrm(br.ravno(),"всем") == 1)
    metka_ras=1;

   if(iceb_u_SRAV("под-ние",br.ravno(),1) == 0)
    {
     //извлекаем номер подразделения
     int podr_v_str=0;
     iceb_u_polen(br.ravno(),&podr_v_str,2,' ');
//     int podr_v_str=atoi(str);
     if(podr_v_str != 0)
      {
       //смотрим в каком подразделении находится табельный номер
       sprintf(strsql,"select podr from Zarn where tabn=%d and god=%d and mes=%d",
       tn,gp,mp);
       if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
        if(atoi(row[0]) == podr_v_str)
         {
          sprintf(strsql,"Находится в подразделении %d\n",podr_v_str);
          zar_pr_insw(strsql,wpredok);
          metka_ras=1;
         }
      }
    }    

   if(tn1 == tn || metka_ras == 1)
    {
     koefpv=nah=0.;
//     nomz=1;
     sprintf(strsql,"\nРасчёт по процентам\n\
---------------------------------------------------\n");
     zar_pr_insw(strsql,wpredok);
     zar_pr_insw(row_alx[0],wpredok);

     /*Входной код*/

     iceb_u_polen(row_alx[0],&str,2,'|');
     kn1.new_plus(str.ravno_udsimv(" ")); /*удаляем пробелы*/
     /*Если первый символ не число то это ссылка на строку с настройкой списка кодов*/
     if(isdigit(kn1.ravno()[0]) == 0) /*Значит первый символ не число*/
      {
       sprintf(strsql,"Ищем настройку %s\n",kn1.ravno());
       zar_pr_insw(strsql,wpredok);
       class iceb_u_str spn("");
       iceb_poldan(kn1.ravno(),&spn,"zarproc.alx",wpredok);
       kn1.new_plus(spn.ravno());      
       sprintf(strsql,"Список входных кодов:%s\n",kn1.ravno());
       zar_pr_insw(strsql,wpredok);
      }

     /*Выходной код*/
     zp.nomz=nomzap=0;
     iceb_u_polen(row_alx[0],&kn2,3,'|');
//     kn2=(short)br.ravno_a);
     if(iceb_u_proverka(nah_only,kn2,0,0) != 0)
      {
       sprintf(strsql,"Код %d исключён из расчёта\n",kn2);
       zar_pr_insw(strsql,wpredok);
       continue;
      }

       sprintf(strsql,"Рaсчёт начисления:%d",kn2);
       zar_pr_insw(strsql,wpredok);

       sprintf(strsql,"select naik from Nash where kod=%d",kn2);
       zar_pr_insw(strsql,wpredok);

       if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
        {
         sprintf(strsql," %s",row[0]);
         zar_pr_insw(strsql,wpredok);
        }

       sprintf(strsql,"\n");
       zar_pr_insw(strsql,wpredok);
         
     kodud=0;
     if(iceb_u_polen(br.ravno(),&str,2,'/') == 0)
      kodud=(short)str.ravno_atoi();
     if(kodud != 0)
      {
       sprintf(strsql,"kodud=%d\n",kodud);
       zar_pr_insw(strsql,wpredok);
       sprintf(strsql,"select shet from Uder where kod=%d",kodud);
       if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
         shetuder.new_plus(row[0]);
       else
        {
         shetuder.new_plus("");
         sprintf(strsql,"Не найден код удержания %d в таблице удержаний !\n",kodud);
         zar_pr_insw(strsql,wpredok);
         kodud=0;
        }
      }
     if(iceb_u_polen(br.ravno(),&str,2,':') == 0)
      zp.nomz=nomzap=(short)str.ravno_atoi();

     iceb_u_polen(br.ravno(),&shet,3,':');

     /*Процент*/
     iceb_u_polen(row_alx[0],&proc,4,'|');

     /*Тариф*/
     mhs=0;
     iceb_u_polen(row_alx[0],&tarif_str,5,'|');

     tarif=0.;
     if(iceb_u_strstrm(tarif_str.ravno(),"окл") == 1)
      tarif=okladw(tn,mp,gp,&mhs,wpredok);
     else
      tarif=tarif_str.ravno_atof();


     iceb_u_polen(tarif_str.ravno(),&den,2,':');
     iceb_u_polen(tarif_str.ravno(),&uprp,3,':');
     iceb_u_polen(tarif_str.ravno(),&kodpodr,4,':');

     if(iceb_u_strstrm(tarif_str.ravno(),"/") != 0) /*номер тарифа/разряд*/
      {
       int nomt=br.ravno_atoi();
       int razrad=0;
       iceb_u_polen(br.ravno(),&razrad,2,'/');
      
       sprintf(strsql,"Номер тарифа/разряд: %d/%d\n",nomt,razrad);
       zar_pr_insw(strsql,wpredok);
       tarif=zartarroz1w(nomt,razrad,NULL);

       sprintf(strsql,"Берём в расчёт=%f\n",tarif);
       zar_pr_insw(strsql,wpredok);
       
      }

     if(iceb_u_strstrm(tarif_str.ravno(),"Ч") != 0)
      {
       sprintf(strsql,"Часовая ставка.\n");
       zar_pr_insw(strsql,wpredok);

	mhs=1;
      }
     if(iceb_u_strstrm(tarif_str.ravno(),"*") != 0)
      {
       sprintf(strsql,"Оклад не зависящий от отработанного времени.\n");
       zar_pr_insw(strsql,wpredok);
	mhs=2;
      }
     if(mhs == 0)
      {
       sprintf(strsql,"Оклад зависящий от отработанного времени.\n");
       zar_pr_insw(strsql,wpredok);
      }
     /*Месяц*/
     iceb_u_polen(row_alx[0],&mesqc,6,'|');
     gg=gp;
     mm=mp;
     if(iceb_u_strstrm(mesqc.ravno(),"п") == 1)
      {
       mhs=2;
       mm-=1;
       if(mm == 0)
        {
         mm=12;
         gg-=1;
        }
      }

     /*Исключающий код*/
     iceb_u_polen(row_alx[0],&str,7,'|');
     if((iskkod=str.ravno_atoi()) != 0)
      {
       sprintf(strsql,"select suma from Zarp where datz >= '%04d-%d-01' \
and  datz <= '%04d-%d-31' and tabn=%d and prn='1' and knah=%d and \
suma <> 0.",gp,mp,gp,mp,tn1,iskkod);
       if(iceb_sql_readkey(strsql,wpredok) > 0)
        {
         sprintf(strsql,"Есть не нулевой исключающий код %d !\n",iskkod);
         zar_pr_insw(strsql,wpredok);
         continue;
        }
      }

    /*rintw("br-%s i=%d\n",br,i);*/
     /*Проверяем есть ли код выходной начисления в списке*/

     sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' \
and knah=%d",tn,kn2); 
     if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
      {
       sprintf(strsql,"Нет кода начисления %d в списке  TN%d !\n",kn2,tn);
       zar_pr_insw(strsql,wpredok);
       continue; 
      }
     if(shet.ravno()[0] == '\0')
       shet.new_plus(row[0]);

     /*Читаем суммы по входным кодам если они есть*/
     sum=sumb=0.;

     if(iceb_u_polen(kn1.ravno(),&br,1,',') != 0)
       br.new_plus(kn1.ravno());
     i=1;
     do
      {
       i++;
       sprintf(strsql,"select suma,shet from Zarp where datz >= '%d-%02d-01' \
and datz <= '%d-%02d-31' and tabn=%d and prn='1' and knah=%d and suma <> 0.",
       gg,mm,gg,mm,tn,br.ravno_atoi());
/*       printw("%s\n",strsql);*/
       if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
        iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

       if(kolstr <= 0)
         continue;

       while(cur.read_cursor(&row) != 0)
        {
    
         sum+=atof(row[0]);         
         if(iceb_u_proverka(shetb,row[1],0,1) == 0)
          {
           sumb+=atof(row[0]);
           shetbd.new_plus(row[1]);
          }           
         else
           shetnbd.new_plus(row[1]);
           
        }

      }
     while(iceb_u_polen(kn1.ravno(),&br,i,',') == 0);

     if(tarif == 0.)
      {
         sprintf(strsql,"Тариф равен нолю !\n");
         zar_pr_insw(strsql,wpredok);
         sprintf(strsql,"sum=%.2f sumb=%.2f proc=%.2f\n",
         sum,sumb,proc);
         zar_pr_insw(strsql,wpredok);

       nah=sum*proc/100.;
       nahb=sumb*proc/100.;

//       printw("nah=%f nahb=%f\n",nah,nahb);
//       OSTANOV();
       koefpv=proc/100.;
      }
     else
      {
       sprintf(strsql,"Тариф:%.2f\n",tarif);
       zar_pr_insw(strsql,wpredok);
       /*Чтение табеля*/
       sprintf(strsql,"select kodt from Nash where kod=%d",
       kn2);
//       printw("%s\n",strsql);
       if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
        {
         sprintf(strsql,"procent-Не найден код начисления %d",kn2);
         iceb_menu_soob(strsql,wpredok);
         continue;
        }
       
       ktab=atoi(row[0]);
       sprintf(strsql,"Код табеля=%d для кода начисления %d\n",ktab,kn2);
       zar_pr_insw(strsql,wpredok);
/*
       if(ktab == 0)
        {
         beep();
         printw("%s %d\n",gettext("procent-Не введён код табеля для начисления"),kn2);
         OSTANOV();
         continue; 
        }
*/
       sprintf(strsql,"select has,dnei,kolrd from Ztab where \
god=%d and mes=%d and tabn=%d and kodt=%d order by nomz asc",
       gp,mp,tn,ktab);
//       printw("\n%s\n",strsql);
//       OSTANOV();
       has=odnei=0.;
       if(sql_readkey(&bd,strsql,&row,&curr) > 0)
        {
         has=atof(row[0]);
         odnei=atof(row[1]);
         sprintf(strsql,"Прочитано часов=%f дней=%f Количество рабочих дней в месяце %s\n",
          has,odnei,row[2]);
         zar_pr_insw(strsql,wpredok);

         if(row[2][0] != '0')
           dneir=atof(row[2]);
         else
          if(ktab != 1)
           {
            sprintf(strsql,"select kolrd from Ztab where \
god=%d and mes=%d and tabn=%d and kodt=1 order by nomz asc",gp,mp,tn);
//       printw("%s\n",strsql);

            if(sql_readkey(&bd,strsql,&row,&curr) > 0)
             {
              if(atof(row[0]) != 0.)
                dneir=atof(row[0]);
              sprintf(strsql,"Берем для расчёта %f рабочих дней в месяце.\n",
                dneir);                
              zar_pr_insw(strsql,wpredok);
             }
           }
          else
           {
            dneir=dnei;
            hasovr=hasov;
          
           }
        }
       else
        {
         hasovr=dneir=0.;
        }

       if(proc != 0.)
	{
	 if(mhs == 0)
          {
           if(iceb_u_strstrm(den.ravno(),"Д") == 1)
            {
             if(dneir != 0.)
   	      {
   	       nah=tarif/dneir*odnei*proc/100.;  /*Оклад зависящий от отработ. врем*/
               koefpv=dneir/odnei*proc/100.;
	        sprintf(strsql,"Расчёт по отработанным дням.\n\
nah=%.2f/%.2f*%.2f*%.2f/100.=%.2f\n",
                tarif,dneir,odnei,proc,nah);
              zar_pr_insw(strsql,wpredok);
              }
             else
              {
               sprintf(strsql,"Не введено количество рабочих дней в месяце !\n");              
               zar_pr_insw(strsql,wpredok);
              }
            }
           else
            {
             if(iceb_u_strstrm(tarif_str.ravno(),"Д") == 1) /*дневная ставка*/
              {
   	       nah=tarif*odnei*proc/100.;  /*Оклад зависящий от отработ. врем*/
               sprintf(strsql,"Расчёт по днeвной ставке.\nnah=%.2f*%.2f*%.2f/100.=%.2f\n",tarif,odnei,proc,nah);
               zar_pr_insw(strsql,wpredok);
              }
             else
              {
               if(hasovr != 0.)
                {
                 nah=tarif/hasovr*has*proc/100.;  /*Оклад зависящий от отработ. врем*/
                 koefpv=hasovr/has*proc/100.;
                 sprintf(strsql,"Расчёт по отработанным часам.\n\nah=%.2f/%.2f*%.2f*%.2f/100.=%.2f\n",
                 tarif,hasovr,has,proc,nah);
                 zar_pr_insw(strsql,wpredok);
                }
               else
                {
                  sprintf(strsql,"Не введено количество рабочих часов в месяце !\n");
                 zar_pr_insw(strsql,wpredok);
                }
              }
            }
          }
	 if(mhs == 1) /*Часовая ставка*/
          {
 	   nah=tarif*has*proc/100.;
           koefpv=proc/100.;
           sprintf(strsql,"Расчёт по часовой ставке.\n\
nah=%.2f*%.2f*%.2f/100.=%.2f\n",
            tarif,has,proc,nah);
           zar_pr_insw(strsql,wpredok);
          }
	 if(mhs == 2) /*Оклад не зависящий от отработанного времени*/
          {
 	   nah=tarif*proc/100.;
           koefpv=proc/100.;
 	    sprintf(strsql,"Не зависит от отработанного времени.\n\
nah=%.2f*%.2f/100.=%.2f\n",
            tarif,proc,nah);
           zar_pr_insw(strsql,wpredok);
          }
  /*     
	 printw("nah=%.2f tarif=%.2f mhs=%d has=%.2f\n\n",
	 nah,tarif,mhs,has);
*/	 
	}
       if(proc == 0.)
        {
          sprintf(strsql,"Сумма не зависящая ни от чего %.2f.\n",tarif);
           zar_pr_insw(strsql,wpredok);
         
         nah=tarif;
         koefpv=1.;
        }
      }
     /*Учет выполения плана подразделением*/
     sprintf(strsql,"uprp=%s kodpodr=%s nah=%.2f nahb=%.2f\n",
       uprp.ravno(),kodpodr.ravno(),nah,nahb);
     zar_pr_insw(strsql,wpredok);
     if(iceb_u_strstrm(uprp.ravno(),"П") == 1)
      {
       if((i=kodpodr.ravno_atoi()) == 0)
          i=podr;

       procentp=zarpodprw(i,&procentpb,wpredok);

       koefpv*=procentp/100.;
       sprintf(strsql,"nah=%.2f procentp=%f koefpv=%f\n",nah,procentp,koefpv);
       zar_pr_insw(strsql,wpredok);
         
      }     
     nah=iceb_u_okrug(nah,okrg);
     nahb=iceb_u_okrug(nahb,okrg);
/*      
     printw("kn2=%d nah=%.2f\n",kn2,nah);
     OSTANOV();      
*/
     if(nahb != 0. && shetb != NULL)
      {
         sprintf(strsql,"Сумма %f Бюджет %f/%s Не бюджет %s\n",
         nah,nahb,shetbd.ravno(),shetnbd.ravno());
       zar_pr_insw(strsql,wpredok);
 

       strcpy(zp.shet,shetbd.ravno());
       bb=nahb;        
     if(iceb_u_strstrm(uprp.ravno(),"П") == 1)
        {
         bb=nahb*procentpb/100.;
         sprintf(strsql,"Бюджет %.2f*%f/100.=%.2f Счёт:%s\n",nahb,procentpb,bb,shetbd.ravno());
         zar_pr_insw(strsql,wpredok);
        }

       if(shetbd.ravno()[0] != '\0')
        {
         strcpy(zp.shet,shetbd.ravno());
         zp.knu=kn2;
         zapzarpw(&zp,bb,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
         if(kodud != 0)
          {
           strcpy(zp.shet,shetuder.ravno());
           zp.knu=kodud;
           zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
          }       
        }

       bb=nah-nahb;        

       if(iceb_u_strstrm(uprp.ravno(),"П") == 1)
        {
         bb=(nah-nahb)*procentp/100.;
         sprintf(strsql,"(%.2f-%.2f)*%f/100.=%.2f Счёт:%s\n",
          nah,nahb,procentp,bb,shetnbd.ravno());
         zar_pr_insw(strsql,wpredok);
        }

       if(shetnbd.ravno()[0] != '\0')
        {
         strcpy(zp.shet,shetnbd.ravno());
         zp.knu=kn2;
         zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
         if(kodud != 0)
          {
           strcpy(zp.shet,shetuder.ravno());
           zp.knu=kodud;
           zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
          }
        }
      }
     else
      {
       strcpy(zp.shet,shet.ravno());

       bb=nah;        
       if(iceb_u_strstrm(uprp.ravno(),"П") == 1)
        {
         bb=nah*procentp/100.;
         sprintf(strsql,"Хозрасчёт %.2f*%f/100.=%.2f Счёт:%s\n",nah,procentp,bb,shet.ravno());
         zar_pr_insw(strsql,wpredok);
        }
       else
        {
          sprintf(strsql,"Сумма=%.2f Счёт:%s\n",bb,shet.ravno());
         zar_pr_insw(strsql,wpredok);
        }
       strcpy(zp.shet,shet.ravno());
       zp.knu=kn2;
       zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
       if(kodud != 0)
        {
          sprintf(strsql,"Удержание=%.2f shetuder=%s kodud=%d\n",bb,shetuder.ravno(),kodud);
         zar_pr_insw(strsql,wpredok);
         strcpy(zp.shet,shetuder.ravno());
         zp.knu=kodud;
         zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
        }
      }

     /*Проверяем на коды отдельного расчёта доплаты до минимальной зарплаты*/
     if(kodotrs != NULL && koddopmin != 0)
      {

       /*Проверяем есть ли код доплаты в списке*/

       sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
       knah=%d",tn,koddopmin); 
       if(sql_readkey(&bd,strsql,&row,&curr) < 1)
         continue;

       shet.new_plus(row[0]);

       for(i1=1; i1 <= kodotrs[0]; i1++)
        if(kn2 == kodotrs[i1])
         break;
       if(i1 > kodotrs[0])
         continue;

       class zar_read_tn1h nastr;
       zar_read_tn1w(1,mp,gp,&nastr,wpredok);

       doplata=nastr.minzar*koefpv-nah;

       sprintf(strsql,"Расчёт доплаты до минимальной зарплаты.\n\
%.2f*%f-%.2f=%.2f (i1=%d kodotrs[0]=%d)\n",
         nastr.minzar,koefpv,nah,doplata,i1,kodotrs[0]);
       zar_pr_insw(strsql,wpredok);

       if(doplata < 0.01)
          continue;
       doplata=iceb_u_okrug(doplata,okrg);
       strcpy(zp.shet,shet.ravno());
       zp.knu=koddopmin;
       zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);

      }
    }
  }
}
########################################
#endif


void procentw(int tn,short mp,short gp,int podr,const char *nah_only,GtkWidget *wpredok)
{

class dirzarsdo_menu_mr srz; /*способ расчёта зарплаты*/

float    dnei=0.; /*Количество рабочих дней в текущем месяце*/
float hasov=0.;
double		koefpv; /*Коэффициент полноты выплаты*/
double		doplata;
//short		nomz=0;
class iceb_u_str str("");
class iceb_u_str br("");
class iceb_u_str kn1(""); /*Перечень входных кодов*/
short           kn2=0;     /*Выходной код*/
short		kodud;   //Код удержания
double          proc;
int           tn1;
double          nah=0.;
double		nahb=0.;
double          tarif=0.;
class iceb_u_str tarif_str("");
double          sum,sumb;
int             i,i1;
short           d;
int             mhs=0; /*Метка часовой ставки*/
short		ktab=0;
class iceb_u_str shet("");
SQL_str         row,row_alx;
char		strsql[4096];
long		kolstr;
float		has=0.;
short		gg,mm;
short		nomzap;
float		odnei=0.;
class iceb_u_str den("");
class iceb_u_str uprp(""); /*Метка учета выполенения плана подразделением*/
class iceb_u_str kodpodr(""); /*Код подразделения план которого надо учесть*/
double		procentp=0.; /*Процент выполнения плана подразделением*/
double		procentpb=0.; /*Процент выполнения плана подразделением бюджет*/
float		dneir=0;    /*Колічество дней взятых для расчёта*/
float		hasovr;   /*Количество часов взятых для расчёта*/
int		iskkod;   /*Исключающий код*/
class iceb_u_str mesqc("");
class iceb_u_str shetbd(""); //Бюджетный счет
class iceb_u_str shetnbd(""); //Не бюджетный счет
class iceb_u_str shetuder(""); //Счёт удержания
class ZARP     zp;
double		bb;
SQLCURSOR curr,cur_alx;
/*
printw("proc-%d %d.%d %d\n",tn,mp,gp,podr);
refresh();
*/
redkalw(mp,gp,&dnei,&hasov,wpredok);
koefpv=proc=nah=tarif=sum=sumb=0.;
dneir=dnei;

sprintf(strsql,"\n%s-Расчёт по процентам\n-----------------------------------------------\n",__FUNCTION__);
zar_pr_insw(strsql,wpredok);

sprintf(strsql,"select str from Alx where fil='zarproc.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s-Не найдены настройки zarproc.alx\n",__FUNCTION__);
  zar_pr_insw(strsql,wpredok);
  return;
 }

class SQLCURSOR cur;
 
iceb_u_dpm(&d,&mp,&gp,5);
/*********************
zp.dz=d;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
short metka_ras=0; //если равна 1-делать расчёт
*********************/

zp.tabnom=tn;
zp.prn=1;
zp.knu=kn2;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
short metka_ras=0; //если равна 1-делать расчёт











while(cur_alx.read_cursor(&row_alx) != 0)
 {
//   printw("%s",row_alx[0]);
   if(row_alx[0][0] == '#')
     continue;
     
   hasovr=hasov;  //Иногда обнуляется
   has=odnei=0.;

   iceb_u_polen(row_alx[0],&br,1,'|');

   tn1=(short)br.ravno_atoi();

   metka_ras=0;

   if(iceb_u_strstrm(br.ravno(),"всем") == 1)
    metka_ras=1;

   if(iceb_u_SRAV("под-ние",br.ravno(),1) == 0)
    {
     //извлекаем номер подразделения
     int podr_v_str=0;
     iceb_u_polen(br.ravno(),&podr_v_str,2,' ');
     if(podr_v_str != 0)
      {
       //смотрим в каком подразделении находится табельный номер
       sprintf(strsql,"select podr from Zarn where tabn=%d and god=%d and mes=%d",tn,gp,mp);
       if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
        if(atoi(row[0]) == podr_v_str)
         {
          sprintf(strsql,"%s-Находится в подразделении %d\n",__FUNCTION__,podr_v_str);
          zar_pr_insw(strsql,wpredok);
          metka_ras=1;
         }
      }
    }    

   if(tn1 == tn || metka_ras == 1)
    {
     koefpv=nah=0.;

     zar_pr_insw(row_alx[0],wpredok); /*показываем найденную настройку*/

     /*Входной код*/
     iceb_u_polen(row_alx[0],&str,2,'|');
     kn1.new_plus(str.ravno_udsimv(" ")); /*Удаляем пробелы*/

     /*Если первый символ не число то это ссылка на строку с настройкой списка кодов*/
     if(isdigit(kn1.ravno()[0]) == 0) /*Значит первый символ не число*/
      {
       sprintf(strsql,"%s-Первый символ не число-ищем настройку %s\n",__FUNCTION__,kn1.ravno());
       zar_pr_insw(strsql,wpredok);

       class iceb_u_str spn("");
       iceb_poldan(kn1.ravno(),&spn,"zarproc.alx",wpredok);
       kn1.new_plus(spn.ravno());      

       sprintf(strsql,"%s-Список входных кодов:%s\n",__FUNCTION__,kn1.ravno());
       zar_pr_insw(strsql,wpredok);
      }

     /*Выходной код*/
     zp.nomz=nomzap=0;
     iceb_u_polen(row_alx[0],&br,3,'|');
     kn2=(short)br.ravno_atoi();
     if(iceb_u_proverka(nah_only,kn2,0,0) != 0)
      {
       sprintf(strsql,"%s-Код начисления %d исключён из расчёта",__FUNCTION__,kn2);
       zar_pr_insw(strsql,wpredok);
       continue;
      }

     sprintf(strsql,"%s-Рaсчёт начисления:%d",__FUNCTION__,kn2);
     zar_pr_insw(strsql,wpredok);

     sprintf(strsql,"select naik from Nash where kod=%d",kn2);
     if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      {
       sprintf(strsql," %s",row[0]);
       zar_pr_insw(strsql,wpredok);
      }
     sprintf(strsql,"\n");
     zar_pr_insw(strsql,wpredok);

     kodud=0;
     if(iceb_u_polen(br.ravno(),&str,2,'/') == 0)
      kodud=(short)str.ravno_atoi();
     if(kodud != 0)
      {

       sprintf(strsql,"%s-kodud=%d\n",__FUNCTION__,kodud);
       zar_pr_insw(strsql,wpredok);

       sprintf(strsql,"select shet from Uder where kod=%d",kodud);
       if(sql_readkey(&bd,strsql,&row,&curr) == 1)
         shetuder.new_plus(row[0]);
       else
        {
         shetuder.new_plus("");
         sprintf(strsql,"%s-Не найден код удержания %d в таблице удержаний !\n",__FUNCTION__,kodud);
         zar_pr_insw(strsql,wpredok);
         kodud=0;
        }
      }
     if(iceb_u_polen(br.ravno(),&str,2,':') == 0)
      zp.nomz=nomzap=(short)str.ravno_atoi();

     iceb_u_polen(br.ravno(),&shet,3,':');

     /*Процент*/
     iceb_u_polen(row_alx[0],&proc,4,'|');

     /*Тариф*/
     mhs=1; /*по умолчанию способ расчёта по отработанному времени (по часам)*/
     iceb_u_polen(row_alx[0],&tarif_str,5,'|');
     
     tarif=0.;
     if(iceb_u_strstrm(tarif_str.ravno(),"окл") == 1)
      {
       sprintf(strsql,"%s-Нашли настройку \"оклад\"\n",__FUNCTION__);
       zar_pr_insw(strsql,wpredok);

       tarif=okladw(tn,mp,gp,&mhs,wpredok);

/******************
      mhs=
        0 - оплата за месяц расчётная по отработанным дням
        1 - оплата за месяц расчётная по отработанным часам
        2 - оплата за месяц постоянная
        3 - оплата за час
        4 - оплата за день
        5 - оплата по тарифу (минимальная зарплата умноженная на тарифный коэффициент)
******************/
  
      }
     else
      {
       tarif=tarif_str.ravno_atof();
      }   
     sprintf(strsql,"%s-Тариф=%f mhs=%d\n",__FUNCTION__,tarif,mhs);
     zar_pr_insw(strsql,wpredok);

     sprintf(strsql,"%s-Способ расчёта:%s\n",__FUNCTION__,srz.strsr.ravno(mhs));
     zar_pr_insw(strsql,wpredok);

     
     iceb_u_polen(tarif_str.ravno(),&den,2,':');
     iceb_u_polen(tarif_str.ravno(),&uprp,3,':');
     iceb_u_polen(tarif_str.ravno(),&kodpodr,4,':');

     if(iceb_u_strstrm(tarif_str.ravno(),"/") != 0) /*номер тарифа/разряд*/
      {
       int nomt=br.ravno_atoi();
       int razrad=0;
       iceb_u_polen(br.ravno(),&razrad,2,'/');
      
       sprintf(strsql,"%s-Номер тарифа/разряд: %d/%d\n",__FUNCTION__,nomt,razrad);
       zar_pr_insw(strsql,wpredok);

       tarif=zartarroz1w(nomt,razrad,NULL);

       sprintf(strsql,"%s-Берём в расчёт=%f\n",__FUNCTION__,tarif);
       zar_pr_insw(strsql,wpredok);
       
      }
     
     if(iceb_u_strstrm(tarif_str.ravno(),"Ч") != 0)
      {
       sprintf(strsql,"%s-Часовая ставка\n",__FUNCTION__);
       zar_pr_insw(strsql,wpredok);

	mhs=3;
      }
     
     if(iceb_u_strstrm(tarif_str.ravno(),"*") != 0)
      {
       sprintf(strsql,"%s-Оклад не зависящий от отработанного времени\n",__FUNCTION__);
       zar_pr_insw(strsql,wpredok);
	mhs=2;
      }
     if(mhs == 0)
      {
       sprintf(strsql,"%s-Оклад зависящий от отработанного времени\n",__FUNCTION__);
       zar_pr_insw(strsql,wpredok);
      }
  
     /*Месяц*/
     iceb_u_polen(row_alx[0],&mesqc,6,'|');
     gg=gp;
     mm=mp;
     if(iceb_u_strstrm(mesqc.ravno(),"п") == 1)
      {
       mhs=2;
       mm-=1;
       if(mm == 0)
        {
         mm=12;
         gg-=1;
        }
      }

     /*Исключающий код*/
     iceb_u_polen(row_alx[0],&str,7,'|');
     if((iskkod=str.ravno_atoi()) != 0)
      {
       sprintf(strsql,"select suma from Zarp where datz >= '%04d-%d-01' \
and  datz <= '%04d-%d-31' and tabn=%d and prn='1' and knah=%d and \
suma <> 0.",gp,mp,gp,mp,tn1,iskkod);
       if(iceb_sql_readkey(strsql,wpredok) > 0)
        {
         sprintf(strsql,"%s-Есть не нулевой исключающий код %d !\n",__FUNCTION__,iskkod);
         zar_pr_insw(strsql,wpredok);
         continue;
        }
      }

     /*Проверяем есть ли код выходной начисления в списке*/

     sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and knah=%d",tn,kn2); 
     if(sql_readkey(&bd,strsql,&row,&curr) != 1)
      {
       sprintf(strsql,"%s-Нет кода начисления %d в списке  TN%d !\n",__FUNCTION__,kn2,tn);
       zar_pr_insw(strsql,wpredok);
       continue; 
      }
     if(shet.ravno()[0] == '\0')
       shet.new_plus(row[0]);

     /*Читаем суммы по входным кодам если они есть*/
     sum=sumb=0.;

     if(iceb_u_polen(kn1.ravno(),&br,1,',') != 0)
       br.new_plus(kn1.ravno());
     i=1;
     do
      {
       i++;
       sprintf(strsql,"select suma,shet from Zarp where datz >= '%d-%02d-01' \
and datz <= '%d-%02d-31' and tabn=%d and prn='1' and knah=%d and suma <> 0.",
       gg,mm,gg,mm,tn,br.ravno_atoi());

       if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
        iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

       if(kolstr <= 0)
        {
         continue;
        }

       while(cur.read_cursor(&row) != 0)
        {
    
         sum+=atof(row[0]);         
         if(iceb_u_proverka(shetb,row[1],0,1) == 0)
          {
           sumb+=atof(row[0]);
           shetbd.new_plus(row[1]);
          }           
         else
           shetnbd.new_plus(row[1]);
           
        }

      }

     while(iceb_u_polen(kn1.ravno(),&br,i,',') == 0);

     if(tarif == 0.)
      {
       sprintf(strsql,"%s-Тариф равен нолю !\n",__FUNCTION__);
       zar_pr_insw(strsql,wpredok);

       sprintf(strsql,"%s-sum=%.2f sumb=%.2f proc=%.2f\n",__FUNCTION__,sum,sumb,proc);
       zar_pr_insw(strsql,wpredok);
       nah=sum*proc/100.;
       nahb=sumb*proc/100.;

       koefpv=proc/100.;
      }
     else
      {
       sprintf(strsql,"%s-Тариф:%.2f\n",__FUNCTION__,tarif);
       zar_pr_insw(strsql,wpredok);
       /*Чтение табеля*/
       sprintf(strsql,"select kodt from Nash where kod=%d",kn2);
//       printw("%s\n",strsql);
       if(sql_readkey(&bd,strsql,&row,&curr) != 1)
        {
         iceb_beep();
         sprintf(strsql,"%s-Не найден код начисления %d\n",__FUNCTION__,kn2);
         iceb_menu_soob(strsql,wpredok);
//         OSTANOV();
         continue;
        }
       
       ktab=atoi(row[0]);
       sprintf(strsql,"%s-Код табеля=%d для кода начисления %d\n",__FUNCTION__,ktab,kn2);
       zar_pr_insw(strsql,wpredok);

       if(ktab == 0)
        {
         sprintf(strsql,"%s-%s %d\n","Не введён код табеля для начисления",__FUNCTION__,kn2);
         zar_pr_insw(strsql,wpredok);
        }

       sprintf(strsql,"select has,dnei,kolrd from Ztab where \
god=%d and mes=%d and tabn=%d and kodt=%d order by nomz asc",gp,mp,tn,ktab);
       has=odnei=0.;
       if(sql_readkey(&bd,strsql,&row,&curr) > 0)
        {
         has=atof(row[0]);
         odnei=atof(row[1]);
         sprintf(strsql,"%s-Прочитано часов=%f дней=%f Количество рабочих дней в месяце %s\n",__FUNCTION__,has,odnei,row[2]);
         zar_pr_insw(strsql,wpredok);

         if(row[2][0] != '0')
          {
           dneir=atof(row[2]);
          }
         else
          if(ktab != 1)
           {
            sprintf(strsql,"select kolrd from Ztab where \
god=%d and mes=%d and tabn=%d and kodt=1 order by nomz asc",gp,mp,tn);

            if(sql_readkey(&bd,strsql,&row,&curr) > 0)
             {
              if(atof(row[0]) != 0.)
                dneir=atof(row[0]);
              sprintf(strsql,"%s-Берем для расчёта %f рабочих дней в месяце\n",__FUNCTION__,dneir);                
              zar_pr_insw(strsql,wpredok);
             }
           }
          else
           {
            dneir=dnei;
            hasovr=hasov;
          
           }
        }
       else
        {
         hasovr=dneir=0.;
        }


/******************
      mhs=
        0 - оплата за месяц расчётная по отработанным дням
        1 - оплата за месяц расчётная по отработанным часам
        2 - оплата за месяц постоянная
        3 - оплата за час
        4 - оплата за день
        5 - оплата по тарифу (минимальная зарплата умноженная на тарифный коэффициент)
******************/
       if(proc != 0.)
	{
	 if(mhs == 0 || mhs == 1)
          {

           if(iceb_u_strstrm(den.ravno(),"Д") == 1 || mhs == 0)
            {
             if(dneir != 0.)
   	      {
   	       nah=tarif/dneir*odnei*proc/100.;  /*Оклад зависящий от отработ. врем*/
               koefpv=dneir/odnei*proc/100.;
               sprintf(strsql,"%s-Расчёт по отработанным дням.\nnah=%.2f/%.2f*%.2f*%.2f/100.=%.2f\n",__FUNCTION__,tarif,dneir,odnei,proc,nah);
               zar_pr_insw(strsql,wpredok);
              }
             else
              {
               sprintf(strsql,"%s-Не введено количество рабочих дней в месяце !\n",__FUNCTION__);
               zar_pr_insw(strsql,wpredok);
              }
            }
           else
            {
             if(iceb_u_strstrm(tarif_str.ravno(),"Д") == 1 && iceb_u_strstrm(tarif_str.ravno(),"оклад") != 1) /*дневная ставка. В слове оклад есть буква "д"*/
              {
   	       nah=tarif*odnei*proc/100.;  /*Оклад зависящий от отработ. врем*/
               sprintf(strsql,"%s-Расчёт по дневной ставке.\nnah=%.2f*%.2f*%.2f/100.=%.2f\n",__FUNCTION__,tarif,odnei,proc,nah);
               zar_pr_insw(strsql,wpredok);
               zar_pr_insw(tarif_str.ravno(),wpredok);
               zar_pr_insw("\n",wpredok);
              }
             else
              {
               if(hasovr != 0.)
                {
                 nah=tarif/hasovr*has*proc/100.;  /*Оклад зависящий от отработ. врем*/
                 koefpv=hasovr/has*proc/100.;
                 sprintf(strsql,"%s-Расчёт по отработанным часам.\nnah=%.2f/%.2f*%.2f*%.2f/100.=%.2f\n",__FUNCTION__,tarif,hasovr,has,proc,nah);
                 zar_pr_insw(strsql,wpredok);
                }
               else
                {
                 sprintf(strsql,"%s-Не введено количество рабочих часов в месяце !\n",__FUNCTION__);
                 zar_pr_insw(strsql,wpredok);
                }
              }
            }
          }

	 if(mhs == 3) /*Часовая ставка*/
          {
 	   nah=tarif*has*proc/100.;
           koefpv=proc/100.;
           sprintf(strsql,"%s-Расчёт по часовой ставке.\nnah=%.2f*%.2f*%.2f/100.=%.2f\n",__FUNCTION__,tarif,has,proc,nah);
           zar_pr_insw(strsql,wpredok);
          }
	 if(mhs == 2) /*Оклад не зависящий от отработанного времени*/
          {
 	   nah=tarif*proc/100.;
           koefpv=proc/100.;
           sprintf(strsql,"%s-Не зависит от отработанного времени.\nnah=%.2f*%.2f/100.=%.2f\n",__FUNCTION__,tarif,proc,nah);
           zar_pr_insw(strsql,wpredok);
          }
  /*     
	 printw("nah=%.2f tarif=%.2f mhs=%d has=%.2f\n\n",
	 nah,tarif,mhs,has);
*/	 
	}

       if(proc == 0.)
        {
         sprintf(strsql,"%s-Сумма не зависящая ни от чего %.2f.\n",__FUNCTION__,tarif);
         zar_pr_insw(strsql,wpredok);
         
         nah=tarif;
         koefpv=1.;
        }
      }
     /*Учет выполения плана подразделением*/
     if(iceb_u_strstrm(uprp.ravno(),"П") == 1)
      {
       sprintf(strsql,"%s-Учёт выполнения плана подразделением=%s Код подразделения=%s nah=%.2f nahb=%.2f\n",
       __FUNCTION__,
       uprp.ravno(),kodpodr.ravno(),nah,nahb);
       zar_pr_insw(strsql,wpredok);

       if((i=kodpodr.ravno_atoi()) == 0)
          i=podr;

       procentp=zarpodprw(i,&procentpb,wpredok);

       koefpv*=procentp/100.;
       sprintf(strsql,"%s-nah=%.2f procentp=%f koefpv=%f\n",__FUNCTION__,nah,procentp,koefpv);
       zar_pr_insw(strsql,wpredok);
         
      }     
     nah=iceb_u_okrug(nah,okrg);
     nahb=iceb_u_okrug(nahb,okrg);
 
/*      
     printw("kn2=%d nah=%.2f\n",kn2,nah);
     OSTANOV();      
*/
     if(nahb != 0. && shetb != NULL)
      {
       sprintf(strsql,"%s-Сумма %f Бюджет %f/%s Не бюджет %s\n",__FUNCTION__,nah,nahb,shetbd.ravno(),shetnbd.ravno());
       zar_pr_insw(strsql,wpredok);
 

       strcpy(zp.shet,shetbd.ravno());
       bb=nahb;        
       if(iceb_u_strstrm(uprp.ravno(),"П") == 1)
        {
         bb=nahb*procentpb/100.;
         sprintf(strsql,"%s-Бюджет %.2f*%f/100.=%.2f Счёт:%s\n",__FUNCTION__,nahb,procentpb,bb,shetbd.ravno());
         zar_pr_insw(strsql,wpredok);
        }

       if(shetbd.ravno()[0] != '\0')
        {
         strcpy(zp.shet,shetbd.ravno());
         zp.knu=kn2;
         zapzarpw(&zp,bb,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
         if(kodud != 0)
          {
           strcpy(zp.shet,shetuder.ravno());
           zp.knu=kodud;
           zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
          }       
        }

       bb=nah-nahb;        

       if(iceb_u_strstrm(uprp.ravno(),"П") == 1)
        {
         bb=(nah-nahb)*procentp/100.;
         sprintf(strsql,"%s-(%.2f-%.2f)*%f/100.=%.2f Счёт:%s\n",__FUNCTION__,nah,nahb,procentp,bb,shetnbd.ravno());
         zar_pr_insw(strsql,wpredok);
        }

       if(shetnbd.ravno()[0] != '\0')
        {
         strcpy(zp.shet,shetnbd.ravno());
         zp.knu=kn2;
         zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
//         zapzarp(d,mp,gp,tn,1,kn2,bb,shetnbd.ravno(),mp,gp,0,nomzap,"",podr,"",zp); 
         if(kodud != 0)
          {
           strcpy(zp.shet,shetuder.ravno());
           zp.knu=kodud;
           zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
//           zapzarp(d,mp,gp,tn,2,kodud,bb*-1,shetuder.ravno(),mp,gp,0,nomzap,"",podr,"",zp); 
          }
        }
      }
     else
      {
       br.new_plus("");
       if(proc != 0.)
        {
         sprintf(strsql,"%.2f%% %s",proc,tarif_str.ravno());       
         br.new_plus(strsql);
        }
       strcpy(zp.shet,shet.ravno());

       bb=nah;        
       if(iceb_u_strstrm(uprp.ravno(),"П") == 1)
        {
         bb=nah*procentp/100.;
         sprintf(strsql,"%s-Хозрасчёт %.2f*%f/100.=%.2f Счёт:%s\n",__FUNCTION__,nah,procentp,bb,shet.ravno());
         zar_pr_insw(strsql,wpredok);
        }
       else
        {
         sprintf(strsql,"%s-Сумма=%.2f Счёт:%s nah=%.2f kodud=%d\n",__FUNCTION__,bb,shet.ravno(),nah,kodud);
         zar_pr_insw(strsql,wpredok);
        }
       strcpy(zp.shet,shet.ravno());

       sprintf(strsql,"%s-%d.%d.%d tn=%d kn2=%d bb=%.2f shet=%s mp=%d gp=%d nomzap=%d\n",
       __FUNCTION__,d,mp,gp,tn,kn2,bb,shet.ravno(),mp,gp,nomzap);
       zar_pr_insw(strsql,wpredok);
        
       zp.knu=kn2;
       zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
//       zapzarp(d,mp,gp,tn,1,kn2,bb,shet.ravno(),mp,gp,0,nomzap,br.ravno(),podr,"",zp); 
       if(kodud != 0)
        {
         sprintf(strsql,"%s-Удержание=%.2f shetuder=%s kodud=%d\n",__FUNCTION__,bb,shetuder.ravno(),kodud);
         zar_pr_insw(strsql,wpredok);
         strcpy(zp.shet,shetuder.ravno());
         zp.knu=kodud;
         zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
//         zapzarp(d,mp,gp,tn,2,kodud,bb*-1,shetuder.ravno(),mp,gp,0,nomzap,br.ravno(),podr,"",zp); 
        }
      }

     /*Проверяем на коды отдельного расчёта доплаты до минимальной зарплаты*/
     if(kodotrs != NULL && koddopmin != 0)
      {

       /*Проверяем есть ли код доплаты в списке*/

       sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and  knah=%d",tn,koddopmin); 
       if(sql_readkey(&bd,strsql,&row,&curr) < 1)
         continue;

       shet.new_plus(row[0]);

       for(i1=1; i1 <= kodotrs[0]; i1++)
        if(kn2 == kodotrs[i1])
         break;
       if(i1 > kodotrs[0])
         continue;

       class zar_read_tn1h nastr;
       zar_read_tn1w(1,mp,gp,&nastr,wpredok);

       doplata=nastr.minzar*koefpv-nah;

       sprintf(strsql,"%s-Расчёт доплаты до минимальной зарплаты.\n%.2f*%f-%.2f=%.2f (i1=%d kodotrs[0]=%d)\n",__FUNCTION__,nastr.minzar,koefpv,nah,doplata,i1,kodotrs[0]);
       zar_pr_insw(strsql,wpredok);

       if(doplata < 0.01)
          continue;
       doplata=iceb_u_okrug(doplata,okrg);
       br.new_plus(kn2);
       strcpy(zp.shet,shet.ravno());
       zp.knu=koddopmin;
       zapzarpw(&zp,nah,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
//       zapzarp(d,mp,gp,tn,1,koddopmin,doplata,shet.ravno(),mp,gp,0,nomz++,br.ravno(),podr,"",zp); 

      }
    }
  }
}

