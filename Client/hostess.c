#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "defines.h"

static void viaggi_assegnati(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];

	// Input for viaggi_assegnati routine



	if(!setup_prepared_stmt(&prepared_stmt, "call viaggio_assegnato_hostess(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize viaggio_assegnato_hostess statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for viaggi_assegnati\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching assigned travels.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito i viaggi assegnati:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}

static void inserisci_cliente(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[5];

	// Input for inserisci cliente routine
	char telefono[16];
	char indirizzo[46];
	char nome[46];
	char email[46];
	char fax[46];


	printf("\ninserire il numero di telefono del cliente: ");
	getInput(16, telefono, false);

	printf("\ninserire un indirizzo del cliente: ");
	getInput(46, indirizzo, false);

	printf("\ninserire il nome del cliente: ");
	getInput(46, nome, false);

	printf("\ninserire la mail del cliente: ");
	getInput(46, email, false);

	printf("\ninserire il fax del cliente: ");
	getInput(16, fax, false);



	if(!setup_prepared_stmt(&prepared_stmt, "call inserisci_cliente(?,?,?,?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize inserisci_cliente statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = telefono;
	param[0].buffer_length = strlen(telefono);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = indirizzo;
	param[1].buffer_length = strlen(indirizzo);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = nome;
	param[2].buffer_length = strlen(nome);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = email;
	param[3].buffer_length = strlen(email);

	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[4].buffer = fax;
	param[4].buffer_length = strlen(fax);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for inserisci_cliente\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while inserting customer.");
		goto out;
	}

	printf("\ncliente inserito correttamente\n");


  out:
	mysql_stmt_close(prepared_stmt);
}

static void inserisci_passeggero(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[9];

	// Input for inserisci_passeggero routine
	char telefono[16];
	char nome[46];
	char eta[3];
	char posto[4];
	char username[46];
	MYSQL_TIME data;
	char itinerario[46];
	MYSQL_TIME oggi;
	int n;
	char password[46];

	printf("\ninserire il numero di telefono del cliente prenotante: ");
	getInput(16, telefono, false);

	printf("\ninserire il nome del passeggero: ");
	getInput(46, nome, false);

	printf("\ninserire l'età del passeggero: ");
	getInput(3, eta, false);

	printf("\ninserire un posto per il passeggero: ");
	getInput(4, posto, false);

	printf("\ninserire un username per il passeggero: ");
	getInput(46, username, false);

	printf("\ninserire una password per il passeggero: ");
	getInput(46, password, false);

	char temp[10];

	memset(&data, 0, sizeof(data));

	printf("\ninserire ora la data del viaggio in cui si vole inserire il passeggero");

	G:
	printf("\ninserire il giorno del viaggio, da 1 a 31: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 1 || n > 31){
		printf("\ngiorno non valido, riprovare: ");
		goto G;
	}

	data.day = n;

	M:
	printf("\ndigitare il mese del viaggio, da 1 a 12: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 1 || n > 12){
		printf("\nmese non valido, riprovare: ");
		goto M;
	}

	data.month = n;

	A:
	printf("\ninserire l'anno del viaggio, da 2000 a 2022: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 2000 || n > 2022){
		printf("\nanno non valido, riprovare: ");
		goto A;
	}

	data.year = n;

	printf("\ndigitare il nome del viaggio in cui si vuole inserire il passeggero: ");
	getInput(46, itinerario, false);


	struct tm *tempo;
  time_t t;
  time(&t);
  tempo = localtime(&t);

  oggi.day = tempo->tm_mday;
	oggi.month = tempo->tm_mon + 1;
	oggi.year = tempo->tm_year + 1900;

	if(!setup_prepared_stmt(&prepared_stmt, "call inserisci_passeggero(?,?,?,?,?,?,?,?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize inserisci_passeggero statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = telefono;
	param[0].buffer_length = strlen(telefono);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = nome;
	param[1].buffer_length = strlen(nome);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = eta;
	param[2].buffer_length = strlen(eta);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = posto;
	param[3].buffer_length = strlen(posto);

	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[4].buffer = username;
	param[4].buffer_length = strlen(username);

	param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[5].buffer = password;
	param[5].buffer_length = strlen(password);

	param[6].buffer_type = MYSQL_TYPE_DATE;
	param[6].buffer = &data;
	param[6].buffer_length = sizeof(data);

	param[7].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[7].buffer = itinerario;
	param[7].buffer_length = strlen(itinerario);

	param[8].buffer_type = MYSQL_TYPE_DATE;
	param[8].buffer = &oggi;
	param[8].buffer_length = sizeof(oggi);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for inserisci_passeggero\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while inserting passenger.");
		goto out;
	}

	printf("\npasseggero inserito correttamente\n");


  out:
	mysql_stmt_close(prepared_stmt);
}

static void prenota_stanza(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[4];
	char options[7] = {'1','2','3','4','5','6','7'};
	char r;

	// Input for prenota_stanza routine
	char indirizzo[46];
	MYSQL_TIME data;
	char itinerario[46];
	char *stanze[7] = {"singola", "doppia", "tripla", "quadrupla", "suite", "doppia con 3° letto", "doppia con 4° letto"};
	char *tipo;
	int n;



	printf("\ninserire l'indirizzo dell'albergo: ");
	getInput(46, indirizzo, false);

	printf("\ninserire l'itinerario: ");
	getInput(46, itinerario, false);


	char temp[10];

	memset(&data, 0, sizeof(data));

	printf("\ninserire ora la data di partenza del viaggio");

	G:
	printf("\ninserire il giorno di partenza del viaggio, da 1 a 31: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 1 || n > 31){
		printf("\ngiorno non valido, riprovare: ");
		goto G;
	}

	data.day = n;

	M:
	printf("\ndigitare il mese di partenza del viaggio, da 1 a 12: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 1 || n > 12){
		printf("\nmese non valido, riprovare: ");
		goto M;
	}

	data.month = n;

	A:
	printf("\ninserire l'anno di partenza del viaggio, da 2000 a 2022: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 2000 || n > 2022){
		printf("\nanno non valido, riprovare: ");
		goto A;
	}

	data.year = n;


	printf("\ninserire un numero da 1 a 7 per scegliere il tipo di stanza da prenotare: \n1)singola \n2)doppia \n3)tripla \n4)quadrupla \n5)suite \n6)doppia con 3° letto \n7)doppia con 4° letto");

	r = multiChoice("Seleziona una tipologia di stanza", options, 7);

	// Salvo il giorno coerentemente a come è salvato sul DB
	switch(r) {
		case '1':
			tipo=stanze[0];
			break;
		case '2':
			tipo=stanze[1];
			break;
		case '3':
			tipo=stanze[2];
			break;
		case '4':
			tipo=stanze[3];
			break;
		case '5':
			tipo=stanze[4];
			break;
		case '6':
			tipo=stanze[5];
			break;
		case '7':
			tipo=stanze[6];
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}



	if(!setup_prepared_stmt(&prepared_stmt, "call prenota_stanza(?,?,?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize prenota_stanza statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = indirizzo;
	param[0].buffer_length = strlen(indirizzo);

	param[1].buffer_type = MYSQL_TYPE_DATE;
	param[1].buffer = &data;
	param[1].buffer_length = sizeof(data);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = itinerario;
	param[2].buffer_length = strlen(itinerario);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = tipo;
	param[3].buffer_length = strlen(tipo);


	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for prenota_stanza\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while booking room.");
		goto out;
	}

	printf("\nstanza prenotata correttamente\n");


  out:
	mysql_stmt_close(prepared_stmt);
}

static void conferma_viaggio(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[3];

	// Input for counter routine
	char telefono[16];
	MYSQL_TIME data;
	char itinerario[46];
	int n;


	printf("\ninserire il numero di telefono dell'hostess da assegnare al viaggio: ");
	getInput(16, telefono, false);

	printf("\ninserire l'itinerario del viaggio che si vuole confermare: ");
	getInput(46, itinerario, false);

	char temp[10];

	memset(&data, 0, sizeof(data));

	printf("\ninserire ora la data del viaggio che si vuole confermare");

	G:
	printf("\ninserire il giorno del viaggio, da 1 a 31: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 1 || n > 31){
		printf("\ngiorno non valido, riprovare: ");
		goto G;
	}

	data.day = n;

	M:
	printf("\ndigitare il mese del viaggio, da 1 a 12: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 1 || n > 12){
		printf("\nmese non valido, riprovare: ");
		goto M;
	}

	data.month = n;

	A:
	printf("\ninserire l'anno del viaggio, da 2000 a 2022: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 2000 || n > 2022){
		printf("\nanno non valido, riprovare: ");
		goto A;
	}

	data.year = n;



	if(!setup_prepared_stmt(&prepared_stmt, "call conferma_viaggio(?,?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize conferma_viaggio statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_DATE;
	param[0].buffer = &data;
	param[0].buffer_length = sizeof(data);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = itinerario;
	param[1].buffer_length = strlen(itinerario);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = telefono;
	param[2].buffer_length = strlen(telefono);


	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for conferma_viaggio\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while confirming travel.");
		goto out;
	}

	printf("\nviaggio confermato correttamente\n");


  out:
	mysql_stmt_close(prepared_stmt);
}

static void inserisci_costo(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[5];
	char options[7] = {'1','2','3','4','5','6','7'};
	char r;

	// Input for inserisci_costo routine
	char indirizzo[46];
	MYSQL_TIME data;
	char itinerario[46];
	char *stanze[7] = {"singola", "doppia", "tripla", "quadrupla", "suite", "doppia con 3° letto", "doppia con 4° letto"};
	char *tipo;
	int costo;
	int n;



	printf("\ninserire l'indirizzo dell'albergo: ");
	getInput(46, indirizzo, false);

	printf("\ninserire l'itinerario: ");
	getInput(46, itinerario, false);


	char temp[10];

	memset(&data, 0, sizeof(data));

	printf("\ninserire ora la data di partenza del viaggio");

	G:
	printf("\ninserire il giorno di partenza del viaggio, da 1 a 31: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 1 || n > 31){
		printf("\ngiorno non valido, riprovare: ");
		goto G;
	}

	data.day = n;

	M:
	printf("\ndigitare il mese di partenza del viaggio, da 1 a 12: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 1 || n > 12){
		printf("\nmese non valido, riprovare: ");
		goto M;
	}

	data.month = n;

	A:
	printf("\ninserire l'anno di partenza del viaggio, da 2000 a 2022: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 2000 || n > 2022){
		printf("\nanno non valido, riprovare: ");
		goto A;
	}

	data.year = n;



	printf("\ninserire un numero da 1 a 7 per scegliere il tipo di stanza da prenotare: \n1)singola \n2)doppia \n3)tripla \n4)quadrupla \n5)suite \n6)doppia con 3° letto \n7)doppia con 4° letto");

	r = multiChoice("Seleziona una tipologia di stanza", options, 7);

	// Salvo il giorno coerentemente a come è salvato sul DB
	switch(r) {
		case '1':
			tipo=stanze[0];
			break;
		case '2':
			tipo=stanze[1];
			break;
		case '3':
			tipo=stanze[2];
			break;
		case '4':
			tipo=stanze[3];
			break;
		case '5':
			tipo=stanze[4];
			break;
		case '6':
			tipo=stanze[5];
			break;
		case '7':
			tipo=stanze[6];
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	printf("\ninserire il costo della stanza: ");
	getInput(5, temp, false);

	costo = atoi(temp);

	if(!setup_prepared_stmt(&prepared_stmt, "call inserisci_costo(?,?,?,?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize inserisci_costo statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = indirizzo;
	param[0].buffer_length = strlen(indirizzo);

	param[1].buffer_type = MYSQL_TYPE_DATE;
	param[1].buffer = &data;
	param[1].buffer_length = sizeof(data);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = itinerario;
	param[2].buffer_length = strlen(itinerario);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = tipo;
	param[3].buffer_length = strlen(tipo);

	param[4].buffer_type = MYSQL_TYPE_LONG;
	param[4].buffer = &costo;
	param[4].buffer_length = sizeof(costo);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for prenota_stanza\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while booking room.");
		goto out;
	}

	printf("\nprezzo registrato correttamente\n");


  out:
	mysql_stmt_close(prepared_stmt);
}





void run_as_hostess(MYSQL *conn)
{
	char options[7] = {'1','2','3','4','5','6', '7'};
	char op;

	printf("Switching to hostess role...\n");

	if(!parse_config("users/hostess.json", &conf)) {
		fprintf(stderr, "Unable to load hostess configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	while(true) {
		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) ricerca viaggi assegnati \n");
		printf("2) inserisci un cliente \n");
		printf("3) inserisci un passeggero \n");
		printf("4) conferma un viaggio\n");
		printf("5) prenota una stanza\n");
		printf("6) inserisci il costo di una stanza\n");
		printf("7) Quit\n");

		op = multiChoice("Select an option", options, 7);

		switch(op) {
			case '1':
				viaggi_assegnati(conn);
				break;

			case '2':
				inserisci_cliente(conn);
				break;

			case '3':
				inserisci_passeggero(conn);
				break;

			case '4':
				conferma_viaggio(conn);
				break;

			case '5':
				prenota_stanza(conn);
				break;

			case '6':
				inserisci_costo(conn);
				break;

			case '7':
				return;

			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}
