#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

#define MAX_DEGREES 128

struct localita {
	char nome[46];
	char regione[46];
};

static void info_visite(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];

	// Input for info_visite routine
	char itinerario[46];

	printf("\ninserire l'itinerario: ");
	getInput(46, itinerario, false);


	if(!setup_prepared_stmt(&prepared_stmt, "call info_visite_autista(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize info_visite_autista statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = itinerario;
	param[0].buffer_length = strlen(itinerario);


	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for info_visite_autista\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching visits' information.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito le informazioni sulle visite:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}

static size_t parse_localita(MYSQL *conn, MYSQL_STMT *stmt, struct localita **ret) {
	int status;
	size_t row = 0;
	MYSQL_BIND param[4];

	char nome[46];
	char regione[46];

	if (mysql_stmt_store_result(stmt)) {
		fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
		exit(0);
	}

	*ret = malloc(mysql_stmt_num_rows(stmt) * sizeof(struct localita));

	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = nome;
	param[0].buffer_length = 46;

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = regione;
	param[1].buffer_length = 46;

	if(mysql_stmt_bind_result(stmt, param)) {
		finish_with_stmt_error(conn, stmt, "Unable to bind column parameters\n", true);
	}

	/* assemble loc general information */
	while (true) {
		status = mysql_stmt_fetch(stmt);

		if (status == 1 || status == MYSQL_NO_DATA)
			break;

		strcpy((*ret)[row].nome, nome);
		strcpy((*ret)[row].regione, regione);

		row++;
	}

	return row;
}

static void cartine(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];
	int status;
	bool first = true;
	struct localita *loc;
	size_t localita = 0;
	char header[512];

	char itinerario[46];

	printf("\ninserire l'itinerario: ");
	getInput(46, itinerario, false);

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call cartine_programma(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize cartine_programma statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = itinerario;
	param[0].buffer_length = strlen(itinerario);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for cartine_programma\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while retrieving the career report.");
		goto out;
	}

	// We have multiple result sets here!
	do {
		// Skip OUT variables (although they are not present in the procedure...)
		if(conn->server_status & SERVER_PS_OUT_PARAMS) {
			goto next;
		}

		if(first) {
			parse_localita(conn, prepared_stmt, &loc);
			first = false;
		} else {
			sprintf(header, "\nCartine disponibili per la seguente località: \nNome: %s\nregione: %s\n", loc[localita].nome, loc[localita].regione);
			dump_result_set(conn, prepared_stmt, header);
			localita++;
		}

		// more results? -1 = no, >0 = error, 0 = yes (keep looking)
	    next:
		status = mysql_stmt_next_result(prepared_stmt);
		if (status > 0)
			finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);

	} while (status == 0);

    out:
	mysql_stmt_close(prepared_stmt);
}

static void viaggi_assegnati(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];

	// Input for viaggi_assegnati routine



	if(!setup_prepared_stmt(&prepared_stmt, "call viaggio_assegnato_autista(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize viaggio_assegnato_autista statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for viaggio_assegnato_autista\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching assigned travels.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito i viaggi e il pullman assegnati:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}

static void orario_fermate(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[2];

	// Input for orario_fermate routine
	char itinerario[46];
	char temp[10];
	int giornata;

	printf("\ninserire l'itinerario: ");
	getInput(46, itinerario, false);

	printf("\ninserire la giornata di interesse: ");
	getInput(10, temp, false);

	giornata = atoi(temp);

	if(!setup_prepared_stmt(&prepared_stmt, "call orario_fermate(?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize orario_fermate statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = itinerario;
	param[0].buffer_length = strlen(itinerario);

	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &giornata;
	param[1].buffer_length = sizeof(giornata);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for orario_fermate\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching stops' time.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito l'orario delle fermate per la giornata selezionata:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}

static void info_alberghi(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];

	// Input for info_alberghi routine
	char itinerario[46];

	printf("\ninserire l'itinerario: ");
	getInput(46, itinerario, false);


	if(!setup_prepared_stmt(&prepared_stmt, "call info_alberghi(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize info_alberghi statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = itinerario;
	param[0].buffer_length = strlen(itinerario);


	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for info_alberghi\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching hotels'.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito le informazioni sugli hotel in cui si alloggerà durante il viaggio:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}

static void distanze(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[2];

	// Input for distanze routine
	char indirizzo_1[46];
	char indirizzo_2[46];

	printf("\ninserire il primo indirizzo: ");
	getInput(46, indirizzo_1, false);

	printf("\ninserire il secondo indirizzo: ");
	getInput(46, indirizzo_2, false);


	if(!setup_prepared_stmt(&prepared_stmt, "call calcola_distanza(?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize calcola_distanza statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = indirizzo_1;
	param[0].buffer_length = strlen(indirizzo_1);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = indirizzo_2;
	param[1].buffer_length = strlen(indirizzo_2);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for calcola_distanza\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching distance between two addresses.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito la distanza richiesta:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}




void run_as_autista(MYSQL *conn) {
	char options[7] = {'1','2','3','4','5','6','7'};
	char op;

	printf("Switching to autista role...\n");

	if(!parse_config("users/autista.json", &conf)) {
		fprintf(stderr, "Unable to load autista configuration\n");
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
		printf("2) informazioni fermate del giorno \n");
		printf("3) informazioni sulle visite di un viaggio \n");
		printf("4) cerca distanza tra due luoghi \n");
		printf("5) cerca le informazioni degli alloggi per un certo viaggio\n");
		printf("6) cerca le cartine disponibili per un certo viaggio\n");
		printf("7) Quit\n");

		op = multiChoice("Select an option", options, 7);

		switch(op) {
			case '1':
				viaggi_assegnati(conn);
				break;

			case '2':
				orario_fermate(conn);
				break;

			case '3':
				info_visite(conn);
				break;

			case '4':
				distanze(conn);
				break;

			case '5':
				info_alberghi(conn);
				break;

			case '6':
				cartine(conn);
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
