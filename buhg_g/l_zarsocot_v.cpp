/*$Id: l_zarsocot_v.c,v 1.16 2013/08/25 08:26:47 sasa Exp $*/
/*12.07.2015	13.09.2006	Белых А.И.	l_zarsocot_v.c
Ввод и корректировка соц-отчислений
*/
#include "buhg_g.h"

enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOD,
  E_SHET,
  E_SUMA_O,
  E_SOCOT_O,
  E_SUMA_B,
  E_SOCOT_B,
  E_PROC,
  KOLENTER  
 };

class l_zarsocot_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  
  int tabnom;
  short mp,gp;
  int kodzap; //Код записи которую корректируют
  
  //реквизиты меню
  class iceb_u_str kod;
  class iceb_u_str shet;
  class iceb_u_str suma_o;
  class iceb_u_str socot_o;
  class iceb_u_str suma_b;
  class iceb_u_str socot_b;
  class iceb_u_str proc;
  
  l_zarsocot_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    kod.new_plus("");
    shet.new_plus("");
    suma_o.new_plus("");
    socot_o.new_plus("");
    suma_b.new_plus("");
    socot_b.new_plus("");
    proc.new_plus("");
    
   }
 };

gboolean   l_zarsocot_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsocot_v_data *data);
void    l_zarsocot_v_v_vvod(GtkWidget *widget,class l_zarsocot_v_data *data);
void  l_zarsocot_v_v_knopka(GtkWidget *widget,class l_zarsocot_v_data *data);
int zarsocot_zap(class l_zarsocot_v_data *data);

void  zarsocot_v_e_knopka(GtkWidget *widget,class l_zarsocot_v_data *data);
int l_zarsocot_v_zap(class l_zarsocot_v_data *data);

extern SQL_baza bd;


int l_zarsocot_v(int tabnom,short mp,short gp,class iceb_u_str *kodzap,
const char *fio,
GtkWidget *wpredok)
{
class iceb_u_str kikz;
char strsql[512];
class l_zarsocot_v_data data;
data.tabnom=tabnom;
data.mp=mp;
data.gp=gp;
data.kodzap=kodzap->ravno_atoi();

if(kodzap->ravno_atoi() != 0)
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select * from Zarsocz where datz='%04d-%02d-01' and tabn=%d and kodz=%d",
  data.gp,data.mp,data.tabnom,data.kodzap);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  
  data.kod.new_plus(row[2]);
  data.shet.new_plus(row[3]);
  data.suma_o.new_plus(row[4]);
  data.socot_o.new_plus(row[5]);
  data.suma_b.new_plus(row[10]);
  data.socot_b.new_plus(row[9]);
  data.proc.new_plus(row[6]);
  kikz.plus(iceb_kikz(row[7],row[8],wpredok));
  
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(kodzap->ravno_atoi() != 0)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
else
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarsocot_v_v_key_press),&data);

iceb_u_str zagolov;
if(kodzap->ravno_atoi() == 0)
 zagolov.new_plus(gettext("Ввод новой записи"));
else
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
 }
zagolov.ps_plus(data.tabnom);
zagolov.plus(" ");
zagolov.plus(fio);

GtkWidget *label=gtk_label_new(zagolov.ravno());

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


//sprintf(strsql,"%s",gettext("Код"));
//label=gtk_label_new(strsql);
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);

sprintf(strsql,"%s",gettext("Код"));
data.knopka_enter[E_KOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD],"clicked",G_CALLBACK(zarsocot_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD],iceb_u_inttochar(E_KOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD],gettext("Выбор кода в списке соц-отчислений"));

data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(zarsocot_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

sprintf(strsql,"%s",gettext("С какой суммы начислено"));
label=gtk_label_new(strsql);
data.entry[E_SUMA_O] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA_O]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_O]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_O]), data.entry[E_SUMA_O],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA_O], "activate",G_CALLBACK(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_O]),data.suma_o.ravno());
gtk_widget_set_name(data.entry[E_SUMA_O],iceb_u_inttochar(E_SUMA_O));

sprintf(strsql,"%s",gettext("Общая сумма соц-отчисления"));
label=gtk_label_new(strsql);
data.entry[E_SOCOT_O] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SOCOT_O]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SOCOT_O]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SOCOT_O]), data.entry[E_SOCOT_O],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SOCOT_O], "activate",G_CALLBACK(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SOCOT_O]),data.socot_o.ravno());
gtk_widget_set_name(data.entry[E_SOCOT_O],iceb_u_inttochar(E_SOCOT_O));

