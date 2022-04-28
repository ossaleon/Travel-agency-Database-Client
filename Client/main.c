#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "defines.h"

typedef enum {
	AUTISTA = 1,
	HOSTESS,
	MECCANICO,
	PASSEGGERO,
	AMMINISTRATORE,
	FAILED_LOGIN
} role_t;


struct configuration conf;

static MYSQL *conn;


static void lista_viaggi(MYSQL *conn){
	MYSQL_STMT *prepared_stmt;

	if(!setup_prepared_stmt(&prepared_stmt, "call lista_viaggi()", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize lista_viaggi statement\n", false);
	}

	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching travels.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito la lista dei viaggi:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}


static void run_as_guest(MYSQL *conn){


	char options[2] = {'1','2'};
	char r;

	while(true){
		printf("\ninserire 1 o 2: \n1) visualizzare i viaggi della compagnia \n2) Quit");

		r = multiChoice("\nSeleziona una scelta", options, 2);


		switch(r) {
			case '1':
				lista_viaggi(conn);
				break;
			case '2':
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
	}

}

static role_t attempt_login(MYSQL *conn, char *username, char *password) {
	MYSQL_STMT *login_procedure;

	MYSQL_BIND param[3]; // Used both for input and output
	int role = 0;

	if(!setup_prepared_stmt(&login_procedure, "call login(?, ?, ?)", conn)) {
		print_stmt_error(login_procedure, "Unable to initialize login statement\n");
		goto err2;
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING; // IN
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING; // IN
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);

	param[2].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[2].buffer = &role;
	param[2].buffer_length = sizeof(role);

	if (mysql_stmt_bind_param(login_procedure, param) != 0) { // Note _param
		print_stmt_error(login_procedure, "Could not bind parameters for login");
		goto err;
	}

	// Run procedure
	if (mysql_stmt_execute(login_procedure) != 0) {
		print_stmt_error(login_procedure, "Could not execute login procedure");
		goto err;
	}

	// Prepare output parameters
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[0].buffer = &role;
	param[0].buffer_length = sizeof(role);

	if(mysql_stmt_bind_result(login_procedure, param)) {
		print_stmt_error(login_procedure, "Could not retrieve output parameter");
		goto err;
	}

	// Retrieve output parameter
	if(mysql_stmt_fetch(login_procedure)) {
		print_stmt_error(login_procedure, "Could not buffer results");
		goto err;
	}

	mysql_stmt_close(login_procedure);
	return role;

    err:
	mysql_stmt_close(login_procedure);
    err2:
	return FAILED_LOGIN;
}

int main(void) {
	role_t role;


	if(!parse_config("users/login.json", &conf)) {
		fprintf(stderr, "Unable to load login configuration\n");
		exit(EXIT_FAILURE);
	}

	conn = mysql_init (NULL);
	if (conn == NULL) {
		fprintf (stderr, "mysql_init() failed (probably out of memory)\n");
		exit(EXIT_FAILURE);
	}

	if (mysql_real_connect(conn, conf.host, conf.db_username, conf.db_password, conf.database, conf.port, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL) {
		fprintf (stderr, "mysql_real_connect() failed\n");
		mysql_close (conn);
		exit(EXIT_FAILURE);
	}

	char options[2] = {'1','2'};
	char r;
	int guest;

	printf("\nscegliere se si vuole: \n1) accedere tramite username e password \n2) continuare come ospite");

	r = multiChoice("\nSeleziona una scelta", options, 2);


	switch(r) {
		case '1':
			guest = 0;
			break;
		case '2':
			guest = 1;
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	if(guest == 0){
		printf("Username: ");
		getInput(128, conf.username, false);
		printf("Password: ");
		getInput(128, conf.password, true);

		role = attempt_login(conn, conf.username, conf.password);


		switch(role) {


			case HOSTESS:
				run_as_hostess(conn);
				break;

			case AUTISTA:
				run_as_autista(conn);
				break;

			case PASSEGGERO:
				run_as_passeggero(conn);
				break;

			case MECCANICO:
				run_as_meccanico(conn);
				break;

			case AMMINISTRATORE:
				run_as_amministratore(conn);
				break;

			case FAILED_LOGIN:
				fprintf(stderr, "Invalid credentials\n");
				exit(EXIT_FAILURE);
				break;

			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}
	}
	else if(guest == 1){
		run_as_guest(conn);
	}
	printf("Bye!\n");

	mysql_close (conn);
	return 0;
}
