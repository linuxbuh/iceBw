/*$Id: imp_zar_kartf_r.c,v 1.5 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	18.06.2013	Белых А.И.	imp_zar_kartf_r.c
импорт карточек работников
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class imp_zar_kartf_r_data
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
  imp_zar_kartf_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_zar_kartf_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_zar_kartf_r_data *data);
gint imp_zar_kartf_r1(class imp_zar_kartf_r_data *data);
void  imp_zar_kartf_r_v_knopka(GtkWidget *widget,class imp_zar_kartf_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int imp_zar_kartf_r(const char *imaf,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
class imp_zar_kartf_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт карточек из файла"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(imp_zar_kartf_r_key_press),&data);

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

label=gtk_label_new(gettext("Импорт карточек из файла"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(imp_zar_kartf_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)imp_zar_kartf_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_zar_kartf_r_v_knopka(GtkWidget *widget,class imp_zar_kartf_r_data *data)
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

gboolean   imp_zar_kartf_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_zar_kartf_r_data *data)
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

gint imp_zar_kartf_r1(class imp_zar_kartf_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[2048];
class iceb_u_str strok("");
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


class iceb_fopen filz;
if(filz.start(data->imafz.ravno(),"r",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafprom[64];
sprintf(imafprom,"prom%d.tmp",getpid());

class iceb_fopen filprom;
if(filprom.start(imafprom,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

#define KOL 26
class iceb_u_str rek[KOL];
OPSHET reksh;
int  kolkart=0;
int razmer=0;
int nom=0;
SQL_str row;
class SQLCURSOR cur;

while(iceb_u_fgets(&strok,filz.ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;
  razmer+=strlen(strok.ravno());

  iceb_printw(strok.ravno(),data->buffer,data->view);
  iceb_pbar(data->bar,work.st_size,razmer);    


  if(iceb_u_polen(strok.ravno(),&rek[0],1,'|') != 0)
   {
    fprintf(filprom.ff,"%s",strok.ravno());
    fprintf(filprom.ff,"#%s %d\n",gettext("Не найдено поле"),1);
    continue;
   }

  for(nom=1; nom < KOL; nom++)
   if(iceb_u_polen(strok.ravno(),&rek[nom],nom+1,'|') != 0)
    {
     fprintf(filprom.ff,"%s",strok.ravno());
     fprintf(filprom.ff,"#%s %d !\n",gettext("Не найдено поле"),nom+1);
     break;
    }

  if(nom < KOL)
   continue;


  /*Проверяем на отсутствие индентификационного кода*/
  if(rek[10].getdlinna() > 1)
   {
    sprintf(strsql,"select tabn,fio from Kartb where inn='%s'",rek[10].ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
     {
      class iceb_u_str menu(rek[10].ravno());
      menu.ps_plus(gettext("Такой индентификационный код уже введён !"));
      sprintf(strsql,"%s %s",row[0],row[1]);
      menu.ps_plus(strsql);
      iceb_menu_soob(menu.ravno(),data->window);

      fprintf(filprom.ff,"%s\n#%s:%s\n",strok.ravno(),gettext("Такой индентификационный код уже введён !"),
      strsql);
      continue;
     } 
   }

  /*если подразделения нет в списке подразделений то обнуляем*/
  sprintf(strsql,"select kod from Podr where kod=%d",rek[4].ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) == 0)
   rek[4].new_plus("0");

  /*если категории нет в списке категорий то обнуляем*/
  sprintf(strsql,"select kod from Kateg where kod=%d",rek[5].ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) == 0)
   rek[5].new_plus("0");
   
  /*проверка счёта*/
   if(iceb_prsh1(rek[8].ravno(),&reksh,data->window) != 0)
    rek[8].new_plus("");
      
  if(rek[0].ravno_atoi() == 0)
   rek[0].new_plus(iceb_get_new_kod("Kartb","tabn",0,data->window));

  /*проверяем есть ли такой табельный номер*/
  sprintf(strsql,"select tabn from Kartb where tabn=%d",rek[0].ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) > 0)
   rek[0].new_plus(iceb_get_new_kod("Kartb","tabn",0,data->window));
     
/*
0  tabn  табельный номер
1  fio   фамилия имя отчество
2  dolg  должность
3  zvan  звание
4  podr  подразделение
5  kateg категория
6  datn  дата начала работы
7  datk  дата конца работы
8  shet  счёт учета
9  sovm  метка совместителя 0-не совм. 1-совместитель
10 inn   индивидуальный налоговый номер
11 adres адрес
12 nomp  номер паспорта
13 vidan кем выдан
14 telef телефон
15 kodg  код города налоговой инспекции
16 harac характер работы
17 lgoti код льгот для формы 8ДР
18 bankshet банковский счёт на который перечисляется зарплата
19 datvd дата выдачи документа
20 tipkk тип кредитной карты
21 denrog день рождения
22 pl     0-мужчина 1-женщина
23 dppz   дата последнего повышения зарплаты
24 kb    код банка для перечисления зарплаты на карт счёт
25 kss код основания для учёта специального стажа
*/
      
  sprintf(strsql,"insert into Kartb (tabn,fio,dolg,zvan,podr,kateg,datn,datk,shet,sovm,inn,adres,nomp,vidan,telef,kodg,harac,lgoti,bankshet,datvd,tipkk,denrog,pl,dppz,kb,kss) \
values (%s,'%s','%s','%s',%s,%s,'%s','%s','%s',%s,'%s','%s','%s','%s','%s',%s,'%s','%s','%s','%s','%s','%s',%s,'%s',%s,'%s')",
  rek[0].ravno(),
  rek[1].ravno_filtr(),
  rek[2].ravno_filtr(),
  rek[3].ravno_filtr(),
  rek[4].ravno(),
  rek[5].ravno(),
  rek[6].ravno(),
  rek[7].ravno(),
  rek[8].ravno_filtr(),
  rek[9].ravno(),
  rek[10].ravno_filtr(),
  rek[11].ravno_filtr(),
  rek[12].ravno_filtr(),
  rek[13].ravno_filtr(),
  rek[14].ravno_filtr(),
  rek[15].ravno(),
  rek[16].ravno_filtr(),
  rek[17].ravno_filtr(),
  rek[18].ravno(),
  rek[19].ravno(),
  rek[20].ravno_filtr(),
  rek[21].ravno(),
  rek[22].ravno(),
  rek[23].ravno(),
  rek[24].ravno(),
  rek[25].ravno_filtr());
  
  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
   {
    fprintf(filprom.ff,"%s",strok.ravno());
    continue;
   }    
  kolkart++;  
 }
iceb_pbar(data->bar,work.st_size,work.st_size);    

filz.end();
filprom.end();
unlink(data->imafz.ravno());

rename(imafprom,data->imafz.ravno());


sprintf(strsql,"%s\n%s:%d",gettext("Загрузка завершена"),
gettext("Количество загруженых карточек"),kolkart);
iceb_menu_soob(strsql,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Загрузка завершена"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);
}
