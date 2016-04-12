#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_EXTERN 26 //trie��nextָ��������Ŀ,ȫ�ַ�ʱӦ����Ϊ256Ȼ���0xff����&����

typedef struct ac_node ac_node;
typedef struct ac_node* ac_node_p;

#define QUEUE_TYPE ac_node_p //���д˺궨��,�Ϳ��԰�queue��װ�ɽӿ�ʹ��
#define FREE_QUEUE_VALUE     //queue->valueΪ��̬�����ڴ���Ҫ���˲���	

int queue_node_num = 0;

//������нṹ���ڼ���ʧЧָ��
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
 * queue�������
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
 * queue��ʼ��
 * return -1 ʧ�� else �ɹ�
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
 *queue Ϊ���ж�
 *return 1 Ϊ�� 0 ��Ϊ��
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
 * queue ���β��ӽ��
 */
void queue_insert(queue *ac_queue,queue_node *node)
{
	ac_queue->tail->next = node;
	ac_queue->tail = node;
	queue_node_num++;
	printf("after insert the queue node num is %d :\n",queue_node_num);	
}

/*
 *queue ��ȡ���׽��valueֵ
 */
QUEUE_TYPE queue_first(queue *ac_queue)
{
	if(queue_is_empty(ac_queue))
	{
		printf("the queue is empty can not return head!!\n");

		return NULL;
	}
	
	return ac_queue->head->next->value; //���ײ���ֵ,�Ӷ��׵���һ����㿪ʼȡֵ
}

/*
 *queue ���׽�������
 */
void queue_delete(queue *ac_queue)
{
	
	if(queue_is_empty(ac_queue))
	{
		printf("the queue is empty we can not delete head\n\n");
		return;
	}

	queue_node *head = ac_queue->head->next; //���ײ���ֵ,�Ӷ��׵���һ����㿪ʼ������
	
	ac_queue->head->next = head->next;
	
	if(head == ac_queue->tail)
	{//������Ϊ���һ��Ԫ��ʱ�������ÿ�
		ac_queue->tail = ac_queue->head;	
	}

	free(head); //�ͷŶ��׽���ڴ�

	queue_node_num--;	
	printf("after delete the queue node num is %d :\n",queue_node_num);	
}

/*
 *queue �ͷ�queue�ڴ�
 */
void queue_destroy(queue *ac_queue)
{
	queue_node *p = NULL;
	p = ac_queue->head;
	
	while(p != NULL)
	{

#ifdef FREE_QUEUE_VALUE
		if(p->value != NULL)
			free(p->value); //valueΪ��̬�����ڴ����������˲���	
#endif
		queue_node *tmp = p->next;
		
		if(p != NULL)
			free(p);
		
		p = tmp;
	}
}

//ac״̬�ڵ�
struct ac_node
{
	int final; //�Ƿ�Ϊһ��ģʽ����β�ı�ʾ
	int model; //��ʶ��ģʽ��Ϊ�ĸ�ģʽ��(������Ǻ�׺��ģʽ,�˴�Ӧ�ø�Ϊ��������)
	ac_node *fail; //��״̬�ڵ��ʧЧָ��

	struct ac_node *next[CHAR_EXTERN];
};

/*
 * ����״̬�ڵ�
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
 * ����trie��
 * return -1 ʧ�� else �ɹ�
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
		 ����дȻ���next_node���������trie������ʧ��,����next_nodeһֱ��ΪNULL
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
* ����ʧЧָ�뺯��
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
		queue_delete(ac_queue);//����Ԫ�س�����

		for(i = 0; i < CHAR_EXTERN; i++)
		{//ͨ�����в���BFS(�������)�ı���˳�������㵱ǰ״̬ÿ���ַ���ʧЧ����

			if(tmp_node->next[i] != NULL) // if CHART_EXTERN=255 tmpnode->next[i&0xff]
			{
				if(tmp_node == root)
				{
					tmp_node->next[i]->fail = root; //��һ��ڵ��ʧЧָ��ָ������
				}
				else
				{
					fail_node = tmp_node->fail; //������ʧЧָ��

					while(fail_node != NULL)
					{//��ֱ���ص������ʱ
						if(fail_node->next[i] != NULL)
						{//�ڸ�����ʧЧָ�����ҵ���ǰ�ַ��������
							tmp_node->next[i]->fail = fail_node->next[i];
							break;
						}

						fail_node = fail_node->fail; //�����ݹ�
					}

					if(fail_node == NULL)
					{//�Ҳ���ʧЧָ�룬��ʧЧָ��Ϊ�����
						tmp_node->next[i]->fail = root;
					}
					
				}

				q_node = queue_build_node(tmp_node->next[i]);
				queue_insert(ac_queue,q_node); //����ǰ��Ľ�������м������й�����ȱ���
				printf("insert into a ac node into queue the state is : %c \n\n", i + 'a');
				printf("insert the ac node pointer is %p\n",tmp_node->next[i]);
			}
		}		
	}
}

/*
 *ģʽƥ�亯��
 *return -1 δƥ�䵽�κ�ģʽ else ƥ�䵽�ĵ�ǰ��ģʽ����ֵ
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
		{//״̬�ڵ�ǰ�ַ�������ת���Ժ��ʧЧת��
			p = p->fail;
		}
		
		p = p->next[index]; //��״̬��������

		if(p == NULL)
		{//δ�ҵ�ʧЧָ��,�Ӹ���㿪ʼ����
			p = root;
		}
		
		tmp_node = p; //���㵱ǰ״̬�µ�ƥ�����(�ھֲ�����ָ���ͱ������������ڴ�ָ������)

		while(tmp_node != root)
		{
			if(tmp_node->final == 1)
			{
				match_num++;
				
				//tmp_node = tmp_node->fail; //ƥ����ģʽ��

				return tmp_node->model; //�˴�������return ���Լ�����ģʽ��
			}

			tmp_node = tmp_node->fail; //ƥ����ģʽ��
		}

	}

	return -1;
}

/*
 *��ӡtrie��
 */
void ac_trie_print(ac_node *root)
{//���ö��н��й�����ȱ�������ӡtrie��
	ac_node *ac_queue[256];
	int i,head,tail;
	head = tail = 0;

	memset(ac_queue,0,sizeof(ac_queue));

	ac_node *tmp = root;

	ac_queue[tail++] = tmp;
	while(head != tail)
	{
		tmp = ac_queue[head++]; //������
		for(i = 0; i < CHAR_EXTERN; i++)
		{
			if(tmp->next[i] != NULL)
			{
				printf("%c  ",i+'a');
				ac_queue[tail++] = tmp->next[i]; //����ǰ������нڵ������
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
