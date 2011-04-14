/*
	lmhosts.h
	Luca Piergentili, 22/01/97
	lpiergentili@yahoo.com

	Indirizzo IP host locale.
	Referenzia la postazione utilizzata come server nella rete locale (vedi elenco sotto).

	Sul server impostare le proprieta' TCP/IP (relative alla rete locale) indicando un
	indirizzo IP statico (192.168.0.1) e specificando come	submask l'indirizzo 255.255.255.0.

	Sui client impostare le proprieta' TCP/IP (relative alla scheda di rete) indicando un
	indirizzo IP statico (da 192.168.0.2 a 192.168.0.n, con n che si incrementa per ogni
	postazione) e specificando come submask l'indirizzo 255.255.255.0.

	Prima di impostare un indirizzo statico nelle proprieta TCP/IP delle postazioni di rete,
	disabilitare la configurazione DNS Internet.
	Dato che la modifica si ripercuote sulle proprieta' TCP/IP dell'accesso telefonico,
	impostare gli indirizzi DNS direttamente nelle proprieta' della connessione telefonica.

	192.168.0.1	server NT
	192.168.0.2	client
	192.168.0.3	  ""
	192.168.0.4	  ""
	192.168.0.5	  ""
	192.168.0.6	  ""
	192.168.0.7	  ""
	192.168.0.8	  ""
*/
#ifndef _LMHOSTS_H
#define _LMHOSTS_H 1

#define DUMMY_HOST		"128.128.128.128"
#define NULL_HOST		"0.0.0.0"
#define LOCAL_HOST		"127.0.0.1"
#define LOCAL_HOST_NAME	"localhost"
#define LOCAL_HOST_NUM	((unsigned long)16777343)

#endif /* _LMHOSTS_H */
