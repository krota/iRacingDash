//header file for yor own functions and definitions.//
//
//
//
//

/////////////////////////////////////////////////////////
void end_session(bool shutdown)
{
	//close_file(g_file, g_ttime);

	if(data)
		//delete[] data;
	data = NULL;

	if(shutdown)
	{
		irsdk_shutdown();
		timeEndPeriod(1);
	}
}

/////////////////////////////////////////////////////////

// exited with ctrl-c
void ex_program(int sig) 
{
	(void)sig;

	//printf("\n\nrecieved ctrl-c, exiting\n\n");

	//system("cls");
	//printf("mspec_shift 1.2\n");

	//
	end_session(true);

	printf("\nrecieved ctrl-c, exited\n\n");

	signal(SIGINT, SIG_DFL);
	exit(0);
}

/////////////////////////////////////////////////////////