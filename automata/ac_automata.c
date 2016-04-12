#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_EXTERN 26 //trie的next指针的最大数目,全字符时应设置为256然后和0xff进行&运算

typedef struct ac_node ac_node;
typedef struct ac_node* ac_node_p;

#define QUEUE_TYPE ac_node_p //进行此宏定义,就可以把queue封装成接口使用
#define FREE_QUEUE_VALUE     //queue->value为动态申请内存需要做此操作	

int queue_node_num = 0;

//定义队列结构用于计算失效指针
typedef struct queue_node
{
	QUEUE_TYPE value;
	struct queue_node *next;
}queue_node;


typedef struct queue
{
	queue_node *head;
	queue_node *tail;
}queue;

/*
 * queue创建结点
 */
queue_node *queue_build_node(QUEUE_TYPE value)
{
	queue_node *node = (queue_node *)malloc(sizeof(queue_node));

	if(node == NULL)
	{
		printf("queue node bulid error memory is full !!\n\n");
		
		return NULL;
	}
	
	node->value = value;

	node->next = NULL;
	
	return node;
}

/*
 * queue初始化
 * return -1 失败 else 成功
 */
queue *queue_init()
{
	queue *ac_queue = (queue *)malloc(sizeof(queue));
	
	if(ac_queue == NULL)
	{
		printf("queue build failed memory is full\n\n");
		
		return NULL;
	}	

	ac_queue->head = ac_queue->tail = queue_build_node(NULL);
	
	if(ac_queue->head == NULL)
	{
		printf("queue build head error memory is full\n\n");

		return NULL;
	}

	//ac_queue->head->next = ac_queue->tail;

	return ac_queue;
}

/*
 *queue 为空判断
 *return 1 为空 0 不为空
 */
int queue_is_empty(queue *ac_queue)
{
	if(ac_queue->head == ac_queue->tail)
	{
		return 1;
	}

	return 0;
}

/*
 * queue 向队尾添加结点
 */
void queue_insert(queue *ac_queue,queue_node *node)
{
	ac_queue->tail->next = node;
	ac_queue->tail = node;
	queue_node_num++;
	printf("after insert the queue node num is %d :\n",queue_node_num);	
}

/*
 *queue 提取队首结点value值
 */
QUEUE_TYPE queue_first(queue *ac_queue)
{
	if(queue_is_empty(ac_queue))
	{
		printf("the queue is empty can not return head!!\n");

		return NULL;
	}
	
	return ac_queue->head->next->value; //队首不存值,从队首的下一个结点开始取值
}

/*
 *queue 队首结点出队列
 */
void queue_delete(queue *ac_queue)
{
	
	if(queue_is_empty(ac_queue))
	{
		printf("the queue is empty we can not delete head\n\n");
		return;
	}

	queue_node *head = ac_queue->head->next; //队首不存值,从队首的下一个结点开始出队列
	
	ac_queue->head->next = head->next;
	
	if(head == ac_queue->tail)
	{//出队列为最后一个元素时将队列置空
		ac_queue->tail = ac_queue->head;	
	}

	free(head); //释放队首结点内存

	queue_node_num--;	
	printf("after delete the queue node num is %d :\n",queue_node_num);	
}

/*
 *queue 释放queue内存
 */
void queue_destroy(queue *ac_queue)
{
	queue_node *p = NULL;
	p = ac_queue->head;
	
	while(p != NULL)
	{

#ifdef FREE_QUEUE_VALUE
		if(p->value != NULL)
			free(p->value); //value为动态申请内存的情况下做此操作	
#endif
		queue_node *tmp = p->next;
		
		if(p != NULL)
			free(p);
		
		p = tmp;
	}
}

//ac状态节点
struct ac_node
{
	int final; //是否为一个模式串结尾的表示
	int model; //标识该模式串为哪个模式串(如果考虑后缀子模式,此处应该改为整型链表)
	ac_node *fail; //该状态节点的失效指针

	struct ac_node *next[CHAR_EXTERN];
};

/*
 * 创建状态节点
 */
ac_node *ac_node_build()
{
	int i;
	ac_node *node = (ac_node *)malloc(sizeof(ac_node));

	if(node == NULL)
	{
		printf("bulid node error the memory is full !! \n\n");
		
		return NULL;
	}

	node->final = 0;
	node->model = -1;
	node->fail = NULL;

	for(i = 0; i < CHAR_EXTERN; i++)
	{
		node->next[i] = NULL;
	}

	return node;
}


/*
 * 创建trie树
 * return -1 失败 else 成功
 */
int ac_trie_build(ac_node *root,char *str,int len,int model)
{
	int i;
	ac_node *tmp = root;
	
	if(tmp == NULL)
	{
		printf("root has not been init!!! \n\n");
		return -1;
	}	

	for(i = 0; i < len; i++)
	{

		/*
		 ac_node *next_node = tmp->next[str[i] - 'a'];
		 这样写然后对next_node操作会造成trie树建立失败,由于next_node一直不为NULL
		 */

		int index = str[i] - 'a'; // if CHAR_EXTERN=256 index = str[i]&0xff
		if(tmp->next[index] == NULL)
		{
			tmp->next[index] = ac_node_build();
			
			if(tmp->next[index] == NULL)
			{
				printf("build node error in ac_trie_build !!\n");
				return -1;
			}

		}
		
		tmp = tmp->next[index];
	}

	tmp->final = 1;
	tmp->model = model;

	return 0;
}


