/*$Id: imp_prov_r.c,v 1.3 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	21.08.2007	Белых А.И.	imp_prov_r.c
импорт проводок
*/
#include <math.h>
#include  <errno.h>
#include <sys/stat.h>
#include "buhg_g.h"

class imp_prov_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  class iceb_u_str imafz;
        
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  imp_prov_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_prov_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_prov_r_data *data);
gint imp_prov_r1(class imp_prov_r_data *data);
void  imp_prov_r_v_knopka(GtkWidget *widget,class imp_prov_r_data *data);


extern SQL_baza bd;
extern double okrcn;

int imp_prov_r(const char *imaf,
GtkWidget *wpredok)
{
char strsql[2048];
class iceb_u_spisok repl_s;
class imp_prov_r_data data;
int gor=0;
int ver=0;

data.imafz.new_plus(imaf);
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт проводок"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(imp_prov_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

GtkWidget *label=NULL;
label=gtk_label_new(gettext("Импорт проводок"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(imp_prov_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)imp_prov_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_prov_r_v_knopka(GtkWidget *widget,class imp_prov_r_data *data)
{
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   imp_prov_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_prov_r_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}

/******************************************/
/******************************************/

gint imp_prov_r1(class imp_prov_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
struct stat work;

if(stat(data->imafz.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Не найден файл"),data->imafz.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

FILE *ff=NULL;
FILE *ffprom=NULL;

if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafprom[64];
sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  sprintf(strsql,"%s %s:%d !",gettext("Ошибка окрытия файла"),imafprom,errno);
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int  val=0;
short d,m,g;
class iceb_u_str shet("");
class iceb_u_str shetk("");
class iceb_u_str kontrsh("");
class iceb_u_str kontrshk("");
double  deb=0.,kre=0.;
class iceb_u_str koment("");
int  kolprov=0;
float razmer=0.;
//#define KOLPOL 17
int nom_kod_kontr=1;
//class iceb_u_str rekkon[KOLPOL];
int koloh=0;
class iceb_u_str rek1("");
double ksd=0.,ksk=0.;
double ideb=0.,ikre=0.;
class iceb_u_str naim_kontr("");
int metka_sumo=0;
class iceb_u_str strok("");
class iceb_u_str bros("");
class OPSHET shetv;
while(iceb_u_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());
  iceb_pbar(data->bar,work.st_size,razmer);    

  iceb_printw(strok.ravno(),data->buffer,data->view);

  if(strok.ravno()[0] == '#')
   continue;
   
  kontrsh.new_plus("");
  kontrshk.new_plus("");

  if(iceb_u_polen(strok.ravno(),&rek1,1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  if(iceb_u_SRAV(rek1.ravno(),"sumo",0) == 0)
   {
    iceb_u_polen(strok.ravno(),&ksd,2,'|');
    iceb_u_polen(strok.ravno(),&ksk,3,'|');
    metka_sumo++;
   }
      

  if(iceb_u_SRAV(rek1.ravno(),"KON",0) == 0)
   {
    if(iceb_zkvsk(strok.ravno(),&nom_kod_kontr,ffprom,data->window) < 0)
     continue;
#if 0
#########################################################333    
    for(int nom=0; nom < KOLPOL; nom++)
     iceb_u_polen(strok.ravno(),&rekkon[nom],nom+2,'|');

    if(rekkon[1].getdlinna() <= 1)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s|\n",gettext("Не введено наименование контрагента"));
      koloh++;
      continue;
     }
    else
     {
      sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",rekkon[1].ravno_filtr());
      if(iceb_sql_readkey(strsql,data->window) == 0) /*не найден код контрагента*/
       {
        /*записывается код контрагента с наименованием*/
        if(iceb_get_kkfname(rekkon[1].ravno(),rekkon[0].ravno(),&rekkon[0],&nom_kod_kontr,data->window) < 0)
         {
          fprintf(ffprom,"%s",strok.ravno());
          fprintf(ffprom,"#%s!\n",gettext("Не смогли добавить контрагента в справочник"));
          koloh++;
          continue;
         }

        class iceb_lock_tables klk("LOCK TABLES Kontragent WRITE,icebuser READ");
        
        if(rekkon[11].getdlinna() > 1)
         {
          sprintf(strsql,"select kod from Gkont where kod=%d",rekkon[11].ravno_atoi());
          if(iceb_sql_readkey(strsql,data->window) <= 0)
            rekkon[11].new_plus("");
         }

        sprintf(strsql,"update Kontragent set \
naikon='%s',\
naiban='%s',\
adres='%s',\
adresb='%s',\
kod='%s',\
mfo='%s',\
nomsh='%s',\
innn='%s',\
nspnds='%s',\
telef='%s',\
grup='%s',\
ktoz=%d,\
vrem=%ld,\
na='%s',\
regnom='%s',\
pnaim='%s',\
gk='%s',\
en=%d \
where kodkon='%s'",
        rekkon[1].ravno_filtr(),
        rekkon[2].ravno_filtr(),
        rekkon[3].ravno_filtr(),
        rekkon[4].ravno_filtr(),
        rekkon[5].ravno_filtr(),
        rekkon[6].ravno_filtr(),
        rekkon[7].ravno_filtr(),
        rekkon[8].ravno_filtr(),
        rekkon[9].ravno_filtr(),
        rekkon[10].ravno_filtr(),
        rekkon[11].ravno_filtr(),
        iceb_getuid(data->window),
        time(NULL),
        rekkon[12].ravno_filtr(),
        rekkon[13].ravno_filtr(),
        rekkon[14].ravno_filtr(),
        rekkon[15].ravno_filtr(),
        rekkon[16].ravno_atoi(),
        rekkon[0].ravno_filtr());


        if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
         {
          fprintf(ffprom,"%s",strok.ravno());
          fprintf(ffprom,"#%s Kontragent !\n",gettext("Ошибка записи в таблицу"));
          koloh++;
          continue;
         }
       }
     }

    continue;
########################################
#endif
   }

  if(iceb_u_polen(strok.ravno(),&bros,1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(bros.ravno()[0] == '\0')
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  val=bros.ravno_atoi();
  if(val != 0 && val != -1)
   {
    iceb_menu_soob(gettext("Не правильная метка проводки !"),data->window);
    
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не правильная метка проводки !"));
    koloh++;
    continue;

   }
  //Берем дату проводки
  if(iceb_u_polen(strok.ravno(),&bros,2,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(iceb_u_rsdat(&d,&m,&g,bros.ravno(),1) != 0)
   {
    
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не верно введена дата проводки !"));
    koloh++;
    continue;
   }

  if(iceb_pvglkni(m,g,data->window) != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Дата заблокирована"));
    continue;
   }

  //Берем счет
  if(iceb_u_polen(strok.ravno(),&shet,3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  if(shet.getdlinna() <= 1)
   {
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не введён счёт"));
   }
  if(iceb_prsh1(shet.ravno(),&shetv,data->window) != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не подходит счёт"));
    continue;
   }

  if(shetv.saldo == 3)
   { 
    
    if(iceb_u_polen(strok.ravno(),&kontrsh,4,'|') != 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s!\n",gettext("Не найдено поле с кодом контрагента для счёта"));
      koloh++;
      continue;
     }

    if(iceb_u_polen(strok.ravno(),&naim_kontr,10,'|') != 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s %s!\n",gettext("Не найдено поле с наименованием контрагента для счёта"),shet.ravno());
      koloh++;
      continue;
     }

    /*определяем код контрагента в общем списке контрагентов*/
    if(iceb_get_kkfname(naim_kontr.ravno(),kontrsh.ravno(),&kontrsh,&nom_kod_kontr,data->window) < 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s %s!\n",gettext("Не смогли определить код контрагента для наименования"),naim_kontr.ravno());
      koloh++;
      continue;
     }

    if(kontrsh.getdlinna() <= 1)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s %s!\n",gettext("Не определён код контрагента для счёта"),shet.ravno());
      koloh++;
      continue;
     }

    //Проверяем введён ли этот контрагент в список счета
    sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",shet.ravno(),kontrsh.ravno());
    if(iceb_sql_readkey(strsql,data->window) == 0)
     {
      sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values('%s','%s',%d,%ld)",shet.ravno_filtr(),kontrsh.ravno_filtr(),iceb_getuid(data->window),time(NULL));
      if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s %s %s Skontr!\n",gettext("Ошибка записи кода контрагента"),kontrsh.ravno(),gettext("в таблицу"));
        koloh++;
        continue;

       }
     }
   }

  if(val == 0)
   {     
    //Берем счёт корреспондент     
    if(iceb_u_polen(strok.ravno(),&shetk,5,'|') != 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      continue;
     }

    if(shetk.getdlinna() <= 1)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s\n",gettext("Не введён счёт корреспондент"));
      continue;
      
     }

    if(iceb_prsh1(shetk.ravno(),&shetv,data->window) != 0)
     {
      fprintf(ffprom,"%s",strok.ravno());
      fprintf(ffprom,"#%s\n",gettext("Не подходит счёт корреспондент"));
      continue;
     }

    if(shetv.saldo == 3)
     { 
      if(iceb_u_polen(strok.ravno(),&kontrshk,6,'|') != 0)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s %s!\n",gettext("Не введено поле с кодом контрагента для счёта"),shetk.ravno());
        koloh++;
        continue;
       }
      if(iceb_u_polen(strok.ravno(),&naim_kontr,11,'|') != 0)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s %s!\n",gettext("Не введено поле с наименованием контрагента для счёта"),shetk.ravno());
        koloh++;
        continue;
       }

      /*определяем код контрагента в общем списке контрагентов*/
      if(iceb_get_kkfname(naim_kontr.ravno(),kontrshk.ravno(),&kontrshk,&nom_kod_kontr,data->window) < 0)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s %s!\n",gettext("Не смогли определить код контрагента для наименования"),naim_kontr.ravno());
        koloh++;
        continue;
       }
      if(kontrshk.getdlinna() <= 1)
       {
        fprintf(ffprom,"%s",strok.ravno());
        fprintf(ffprom,"#%s %s!\n",gettext("Не введён код контрагента для счёта"),shetk.ravno());
        koloh++;
        continue;
       }

      //Проверяем введён ли этот контрагент в список счета
      sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",shetk.ravno(),kontrshk.ravno());
      if(iceb_sql_readkey(strsql,data->window) == 0)
       {
        sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values('%s','%s',%d,%ld)",shetk.ravno_filtr(),kontrshk.ravno_filtr(),iceb_getuid(data->window),time(NULL));
        if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
         {
          fprintf(ffprom,"%s",strok.ravno());
          fprintf(ffprom,"#%s %s %s Skontr!\n",gettext("Ошибка записи кода контрагента"),kontrshk.ravno(),gettext("в таблицу"));
          koloh++;
          continue;

         }
       }
     }
  }

  //Берем дебет     
  if(iceb_u_polen(strok.ravno(),&deb,7,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Берем кредит     
  if(iceb_u_polen(strok.ravno(),&kre,8,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(deb+kre == 0.)
   {
    iceb_menu_soob(gettext("Не введён ни дебет ни кредит !"),data->window);
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Не введён ни дебет ни кредит !"));
    koloh++;
    continue;
   }

  if( deb != 0. && kre != 0.)
   {
    iceb_menu_soob(gettext("Введен и дебет и кредит !"),data->window);
    fprintf(ffprom,"%s",strok.ravno());
    fprintf(ffprom,"#%s\n",gettext("Введен и дебет и кредит !"));
    koloh++;
    continue;
   }

  //Берем комментарий
  if(iceb_u_polen(strok.ravno(),&koment,9,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  
  if(iceb_zapprov(val,g,m,d,shet.ravno(),shetk.ravno(),kontrsh.ravno(),kontrshk.ravno(),"","","",deb,kre,koment.ravno(),2,0,time(NULL),0,0,0,0,0,data->window) == 0)
   {
    kolprov++;
    ideb+=deb;
    ikre+=kre;  
   }
 }
iceb_pbar(data->bar,work.st_size,work.st_size); /*изза того что убираются обратные бакслеши и символы возврата карретки*/

sprintf(strsql,"\n%s:%.2f\n",gettext("Cума по дебету"),ideb);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s:%.2f\n",gettext("Cума по кредиту"),ikre);
iceb_printw(strsql,data->buffer,data->view);

fprintf(ffprom,"\n%s:%.2f\n",gettext("Cума по дебету"),ideb);
fprintf(ffprom,"%s:%.2f\n",gettext("Cума по кредиту"),ikre);

if(metka_sumo != 0)
 {
  if(fabs(ideb-ksd) > 0.009)
   {
    fprintf(ffprom,"\n#%s: %.2f != %.2f\n",gettext("Итоговые дебеты не равны"),ideb,ksd);
    sprintf(strsql,"\n%s: %.2f != %.2f\n\n",gettext("Итоговые дебеты не равны"),ideb,ksd);
    iceb_printw(strsql,data->buffer,data->view);
   }

  if(fabs(ikre-ksk) > 0.009)
   {
    fprintf(ffprom,"\n#%s: %.2f != %.2f\n\n",gettext("Итоговые кредиты не равны"),ikre,ksk);
    sprintf(strsql,"\n%s: %.2f != %.2f\n\n",gettext("Итоговые кредиты не равны"),ikre,ksk);
    iceb_printw(strsql,data->buffer,data->view);
   }
 }


fclose(ff);
fclose(ffprom);
unlink(data->imafz.ravno());

rename(imafprom,data->imafz.ravno());


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

sprintf(strsql,"%s\n%s:%d\n%s:%.2f\n%s:%.2f",
gettext("Загрузка завершена"),
gettext("Количество загруженых проводок"),kolprov,
gettext("Сумма по дебету"),ideb,
gettext("Сумма по кредиту"),ikre);

iceb_menu_soob(strsql,data->window);
data->kon_ras=0;



data->voz=0;

return(FALSE);
}
