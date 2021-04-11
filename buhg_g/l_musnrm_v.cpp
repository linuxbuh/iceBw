/*$Id:$*/
/*10.03.2017	10.03.2017	Белых А.И.	l_musnrm_v.cpp
Ввод и корректировка 
*/
#include "buhg_g.h"
#include "l_musnrm.h"
enum
 {
  E_KOD_DET,
  E_EID,
  E_KOD_MAT,
  E_EIM,
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

class l_musnrm_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *opt;
  GtkWidget *opt1;
  GtkWidget *window;
  GtkWidget *label_naim_det;
  GtkWidget *label_naim_mat;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  
  int nom_zap;
  class iceb_u_str kod_det;
  class iceb_u_str eid;
  class iceb_u_str kod_mat;
  class iceb_u_str eim;
  class iceb_u_str kolih;
  class iceb_u_str koment;
  int vs;
  int vz;

  l_musnrm_v_data() //Конструктор
   {
    vs=vz=0;
    kl_shift=0;
    voz=1;
    kod_det.plus("");
    eid.plus("");
    kod_mat.plus("");
    eim.plus("");
    kolih.plus("");
    koment.plus("");
   }

  void read_rek()
   {
    kod_det.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_DET])));
    eid.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_EID])));
    kod_mat.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_MAT])));
    eim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_EIM])));

    kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));

    vs=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));

    vz=gtk_combo_box_get_active (GTK_COMBO_BOX(opt1));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   l_musnrm_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_musnrm_v_data *data);
void  l_musnrm_v_knopka(GtkWidget *widget,class l_musnrm_v_data *data);
void    l_musnrm_v_vvod(GtkWidget *widget,class l_musnrm_v_data *data);
int l_musnrm_zap(class l_musnrm_v_data *data);
void  l_musnrm_v_e_knopka(GtkWidget *widget,class l_musnrm_v_data *data);


extern SQL_baza  bd;

int l_musnrm_v(int nom_zap,class iceb_u_str *kod_det,GtkWidget *wpredok)
{
int gor=0;
int ver=0;

class l_musnrm_v_data data;
char strsql[2048];
class iceb_u_str kikz;
data.name_window.plus(__FUNCTION__);
data.nom_zap=nom_zap;
class iceb_u_str naim_det("");
class iceb_u_str naim_mat("");

if(data.nom_zap > 0)
 {
  sprintf(strsql,"select * from Musnrm where nz=%d",data.nom_zap);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
 
  data.kod_det.new_plus(row[1]);
  data.eid.new_plus(row[2]);
  data.kod_mat.new_plus(row[3]);

  sprintf(strsql,"%.10g",atof(row[4]));
  data.kolih.new_plus(strsql);

  data.eim.new_plus(row[5]);
  data.vs=atoi(row[6]);
  data.vz=atoi(row[7]);
  data.koment.new_plus(row[8]);
    
  kikz.plus(iceb_kikz(row[9],row[10],wpredok));
  
  sprintf(strsql,"select naimat from Material where kodm=%d",data.kod_det.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_det.new_plus(row[0]);
  else
   naim_det.new_plus("");

  sprintf(strsql,"select naimat from Material where kodm=%d",data.kod_mat.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_mat.new_plus(row[0]);
  else
   naim_mat.new_plus("");

 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.nom_zap != 0)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_musnrm_v_key_press),&data);

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


sprintf(strsql,"%s:%s",gettext("Деталь"),naim_det.ravno());
data.label_naim_det=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (vbox),data.label_naim_det, TRUE, TRUE, 0);
printf("%s\n",__FUNCTION__);

sprintf(strsql,"%s:%s",gettext("Материал"),naim_mat.ravno());
data.label_naim_mat=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (vbox),data.label_naim_mat, TRUE, TRUE, 0);

for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);


GtkWidget *hbox_vs = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_pack_start (GTK_BOX (vbox),hbox_vs, TRUE, TRUE, 0);

label=gtk_label_new(gettext("Вид списания"));
gtk_box_pack_start (GTK_BOX (hbox_vs), label, FALSE, FALSE, 0);

class iceb_u_spisok spmenu;
spmenu.plus(gettext("Основной"));
spmenu.plus(gettext("Альтернативный"));


iceb_pm_vibor(&spmenu,&data.opt,data.vs);
gtk_box_pack_start (GTK_BOX (hbox_vs),data.opt, TRUE, TRUE,1);


GtkWidget *hbox_vz = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_pack_start (GTK_BOX (vbox),hbox_vz, TRUE, TRUE, 0);

label=gtk_label_new(gettext("Вид заготовки"));
gtk_box_pack_start (GTK_BOX (hbox_vz), label, FALSE, FALSE, 0);

spmenu.new_plus(gettext("Материал"));
spmenu.plus(gettext("Покупная заготовка"));


iceb_pm_vibor(&spmenu,&data.opt1,data.vz);
gtk_box_pack_start (GTK_BOX (hbox_vz),data.opt1, TRUE, TRUE,1);


gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


data.knopka_enter[E_KOD_DET]=gtk_button_new_with_label(gettext("Код детали"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_DET]), data.knopka_enter[E_KOD_DET], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_DET],"clicked",G_CALLBACK(l_musnrm_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_DET],iceb_u_inttochar(E_KOD_DET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_DET],gettext("Выбор детали"));

data.entry[E_KOD_DET] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_DET]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_DET]), data.entry[E_KOD_DET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_DET], "activate",G_CALLBACK(l_musnrm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_DET]),data.kod_det.ravno());
gtk_widget_set_name(data.entry[E_KOD_DET],iceb_u_inttochar(E_KOD_DET));/******************/




