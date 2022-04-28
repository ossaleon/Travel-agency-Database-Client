#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"


static void ordina_ricambio(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];

	// Input for ordina_ricambio routine
	char codice[6];



	printf("\ncodice ricambio: ");
	getInput(6, codice, false);

	if(!setup_prepared_stmt(&prepared_stmt, "call ordina_ricambio(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize ordina_ricambio statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = codice;
	param[0].buffer_length = strlen(codice);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for ordina_ricambio\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching counter value.");
		goto out;
	}

	printf("\n pezzo di ricambio ordinato correttamente");

  out:
	mysql_stmt_close(prepared_stmt);
}


static void utilizza_ricambio(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;

	if(!setup_prepared_stmt(&prepared_stmt, "call ricambi_disponibili()", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize ricambi_disponibili statement\n", false);
	}

	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searchin available spare .");
		mysql_stmt_close(prepared_stmt);
		return;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito i pezzi di ricambio disponibili:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

	mysql_stmt_close(prepared_stmt);

	char options[2] = {'1','2'};
	char r;
	int disp;

	printf("\n il pezzo di tuo interesse è disponibile?: \n1) si \n2) no");

	r = multiChoice("\n scegliere un opzione", options, 2);

	switch(r) {
		case '1':
			disp = 1;
			break;
		case '2':
			disp = 0;
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	if(disp == 1){



		// Input for counter routine
		MYSQL_BIND param[3];
		char pullman[8];
		char codice[6];
		MYSQL_TIME inizio;
		int n;
		char temp[10];



		printf("\ninserire targa pullman: ");
		getInput(8, pullman, false);
		printf("\n inserire codice del pezzo: ");
		getInput(6, codice, false);

		memset(&inizio, 0, sizeof(inizio));

		G:
		printf("\ninserire il giorno di inizio, da 1 a 31: ");
		getInput(10, temp, false);
		n = atoi(temp);
		if(n < 1 || n > 31){
			printf("\ngiorno non valido, riprovare: ");
			goto G;
		}

		inizio.day = n;

		M:
		printf("\ninserire il mese di inizio, da 1 a 12: ");
		getInput(10, temp, false);
		n = atoi(temp);
		if(n < 1 || n > 12){
			printf("\nmese non valido, riprovare: ");
			goto M;
		}

		inizio.month = n;

		A:
		printf("\ninserire l'anno di inizio, da 2000 a 2022: ");
		getInput(10, temp, false);
		n = atoi(temp);
		if(n < 2000 || n > 2022){
			printf("\nanno non valido, riprovare: ");
			goto A;
		}

		inizio.year = n;



		if(!setup_prepared_stmt(&prepared_stmt, "call usa_ricambio(?,?,?)", conn)) {
			finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize usa_ricambio statement\n", false);
		}

	// Prepare parameters
		memset(param, 0, sizeof(param));

		param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[0].buffer = codice;
		param[0].buffer_length = strlen(codice);

		param[1].buffer_type = MYSQL_TYPE_DATE;
		param[1].buffer = &inizio;
		param[1].buffer_length = sizeof(inizio);

		param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[2].buffer = pullman;
		param[2].buffer_length = strlen(pullman);

		if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
			finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for usa_ricambio\n", true);
		}

		// Run procedure
		if (mysql_stmt_execute(prepared_stmt) != 0) {
			print_stmt_error (prepared_stmt, "An error occurred while changing spare.");
			goto out;
		}

		printf("\n pezzo di ricambio utilizzato correttamente");

		out:
		mysql_stmt_close(prepared_stmt);
	}
	else{
		printf("\n è possiile ordinare i pezzi dalla schermata iniziale");
	}
}


static void counter(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];

	// Input for counter routine
	char pullman[8];



	printf("\ntarga pullman: ");
	getInput(8, pullman, false);

	if(!setup_prepared_stmt(&prepared_stmt, "call lettura_contatore(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize counter statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = pullman;
	param[0].buffer_length = strlen(pullman);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for counter\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching counter value.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito il valore del contatore:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}


static void next_inspection(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[2];

	// Input for counter routine
	char pullman[8];
	char options[8] = {'1','2','3','4','5','6','7','8'};
	char *controlli[8] = {"motore", "convergenza", "freni", "pneumatici", "luci", "batteria", "sospensioni", "carrozzeria"};
  char *tipo;
  char r;

	printf("\ntarga pullman: ");
	getInput(8, pullman, false);

	printf("\ninserire un numero da 1 a 8 per scegliere il controllo ordinario di interesse: \n1)motore \n2)convergenza \n3)freni \n4)pneumatici \n5)luci \n6)batteria \n7)sospensioni \n8)carrozzeria ");

	r = multiChoice("\nSeleziona un controllo", options, 8);

	// Salvo il giorno coerentemente a come è salvato sul DB
	switch(r) {
		case '1':
			tipo=controlli[0];
			break;
		case '2':
			tipo=controlli[1];
			break;
		case '3':
			tipo=controlli[2];
			break;
		case '4':
			tipo=controlli[3];
			break;
		case '5':
			tipo=controlli[4];
			break;
		case '6':
			tipo=controlli[5];
			break;
		case '7':
			tipo=controlli[6];
			break;
		case '8':
			tipo=controlli[7];
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	if(!setup_prepared_stmt(&prepared_stmt, "call prossima_revisione(?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize next_inspection statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = pullman;
	param[0].buffer_length = strlen(pullman);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = tipo;
	param[1].buffer_length = strlen(tipo);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for next_inspection\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching next inspection.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito a quanti chilometri il pullamn dovrà ripetere il controllo:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}


static void update_next_inspection(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;


	if(!setup_prepared_stmt(&prepared_stmt, "call revisioni_da_effettuare()", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize revisioni_da_effettuare statement\n", false);
	}

	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searchin next revisions .");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito la lista dei controlli da effettuare:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);


	MYSQL_BIND param[13];
  bool is_null = 1;

	// Input for next_inspection routine
	char pullman[8];
	char motivo[1000];
	int n;
	char options_b[2] = {'1','2'};
	char r;
	int nulls[8] = {1,1,1,1,1,1,1,1};
	MYSQL_TIME inizio;
	MYSQL_TIME fine;
	char temp[10];
	param[2].is_null = &is_null;


	printf("\ntarga pullman: ");
	getInput(8, pullman, false);


	printf("\ndigitare 1 se un controllo ordinario oppure 2 se straordinario, poi premere invio: ");

	r = multiChoice("digitare 1 o 2", options_b, 2);

	switch(r) {
		case '1':
			n=1;
			break;
		case '2':
			n=2;
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	int motivo_bool = 0;

	if(n == 1){

		motivo_bool = 1;

		char options_c[8] = {'1','2','3','4','5','6','7','8'};




		printf("\ndigitare il numero di controlli che si vogliono effettuare: ");
		getInput(1, temp, false);
		n = atoi(temp);




		printf("\nscegliere %d controlli digitando il numero e premendo invio, uno per volta: ", n);

		int i;

		for(i = 0; i < n; i++) {
			printf("\ninserire un numero da 1 a 8 per scegliere il controllo ordinario di interesse: \n1)motore \n2)convergenza \n3)freni \n4)pneumatici \n5)luci \n6)batteria \n7)sospensioni \n8)carrozzeria ");


			r = multiChoice("Seleziona un controllo", options_c, 8);


			switch(r) {
				case '1':
					if(nulls[0] == 0){
						printf("\ncontrollo gia selezionato, si prega di selezionarne un altro");
						i = i - 1;
					}
					else{
						nulls[0] = 0;
					}
					break;
				case '2':
					if(nulls[1] == 0){
						printf("\ncontrollo gia selezionato, si prega di selezionarne un altro");
						i = i - 1;
					}
					else{
						nulls[1] = 0;
					}
					break;
				case '3':
					if(nulls[2] == 0){
						printf("\ncontrollo gia selezionato, si prega di selezionarne un altro");
						i = i - 1;
					}
					else{
						nulls[2] = 0;
					}
					break;
				case '4':
					if(nulls[3] == 0){
						printf("\ncontrollo gia selezionato, si prega di selezionarne un altro");
						i = i - 1;
					}
					else{
						nulls[3] = 0;
					}
					break;
				case '5':
					if(nulls[4] == 0){
						printf("\ncontrollo gia selezionato, si prega di selezionarne un altro");
						i = i - 1;
					}
					else{
						nulls[4] = 0;
					}
					break;
				case '6':
					if(nulls[5] == 0){
						printf("\ncontrollo gia selezionato, si prega di selezionarne un altro");
						i = i - 1;
					}
					else{
						nulls[5] = 0;
					}
					break;
				case '7':
					if(nulls[6] == 0){
						printf("\ncontrollo gia selezionato, si prega di selezionarne un altro");
						i = i - 1;
					}
					else{
						nulls[6] = 0;
					}
					break;
				case '8':
					if(nulls[7] == 0){
						printf("\ncontrollo gia selezionato, si prega di selezionarne un altro");
						i = i - 1;
					}
					else{
						nulls[7] = 0;
					}
					break;
				default:
					fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
					abort();
			}
		}
	}
	else{
		printf("\ninserire il motivo ella revisione straordinaria: ");
		getInput(1000, motivo, false);

	}


	memset(&inizio, 0, sizeof(inizio));

	G:
	printf("\ninserire il giorno di inizio, da 1 a 31: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 1 || n > 31){
		printf("\ngiorno non valido, riprovare: ");
		goto G;
	}

	inizio.day = n;

	M:
	printf("\ninserire il mese di inizio, da 1 a 12: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 1 || n > 12){
		printf("\nmese non valido, riprovare: ");
		goto M;
	}

	inizio.month = n;

	A:
	printf("\ninserire l'anno di inizio, da 2000 a 2022: ");
	getInput(10, temp, false);
	n = atoi(temp);
	if(n < 2000 || n > 2022){
		printf("\nanno non valido, riprovare: ");
		goto A;
	}

	inizio.year = n;

	int fine_bool;

	printf("\ndigitare  1 se si vuole inserire la data di fine, 2 se la revisione è in corso");

	r = multiChoice("selezionare una delle op", options_b, 2);

	switch(r) {
		case '1':
			fine_bool = 0;
			break;
		case '2':
			fine_bool = 1;
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	if(fine_bool == 0){

		memset(&fine, 0, sizeof(fine));

		Gg:
		printf("\ninserire il giorno di fine, da 1 a 31: ");
		getInput(10, temp, false);
		n = atoi(temp);
		if(n < 1 || n > 31){
			printf("\ngiorno non valido, riprovare: ");
			goto Gg;
		}

		fine.day = n;

		Mm:
		printf("\ninserire il mese di fine, da 1 a 12: ");
		getInput(10, temp, false);
		n = atoi(temp);
		if(n < 1 || n > 12){
			printf("\nmese non valido, riprovare: ");
			goto Mm;
		}

		fine.month = n;

		Aa:
		printf("\ninserire l'anno di fine, da 2000 a 2022: ");
		getInput(10, temp, false);
		n = atoi(temp);
		if(n < 2000 || n > 2022){
			printf("\nanno non valido, riprovare: ");
			goto Aa;
		}

		fine.year = n;
	}

	if(!setup_prepared_stmt(&prepared_stmt, "call inserisci_sessione_controllo(?,?,?,?,?,?,?,?,?,?,?,?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize update_next_inspection statement\n", false);
	}

		// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = pullman;
	param[0].buffer_length = strlen(pullman);

	param[1].buffer_type = MYSQL_TYPE_DATE;
	param[1].buffer = &inizio;
	param[1].buffer_length = sizeof(inizio);


	param[2].buffer_type = MYSQL_TYPE_DATE;
	param[2].buffer = &fine;
	param[2].buffer_length = sizeof(fine);
	if(fine_bool == 1){
		param[2].is_null = &is_null;
	}


	param[3].buffer_type = MYSQL_TYPE_STRING;
	param[3].buffer = motivo;
	param[3].buffer_length = strlen(motivo);
	if(motivo_bool == 1){
		param[3].is_null = &is_null;
	}

	param[4].buffer_type = MYSQL_TYPE_LONG;
	param[4].buffer = &nulls[0];
	param[4].buffer_length = sizeof(nulls[0]);

	param[5].buffer_type = MYSQL_TYPE_LONG;
	param[5].buffer = &nulls[1];
	param[5].buffer_length = sizeof(nulls[1]);

	param[6].buffer_type = MYSQL_TYPE_LONG;
	param[6].buffer = &nulls[2];
	param[6].buffer_length = sizeof(nulls[2]);

	param[7].buffer_type = MYSQL_TYPE_LONG;
	param[7].buffer = &nulls[3];
	param[7].buffer_length = sizeof(nulls[3]);

	param[8].buffer_type = MYSQL_TYPE_LONG;
	param[8].buffer = &nulls[4];
	param[8].buffer_length = sizeof(nulls[4]);

	param[9].buffer_type = MYSQL_TYPE_LONG;
	param[9].buffer = &nulls[5];
	param[9].buffer_length = sizeof(nulls[5]);

	param[10].buffer_type = MYSQL_TYPE_LONG;
	param[10].buffer = &nulls[6];
	param[10].buffer_length = sizeof(nulls[6]);

	param[11].buffer_type = MYSQL_TYPE_LONG;
	param[11].buffer = &nulls[7];
	param[11].buffer_length = sizeof(nulls[7]);

	param[12].buffer_type = MYSQL_TYPE_STRING;
	param[12].buffer = conf.username;
	param[12].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for next_inspection\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while inserting new inspection session.");
		goto out_2;
	}

	printf("revisione inserita correttamente\n");

	out_2:
	mysql_stmt_close(prepared_stmt);

}


static void revisioni(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;




	if(!setup_prepared_stmt(&prepared_stmt, "call revisioni_da_effettuare()", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize revisioni_da_effettuare statement\n", false);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while searching inspections.");
		goto out;
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nDi seguito le rervisoni da effettuare:");


	//per consumare il result set
	if (mysql_stmt_next_result(prepared_stmt) > 0){
		finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	}

  out:
	mysql_stmt_close(prepared_stmt);
}




void run_as_meccanico(MYSQL *conn)
{
	char options[7] = {'1','2','3','4','5','6','7'};
	char op;

	printf("Switching to meccanico role...\n");

	if(!parse_config("users/meccanico.json", &conf)) {
		fprintf(stderr, "Unable to load meccanico configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	while(true) {
		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) ricerca contatore pullman \n");
		printf("2) ricerca prossima revisione di un certo tipo per un pullman \n");
		printf("3) effettua una revisione \n");
		printf("4) utilizza un pezzo di ricambio \n");
		printf("5) ordina ricambio \n");
		printf("6) visualizza le revisioni che bisogna effettuare \n");
		printf("7) Quit\n");

		op = multiChoice("Select an option", options, 7);

		switch(op) {
			case '1':
				counter(conn);
				break;

			case '2':
				next_inspection(conn);
				break;

			case '3':
				update_next_inspection(conn);
				break;

			case '4':
				utilizza_ricambio(conn);
				break;

			case '5':
				ordina_ricambio(conn);
				break;

			case '6':
				revisioni(conn);
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
