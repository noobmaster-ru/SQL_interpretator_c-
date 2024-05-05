#if !defined(_Table_h_)
#define _Table_h_

#define MaxFieldNameLen 30 /* ���ᨬ��쭮� �᫮ ᨬ����� � �������� ���� */

enum Errors /* ��।���� �������� ���� �訡�� �� ࠡ�� � ⠡��栬� */
{
  OK,             /* �訡�� ��� */
  CantCreateTable,
  CantOpenTable,
  FieldNotFound,
  BadHandle,
  BadArgs,
  CantMoveToPos,
  CantWriteData,
  CantReadData,
  CorruptedData,
  CantCreateHandle,
  ReadOnlyFile,
  BadFileName,
  CantDeleteTable,
  CorruptedFile,
  BadPos,
  BadFieldType,
  BadFieldLen,
  NoEditing,
  BadPosition,
};

/* �������� ᮮ�饭�� �� �訡��� */
extern char* ErrorText[];

/* ������ ⠡���, � ���ன ࠡ�⠥� ���짮��⥫�, ������ ᮮ⢥��⢮���� ���ਯ�� ⨯� THandle, ����砥�� �� ��楤��� ������ ⠡����. �� ����樨 ࠡ��� � ⠡��栬� �ᯮ����� ��� ���ਯ�� ��� ��뫮� �� �������� ⠡����.*/

typedef struct Table * THandle;

typedef unsigned Bool; /* �㫥�᪨� ⨯ */    

#define TRUE 1
#define FALSE 0

/* ������騩 ⨯ ��।���� �������� ⨯� ������, �࠭���� � ⠡���� */

enum FieldType
{
  Text,     /* ��ப� ( �� ����� 256 ᨬ�����) */
  Long,     /* 楫�� ������� �������� �᫮ */
  Logic
};

/* �� ������� ������ ���ᠭ�� ������ ���� ⠡���� */

struct FieldDef
{
  char name[MaxFieldNameLen]; /* ��� ������� ���� */
  enum FieldType type; /* ⨯ ���� */
  unsigned len; /* ����� ���� � ����� */
};

/* �������� ������� ��।���� �������� ⠡���� � �ᯮ������ �� ᮧ����� ����� ⠡����. ���ଠ�� �� �⮩ ������� �࠭���� � ⠡��筮� 䠩�� */

struct TableStruct
{
  unsigned numOfFields; /* �᫮ ����� � ⠡��� */
  struct FieldDef *fieldsDef; /* �������᪨� ���ᨢ, 
  ����� ������� ���ண� - ���ᠭ�� ���� ⠡���� */
};

/******************************************************/
/*  ���� ����뢠���� ���⨯� �㭪権 ��� ࠡ��� 	*/
/*  � ⠡��栬�. ��᫥ ������� ���⨯� ᫥��� 		*/
/*  ��⪨� �������਩							*/

enum Errors createTable(char *tableName,
			struct TableStruct *tableStruct);

/* �㭪�� ᮧ���� ����� ⠡���� � ������� ������ � ������ன. �� �⮬ ᮧ������ ���� 䠩� � ⥪�饩 ��४�ਨ, � ��砫��� �������� 䠩�� ��࠭���� ������� ⠡���� */

enum Errors deleteTable(char *tableName);

/* �㭪�� 㤠��� ⠡���� � ������� ������ */

enum Errors openTable(char *tableName, THandle *tableHandle);

/* �㭪�� ���뢠�� ⠡���� � ������� ������ ��� ࠡ���. �� ����� tableHandle �����뢠���� ���ਯ��, ����� ������ �ᯮ�짮������ ��� ��뫮� �� ������ ⠡����. �� �⮬ ���뢠���� 䠩� ⠡����. �� �᭮�� ��������� 䠩�� ⠡����, �࠭�饣� ���ଠ�� � ������� ⠡����, � ����⨢��� ����� ���������� ᮮ⢥�����騩 ������� ⨯� struct TableStruct. ��������� ����� ��� ࠡ��� � ⥪�饩 ������� � ��� ᮧ����� ����� �����. ������ ���஢ ��।������ �� �᭮�� �������� ⠡����. */

enum Errors closeTable(THandle tableHandle);

/* �㭪�� ����뢠�� ⠡���� ��� �ᯮ�짮�����. �� �⮬ �᢮��������� ����, ���������� ������ ⠡��楩,� �����, ����뢠���� 䠩� ⠡����. */

enum Errors moveFirst(THandle tableHandle);

/* �㭪�� ��⠭�������� 㪠��⥫� 䠩�� �� ����� ������ (�᫨ ��� ����) � ���뢠�� ������ � ���� ⥪�饩 �����. �᫨ ⠡��� ����, � ���ﭨ� ���� ⥪�饩 ����� �� ��।�����. �� �⮬ �㭪樨 afterLast � beforeFirst �뤠�� ���祭�� TRUE.*/

enum Errors moveLast(THandle tableHandle);

/* �㭪�� ��⠭�������� 㪠��⥫� 䠩�� �� ��᫥���� ������ (�᫨ ��� ����) � ���뢠�� ������ � ���� ⥪�饩 �����.. �᫨ ⠡��� ����, � ���ﭨ� ���� ⥪�饩 ����� �� ��।�����. �� �⮬ �㭪樨 afterLast � beforeFirst �뤠�� ���祭�� TRUE.*/