data.knopka_enter[E_EID]=gtk_button_new_with_label(gettext("Единица измерения детали"));
gtk_box_pack_start (GTK_BOX (hbox[E_EID]), data.knopka_enter[E_EID], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_EID],"clicked",G_CALLBACK(l_musnrm_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_EID],iceb_u_inttochar(E_EID));
gtk_widget_set_tooltip_text(data.knopka_enter[E_EID],gettext("Выбор единицы измерения"));

data.entry[E_EID] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EID]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_EID]), data.entry[E_EID], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_EID], "activate",G_CALLBACK(l_musnrm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EID]),data.eid.ravno());
gtk_widget_set_name(data.entry[E_EID],iceb_u_inttochar(E_EID));/******************/


data.knopka_enter[E_KOD_MAT]=gtk_button_new_with_label(gettext("Код материала"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_MAT]), data.knopka_enter[E_KOD_MAT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_MAT],"clicked",G_CALLBACK(l_musnrm_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_MAT],iceb_u_inttochar(E_KOD_MAT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_MAT],gettext("Выбор материала"));

data.entry[E_KOD_MAT] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_MAT]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_MAT]), data.entry[E_KOD_MAT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_MAT], "activate",G_CALLBACK(l_musnrm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_MAT]),data.kod_mat.ravno());
gtk_widget_set_name(data.entry[E_KOD_MAT],iceb_u_inttochar(E_KOD_MAT));/******************/


data.knopka_enter[E_EIM]=gtk_button_new_with_label(gettext("Единица измерения материала"));
gtk_box_pack_start (GTK_BOX (hbox[E_EIM]), data.knopka_enter[E_EIM], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_EIM],"clicked",G_CALLBACK(l_musnrm_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_EIM],iceb_u_inttochar(E_EIM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_EIM],gettext("Выбор единицы измерения"));

data.entry[E_EIM] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EIM]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_EIM]), data.entry[E_EIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_EIM], "activate",G_CALLBACK(l_musnrm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EIM]),data.eim.ravno());
gtk_widget_set_name(data.entry[E_EIM],iceb_u_inttochar(E_EIM));/******************/



label=gtk_label_new(gettext("Количество материала"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);

data.entry[E_KOLIH] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLIH]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(l_musnrm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));/******************/


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),60);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_musnrm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));/******************/


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_musnrm_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_musnrm_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_musnrm_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);


gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 kod_det->new_plus(data.kod_det.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_musnrm_v_e_knopka(GtkWidget *widget,class l_musnrm_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");
char strsql[1024];
int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_KOD_DET:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->kod_det.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_DET]),data->kod_det.ravno());

      sprintf(strsql,"%s:%s",gettext("Деталь"),naim.ravno());            
      gtk_label_set_text(GTK_LABEL(data->label_naim_det),strsql);
     }

    return;  

  case E_KOD_MAT:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->kod_mat.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_MAT]),data->kod_mat.ravno());
      sprintf(strsql,"%s:%s",gettext("Материал"),naim.ravno());            
      gtk_label_set_text(GTK_LABEL(data->label_naim_mat),strsql);
     }

    return;  

  case E_EID:

    if(iceb_l_ei(1,&kod,&naim,data->window) == 0)
     {
      data->eid.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_EID]),data->eid.ravno());
     }

    return;  

  case E_EIM:

    if(iceb_l_ei(1,&kod,&naim,data->window) == 0)
     {
      data->eim.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_EIM]),data->eim.ravno());
     }

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_musnrm_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_musnrm_v_data *data)
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
void  l_musnrm_v_knopka(GtkWidget *widget,class l_musnrm_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_musnrm_zap(data) == 0)
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

void    l_musnrm_v_vvod(GtkWidget *widget,class l_musnrm_v_data *data)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim("");

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOD_DET:
    data->kod_det.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(data->kod_det.getdlinna() <= 1)
      break;
           
    sprintf(strsql,"select naimat from Material where kodm=%d",data->kod_det.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);
    else
     naim.new_plus("");

    sprintf(strsql,"%s:%s",gettext("Деталь"),naim.ravno());            
    gtk_label_set_text(GTK_LABEL(data->label_naim_det),strsql);

    break;

  case E_KOD_MAT:
    data->kod_mat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(data->kod_mat.getdlinna() <= 1)
      break;
           
    sprintf(strsql,"select naimat from Material where kodm=%d",data->kod_mat.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);
    else
     naim.new_plus("");

    sprintf(strsql,"%s:%s",gettext("Материал"),naim.ravno());            
    gtk_label_set_text(GTK_LABEL(data->label_naim_mat),strsql);

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
int l_musnrm_zap(class l_musnrm_v_data *data)
{
char strsql[1024];



if(data->nom_zap == 0)
 {

  sprintf(strsql,"insert into Musnrm (kd,eid,km,kol,eim,vs,vz,kom,ktoz,vrem) values (%d,'%s',%d,%.10g,'%s',%d,%d,'%s',%d,%ld)",
  data->kod_det.ravno_atoi(),
  data->eid.ravno_filtr(),
  data->kod_mat.ravno_atoi(),
  data->kolih.ravno_atof(),
  data->eim.ravno_filtr(),
  data->vs,
  data->vz,
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),time(NULL));
 }
else
 {
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Musnrm \
set \
kd=%d,\
eid='%s',\
km=%d,\
kol=%.10g,\
eim='%s',\
vs=%d,\
vz=%d,\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where nz=%d",
  data->kod_det.ravno_atoi(),
  data->eid.ravno_filtr(),
  data->kod_mat.ravno_atoi(),
  data->kolih.ravno_atof(),
  data->eim.ravno_filtr(),
  data->vs,
  data->vz,
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),time(NULL),
  data->nom_zap);
   
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

return(0);

}
