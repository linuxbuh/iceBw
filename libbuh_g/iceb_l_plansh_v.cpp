/*$Id: iceb_l_plansh_v.c,v 1.39 2013/09/26 09:47:04 sasa Exp $*/
/*22.11.2017	03.01.2004	Белых А.И.	iceb_l_plansh_v.c
Ввод и корректировка счета в план счетов
*/
#include        "iceb_libbuh.h"
#include        "iceb_l_plansh.h"

enum
 {
  E_SHET,
  E_NAIM,
  KOLENTER  
 };

enum
 {
  FK2,
  FK10,
  KOL_FK
 };

class korshet_data
 {
  public:
  GtkWidget *window;
  iceb_u_str sshet;
  iceb_u_str nshet;
 };
 
class iceb_l_plansh_v
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *menutip;
  GtkWidget *menuvid;
  GtkWidget *menusaldo;
  GtkWidget *menustatus;
  class iceb_u_str name_window;
 
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class  plansh_rek rk;
  short      saldo; //для проверки при попытке развернуть сальдо
  short      status; //для проверки при попытке изменить статус
    
  class iceb_u_str shet_i;
  
  iceb_l_plansh_v() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    rk.tip=gtk_combo_box_get_active (GTK_COMBO_BOX(menutip));
    rk.vid=gtk_combo_box_get_active (GTK_COMBO_BOX(menuvid));
    rk.saldo=gtk_combo_box_get_active (GTK_COMBO_BOX(menusaldo));
    rk.status=gtk_combo_box_get_active (GTK_COMBO_BOX(menustatus));
   }
 };

gboolean	obr_klav_plansh_vvod(GtkWidget *widget,GdkEventKey *event,class iceb_l_plansh_v *data);
GtkWidget       *tipsh(int);
GtkWidget       *vidsh(int);
GtkWidget       *tipsaldo(int);
GtkWidget       *status(int);
int zplansh(class iceb_l_plansh_v *data);
void get_shet(GtkWidget *widget,char *text);
int  prnsh(const char *shet,GtkWidget*);
void		korshet1(const char*,const char*,GtkWidget*);
void  plansh_v_knopka(GtkWidget *widget,class iceb_l_plansh_v *data);
void    plansh_vvod(GtkWidget *widget,class iceb_l_plansh_v *data);

int iceb_l_plansh_v(class iceb_u_str *shetk,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class iceb_u_str stroka;
class iceb_u_str strokatit;
char		strsql[512];
SQL_str		row;
class iceb_u_str      kikz;
class iceb_l_plansh_v data;
data.shet_i.plus(shetk->ravno());

data.rk.clear_zero();
data.name_window.plus(__FUNCTION__);

if(data.shet_i.getdlinna() > 1)
 {
  if(data.shet_i.ravno()[0] == '\0')
   {
    iceb_menu_soob(gettext("Не выбрана запись !"),wpredok);
    return(1);
   }

  SQLCURSOR cur;
  sprintf(strsql,"select * from Plansh where ns='%s'",data.shet_i.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.shet.new_plus(row[0]);
  data.rk.naim.new_plus(row[1]);
  data.rk.tip=atoi(row[2]);
  data.rk.vid=atoi(row[3]);    
  data.saldo=data.rk.saldo=atoi(row[6]);    
  if(data.rk.saldo == 3)
    data.rk.saldo=1;
  data.status=data.rk.status=atoi(row[7]);    
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

if(data.shet_i.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
  strokatit.new_plus(strsql);
  stroka.new_plus(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  strokatit.new_plus(strsql);

  stroka.new_plus(gettext("Корректировка записи"));
  stroka.ps_plus(kikz.ravno());
 }
gtk_window_set_title(GTK_WINDOW(data.window),strokatit.ravno());
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(obr_klav_plansh_vvod),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label1=gtk_label_new(stroka.ravno());

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), label1, FALSE, FALSE, 1);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 1);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox1),TRUE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 1);

GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox2),TRUE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 1);

GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, FALSE, 1);


GtkWidget *label[6];
label[0]=gtk_label_new(gettext("Номер счета"));
label[1]=gtk_label_new(gettext("Наименование счета"));
label[2]=gtk_label_new(gettext("Тип счета"));
label[3]=gtk_label_new(gettext("Вид счета"));
label[4]=gtk_label_new(gettext("Сальдо"));
label[5]=gtk_label_new(gettext("Статус"));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(plansh_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(plansh_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),20);
gtk_entry_set_text (GTK_ENTRY (data.entry[E_SHET]),data.rk.shet.ravno());
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(plansh_vvod),&data);
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),255);
gtk_entry_set_text (GTK_ENTRY (data.entry[E_NAIM]),data.rk.naim.ravno());
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(plansh_vvod),&data);
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

