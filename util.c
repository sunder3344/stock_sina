#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<string.h>
#include<unistd.h>
#include<iconv.h>
#define OUTLEN 255

/*代码转换:从一种编码转为另一种编码*/
int code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
    iconv_t cd;
    int rc;
    char **pin = &inbuf;
    char **pout = &outbuf;
     
    cd = iconv_open(to_charset,from_charset);
    if (cd==0) return -1;
    memset(outbuf,0,outlen);
    if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
    iconv_close(cd);
    return 0;
}
/*UNICODE码转为GB2312码*/
int u2g(char *inbuf,int inlen,char *outbuf,int outlen) {
    return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
}

/*GB2312码转为UNICODE码*/
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen) {
    return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
}

void explode2(char * str, char seperator, char *data[]) {
	int len = strlen(str);
	int i;
	int pre_flag = 0;
	int count = 0;
	
	printf("data pointer = %p\n", data);
	for(i = 0; i < len; i++) {
		if (*(str + i) == seperator) {
			char *temp;
			printf("aaa = %d\n", i - pre_flag);
			temp = (char *)malloc(sizeof(char) * (i - pre_flag));
			strncpy(temp, str + pre_flag, i - pre_flag);
			printf("temp = %s\n", temp);
			data[count] = temp;
			printf("count = %d\n", count);
			printf("data pointer = %p\n", data[count]);
			printf("data['count'] = %s\n", data[count]);
			pre_flag = i + 1;
			count++;
		}
	}
	if (pre_flag < len) {
		data[count] = str + pre_flag;
	}
}

void explode(char str[], char seperator, char *data[]) {
	int len = strlen(str);
	//char * data[32];
	int i;
	int pre_flag = 0;
	int count = 0;
	
	//printf("data pointer = %p\n", data);
	//printf("len = %d\n", len);
	for(i = 0; i < len; i++) {
		if (str[i] == seperator) {
			str[i] = '\0';
			char * temp;
			temp = (char *)malloc(sizeof(char) * (i - pre_flag + 1));
            /*if (i - pre_flag > 0) {
                temp = (char *)realloc(temp, sizeof(char) * (i - pre_flag + 1));
            } else {
                temp = (char *)realloc(temp, sizeof(char) * 1);
            }*/
			strncpy(temp, str + pre_flag, i - pre_flag + 1);
            char *p = temp;
			//printf("temp = %s\n", temp);
			data[count] = temp;
			pre_flag = i + 1;
			count++;
		}
	}
	if (pre_flag < len) {
		data[count] = str + pre_flag;
	}
}
