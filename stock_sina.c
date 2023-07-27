#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "util.c"
#include <curl/curl.h>
#define STOCK_LINE 30

char * res;

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
	//printf("ptr = %s\n", ptr);
	//printf("userdata = %s\n", userdata);
    if (res == NULL) {
	    res = (char *)realloc(res, sizeof(char) * (strlen(ptr)+1));
	    strcpy(res, ptr);
    } else {
        res = (char *)realloc(res, sizeof(char) * (strlen(res) + strlen(ptr) + 1));
        strcat(res, ptr);
    }
	//printf("res_callback:=%s\n", res);
	return size * nmemb;
}

int main(int argc, char *argv[]) {
	res = (char *)malloc(sizeof(char) * 100000);
    res = NULL;
	//read stock code from config
	FILE *fp = fopen("config_sina.ini", "r");
	if (NULL == fp) {
		perror("Open config file failed!");
	}
	
	//输出标题
	printf("%-15s|%-15s|%-15s|%-15s|%-15s|%-20s|%-20s|%-20s|%-20s|\n", "名称", "当前", "涨幅(%)", "昨收", "今开", "当日最高", "当日最低", "成交数(手)", "成交金额(万元)");

	char line[10];
	char * stockcode;
	stockcode = (char *)malloc(sizeof(char) * 10);
	stockcode = 0;
	char *flag = ",";
    int stock_num = 0;
	while (!feof(fp)) {
		memset(line, 0, sizeof(line));
		if (fgets(line, sizeof(line), fp) != NULL) {
			int len = strlen(line);
			//printf("line:=%s, stockcode:=%s\n", line, stockcode);
			if (stockcode == 0) {
				stockcode = (char *)realloc(stockcode, sizeof(char) * (len + 1));
                line[len - 1] = '\0';
				strcpy(stockcode, line);
                stock_num++;
			} else {
				int bigLen = strlen(stockcode);
				stockcode = (char *)realloc(stockcode, sizeof(char) * (bigLen + 1));
				strcat(stockcode, flag);
				bigLen = strlen(stockcode);
				stockcode = (char *)realloc(stockcode, sizeof(char) * (bigLen + len + 1));
                line[len - 1] = '\0';
				strcat(stockcode, line);
                stock_num++;
			}
		}
	}
	fclose(fp);

	CURL * curl = curl_easy_init();
	CURLcode code;
	char * url = NULL;
	char * sector = "https://hq.sinajs.cn/list=";
	url = (char *)malloc(sizeof(char) * (strlen(sector) + 1));
	strncpy(url, sector, strlen(sector));
	//printf("urlurlurl:=%s, len=%d\n\n", url, strlen(sector));
	int urllen = strlen(url);
	url = (char *)realloc(url, sizeof(char) * (urllen + strlen(stockcode) + 1));
	strcat(url, stockcode);
	//printf("url:=%s\n", url);

	struct curl_slist * head = NULL;
    head = curl_slist_append(head, "Referer:https://finance.sina.com.cn");

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, head);
	code = curl_easy_perform(curl);
	curl_slist_free_all(head);
	curl_easy_cleanup(curl);
    //printf("res:=%s\n", res);

    //printf("stock_num = %d\n", stock_num);
    char seperators = ';';
    char * datum[stock_num];
    explode(res, seperators, datum);
    int i;
    char * flag2 = "\"";
    char seperator = ',';
    for (i = 0; i < stock_num; i++) {
        //printf("%s\n", datum[i]);
        int pos = strcspn(datum[i], flag2);
        char * res2 = datum[i] + pos + 1;
        pos = strcspn(res2, flag2);
        char str[pos];
        strncpy(str, res2, pos);
        //printf("%s\n", str);

        int len2 = strlen(str);
        char out[len2 + 1];
        int rc = g2u(str, strlen(str), out, len2);
        //printf("%s\n", out);
    
        char * data[34];
        explode(out, seperator, data);
        //int j;
        //for (j = 0; j < 32; j++) {
        //        printf("data[%d] = %s\n", j, data[j]);
        //}
        
        double init_price = atof(data[1]);
        double yesterday_price = atof(data[2]);
        double current_price = atof(data[3]);
        double top_price = atof(data[4]);
        double end_price = atof(data[5]);
        long deal_num = atol(data[8]);
        double deal_amount = atof(data[9]);

        double surplus = current_price - yesterday_price;
        double rate = surplus * 100 / yesterday_price;
        deal_num = deal_num / 100;
        deal_amount = deal_amount / 10000;
            
        if (strlen(data[0]) == 12) {    
            printf("%-17s|%-13.2f|%-13.2f|%-13.2f|%-13.2f|%-16.2f|%-16.2f|%-16d|%-14.2f|\n", data[0], 
                        current_price, rate, yesterday_price, init_price, top_price, end_price, deal_num, deal_amount);
        } else {
            printf("%-16s|%-13.2f|%-13.2f|%-13.2f|%-13.2f|%-16.2f|%-16.2f|%-16d|%-14.2f|\n", data[0],
                        current_price, rate, yesterday_price, init_price, top_price, end_price, deal_num, deal_amount);
        }
    } 

    free(stockcode);
	free(res);
	free(url);
	return 0;
}