data.menutip = tipsh(data.rk.tip);
data.menuvid = vidsh(data.rk.vid);
data.menusaldo = tipsaldo(data.rk.saldo);
data.menustatus = status(data.rk.status);



gtk_box_pack_start (GTK_BOX (vbox1), label[0], FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2),data.entry[E_SHET], TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox1), label[1],FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2),data.entry[E_NAIM], TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox1), label[2],FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2), data.menutip, TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox1), label[3],FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2), data.menuvid, TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox1), label[4],FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2), data.menusaldo,TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox1), label[5],FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox2), data.menustatus, TRUE, TRUE, 1);

gtk_box_pack_start(GTK_BOX(hbox1), data.knopka[0], TRUE, TRUE, 1);
gtk_box_pack_start(GTK_BOX(hbox1), data.knopka[1], TRUE, TRUE, 1);


gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 shetk->new_plus(data.rk.shet.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
 
return(data.voz);

}
/*****************************************/
/*Обработчик клавиатуры на нажатие для окна ввода и корректировки записи*/
/******************************************/

gboolean	obr_klav_plansh_vvod(GtkWidget *widget,GdkEventKey *event,class iceb_l_plansh_v *data)
{
//printf("obr_klav_plansh_vvod-Нажата клавиша\n");

switch(event->keyval)
 {


  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);


  case GDK_KEY_Escape:  //Выход и
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша переключения регистра\n");

    data->kl_shift=1;

    return(TRUE);
       

  
 }

 return(TRUE);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  plansh_v_knopka(GtkWidget *widget,class iceb_l_plansh_v *data)
{

int knop=atoi(gtk_widget_get_name(widget));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:

    if(zplansh(data) != 0)
     return;
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=0;
    gtk_widget_destroy(data->window);
    
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

void    plansh_vvod(GtkWidget *widget,class iceb_l_plansh_v *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_SHET:
    data->rk.shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(iceb_u_SRAV(data->rk.shet.ravno(),data->shet_i.ravno(),0) != 0)
      prnsh(data->rk.shet.ravno(),data->window);
    break;
  case E_NAIM:
    data->rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }
enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}


/*********************************************/
/**********************************************/

/*****************************/
/*Меню выбора типа счета     */
/*****************************/

GtkWidget *tipsh(int nomerstr)
{

GtkWidget *opt;
class iceb_u_spisok strmenu;
strmenu.plus(gettext("Активный"));
strmenu.plus(gettext("Пассивный"));
strmenu.plus(gettext("Активно-пассивный"));
strmenu.plus(gettext("Контрактивный"));
strmenu.plus(gettext("Контрпасивный"));

iceb_pm_vibor(&strmenu,&opt,nomerstr);

return(opt);
}

/*****************************/
/*Меню выбора вида счета     */
/*****************************/

GtkWidget *vidsh(int nomerstr)
{
GtkWidget *opt;
class iceb_u_spisok strmenu;
strmenu.plus(gettext("Счёт"));
strmenu.plus(gettext("Субсчёт"));

iceb_pm_vibor(&strmenu,&opt,nomerstr);

return(opt);
}
/*****************************/
/*Меню выбора типа сальдо    */
/*****************************/

GtkWidget *tipsaldo(int nomerstr)
{
GtkWidget *opt;
class iceb_u_spisok strmenu;
strmenu.plus(gettext("Свёрнутое"));
strmenu.plus(gettext("Развёрнутое"));

iceb_pm_vibor(&strmenu,&opt,nomerstr);

return(opt);
}

/*****************************/
/*Меню выбора статуса счета    */
/*****************************/

