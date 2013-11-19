#include <stdio.h>
#include <unistd.h>
#include <string.h>

enum HSMS_STATES {
  HSMS_NOT_ENABLED = 0,
  HSMS_TCP_NOT_CONNECTED,
  HSMS_NOT_SELECTED,
  HSMS_SELECTED
};

//int HSMS_Start(const unsigned short int port);
int HSMS_Start();
int HSMS_Stop();
int HSMS_Send(const char * s);
int HSMS_SendReply(const char * s, const unsigned int sb);
int HSMS_SendAndWait(const char * s, char * rs, int rsbuf);
int GetHSMSState();

void HSMS_Event(const char *s, unsigned int replyflag, unsigned int sb) {
  printf("************Receive ROMARIC HSMS_Event*****\n");
  printf("%s %u\n", s, sb);

  char str[] = "|MCS_SUCCESS|MCS Server Successfully|2|3MKZ48035.1|E00000001|L00000001|3MKZ48036.1|E00000001|L00000001|";
  //str = s;
  char delims[] = "|";
  char *result = NULL;
  result = strtok(str, delims);
  while (result != NULL) {
    printf("result is \"%s\"\n", result);
    result = strtok(NULL, delims);
  }

  printf("********Sending Message******\n");
  if (replyflag)         //should only send reply if replyflag is set
  {
    HSMS_SendReply("|MCS_GET_LOT_ALL|VMS MCS Client query|2|3MKZ48035.1|3MKZ48036.1", sb);
  }
  printf("***********************************\n");

}

int main(int argc, char **argv) {
  char * Str_MCS_Get_LotLoc = "|MCS_GET_LOT_loc|MCSMBXsrv query|3MKZ48035.1|";
  char * Str_MCS_SND_LotLoc = "|MCS_SEND_LOT_loc|MCSMBXsrv query|3MKZ48035.1|E0000001|L0000001|";
  char * Str_MCS_CLR_LotLoc = "|MCS_CLR_LOT_loc|MCSMBXsrv query|3MKZ48035.1|";
  char * Str_MCS_MOV_LotLoc = "|MCS_MOVE_LOT_loc|MCSMBXsrv query|HPTEST|3MKZ48035.1|E0000002|L0000002|1|";
  char * Str_MCS_GET_LotALL = "|MCS_GET_LOT_ALL|VMS MCS Client query|2|3MKZ48035.1|3MKZ48036.1|";

  char * Str_Event_Report = "CMD/A=EVENT_REPORT MID/A=HOST MTY/A=E ECD/U4=0 ETX/A= EVENT_ID/A=LOC_CHANGED";
  char SrvCmd[32000] = { 0 };
  char messageCmd[32000] = { 0 };
  char receivedMsg[256];
  int x = 10;
  char command[32000] = { 0 };
  HSMS_Start(8001);

  //while (--x) {
  while (GetHSMSState() != HSMS_SELECTED) {
    printf("HSMS is not connected,retry again...%d\n", x--);
    sleep(2);
  }
  printf("Connect successfully,HSMS State= %d\n", GetHSMSState());
  printf("\n\nplease Choose Mode:[S]YNC/[U]NSYNC/[E]XIT:");
  do {
    scanf("%[^\n]s", SrvCmd);
    fflush (stdin);
    getchar();

    if (strcmp(SrvCmd, "EXIT") == 0 || strcmp(SrvCmd, "E") == 0 || strcmp(SrvCmd, "e") == 0) {

      break;
    } else {
      if (strcmp(SrvCmd, "UNSYNC") == 0 || strcmp(SrvCmd, "U") == 0 || strcmp(SrvCmd, "u") == 0
          || strcmp(SrvCmd, "SYNC") == 0 || strcmp(SrvCmd, "S") == 0 || strcmp(SrvCmd, "s") == 0) {

        printf("you have chosen %s mode,please input delimited or VFEI message...\n", SrvCmd);

        scanf("%[^\n]s", messageCmd);
        fflush(stdin);
        getchar();

        if (strcmp(SrvCmd, "SYNC") == 0 || strcmp(SrvCmd, "S") == 0 || strcmp(SrvCmd, "s") == 0) {

          if (HSMS_SendAndWait(messageCmd, receivedMsg, 256)) {
            printf("\nReplied string from HSMS:\n%s\n", receivedMsg);
          }

        } else if (strcmp(SrvCmd, "UNSYNC") == 0 || strcmp(SrvCmd, "U") == 0 || strcmp(SrvCmd, "u") == 0) {
          HSMS_Send(messageCmd);
        }
      }
    }

    /*if (strcmp(SrvCmd, "GET_LOT_LOC") == 0 || strcmp(SrvCmd, "GET_LOT_ALL") == 0) {
     if (strcmp(SrvCmd, "GET_LOT_LOC") == 0) {
     strncpy(command, Str_MCS_Get_LotLoc, strlen(Str_MCS_Get_LotLoc));

     }
     if (strcmp(SrvCmd, "GET_LOT_ALL") == 0) {
     strncpy(command, Str_MCS_GET_LotALL, strlen(Str_MCS_GET_LotALL));

     }
     printf("Command is %s,sending message: %s\n", SrvCmd, command);
     if (HSMS_SendAndWait(command, receivedMsg, 256)) {
     printf("Got string reply\n%s\n", receivedMsg);
     }

     } else {
     if (strcmp(SrvCmd, "SEND_LOT_LOC") == 0) {
     strncpy(command, Str_MCS_SND_LotLoc, strlen(Str_MCS_SND_LotLoc));
     }

     if (strcmp(SrvCmd, "CLR_LOT_LOC") == 0) {
     strncpy(command, Str_MCS_CLR_LotLoc, strlen(Str_MCS_CLR_LotLoc));
     }

     if (strcmp(SrvCmd, "MOVE_LOT_LOC") == 0) {
     strncpy(command, Str_MCS_MOV_LotLoc, strlen(Str_MCS_MOV_LotLoc));
     }
     printf("Command is %s,sending message: %s\n", SrvCmd, command);
     HSMS_Send(command);
     }*/

    printf("\n\n please Choose Mode:[S]YNC/[U]NSYNC/[E]XIT:");

  } while (1);

  sleep(2);
  HSMS_Stop();

  printf("\nExit the test program successfully.");
  return 0;
}
