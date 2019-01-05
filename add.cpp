#include<stdio.h>
#include"page.cpp"
#include "load.cpp"
#include"BPlusTree.cpp"
int checkPage_Free(Page *p,int TNo)//ҳ�Ƿ��п��пռ� 
{
	if(p->TableNo != TNo)
		return 0;
	for(int t = 0;t < MAX_OF_RECORD;t++)
		if(strlen(p->KeyList[t])==0)
			{return 1;} 
	return 0;
}
int checkkey_UNIQUE(PageTable *PT,char *key)//key�Ƿ��ظ� 
{
	Page *P = PT->next;
	while(P)
	{
		if(strlen(P->KeyList[MAX_OF_RECORD-1]) != 0)//�˿���м�¼ 
		{
			//printf("1111\n");
			//
			for(int i = MAX_OF_RECORD-1;i>=0;i--)
			{
				if(strlen(P->KeyList[i])==0)
					break;
				if(strcmp(P->KeyList[i],key)==0){
					return 1;
				}
			}
		}
		P = P->next;
	}
	return 0;
}
PageNode* checkBuf_Free(PageTable *PT,BufferQue* Buf,int TNo)//�������Ƿ��п��пռ� �����ػ������ڵ� 
{
	PageNode *pn = Buf->front->next;
	while(pn)
	{
		Page *p = pn->page;
		if(checkPage_Free(p,TNo)==1)
			return pn;
		pn=pn->next;
	}
	return NULL;
 }
Page* checkPT_Free(PageTable *PT,int TNo)//ҳ�����Ƿ��п��пռ䣬����ҳ 
{

	Page *p=PT->next;
	while(p){
		if(!p->IFBuffer)
			if(checkPage_Free(p,TNo)==1)
				return p;
		p=p->next;
	}
	
	return NULL;
 } 
int addKeyList(Page *&p,char *key)//���пռ���� 
{

	for(int t = MAX_OF_RECORD-1;t>=0;t--)
		{
		if(strlen(p->KeyList[t]) == 0) 
			
			{
				//printf("4\n");
				//p->KeyList[t][8]=key;
				strcpy(p->KeyList[t],key);
				//printf("%d\n",t);
				p->F_Update=1;
				return t;
	    	}
	    }
	
}
Block* Rec_IN_Block(Block *b,char **value,int TNo) //������Ӽ�¼
{
	    char table[255];
		sprintf(table, "%.4d",TNo);
		
		Record *record=load(table,value);
		//strcpy(record->Head->tablenum,table);
		char *a=input (record);
		//��valueʶ���record 
		//printf("%s\n",a);
		//printf("11111\n");
		if (b->FirstAvailableBlock==-1) 
		{
			printf("error,block %d is full",b->BlockNo);
			return NULL;
		}
		//cur_block=ReadBlock(tableno,FirstBlockAvailble);//���ļ��ж������е�һ�����õĿ���Ϊ��ǰ�����Ŀ�,cur_block����ǰ�����Ŀ�(��������δ���壬Ӧ�������ж���)
		//�������뻺������
		//������¿����Ϣ 
		strcpy(b->Re[b->FirstAvailableBlock],a);//�Ѽ�¼д���׸��յļ�¼��
		
		b->RecordCondition[b->FirstAvailableBlock]=1;//�Ѵ�������Ϊ��ʹ��
		b->LastUsedRecord=b->FirstAvailableBlock; //���һ����ʹ�õĿ��Ǵ˿�
		
        b->FirstAvailableBlock=b->FirstAvailableBlock-1;
        free(a); 
        return b;
}
void addRecord(BPlusTree &BT,PageTable *&PT,BufferQue* &Buf,char *key,char** value,int TNo) //��TNo��������һ����¼ 
{
	
	//������������ҳ���Ƿ��п��пռ�
	if(checkkey_UNIQUE(PT,key) == 1){
		printf("��¼��ֵ�ظ�������ʧ�ܣ�\n");
		return;
	}
	//printf("!!!!!!!!!!!!\n"); 
	int offset = 0;
	PageNode *p1 = checkBuf_Free(PT,Buf,TNo);	
	if(p1){//p1�ǿգ������ػ������п��Բ����¼��ҳ 
		//printf("���������п��пռ�\n");
		//printf("���%d�����¼\n",p1->block->BlockNo);
		//printf("%d\n",p1->page->PageID);
		Block *c=(Block *)malloc(sizeof(Block));
		c=p1->block;
		c=Rec_IN_Block(c,value,TNo);
		//��������������������������������- 
			//ID=p1->BlockID;
			//��b�в���char **value 
		//����������������������������--  

		
		offset = addKeyList(p1->page,key);//����ҳ��key����
		BT = Insert(BT,atoi(key),TNo,c->BlockNo,offset);//����B+�� 
	//	for(int i=0;i<20;i++)
	//	{printf("%s\n",p1->page->KeyList[i]);} 
	}
	else{//p1Ϊ��,���������� 
		//printf("1\n");
		//printf("2\n");
		Page *p2 = checkPT_Free(PT,TNo);//////////////
		//printf("2\n");
		if(p2){//p2�ǿգ�����ҳ���п��Բ����¼��ҳ 
			//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); 
			Block *b=FIFO(PT,Buf,p2); 
			//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); 
			//��������������������������������- 
			//ID=p2->BlockID;
			//��b�в���char **value 
			//����������������������������--
            b=Rec_IN_Block(b,value,TNo);
			//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); 
			offset = addKeyList(p2,key);//����ҳ��key����
			//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); 
			BT = Insert(BT, atoi(key),TNo,b->BlockNo,offset);////����B+��
			//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");  
		}
		else
			printf("���ݿ�ҳ��ռ�����\n");
	}
	//Block *c=(Block *)malloc(sizeof(Block));
	//c=Buf->front->next->block;
	
}