sprintf(strsql,"%s",gettext("С какой суммы начислено бюджет"));
label=gtk_label_new(strsql);
data.entry[E_SUMA_B] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA_B]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_B]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_B]), data.entry[E_SUMA_B],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA_B], "activate",G_CALLBACK(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_B]),data.suma_b.ravno());
gtk_widget_set_name(data.entry[E_SUMA_B],iceb_u_inttochar(E_SUMA_B));

sprintf(strsql,"%s",gettext("Сумма бюджетного соц-отчисления"));
label=gtk_label_new(strsql);
data.entry[E_SOCOT_B] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SOCOT_B]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SOCOT_B]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SOCOT_B]), data.entry[E_SOCOT_B],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SOCOT_B], "activate",G_CALLBACK(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SOCOT_B]),data.socot_b.ravno());
gtk_widget_set_name(data.entry[E_SOCOT_B],iceb_u_inttochar(E_SOCOT_B));

sprintf(strsql,"%s",gettext("Процент"));
label=gtk_label_new(strsql);
data.entry[E_PROC] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PROC]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), data.entry[E_PROC],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PROC], "activate",G_CALLBACK(l_zarsocot_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROC]),data.proc.ravno());
gtk_widget_set_name(data.entry[E_PROC],iceb_u_inttochar(E_PROC));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_zarsocot_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_zarsocot_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 kodzap->new_plus(data.kod.ravno());

return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  zarsocot_v_e_knopka(GtkWidget *widget,class l_zarsocot_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_KOD:
    if(l_zarsocf(1,&kod,&naim,data->window) == 0)
     data->kod.new_plus(kod.ravno());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->kod.ravno());

    return;
  case E_SHET:
    iceb_vibrek(1,"Plansh",&data->shet,&naim,data->window);
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarsocot_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsocot_v_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);


  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zarsocot_v_v_knopka(GtkWidget *widget,class l_zarsocot_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(l_zarsocot_v_zap(data) != 0)
     return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_zarsocot_v_v_vvod(GtkWidget *widget,class l_zarsocot_v_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOD:
    data->kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SUMA_O:
    data->suma_o.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SUMA_B:
    data->suma_b.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SOCOT_O:
    data->socot_o.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SOCOT_B:
    data->socot_b.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PROC:
    data->proc.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_SHET:
    data->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
 enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/***********************************/
/*Запись*/
/****************************/

int l_zarsocot_v_zap(class l_zarsocot_v_data *data)
{


if(data->kod.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }
char strsql[512];
 

 //Проверяем есть ли такой код 
sprintf(strsql,"select kod from Zarsoc where kod=%s",data->kod.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
{
 sprintf(strsql,"%s %s !",gettext("Не найден код соц. страхования"),data->kod.ravno());
 iceb_menu_soob(strsql,data->window);
 return(1);
}

 //Проверяем есть ли такой счет
struct OPSHET shetv;
if(iceb_prsh1(data->shet.ravno(),&shetv,data->window) != 0)
 return(1);

time_t vrem;  
time(&vrem);
 
if(data->kodzap == 0)
  sprintf(strsql,"insert into Zarsocz values (datz,tabn,kodz,shet,sumas,sumap,proc,ktoz,vrem,sumapb,sumasb) \
('%d-%d-01',%d,%d,'%s',%.2f,%.2f,%.2f,%d,%ld,%.2f,%.2f)",
  data->gp,data->mp,data->tabnom,data->kod.ravno_atoi(),data->shet.ravno(),data->suma_o.ravno_atof(),data->socot_o.ravno_atof(),
  data->proc.ravno_atof(),iceb_getuid(data->window),vrem,data->suma_b.ravno_atof(),
  data->socot_b.ravno_atof());

if(data->kodzap != 0)
   sprintf(strsql,"update Zarsocz set \
shet='%s',\
sumas=%.2f,\
sumap=%.2f,\
proc=%.2f,\
ktoz=%d,\
vrem=%ld,\
sumapb=%.2f,\
sumasb=%.2f \
where datz='%04d-%d-01' and tabn=%d and kodz=%d",
   data->shet.ravno(),data->suma_o.ravno_atof(),data->socot_o.ravno_atof(),
  data->proc.ravno_atof(),iceb_getuid(data->window),vrem,data->suma_b.ravno_atof(),
  data->socot_b.ravno_atof(),
  data->gp,data->mp,data->tabnom,data->kodzap);

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
  return(1);

return(0);
}