enum Errors moveNext(THandle tableHandle);

/* �㭪�� ��⠭�������� 㪠��⥫� 䠩�� �� ᫥������ � 䠩�� ������ (�᫨ ��� ����) � ���뢠�� ������ � ���� ⥪�饩 �����.. �᫨ ���� 㦥 ��室���� �� ��᫥���� �����, � �� ���室�� � ���ﭨ� "��᫥ ��᫥����", � ���஬ ᮤ�ন��� ���� �� ��।�����. �� �⮬ �㭪�� afterLast �뤠�� ���祭�� TRUE.*/


enum Errors movePrevios(THandle tableHandle);

/* �㭪�� ��⠭�������� 㪠��⥫� 䠩�� �� �।����� ������ (�᫨ ��� ����) � ���뢠�� ������ � ���� ⥪�饩 �����.. �᫨ ���� 㦥 ��室���� �� ��ࢮ� �����, � �� ���室�� � ���ﭨ� "��। ��ࢮ�", � ���஬ ᮤ�ন��� ���� �� ��।�����. �� �⮬ �㭪�� beforeFirst �뤠�� ���祭�� TRUE.*/

Bool beforeFirst(THandle tableHandle);

/* �㭪�� �뤠�� ���祭�� TRUE, �᫨ ⠡��� ���� ��� �᫨ � ���ﭨ� "�� ��ࢮ� �����" �믮������ ������ movePrevios, ���� �뤠���� ���祭�� FALSE. */

Bool afterLast(THandle tableHandle);

/* �㭪�� �뤠�� ���祭�� TRUE, �᫨ ⠡��� ���� ��� �᫨ � ���ﭨ� "�� ��᫥���� �����" �믮������ ������ moveNext, ���� �뤠���� ���祭�� FALSE. */

enum Errors getText(THandle tableHandle, char *fieldName,char **pvalue);

/* �㭪�� ��ᢠ����� ��६����� pvalue 㪠��⥫� �� ��ப� - ���祭�� ���� fieldName. */

enum Errors getLong(THandle tableHandle, char *fieldName,
		    long *pvalue);

/* �㭪�� ��ᢠ����� ��६����� pvalue 楫�� �᫮ - ���祭�� ���� fieldName. */

enum Errors startEdit(THandle tableHandle);

/* �㭪�� �ᯮ������ ��। ��砫�� ।���஢���� ⥪�饩 ����� */ 

enum Errors putText(THandle tableHandle, char *fieldName,
		    char *value);

/* �㭪�� ��ᢠ����� ���� fieldName - ���祭��-��ப� */

enum Errors putLong(THandle tableHandle, char *fieldName,
		    long value);

/* �㭪�� ��ᢠ����� ���� fieldName ���祭�� - 楫�� �᫮ */

enum Errors finishEdit(THandle tableHandle);

/* �㭪�� �����稢��� ।���஢���� ⥪�饩 ����� (� ����). �� �⮬ �ந�������� ������ ����������� ����� � 䠩�. */

enum Errors createNew(THandle tableHandle);

/* �㭪�� ������ ���� ����� �����, ��� �ᯮ������ ��। ����������� ���� ����� ����� ������묨 ���祭�ﬨ */

/* ������騥 ��� �㭪樨 ��ᢠ����� ���祭�� ᮮ⢥�����騬 ���� � ���� ����� �����. */

enum Errors putTextNew(THandle tableHandle, char 								*fieldName, char *value);
enum Errors putLongNew(THandle tableHandle, 
					char * fieldName, long value);

enum Errors insertNew(THandle tableHandle);

/* �㭪�� ��⠢��� ����� ������ �� ���� ����� ����� ��। ⥪�饩 ������� (� ᤢ���� "墮��").*/

enum Errors insertaNew(THandle tableHandle);

/* �㭪�� ��⠢��� ����� ������ �� ���� ����� ����� � ��砫� ⠡����.*/

enum Errors insertzNew(THandle tableHandle);

/* �㭪�� ��⠢��� ����� ������ �� ���� ����� ����� � ����� ⠡����.*/

enum Errors deleteRec(THandle tableHandle);

/* �㭪�� 㤠��� ⥪���� ������. �� �⮬, �᫨ ���� ᫥����� ������, � ��� ��⮬���᪨ �⠭������ ⥪�饩, �᫨ ���, � ������ ���ﭨ� "��᫥ ��᫥����". */

char *getErrorString(enum Errors code);

/* �㭪�� �� ���� �訡�� �뤠�� �� �।�⠢����� � ���� ��ப�.*/

enum Errors getFieldLen(THandle tableHandle,char*fieldName,unsigned *plen);

/* �㭪�� �뤠�� ����� ���� ⠡���� � �����. */

enum Errors getFieldType(THandle tableHandle, 
			char *fieldName, enum FieldType *ptype);
/* �㭪�� �뤠�� ⨯ ���� ⠡����. */

enum Errors getFieldsNum(THandle tableHandle, 
								unsigned *pNum);
/* �㭪�� �뤠�� �᫮ ����� � ⠡���. */

enum Errors getFieldName(THandle tableHandle, 
			unsigned index, char **pFieldName);
/* �㭪�� �� ������ ���� � ⠡��� �뤠�� ��� ���. ���� �㬥������ � 1. */
#endif