GtkWidget *status(int nomerstr)
{
GtkWidget *opt;
class iceb_u_spisok strmenu;
strmenu.plus(gettext("Балансовый"));
strmenu.plus(gettext("Внебалансовый"));

iceb_pm_vibor(&strmenu,&opt,nomerstr);
return(opt);
}
/************************************/
/*Запись в план счетов              */
/************************************/
int zplansh(class iceb_l_plansh_v *data)
{
time_t vrem;
char   strsql[512];
data->read_rek();

if(data->rk.shet.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён номер счета !"),data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

short metkor=0;

time(&vrem);
if(data->rk.saldo == 1)
 data->rk.saldo=3;
 
if(data->shet_i.getdlinna() <= 1)        
 {

  if(prnsh(data->rk.shet.ravno(),data->window) != 0)
    return(1);


  sprintf(strsql,"insert into Plansh \
values ('%s','%s',%d,%d,%d,%ld,%d,%d,%d,%d,0)",
  data->rk.shet.ravno_filtr(),
  data->rk.naim.ravno_filtr(),
  data->rk.tip,
  data->rk.vid,
  iceb_getuid(data->window),vrem,
  data->rk.saldo,
  data->rk.status,
  data->rk.val,0);
 }
else
 {
  if(data->saldo == 0 && data->rk.saldo == 3)
   {
    sprintf(strsql,"select sh from Prov where sh='%s' \
and kodkon='' limit 1",data->shet_i.ravno_filtr());
    if(sql_readkey(&bd,strsql) > 0)
     {
      iceb_u_str repl;
      
      repl.plus(gettext("По этому счету уже введены проводки без кода контрагента !"));
      repl.ps_plus(gettext("Сальдо можно развернуть после удаления этих проводок"));
      iceb_menu_soob(&repl,data->window);
      return(1);
     }
   }

  if(data->status != data->rk.status)
   {

    //Проверяем есть ли проводки по этому счету
    sprintf(strsql,"select sh from Prov where sh='%s' limit 1",data->shet_i.ravno_filtr());
    if(sql_readkey(&bd,strsql) > 0)
     {
      iceb_u_str repl;
      
      repl.plus(gettext("По этому счету уже введены проводки !"));
      repl.ps_plus(gettext("Статус можно изменить только после удаления всех проводок по этому счету"));
      iceb_menu_soob(&repl,data->window);
      return(1);
     }

   }

  if(iceb_u_SRAV(data->shet_i.ravno(),data->rk.shet.ravno(),0) != 0)
   {
    if(prnsh(data->rk.shet.ravno(),data->window) != 0)
      return(1);

    iceb_u_str SP;
    SP.plus(gettext("Кооректировать номер счета ? Вы уверены !"));

    if(iceb_menu_danet(&SP,2,NULL) == 2)
     return(1);
    metkor=1;
    
   }




  sprintf(strsql,"update Plansh \
set \
ns='%s',\
nais='%s',\
tips=%d,\
vids=%d,\
ktoi=%d,\
vrem=%ld,\
saldo=%d,\
stat=%d,\
val=%d \
where ns='%s'",
  data->rk.shet.ravno_filtr(),
  data->rk.naim.ravno_filtr(),
  data->rk.tip,
  data->rk.vid,
  iceb_getuid(data->window),vrem,
  data->rk.saldo,
  data->rk.status,
  data->rk.val,
  data->shet_i.ravno_filtr());
 }

//printf("\n%s-strsql=%s\n",__FUNCTION__,strsql);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),data->window);
   return(1);
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
   return(1);
  }
 }
if(metkor == 1)
 {
  gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
  korshet1(data->shet_i.ravno_filtr(),data->rk.shet.ravno_filtr(),data->window);
 }
return(0);

}


/****************************/
/*Прверка номера счета*/
/**************************/

int  prnsh(const char *shet,GtkWidget *wpredok)
{
char  strsql[512];

sprintf(strsql,"select ns from Plansh where ns='%s'",shet);
if(sql_readkey(&bd,strsql) > 0)
 {
  sprintf(strsql,gettext("Счет %s уже есть в плане счетов !"),shet);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
return(0);
}




/*****************************************************************/
/*Программа каскадной корректировки номера счета во всех таблицах*/
/*****************************************************************/
void korshet1(const char *sshet, //Старый номер счета
const char *nshet,
GtkWidget *wpredok)  //Новый номер счета
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);
char		strsql[512];

int kolstr=28;
float kolstr1=0.;

sprintf(strsql,"update Skontr set ns='%s' where ns='%s'",nshet,sshet);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return;
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
   return;
  }
 }
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Saldo set ns='%s' where ns='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);


sprintf(strsql,"update Prov set sh='%s' where sh='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Prov set shk='%s' where shk='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Kart set shetu='%s' where shetu='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);


sprintf(strsql,"update Kartb set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Nash set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);


sprintf(strsql,"update Uder set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);


sprintf(strsql,"update Zarp set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);


sprintf(strsql,"update Zarn set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Zarn1 set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Zarsoc set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Zarsocz set shet='%s' where shet='%s'",nshet,sshet);
iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);


sprintf(strsql,"update Uosinp set shetu='%s' where shetu='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Uslugi set shetu='%s' where shetu='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Usldokum1 set shetu='%s' where shetu='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Usldokum2 set shetu='%s' where shetu='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Kas set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Kasop1 set shetkas='%s' where shetkas='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Kasop1 set shetkor='%s' where shetkor='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Kasop2 set shetkas='%s' where shetkas='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Kasop2 set shetkor='%s' where shetkor='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Kasord set shetk='%s' where shetk='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Ukrkras set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Ukrdok1 set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Upldok2a set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);


sprintf(strsql,"update Opldok set shet='%s' where shet='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

sprintf(strsql,"update Zarsdo set sh='%s' where sh='%s'",nshet,sshet);

iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kolstr,++kolstr1);

}
