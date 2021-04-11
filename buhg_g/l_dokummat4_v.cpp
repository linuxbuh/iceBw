/*$Id: l_dokummat4_v.c,v 1.27 2014/05/07 10:58:06 sasa Exp $*/
/*25.02.2017	25.02.2017	Белых А.И.	l_dokummat4_v.c
Ввод и корректировка категорий работников
*/
#include "buhg_g.h"
#include "l_dokummat4.h"
enum
 {
  E_KOD_D,
  E_KOLIH,
  E_KOMENT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_dokummat4_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_det;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  

  class iceb_u_str kod_mat;
  class iceb_u_str nom_kart;
  

  class iceb_u_str kod_det;
  class iceb_u_str kolih;
  class iceb_u_str koment;

  short dd,md,gd;
  int skl;
  class iceb_u_str nomdok;
  int nomer_zap;  
  
  
  l_dokummat4_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kod_det.plus("");
    kolih.plus("");
    koment.plus("");
   }

  void read_rek()
   {
    kod_det.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_D])));
    kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   l_dokummat4_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_dokummat4_v_data *data);
void  l_dokummat4_v_knopka(GtkWidget *widget,class l_dokummat4_v_data *data);
void    l_dokummat4_v_vvod(GtkWidget *widget,class l_dokummat4_v_data *data);
int l_dokummat4_zap(class l_dokummat4_v_data *data);
int l_dokummat4_sm(short dd,short md,short gd,int skl,const char *nomdok,class iceb_u_str *kod_mat,class iceb_u_str *nom_kart,GtkWidget *wpredok);
double dirdokummat4_ksm(int nomkart,class l_dokummat4_v_data *data);
void  l_dokummat4_v_e_knopka(GtkWidget *widget,class l_dokummat4_v_data *data);


extern SQL_baza  bd;

int l_dokummat4_v(short dd,short md,short gd,int skl,const char *nomdok,int nomer_zap,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
SQL_str row;
class SQLCURSOR cur;
class l_dokummat4_v_data data;
char strsql[512];
class iceb_u_str kikz;
double kol_spis_mat=0.;
double kol_spis_dok=0.;
class iceb_u_str naim_km("");

data.name_window.plus(__FUNCTION__);
data.dd=dd;
data.md=md;
data.gd=gd;
data.skl=skl;
data.nomdok.plus(nomdok);
data.nomer_zap=nomer_zap;


if(data.nomer_zap != 0)
 {
  sprintf(strsql,"select km,nk,kd,kol,kom,ktoz,vrem from Dokummat4 where nz=%d",nomer_zap);
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.kod_mat.new_plus(row[0]);
  data.nom_kart.new_plus(row[1]);  
  data.kod_det.new_plus(row[2]);
  

  sprintf(strsql,"%.6g",atof(row[3]));
  data.kolih.new_plus(strsql);
  data.koment.new_plus(row[4]);  

  kikz.plus(iceb_kikz(row[5],row[6],wpredok));

 }
else
 {
  if(l_dokummat4_sm(data.dd,data.md,data.gd,data.skl,data.nomdok.ravno(),&data.kod_mat,&data.nom_kart,wpredok) != 0)
     return(1);

  /*узнаём количество списанного материала в карточке*/
  kol_spis_mat=readkolkw(data.skl,data.nom_kart.ravno_atoi(),data.dd,data.md,data.gd,data.nomdok.ravno(),wpredok);  

  if(kol_spis_mat == 0.)
   {
    iceb_menu_soob(gettext("Материал не подтверждён!"),wpredok);
    return(1);
   }


  /*узнаём количество уже списанного*/
  kol_spis_dok=dirdokummat4_ksm(data.nom_kart.ravno_atoi(),&data);

  if(kol_spis_dok >= kol_spis_mat)
   {
    iceb_menu_soob(gettext("Материал уже списан!"),wpredok);
    return(1);
   }

  data.kolih.new_plus(kol_spis_mat-kol_spis_dok);  
 } 



/*узнаём наименование материала*/
sprintf(strsql,"select naimat from Material where kodm=%d",data.kod_mat.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_km.new_plus(row[0]);
else
 naim_km.new_plus("");



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;




if(data.nomer_zap == 0)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));

  class iceb_u_str repl;
  repl.plus(gettext("Ввод новой записи"));
  repl.ps_plus(gettext("Материал")),
  repl.plus(":",data.kod_mat.ravno());
  repl.plus(" ",naim_km.ravno());
  
  label=gtk_label_new(repl.ravno());
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  class iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(gettext("Материал")),
  repl.plus(":",data.kod_mat.ravno());
  repl.plus(" ",naim_km.ravno());
  repl.ps_plus(kikz.ravno());

  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_dokummat4_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
 gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);