/*
* 创建失效指针函数
*/

void ac_build_fail(ac_node *root,queue *ac_queue)
{
	int i;
	queue_node *q_node = NULL;
	ac_node *tmp_node = NULL;
	ac_node *fail_node = NULL;

	q_node = queue_build_node(root);
	queue_insert(ac_queue,q_node);

	while(!queue_is_empty(ac_queue))
	{
		tmp_node = queue_first(ac_queue);
		printf("out the queue the ac node pointer is %p \n",tmp_node);
		queue_delete(ac_queue);//队首元素出队列

		for(i = 0; i < CHAR_EXTERN; i++)
		{//通过队列采用BFS(广度优先)的遍历顺序来计算当前状态每个字符的失效函数

			if(tmp_node->next[i] != NULL) // if CHART_EXTERN=255 tmpnode->next[i&0xff]
			{
				if(tmp_node == root)
				{
					tmp_node->next[i]->fail = root; //第一层节点的失效指针指向根结点
				}
				else
				{
					fail_node = tmp_node->fail; //父结点的失效指针

					while(fail_node != NULL)
					{//当直到回到根结点时
						if(fail_node->next[i] != NULL)
						{//在父结点的失效指针中找到当前字符的外向边
							tmp_node->next[i]->fail = fail_node->next[i];
							break;
						}

						fail_node = fail_node->fail; //继续递归
					}

					if(fail_node == NULL)
					{//找不到失效指针，则失效指针为根结点
						tmp_node->next[i]->fail = root;
					}
					
				}

				q_node = queue_build_node(tmp_node->next[i]);
				queue_insert(ac_queue,q_node); //将当前层的结点插入队列继续进行广度优先遍历
				printf("insert into a ac node into queue the state is : %c \n\n", i + 'a');
				printf("insert the ac node pointer is %p\n",tmp_node->next[i]);
			}
		}		
	}
}

/*
 *模式匹配函数
 *return -1 未匹配到任何模式 else 匹配到的当前的模式串的值
 */
int ac_query(ac_node *root,char *str,int len)
{

	int i,match_num = 0;
	int index = 0;

	ac_node *tmp_node = NULL;
	ac_node *p = root;
	
	for(i = 0; i < len; i++)
	{
		index = str[i] - 'a'; // if CHAR_EXTERN=256 index = str[i]&0xff	

		while(p->next[index] == NULL && p != root)
		{//状态在当前字符不进行转移以后的失效转移
			p = p->fail;
		}
		
		p = p->next[index]; //将状态进行下移

		if(p == NULL)
		{//未找到失效指针,从根结点开始继续
			p = root;
		}
		
		tmp_node = p; //计算当前状态下的匹配情况(在局部定义指针型变量经常出现内存指向问题)

		while(tmp_node != root)
		{
			if(tmp_node->final == 1)
			{
				match_num++;
				
				//tmp_node = tmp_node->fail; //匹配子模式串

				return tmp_node->model; //此处不进行return 可以计算多个模式串
			}

			tmp_node = tmp_node->fail; //匹配子模式串
		}

	}

	return -1;
}

/*
 *打印trie树
 */
void ac_trie_print(ac_node *root)
{//利用队列进行广度优先遍历并打印trie树
	ac_node *ac_queue[256];
	int i,head,tail;
	head = tail = 0;

	memset(ac_queue,0,sizeof(ac_queue));

	ac_node *tmp = root;

	ac_queue[tail++] = tmp;
	while(head != tail)
	{
		tmp = ac_queue[head++]; //出队列
		for(i = 0; i < CHAR_EXTERN; i++)
		{
			if(tmp->next[i] != NULL)
			{
				printf("%c  ",i+'a');
				ac_queue[tail++] = tmp->next[i]; //将当前层的所有节点入队列
			}
		}
		
		printf("\n");
			
	}
}


#define LIB_MODEL_NUM 10

int main()
{
#if 1
	char *lib_model_str[10] = {"wwwgooglecom",\
			     "wwwbaiducom",\
			     "www",\
			     "googlecom",\
			     "guoxin",\
	                     "she",\
	                     "her",\
	                     "shr",\
	                     "yes",\
			     "say"};

#endif

#if 0

	char *lib_model_str[LIB_MODEL_NUM] = {
		"she",\
		"he",\
		"say",\
		"shr",\
		"her"\
	};
#endif

	int i;
	char str[1024];
	queue *ac_queue = NULL;
	ac_node *root = NULL; 

	ac_queue = queue_init();
	root = ac_node_build();

	for(i = 0; i < LIB_MODEL_NUM; i++)
	{
		ac_trie_build(root,lib_model_str[i],strlen(lib_model_str[i]),i);	
	}
	

	ac_trie_print(root);
		
	ac_build_fail(root,ac_queue);
	
	while(1)
	{
		printf("input the match string:\n\n");
		
		scanf("%s",str);

		if(strcmp(str,"quit") == 0)
		{
			return -1;
		}

		int model = ac_query(root,str,strlen(str));

		if(model == -1)
		{
			printf("not match!!\n\n");
		}
		else
		{
			printf("match the model '%d' and the model string is %s \n\n",model,lib_model_str[model]);
		}
	}

	return 0;
}
