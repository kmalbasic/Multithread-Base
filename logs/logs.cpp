#include "logs.hpp"


void logs::lsavef(char* txt, log_type type) {

	std::string title;

	// conversion, very ghetto

	std::string txts;
	
	switch (type) {

	case DEBUG_LOG:
		title = title + "[debug] ";
		txts = txts + txt;
		break;
	case ERROR_LOG:
		title = title + "[error] ";
		txts = txts + txt;
		break;
	case DEFAULT_LOG:
		title = title + "[log] ";
		txts = txts + txt;
		break;
	case START_LOG:
		title = title + "[startup] ";
		txts = txts + txt;
		break;
	case END_LOG:
		title = title + "[end] ";
		txts = txts + txt;
		break;

	}

	// also kinda ghetto ngl

	std::string str_cc = title + txts;
	const char* log_print = str_cc.c_str();


	FILE* log_file;

	time_t t = time(0);   // get time now
	struct tm* now = localtime(&t);

	char date_buffer[80];
	strftime(date_buffer, 80, "%Y-%m-%d.", now);

	log_file = fopen(date_buffer, "a"); // we will just append to the file

	if (log_file == NULL) {
		MessageBox(0, "There was an error with your log file. It's most \n likely not present so we will create it for \n you.", "Log exception", MB_OK); // sexy
	}

	// or else..?

	if (log_file)
	{
		fputs(log_print,log_file);
		fputc('\n', log_file);
	}

	fclose(log_file);
}


void logs::popup(char* txt, log_type type) {

	
	char prefix[32];

	// conversion, very ghetto

	char txt_c[1024]; // maybe a tad too much
	strcpy(txt_c, txt);
	switch (type) {

	case DEBUG_LOG:
		strcpy(prefix, "[debug] ");
		MessageBox(0, txt_c, prefix, MB_OK);  // in case you use unicode, there will be an issue with Messagebox requiring you to pass LPCWSTR, either convert
		break;                                // it like I did in memory functions header or simply add an L before your string. 
	case ERROR_LOG:
		strcpy(prefix, "[error] ");
		MessageBox(0, txt_c, prefix, MB_OK);
		break;
	case DEFAULT_LOG:
		strcpy(prefix, "[log] ");
		MessageBox(0, txt_c, prefix, MB_OK);
		break;
	case START_LOG:
		strcpy(prefix, "[startup] ");
		MessageBox(0, txt_c, prefix, MB_OK);
		break;
	case END_LOG:
		strcpy(prefix, "[end] ");
		MessageBox(0, txt_c, prefix, MB_OK);
		break;

	}

}