sprintf(strsql,"%s:",gettext("Деталь"));
data.label_det=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (vbox),data.label_det, TRUE, TRUE, 0);


for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


//label=gtk_label_new(gettext("Код детали"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD_D]), label, FALSE, FALSE, 0);

data.knopka_enter[E_KOD_D]=gtk_button_new_with_label(gettext("Код детали"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_D]), data.knopka_enter[E_KOD_D], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_D],"clicked",G_CALLBACK(l_dokummat4_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_D],iceb_u_inttochar(E_KOD_D));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_D],gettext("Выбор детали"));

data.entry[E_KOD_D] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_D]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_D]), data.entry[E_KOD_D], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_D], "activate",G_CALLBACK(l_dokummat4_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_D]),data.kod_det.ravno());
gtk_widget_set_name(data.entry[E_KOD_D],iceb_u_inttochar(E_KOD_D));/******************/

label=gtk_label_new(gettext("Количество"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);

data.entry[E_KOLIH] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLIH]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(l_dokummat4_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));/******************/

label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),60);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_dokummat4_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));/******************/


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_dokummat4_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_dokummat4_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введённой информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_dokummat4_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_dokummat4_v_e_knopka(GtkWidget *widget,class l_dokummat4_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_KOD_D:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->kod_det.plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_D]),data->kod_det.ravno());
      char bros[1024];
      sprintf(bros,"%s:%d %s",gettext("Деталь"),data->kod_det.ravno_atoi(),naim.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_det),bros);
      
     }

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_dokummat4_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_dokummat4_v_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

    return(TRUE);
  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");

    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
  //  printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_dokummat4_v_knopka(GtkWidget *widget,class l_dokummat4_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_dokummat4_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_dokummat4_v_vvod(GtkWidget *widget,class l_dokummat4_v_data *data)
{
class iceb_u_str naim_km("");
SQL_str row1;
class SQLCURSOR cur1;
char strsql[1024];

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOD_D:
    data->kod_det.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    /*узнаём наименование материала*/
    sprintf(strsql,"select naimat from Material where kodm=%d",data->kod_det.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     naim_km.new_plus(row1[0]);
    else
     naim_km.new_plus("");

    sprintf(strsql,"%s:%d %s",gettext("Деталь"),data->kod_det.ravno_atoi(),naim_km.ravno());
    gtk_label_set_text(GTK_LABEL(data->label_det),strsql);

    break;

  case E_KOLIH:
    data->kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_dokummat4_zap(class l_dokummat4_v_data *data)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

if(iceb_pbpds(data->md,data->gd,data->window) != 0)
 return(1);

if(data->kod_det.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }

if(data->kod_det.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым и не равным нолю!"),data->window);
  return(1);
 }

sprintf(strsql,"select naimat from Material where kodm=%d",data->kod_det.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %d",gettext("Не найден код детали"),data->kod_det.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(data->kolih.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введено количество!"),data->window);
  return(1);
 }




if(data->nomer_zap == 0)
 {
  sprintf(strsql,"insert into Dokummat4 (datd,skl,nomd,km,nk,kd,kol,kom,ktoz,vrem) \
  values('%04d-%02d-%02d',%d,'%s',%d,%d,%d,%.6g,'%s',%d,%ld)",
  data->gd,data->md,data->dd,
  data->skl,
  data->nomdok.ravno(),
  data->kod_mat.ravno_atoi(),
  data->nom_kart.ravno_atoi(),
  data->kod_det.ravno_atoi(),
  data->kolih.ravno_atof(),
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL));
 }
else
 {
  
  sprintf(strsql,"update Dokummat4 set \
kd=%d,\
kol=%.6f,\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where nz=%d",  
  data->kod_det.ravno_atoi(),
  data->kolih.ravno_atof(),
  data->koment.ravno_filtr(),  
  iceb_getuid(data->window),
  time(NULL),
  data->nomer_zap);  

 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
/******************************/
/*Узнаём количество уже списанного материала*/
/************************************/
double dirdokummat4_ksm(int nomkart,class l_dokummat4_v_data *data)
{
class SQLCURSOR cur;
char strsql[1024];
double kolih=0.;
int kolstr=0;
SQL_str row;

sprintf(strsql,"select kol from Dokummat4 where datd='%04d-%02d-%02d' and \
skl=%d and nomd='%s' and nk=%d",
data->gd,data->md,data->dd,data->skl,data->nomdok.ravno(),nomkart);

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  kolih+=atof(row[0]);
 }


return(kolih);
}
