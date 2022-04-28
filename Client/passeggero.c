#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

static void verifica_viaggio(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];

	// Input for verifica_viaggio routine



	if(!setup_prepared_stmt(&prepared_stmt, "call stato_viaggio(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize stato_viaggio statement\n", false);
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
		print_stmt_error (prepared_stmt, "An error occurred while searching travel state.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito lo stato del viaggio:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}

static void info_visite(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];

	// Input for verifica_viaggio routine



	if(!setup_prepared_stmt(&prepared_stmt, "call info_visite_passeggero(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize info_visite_passeggero statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for info_visite_passeggero\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching visits' informations.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito lo stato del viaggio:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}





void run_as_passeggero(MYSQL *conn)
{
	char options[3] = {'1','2','3'};
	char op;

	printf("Switching to passeggero role...\n");

	if(!parse_config("users/passeggero.json", &conf)) {
		fprintf(stderr, "Unable to load passeggero configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	while(true) {
		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) verifica lo stato del viaggio \n");
		printf("2) visualizza informazioni sulle visite del viaggio \n");
		printf("3) Quit\n");

		op = multiChoice("Select an option", options, 3);

		switch(op) {
			case '1':
				verifica_viaggio(conn);
				break;

			case '2':
				info_visite(conn);
				break;

			case '3':
				return;

			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}
