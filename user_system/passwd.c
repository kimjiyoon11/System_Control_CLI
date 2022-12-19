#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<ctype.h>

int verify(char *passwd);
int lenCheck(char *passwd);
int strcheck(char *passwd);
int cmpId(char *passwd);
int cmpOrgPw(char *passwd);
int isspecialchar(char x);
int sameChar(char *passwd);
int numCheck(char *passwd);

int qwertyCheck(char *passwd);

int qwerCheck(char *passwd);
int QWERCheck(char *passwd);
int descqwerCheck(char *passwd);
int descQWERCheck(char *passwd);

int asdfCheck(char *passwd);
int ASDFCheck(char *passwd);
int descasdfCheck(char *passwd);
int descASDFCheck(char *passwd);

int zxcvCheck(char *passwd);
int ZXCVCheck(char *passwd);
int desczxcvCheck(char *passwd);
int descZXCVCheck(char *passwd);

char ID[1024] = "IDtest1346!";
char PW[1024] = "Test1346()!";

int main(){
	
	char passwd[1024];
	char passwd1[1024];
	
	while(1){
		printf("password : ");
		scanf("%s", passwd);
	
		if(!verify((char *)passwd))
		{
			printf("main error\n");
		}
		else
		{
			break;
		}	
	}
	
	
	while(1){
		printf("password 1 : ");
		scanf("%s", passwd1);
	
		if(strcmp(passwd,passwd1) == 0)
		{
			printf("true");
			break;
		}
		

	}
}

int verify(char *passwd){
	
	if(!lenCheck((char *)passwd)){
		return false;
	}
	
	if(!strcheck((char *)passwd)){
		return false;
	}
	
	if(!cmpId((char *)passwd)){
		return false;
	}
	
	if(!cmpOrgPw((char *)passwd)){
		return false;
	}
	
	if(!sameChar((char *)passwd)){
		return false;
	}
	if(!numCheck((char *)passwd)){
		return false;
	}
	
	if(!qwertyCheck((char *)passwd)){
	printf("[error] qwertyCheck\n");
		return false;
	}
	
	return true;
}

int lenCheck(char *passwd){
	
	long int strLen = 0;
	
	strLen = strlen(passwd);
	
	if(strLen < 10 || strLen > 20)
	{
		printf("strlen error : %ld\n", strLen);
		return false;
	}
	
	return true;

}

int isspecialchar(char x){

	/*if(x >= 40 && x <= 47)
		return true;
	else
		return false;*/
		
	char strKeyboard[] = "~!@#$%^&*()-_+=./?";
	char* ptr = NULL;
	
	ptr = strchr(strKeyboard, x);
		
	if(ptr == NULL){
		ptr = NULL;
		return false;
	}
	
	return true;

}
int strcheck(char *passwd){
	
	int upCount = 0;
	int lowCount = 0;
	int numCount = 0;
	int etcCount = 0;
	
	for(int i = 0; i < strlen(passwd); i++){
	
		if(isupper(passwd[i]))
			upCount++;

		if(islower(passwd[i]))
			lowCount++;
			
		if(isdigit(passwd[i]))
			numCount++;
						
		if(isspecialchar(passwd[i]))
			etcCount++;
		
		}
		
	if(upCount == 0 || lowCount == 0 || numCount == 0 || etcCount == 0){
		printf("[error] passwd format error\n");
		return false;
	}
}

int cmpId(char *passwd){

printf("id test : %s", passwd);
	if(strcmp(ID, passwd) == 0)
	{
		printf("[error] same ID!!\n");
		return false;
	}
	
	return true;
}

int cmpOrgPw(char *passwd){

	if(strcmp(PW, passwd) == 0)
	{
		printf("[error] same PW!!\n");
		return false;
	}
	
	return true;
}

int sameChar(char *passwd){
	
	int sameCount = 1;
				
	for(int i = 0; i < strlen(passwd); i++){
	
		if(passwd[i] == passwd[i+1]){
		
			sameCount++;
						
			if(sameCount >= 3){			
				return false;
			}
		}
		else
		{
			sameCount = 0;
		}
	}
	
	return true;
}

