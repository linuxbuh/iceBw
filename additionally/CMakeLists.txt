#cборка iceBw
message ("Построение дополнительных программ")

#project(iceBw) #имя проекта


#сборка программы диспетчиризации такси
# Установка переменной со списком исходников
set(SOURSE_BIN i_taxi.cpp 
	lzaktaxi_p.cpp lzaktaxiv.cpp l_zaktaxi.cpp 
	l_taxikz.cpp l_taxikz_p.cpp l_taxikz_v.cpp
	l_taxisk.cpp l_taxisk_p.cpp l_taxisk_v.cpp
	l_taxitel.cpp l_taxitel_p.cpp l_taxitel_v.cpp
	l_vod.cpp l_vod_p.cpp l_vod_v.cpp 
	rpod_l.cpp rpod_v.cpp rpod_p.cpp 
	taxi_oth.cpp taxi_oth_v.cpp taxinast.cpp 
	taxi_oth_r.cpp taxi_saldo.cpp prov_zak_time.cpp)


add_executable(i_taxi ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_taxi buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка программы разметки таблиц для программі диспетчеризации такси
# Установка переменной со списком исходников
set(SOURSE_BIN i_sbtaxi.cpp i_sbtaxi_r.cpp)

add_executable(i_sbtaxi ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_sbtaxi buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка программы выписки счетов в ресторанах 
# Установка переменной со списком исходников
set(SOURSE_BIN i_rest.cpp 
	kod_podr_v.cpp 
	read_card.cpp restdok_rshet.cpp restdok_uddok.cpp restnast.cpp rest_flag_kl.cpp rest_oth_r.cpp 
	rest_oth_v.cpp rest_sal_kl.cpp rest_sptov_m.cpp rpod_l.cpp rpod_p.cpp rpod_v.cpp 
	l_grkl.cpp l_grkl_v.cpp l_grkl_p.cpp 
	l_klientr.cpp l_klientr_bl.cpp l_klientr_v.cpp l_klientr_vg.cpp l_klientr_p.cpp l_klient_imp.cpp 
	l_restdok.cpp l_restdok_v.cpp l_restdok2.cpp l_restdok2_vgr.cpp l_restdok2_vmu.cpp l_restdok2_znm.cpp 
	l_spisok.cpp l_spisok_v.cpp l_sptur.cpp l_sptur_v.cpp l_srestdok.cpp l_srestdok_p.cpp 
	l_vhvi.cpp l_vhvi_r.cpp l_vhvi_p.cpp l_vmat.cpp l_vrint.cpp l_vrint_k.cpp l_vrint_v.cpp l_vrint_p.cpp 
	m_poird.cpp 
	osttov.cpp 
	sumapsh.cpp 
	t_msql_error.cpp 
	vip_shet.cpp vrshet_nomdok.cpp vrshet_v.cpp 
	zap_rest_dok.cpp zap_vrest_dok.cpp zap_v_kas.cpp)

add_executable(i_rest ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_rest buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************


#сборка программы разметки таблиц для программы выписки счетов
# Установка переменной со списком исходников
set(SOURSE_BIN i_sozbrest.cpp i_sozbrest_r.cpp)

add_executable(i_sozbrest ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_sozbrest buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка программа модификации базы данных
# Установка переменной со списком исходников
set(SOURSE_BIN i_alter_rt.cpp i_alter_rtr.cpp)

add_executable(i_alter_rt ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_alter_rt buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************


#сборка программа кассира 
# Установка переменной со списком исходников
set(SOURSE_BIN i_rest_k.cpp 
	kod_podr_v.cpp 
	l_grkl.cpp l_grkl_p.cpp l_grkl_v.cpp
	l_klientr.cpp l_klientr_bl.cpp l_klientr_p.cpp l_klientr_v.cpp l_klientr_vg.cpp l_klient_imp.cpp 
	l_restdok1.cpp l_saldokl.cpp l_spis_opl.cpp l_spis_opl_p.cpp l_spis_opl_z.cpp
	l_vhvi_pkr.cpp 
	otm_opl_dok.cpp otm_opl_doks.cpp 
	prov_vv.cpp prov_vv_r.cpp 
	ras_kl.cpp ras_sal_kas.cpp ras_sal_kl.cpp ras_sheks.cpp read_card.cpp rest_flag_kl.cpp restdok_rshet.cpp restnast_k.cpp 
	rpod_l.cpp rpod_p.cpp rpod_v.cpp
	spdnr_r.cpp sumapsh.cpp 
	t_msql_error.cpp 
	vid_kar_kl.cpp vsnsk.cpp vsvks.cpp vv_kolih.cpp
	zap_v_kas.cpp) 

add_executable(i_rest_k ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_rest_k buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка программа расчёта сальдо
# Установка переменной со списком исходников
set(SOURSE_BIN i_rest_saldo.cpp
	restdok_uddok.cpp restnast.cpp rest_saldo_r.cpp rest_saldo_del_r.cpp
	t_msql_error.cpp 
	zap_v_kas.cpp)

add_executable(i_rest_saldo ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_rest_saldo buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка программа расчёта сальдо
# Установка переменной со списком исходников
set(SOURSE_BIN i_taxi_saldo.cpp 
	rpod_l.cpp rpod_v.cpp rpod_p.cpp 
	l_taxisk.cpp l_taxisk_p.cpp l_taxisk_v.cpp
	l_tk_saldo.cpp l_tk_saldo_v.cpp l_tk_saldo_p.cpp 
	taxinast.cpp taxi_saldo_r.cpp taxi_saldo.cpp taxi_ud_god.cpp)

add_executable(i_taxi_saldo ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_taxi_saldo buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка программы просмотра записей с турникетов на вход и выход
# Установка переменной со списком исходников
set(SOURSE_BIN i_pros_vv.cpp 
	l_grkl.cpp l_grkl_p.cpp l_grkl_v.cpp 
	l_klientr.cpp l_klientr_bl.cpp l_klientr_p.cpp l_klientr_v.cpp l_klientr_vg.cpp l_klient_imp.cpp 
	l_vhvi.cpp l_vhvi_p.cpp l_vhvi_r.cpp 
        l_spis_opl.cpp l_spis_opl_p.cpp l_spis_opl_z.cpp 
	rpod_l.cpp rpod_p.cpp rpod_v.cpp 
	ud_zap_vv.cpp)

add_executable(i_pros_vv ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_pros_vv buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка программы проверки возможности входа/выхода на турникете
# Установка переменной со списком исходников
set(SOURSE_BIN i_kontr_vv.cpp ras_sal_kl_t.cpp t_msql_error.cpp otm_opl_doks_t.cpp otm_opl_dok_t.cpp sumapsh_t.cpp t_msql_error.cpp zap_v_kas.cpp  i_vv_start.cpp)

add_executable(i_kontr_vv ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_kontr_vv buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка программы записи в базу когда и кто вышел/зашёл
# Установка переменной со списком исходников
set(SOURSE_BIN i_zapis_vv.cpp t_msql_error.cpp zap_rest_dok.cpp zap_vrest_dok.cpp vrshet_nomdok.cpp t_msql_error.cpp  i_vv_start.cpp)

add_executable(i_zapis_vv ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_zapis_vv buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************
