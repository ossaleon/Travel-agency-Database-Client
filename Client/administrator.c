#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"


static void add_employee(MYSQL *conn, int ruolo)
{
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[6];

	// Input for the registration routine
	char tel[16];
	char name[46];
	char surname[46];
	char username[46];
	char password[46];
	char *r[3] = {"hostess", "meccanico", "autista"};

	// Get the required information
	printf("\n %s telephone number: ", r[ruolo]);
	getInput(16, tel, false);
	printf("\n%s name: ", r[ruolo]);
	getInput(46, name, false);
	printf("%s surname: ", r[ruolo]);
	getInput(46, surname, false);
	printf("%s username: ", r[ruolo]);
	getInput(46, username, false);
	printf("create a password for the %s: ", r[ruolo]);
	getInput(46, password, false);
	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_employee(?, ?, ?, ?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize employee insertion statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = tel;
	param[0].buffer_length = strlen(tel);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = name;
	param[1].buffer_length = strlen(name);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = surname;
	param[2].buffer_length = strlen(surname);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING; // OUT
	param[3].buffer = username;
	param[3].buffer_length = strlen(username);

	param[4].buffer_type = MYSQL_TYPE_VAR_STRING; // OUT
	param[4].buffer = password;
	param[4].buffer_length = strlen(password);

	param[5].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[5].buffer = &ruolo;
	param[5].buffer_length = sizeof(ruolo);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for employee insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while adding the employee.");
	} else{
		printf("%s correctly added \n", r[ruolo]);
	}

	mysql_stmt_close(prepared_stmt);
}

static void create_administrator(MYSQL *conn)
{
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[3];

	// Input for the registration routine
	char username[46];
	char password[46];
	char *ruolo = "amministratore";

	// Get the required information
	printf("\nUsername: ");
	getInput(46, username, false);
	printf("password: ");
	getInput(46, password, true);


	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call crea_utente(?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize admin insertion statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = ruolo;
	param[2].buffer_length = strlen(ruolo);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for admin insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while adding the admin.");
	} else {
		printf("admin correctly added...\n");
	}

	mysql_stmt_close(prepared_stmt);
}



void run_as_amministratore(MYSQL *conn)
{
	char options[5] = {'1', '2', '3', '4', '5'};
	char op;

	printf("Switching to administrative role...\n");

	if(!parse_config("users/amministratore.json", &conf)) {
		fprintf(stderr, "Unable to load administrator configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	while(true) {
		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) Add new hostess\n");
		printf("2) Add new mechanic\n");
		printf("3) Add new driver\n");
		printf("4) Add new administrator\n");
		printf("5) Quit\n");

		op = multiChoice("Select an option", options, 5);

		switch(op) {
			case '1':
				add_employee(conn, 0);
				break;
			case '2':
				add_employee(conn, 1);
				break;
			case '3':
				add_employee(conn, 2);
				break;
			case '4':
				create_administrator(conn);
				break;
			case '5':
				return;

			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}
