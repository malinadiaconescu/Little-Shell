#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include <stdbool.h>
#include<readline/history.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include <dirent.h>


#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
void clear()
{
	printf("\033[H\033[J");
}
void director()
{
	char direc[100000];
	getcwd(direc,sizeof(direc));
	printf("\n %s \n",direc);
}
bool input(char* a)
{
	char* comanda;
	comanda=readline(">>>");
	if(strlen(comanda))
	{
	strcpy(a,comanda);add_history(comanda);
	return 1;
	}
	else return 0;
}
//functie care sa delimiteze
int multiple_commands(char* comanda,char** comenzi)
{
	for(int contor=0;contor<2;contor++)
	{
		comenzi[contor]=strsep(&comanda,"|");
		if(comenzi[contor]==NULL) break;
	}
	if(comenzi[1]) return 1;//mai multe comenzi
	else return 0;//una
}
void spatii(char *comanda,char**comanda_gata)
{
	for(int contor=0;contor<101;contor++)
	{
		comanda_gata[contor]=strsep(&comanda," ");
		if(comanda_gata[contor]==NULL) break;
		if(strlen(comanda_gata[contor])==0) contor--;
	}
}
void exec1(char**comenzi)
{
	int i=0;
	pid_t pid=fork();
	if(pid==-1)
	{
		printf("\n Failed forking child :( \n");
		return;
	}
	else if(pid==0)
	{
		
		if(execvp(comenzi[0],comenzi)<0)
			printf("\n Failed to execute command :( \n");
		//else printf("\n Command executed succesfully :) \n");
		exit(0);
	}
	else 
	{
		wait(NULL);
		return;
	}
}
void exec2(char**comanda1,char**comanda2)
{
	/*int i=0;
	while(comanda1[i]!=NULL)
	{
		printf("%s \n",comanda1[i++]);
	}
	i=0;
	while(comanda2[i]!=NULL)
	{
		printf("%s \n",comanda2[i++]);
	}*/
	int fd[2];
	pid_t proces1,proces2;
	if(pipe(fd)<0)
	{
		printf("\n Failed to initialize the pipe :(");
		return;
	}
	proces1=fork();
	if(proces1<0)
	{
		printf("\n Failed to fork :( \n");
		return;
	}
	if(proces1==0)
	{
	//prima comanda se executa
		close(fd[0]);
	//scriu rezultatul pt a fi folosit la cealalta
		dup2(fd[1],STDOUT_FILENO);
		close(fd[1]);
		
		if(execvp(comanda1[0],comanda1)<0)
		{
			printf("\n Failed to execute command 1 :( \n");
			exit(0);
		}
	}
	else
	{	
		proces2=fork();
		if(proces2<0)
		{
			printf("\n Failed to fork :( \n");
			return;
		}
		if(proces2==0)
		{
		//execut a doua comanda
			close(fd[1]);
		//trebuie citit rezultatul
			dup2(fd[0],STDIN_FILENO);
			close(fd[0]);
			
			/*int i=0;
			while(strlen(comanda2[i]))
			printf("%s \n",comanda2[i++]);*/
			if(execvp(comanda2[0],comanda2)<0)
			{
				printf("\n Failed to execute command 2 :( \n");
				exit(0);
			}
			else exit(1);
			
		}
		else
		{
			close(fd[1]);
			wait(NULL);
			wait(NULL);
		}
	}
}
/*void procesare2(char** comenzi_gata,char** comenzi1,char **comenzi2)
{
	spatii(comenzi_gata[0],comenzi1);
	spatii(comenzi_gata[1],comenzi2);
}
int procesare(char*comanda,char**comenzi_gata)
{
	spatii(comanda,comenzi_gata);
} */
bool redirection(char*comanda_curenta,char*file)
{
	if(strchr(comanda_curenta,'>')) 
	{
	//printf("%s",strchr(comanda_curenta,'>')+1);
	strcpy(file,strchr(comanda_curenta,'>')+1);
	int i=0;
	comanda_curenta[strchr(comanda_curenta,'>')-comanda_curenta]='\0';
	while(i<strlen(file))
	{
		if(file[i]==' ')
		{for(int j=i;j<strlen(file);j++)
		file[j]=file[j+1];}
		else i++;
	}
	printf("%s",file);
	return 1;
	}
	else return 0;
}
int main()
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//SERVER
	//system("./lol &");
	char comanda_curenta[101];
	clear();
	printf("\n~~~~~~~~~~~~~~~~~MALINA's SHELL~~~~~~~~~~~~~~~~~\n");
	sleep(1);
	clear();
	while(1)
	{
	director();
	freopen ("/dev/tty", "a", stdout);
	if(input(comanda_curenta))
	{
		char file[101];
		if(redirection(comanda_curenta,file)==1)
		freopen(file,"w+",stdout);
		if(strcmp(comanda_curenta,"exit")==0)
		{
			printf("\n Thank you for playing with my shell! Goodbye! :) \n");
			exit(0);
		}
		else if(strcmp(comanda_curenta,"vs")==0)
		{
			printf("Malina's shell version 1.0");
		}
		else if(strcmp(comanda_curenta,"help")==0)
		{
			printf("\n Help! \n");
			printf("cd - move through directories; classic cd \n");
			printf("vs - shows the version of the application \n");
			printf("nl - implementation of nl; parameters known are -s, -d \n");
			printf("mv - implementation of mv; parameters known are -i, -t, -S \n \n");
			printf("Other things that Malina's Shell knows :) \n");
			printf("Support for pipe commands \n");
			printf("Output redirection \n");
			printf("Has a server running in the background to which you can connect \n");
			
		}
		else
		{
			char *tabel_comenzi[101];
			//redirection(comanda_curenta,file);
			int number_commands=multiple_commands(comanda_curenta,tabel_comenzi);	
			if(number_commands==0)
			{
				char*comenzi_gata[101];
				spatii(comanda_curenta,comenzi_gata);
				//procesare(comanda_curenta,comenzi_gata);
				if(strcmp(comenzi_gata[0],"cd")==0)
				{
				int parti=0;
				while(comenzi_gata[parti]!=NULL) parti++;
					parti--;
				if(parti==1)
				{if(chdir(comenzi_gata[1])<0) printf("\n Error :(  \n");}
				else chdir("/home/malina");
				}
				else if(strcmp(comenzi_gata[0],"nl")==0)
				{
					//printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
					int parti=0;
					while(comenzi_gata[parti]!=NULL) parti++;
					parti--;
					if(parti==3)
					{if(strcmp(comenzi_gata[1],"-s")==0)
					{
					FILE *f;
					f=fopen(comenzi_gata[3],"r");
					char*line;
					size_t len=0;
					ssize_t read;
					int contor=0;
					while((read=getline(&line,&len,f))!=-1)
					{
					contor++;
					printf("%d. %s%s",contor,comenzi_gata[2],line);
					}
					printf("%d lines detected \n",contor);
					}
					else if(strcmp(comenzi_gata[1],"-d")==0)
					{
					
					FILE *f;
					f=fopen(comenzi_gata[3],"r");
					char*line;
					size_t len=0;
					ssize_t read;
					int contor=0;
					
					if(strlen(comenzi_gata[2])==4)
					{
					comenzi_gata[2][0]=comenzi_gata[2][1];
					comenzi_gata[2][1]=comenzi_gata[2][2];
					comenzi_gata[2][2]='\0';
					//printf("%s",comenzi_gata[2]);
					while((read=getline(&line,&len,f))!=-1)
					{
					contor++;
					int i=0;
					printf("%d. ",contor);
					while(line[i])
					{
					 if(line[i]==comenzi_gata[2][0]&&line[i+1]==comenzi_gata[2][1])
					 {printf("page break!");i++;}
					 else printf("%c",line[i]);
					 i++;
					}
					printf("\n");
					}
					}else printf("\n Error :( \n");
					}
					else printf("\n Could not execute command :( \n");
					}
					else if(parti==1)
					{
					FILE *f;
					f=fopen(comenzi_gata[1],"r");
					char*line;
					size_t len=0;
					ssize_t read;
					int contor=0;
					while((read=getline(&line,&len,f))!=-1)
					{
					contor++;
					//printf("%d. %s%s",contor,comenzi_gata[2],line);
					}
					printf("%d lines detected \n",contor);
					}
					else printf("\n Could not execute command :( \n");
					
				}
				else if(strcmp(comenzi_gata[0],"mv")==0)
				{
					int parti=0;
					while(comenzi_gata[parti]!=NULL) parti++;
					parti--;
					if(strcmp(comenzi_gata[1],"-i")==0&&parti==3)
					{
					FILE *f;
					f=fopen(comenzi_gata[2],"r");
					char*line;
					size_t len=0;
					ssize_t read;
					bool ok=0;
					char *raspuns;
					bool executa=0;
					raspuns=readline("Do you want to do this? y/n ");
					if(strcmp(raspuns,"y")==0)
					{
						ok=1;executa=1;
					}
					else if(strcmp(raspuns,"n")==0)
					{
						ok=0;
					}
					if(executa==1)
					{
					
					freopen(comenzi_gata[3],"w+",stdout);
					while((read=getline(&line,&len,f))!=-1)
					{
						printf("%s \n", line);
					}
					remove(comenzi_gata[2]);
					freopen ("/dev/tty", "a", stdout);
					}
					}
					else if(strcmp(comenzi_gata[1],"-t")==0&&parti==3)
					{
					//printf("AAAAAAAAAAAAAAAAAAAAAAA");
					//rename(comenzi_gata[3],comenzi_gata[2]);
					char comanda[200];
					strcpy(comanda,"mv ");
					strcat(comanda,comenzi_gata[3]);
					//strcat(comanda,"\\*\" \"");
					strcat(comanda," ");
					strcat(comanda,comenzi_gata[2]);
					strcat(comanda,"\0");
					//strcat(comanda,"\" ");
					system(comanda);
					}
					else if(strcmp(comenzi_gata[1],"-S")==0&&parti==2)
					{
					char file[101];char help[2]="~";
					char*line;
					size_t len=0;
					ssize_t read;
					strcpy(file,help);
					strcat(file,comenzi_gata[2]);
					printf("%s",file);
					FILE *f=fopen(file,"w");
					FILE *reading=fopen(comenzi_gata[2],"r");
					while((read=getline(&line,&len,reading))!=-1)
					{
						fprintf(f,"%s \n", line);
					}
					fclose(f);
					fclose(reading);
					}
					else if(strcmp(comenzi_gata[1],"-b")==0&&parti==2)
					{
					DIR *d;
					struct dirent* dir;
					char direc[100000];
					getcwd(direc,sizeof(direc));
					d=opendir(direc);
					if(d)
					{
						while((dir=readdir(d))!=NULL)
						{
							if(dir->d_type==DT_REG)
							{
							char file[101];char help[2]="~";
							char*line;
							size_t len=0;
							ssize_t read;
							strcpy(file,help);
							strcat(file,dir->d_name);
							printf("%s",file);
							FILE *f=fopen(file,"w");
							FILE *reading=fopen(dir->d_name,"r");
							while((read=getline(&line,&len,reading))!=-1)
							{
								fprintf(f,"%s \n", line);
							}
							fclose(f);
							fclose(reading);
							}
						}
					}
					
					}
					else printf("\n Could not execute command :( \n");
				}
				else exec1(comenzi_gata);
			}
			else if(number_commands==1)
			{
				//printf("%s","saluuuut");
				char*comanda1[101],*comanda2[101];
				spatii(tabel_comenzi[0],comanda1);
				spatii(tabel_comenzi[1],comanda2);
				//procesare2(tabel_comenzi,comanda1,comanda2);
				//int i=0;
				/*while(comanda1[i]!=NULL)
				{
				printf("%s \n",comanda1[i++]);
				}
				i=0;
				while(comanda2[i]!=NULL)
				{
				printf("%s \n",comanda2[i++]);
				}*/				
				exec2(comanda1,comanda2);
			}
		}
		//fclose(stdout);		
	}
	}
		
}