int numCheck(char *passwd){
	
	int ascCount = 1;
	int descCount = 1;
			
	for(int i = 0; i < strlen(passwd); i++){
	
		if(passwd[i] >= 49 && passwd[i] <= 57){		
			if(passwd[i+1] == passwd[i]+1){
				
				ascCount++;

				if(ascCount >= 3){			
					return false;
				}
			}
			else
			{
				ascCount = 0;
			}
		}
	}
	
	for(int i = 0; i < strlen(passwd); i++){
	
		if(passwd[i] >= 49 && passwd[i] <= 57){		
			if(passwd[i+1] == passwd[i]-1){
				
				descCount++;

				if(descCount >= 3){			
					return false;
				}
			}
			else
			{
				descCount = 0;
			}
		}
	}
	
	return true;
}

int qwertyCheck(char *passwd){
		
	char cmpStr1[5] = "";
	char cmpStr2[5] = "";
		
	int passwdSize = strlen(passwd);
	
	char lowerStr[passwdSize];
	char upperStr[passwdSize];
	
	int count = 0;
	
	count = strlen(passwd)-4;
	
	for(int i = 0; i <= count; i++){
	
		for(int j = 0; j <= passwdSize; j++){

			upperStr[j] = toupper(passwd[j]);
			lowerStr[j] = tolower(passwd[j]);
		
		}
			
		strncpy(cmpStr1, lowerStr+i, sizeof(cmpStr1)-1);		
		strncpy(cmpStr2, upperStr+i, sizeof(cmpStr2)-1);
		
		if(!qwerCheck((char *)cmpStr1)){ return false; break;}
		if(!QWERCheck((char *)cmpStr2)){ return false; break;}
		if(!descqwerCheck((char *)cmpStr1)){ return false; break;}
		if(!descQWERCheck((char *)cmpStr2)){ return false; break;}
				
		if(!asdfCheck((char *)cmpStr1)){ return false; break;}
		if(!ASDFCheck((char *)cmpStr2)){ return false; break;}
		if(!descasdfCheck((char *)cmpStr1)){ return false; break;}
		if(!descASDFCheck((char *)cmpStr2)){ return false; break;}
				
		if(!zxcvCheck((char *)cmpStr1)){ return false; break;}
		if(!ZXCVCheck((char *)cmpStr2)){ return false; break;}
		if(!desczxcvCheck((char *)cmpStr1)){ return false; break;}
		if(!descZXCVCheck((char *)cmpStr2)){ return false; break;}		
	}
	
	return true;
}

int qwerCheck(char *passwd){
		
	char strKeyboard[] = "qwertyuiop";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}

int QWERCheck(char *passwd){
		
	char strKeyboard[] = "QWERTYUIOP";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}

int descqwerCheck(char *passwd){
	
	char strKeyboard[] = "poiuytrewq";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}

int descQWERCheck(char *passwd){

	char strKeyboard[] = "POIUYTREWQ";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}


int asdfCheck(char *passwd){
	
	char strKeyboard[] = "asdfghjkl";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}

int ASDFCheck(char *passwd){

	char strKeyboard[] = "ASDFGHJKL";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}
int descasdfCheck(char *passwd){
	
	char strKeyboard[] = "lkjhgfdsa";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}

int descASDFCheck(char *passwd){

	char strKeyboard[] = "LKJHGFDSA";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}
int zxcvCheck(char *passwd){
	
	char strKeyboard[] = "zxcvbnm";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}

int ZXCVCheck(char *passwd){

	char strKeyboard[] = "ZXCVBNM";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}
int desczxcvCheck(char *passwd){
	
	char strKeyboard[] = "mnbvcxz";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}

int descZXCVCheck(char *passwd){

	char strKeyboard[] = "MNBVCXZ";
	
	char* ptr = NULL;
	
	ptr = strstr(strKeyboard, passwd);
		
	if(ptr != NULL){
		ptr = NULL;
		return false;
	}
	
	return true;
}
