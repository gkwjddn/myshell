#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h> //hostname과 username을 알아내기 위해 필요한 헤더파일
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN_LINE    10
#define LEN_HOSTNAME    100 //hostname의 글자수를 지정한다. 예로 hostname이 DESKTOP일 경우 LEN_HOSTNAME을 1로 지정해주면 hostname은 D가 된다.

int main(void)
{
    char command[MAX_LEN_LINE];
    char *args[] = {command, NULL};
    int ret, status;
    pid_t pid, cpid;

    
    //추가구현기능 1: 사용자 id로 사용자 정보를 구하기
    uid_t   user_id;
    struct passwd *user_pw;
    user_id = getuid();
    user_pw = getpwuid(user_id);
    printf( "user name      :%s\n", user_pw->pw_name  );
    printf( "user id        :%d\n", user_pw->pw_uid   );
    printf( "group id       :%d\n", user_pw->pw_gid   );
    printf( "reall name     :%s\n", user_pw->pw_gecos );
    printf( "home directory :%s\n", user_pw->pw_dir   );
    printf( "shell program  :%s\n", user_pw->pw_shell );

    //처음으로 hostname과 username을 받아서 보여주는 코드
    char hostname[LEN_HOSTNAME + 1];
    memset(hostname, 0x00, sizeof(hostname));
    gethostname(hostname, LEN_HOSTNAME);
    printf("username은 %s이고 ", getpwuid(getuid())->pw_name);
    printf("hostname은 %s입니다.\n", hostname);

    while (true) {
        char *s;
        int len;
        int num=0;
        char cmd[10][10];

        //필수구현요소 3: username@hostname $ 형태로 쉘 기본입력창을 띄운다.
        printf("\n%s@%s $ ", getpwuid(getuid())->pw_name, hostname);
        //s에 input을 받으며 input이 없으면 이 코드는 더이상 루프를 돌지 않고 input을 계속 기다린다.
        s = fgets(command, MAX_LEN_LINE, stdin);

        //필수구현요소 1: exit을 입력받으면 내가 구현한 쉘은 종료된다.
        if(strcmp(s, "exit\n") == 0)
        {
            printf("exit을 입력받아 Shell을 종료하겠습니다. \n");
            break;
        }

        //필수구현요소 2: a; b; c 와 같이 입력시 a, b, c 세개의 프로그램 차례대로 실행
        for(int i=0; i<len; i++)
        {
            if(s[i] ==';')
                num++;
        }
        printf("%d개의 프로그램을 실행하겠습니다.\n", num+1);
        num=-0;
        char *ptr = strtok(s, "; ");
        while(ptr != NULL)
        {
            printf(" %s \n", ptr);
            strcpy(cmd[num], ptr);
            num++;
            ptr = strtok(NULL, "; ");
        }
        for(int i=0; i<num; i++)
        {
            s = cmd[i];
            strcpy(command, s);
            len = strlen(command);
        }


        //추가기능구현 2: help를 입력받으면 구현한 기능들을 어떻게 사용하는지 알려줍니다. 
        if(strcmp(s, "help\n") == 0)
        {
            printf("\n쉘을 만드는 프로젝트입니다.\n");
            printf("exit을 입력하시면 쉘이 종료됩니다.\n");
            printf("여러 파일을 입력하시려면 a; b; c 식으로 입력해주세요.\n");
            printf("자동차IT융합학과 20153440 하정우\n");
            continue;
        }


        
        if (s == NULL) {
            fprintf(stderr, "fgets failed\n");
            exit(1);
        }
        
        len = strlen(command);
        printf("%d\n", len);
        if (command[len - 1] == '\n') {
            command[len - 1] = '\0'; 
        }
        
        printf("[%s]\n", command);
      
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } 
        if (pid != 0) {  /* parent */
            cpid = waitpid(pid, &status, 0);
            if (cpid != pid) {
                fprintf(stderr, "waitpid failed\n");        
            }
            printf("Child process terminated\n");
            if (WIFEXITED(status)) {
                printf("Exit status is %d\n", WEXITSTATUS(status)); 
            }
        }
        else {  /* child */
            ret = execve(args[0], args, NULL);
            if (ret < 0) {
                fprintf(stderr, "execve failed\n");   
                return 1;
            }
        } 
    }
    return 0;
}
