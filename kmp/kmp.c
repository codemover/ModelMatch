#include <string.h>
#include <stdio.h>

#define LIB_MATCH_NUM  5
#define LIB_STRING_LEN 40

char lib_match[LIB_MATCH_NUM][LIB_STRING_LEN] =
{
	"www.baidu.comw",
	"www.google.com",
	"www",
	"baidu.com",
	"lualu",
};

int lib_match_len[LIB_MATCH_NUM];

int lib_match_next[LIB_MATCH_NUM][LIB_STRING_LEN + 1] = {0};


void get_next();

void lib_match_init()
{
	memset(lib_match_len,0,sizeof(lib_match_len));
	int i;
	for(i = 0; i < LIB_MATCH_NUM; i++)
	{
		lib_match_len[i] = strlen(lib_match[i]); 
	}

	get_next(); // init next array 
}

void get_next()
{
	int i,j,k;

	for(i = 0; i < LIB_MATCH_NUM; i++)
	{
		
		lib_match_next[i][0] = 0;
		lib_match_next[i][1] = 0;
		
		k = 0; //point to the last next[j]
		int len = lib_match_len[i];

		for( j = 1; j < len; j++)
		{//j start from 1 because next start from 2
			k = lib_match_next[i][j]; //k = next[j];
			
			while(k > 0 && lib_match[i][j] != lib_match[i][k])
			{
				k = lib_match_next[i][k];
			}

			if(lib_match[i][j] == lib_match[i][k])
			{//s[j] == s[next[k]] next[i+1] = next[k] +1
				k++; 
			}

			lib_match_next[i][j+1] = k;
		}
	
	}

}

int lib_search(char *orignal,int len)
{
	int i,j,k;
	
	for(i = 0; i < LIB_MATCH_NUM; i++)
	{
		char *tmp = orignal; // match each model need to start from the first
		k = 0; // k start from the last next array

		for(j = 0; j < len; j++)
		{
			
			while( k > 0 && tmp[j] != lib_match[i][k])
			{
				k = lib_match_next[i][k];
			}

			if(tmp[j] == lib_match[i][k])
			{
				k++;
			}

			if(k == lib_match_len[i])
			{
				printf("match the model string %d : %s\n", i,lib_match[i]);
				printf("match at the position  :%d\n\n", j - lib_match_len[i] + 1);
				
				// here can return the i to know which model string have been matched
			
				k = lib_match_next[i][k]; //to match another this model string 
			}

		}

	}	

	return 0;
}

void print_next()
{
	int i,j;
	
	for(i = 0; i < LIB_MATCH_NUM; i++)
	{
		printf("model string : %s \n",lib_match[i]);
		printf("next array:");
		for(j = 0; j < lib_match_len[i]+1;j++)
		{
			printf(" %d",lib_match_next[i][j]);
		}
		printf("\n\n");
	}
}


int main()
{
	char orignal[40];
		
	lib_match_init();

	print_next();

	while(1)
	{	

		scanf("%s",orignal);

		if(strcmp(orignal,"0") == 0)
		{
			return 0;
		}

		lib_search(orignal,strlen(orignal));
	}

	return 0;

}
